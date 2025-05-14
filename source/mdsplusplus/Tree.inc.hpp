#ifndef MDSPLUS_TREE_INC_HPP
#define MDSPLUS_TREE_INC_HPP

#include "Tree.hpp"
#include "TreeNode.hpp"
#include "DataView.hpp"

namespace mdsplus {

inline void Tree::open()
{
    int status;

    if (_open) {
        close();
    }

    switch (_mode) {
    case Mode::Normal:
        status = _TreeOpen(&_dbid, _treename.c_str(), _shot, 0);
        break;
    case Mode::ReadOnly:
        status = _TreeOpen(&_dbid, _treename.c_str(), _shot, 1);
        break;
    case Mode::Edit:
        status = _TreeOpenEdit(&_dbid, _treename.c_str(), _shot);
        break;
    case Mode::New:
        status = _TreeOpenNew(&_dbid, _treename.c_str(), _shot);
        break;
    default: ;
    }

    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    // Initialize TreeNode parent class to \\TOP
    _tree = this;
    _nid = 0;

    _open = true;
}

inline void Tree::close()
{
    int status = _TreeClose(&_dbid, _treename.c_str(), _shot);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    // Cleanup TreeNode
    _tree = nullptr;
    _nid = -1;

    _open = false;
}

inline void Tree::write()
{
    int status = _TreeWriteTree(&_dbid, _treename.c_str(), _shot);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

inline std::vector<std::string> Tree::findTagWild(const std::string& wildcard) const
{
    std::vector<std::string> tags;

    int outnid = _nid;
    void * findContext = nullptr;
    while (true)
    {
        char * tag = _TreeFindTagWild(getDBID(), const_cast<char *>(wildcard.c_str()), &outnid, &findContext);
        if (!tag) {
            break;
        }

        tags.push_back(tag);
    }

    TreeFindTagEnd(&findContext);

    return tags;
}

inline void Tree::setDefaultNode(const TreeNode& node) const
{
    int status = _TreeSetDefaultNid(getDBID(), node.getNID());
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

inline TreeNode Tree::getDefaultNode() const
{
    int nid;
    int status = _TreeGetDefaultNid(getDBID(), &nid);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(getTree(), nid);
}

template <typename ResultType /*= Data*/, typename ...ArgTypes>
ResultType Tree::compileData(const std::string& expression, ArgTypes... args) const
{
    DataView argExp(expression);
    std::vector<DataView> argList = { DataView(args)... };

    std::vector<mdsdsc_t *> dscList = { argExp.getDescriptor() };
    for (const auto& arg : argList) {
        dscList.push_back(arg.getDescriptor());
    }

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    int status = TdiIntrinsic(OPC_COMPILE, dscList.size(), dscList.data(), &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out), getTree()).releaseAndConvert<ResultType>();
}

template <typename ResultType /*= Data*/, typename ...ArgTypes>
ResultType Tree::executeData(const std::string& expression, ArgTypes... args) const
{
    DataView argExp(expression);
    std::vector<DataView> argList = { DataView(args)... };

    std::vector<mdsdsc_t *> dscList = { argExp.getDescriptor() };
    for (const auto& arg : argList) {
        dscList.push_back(arg.getDescriptor());
    }

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    int status = _TdiIntrinsic(getContext(), OPC_EXECUTE, dscList.size(), dscList.data(), &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out), getTree()).releaseAndConvert<ResultType>();
}

template <typename ResultType>
inline ResultType Tree::_getDBI(int16_t code) const
{
    ResultType value = {};
    dbi_itm itm_lst[] = {
        { sizeof(value), code, &value, nullptr },
        { 0, DbiEND_OF_LIST, nullptr, nullptr },
    };

    int status = _TreeGetDbi(_tree->getDBID(), itm_lst);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return value;
}

inline std::string Tree::_getStringDBI(int16_t code, int16_t size) const
{
    std::string buffer;
    buffer.resize(size);

    int retlen = 0;
    dbi_itm itemList[] = {
        { size, code, (void *)buffer.data(), &retlen },
        { 0, DbiEND_OF_LIST, nullptr, nullptr },
    };

    int status = _TreeGetDbi(getTree()->getDBID(), itemList);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    buffer.resize(retlen);

    return buffer;
}

inline void Tree::_setDBI(int16_t code, int value) const
{
    int retlen = 0;
    dbi_itm itemList[] = {
        { sizeof(value), code, &value },
        { 0, DbiEND_OF_LIST, nullptr, nullptr },
    };

    int status = _TreeSetDbi(getTree()->getDBID(), itemList);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

} // namespace mdsplus

#endif // MDSPLUS_TREE_INC_HPP