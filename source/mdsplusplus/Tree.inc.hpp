#ifndef MDSPLUS_TREE_INC_HPP
#define MDSPLUS_TREE_INC_HPP

#include "Tree.hpp"
#include "TreeNode.hpp"
#include "DataView.hpp"
#include "String.hpp"

namespace mdsplus {

inline std::vector<int> Tree::getShotDB(
    const std::string& treename,
    const std::string& path /*= ""*/,
    int lower /*= INT_MIN */,
    int higher /*= INT_MAX */
) {
    // TODO:
    return {};
}

inline std::string Tree::getFileName(const std::string& subtree /*= {}*/)
{
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    const char * treename = (subtree.empty() ? nullptr : subtree.c_str());
    int status = _TreeFileName(getDBID(), const_cast<char *>(treename), getShot(), &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<String>().getString();
}

inline Mode from_string(const std::string& mode)
{
    if (mode == "normal" || mode == "NORMAL") {
        return Mode::Normal;
    }
    else if (mode == "edit" || mode == "EDIT") {
        return Mode::Edit;
    }
    else if (mode == "new" || mode == "NEW") {
        return Mode::Edit;
    }
    
    return Mode::ReadOnly;
}

inline void Tree::open(const std::string& treename, int shot, Mode mode /*= Mode::Normal*/)
{
    int status;

    if (mode == Mode::View) {
        return;
    }

    if (_dbid != nullptr) {
        close();
    }

    _treename = treename;
    _shot = shot;
    _mode = mode;
    
    std::string env_name;
    const char * old_path = nullptr;
    if (!_path.empty()) {
        std::string lower(_treename);
        for (auto& c : lower) {
            c = ::tolower(c);
        }

        std::string env_name = lower + "_path";

        old_path = ::getenv(env_name.c_str());
        ::setenv(env_name.c_str(), _path.c_str(), true);
    }

    printf("%d\n", _shot);

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

    if (!_path.empty()) {
        if (old_path) {
            ::setenv(env_name.c_str(), old_path, true);
        }
        else {
            ::unsetenv(env_name.c_str());
        }
    }

    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    // Initialize TreeNode parent class to \\TOP
    _tree = this;
    _nid = 0;
}

inline void Tree::close()
{
    int status = _TreeClose(&_dbid, nullptr, 0);
    if (IS_NOT_OK(status) && status != TreeNOT_OPEN) {
        throwException(status);
    }

    // Cleanup TreeNode
    _tree = nullptr;
    _nid = -1;
}

inline void Tree::write()
{
    int status = _TreeWriteTree(&_dbid, nullptr, 0);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

inline void Tree::createPulse(int shot) const
{
    // TODO: copy_only_this ?

    int nid = getNID();
    int status = _TreeCreatePulseFile(getDBID(), shot, 0, &nid);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

// inline std::vector<TreeNode> Tree::findNodeWild(const std::string& wildcard) const
// {
//     std::vector<TreeNode> nodes;

//     int usageMask = 0xFFFF;

//     void * findContext = nullptr;
//     while (true)
//     {
//         int outnid;
//         int status = _TreeFindNodeWild(
//             getDBID(),
//             wildcard.c_str(),
//             &outnid,
//             &findContext,
//             usageMask
//         );
//         if (status == TreeNMN || status == TreeNNF) {
//             break;
//         }
//         else if (IS_NOT_OK(status)) {
//             throwException(status);
//         }

//         nodes.push_back(TreeNode(getTree(), outnid));
//     }

//     _TreeFindNodeEnd(getDBID(), &findContext);

//     return nodes;
// }

inline std::vector<std::string> Tree::findTagWild(const std::string& wildcard) const
{
    std::vector<std::string> tags;

    int outnid;
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
ResultType Tree::compileData(const std::string& expression, const ArgTypes&... args) const
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
ResultType Tree::executeData(const std::string& expression, const ArgTypes&... args) const
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

    int status = _TreeGetDbi(getDBID(), itm_lst);
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

    int status = _TreeGetDbi(getDBID(), itemList);
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

    int status = _TreeSetDbi(getDBID(), itemList);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

} // namespace mdsplus

#endif // MDSPLUS_TREE_INC_HPP