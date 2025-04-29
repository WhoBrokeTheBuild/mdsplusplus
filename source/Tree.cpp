#include <mdsplus.hpp>

#include <algorithm>

#include <treeshr.h>

namespace mdsplus {

TreeNode::TreeNode()
{ }

TreeNode::TreeNode(Tree * tree, int nid)
    : _tree(tree)
    , _nid(nid)
{ }

TreeNode::~TreeNode()
{ }

TreeNode TreeNode::getNode(std::string_view path) const
{
    int status;
    int nid = 0;

    status = _TreeFindNodeRelative(getTree()->getDBID(), path.data(), _nid, &nid);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(_tree, nid);
}

#ifdef __cpp_lib_optional
std::optional<TreeNode> TreeNode::tryGetNode(std::string_view path) const
{
    int status;
    int nid = 0;

    status = _TreeFindNodeRelative(getTree()->getDBID(), path.data(), _nid, &nid);
    if (status == TreeNNF) {
        return std::nullopt;
    }
    else if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(_tree, nid);
}
#endif

TreeNode TreeNode::addNode(std::string_view path, Usage usage) const
{
    int status;
    int nid = 0;

    status = _TreeAddNode(getTree()->getDBID(), path.data(), &nid, static_cast<unsigned char>(usage));
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(_tree, nid);
}

TreeNode TreeNode::addDevice(std::string_view path, std::string_view model) const
{
    int status;
    int nid = 0;

    status = _TreeAddConglom(getTree()->getDBID(), path.data(), model.data(), &nid);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(_tree, nid);
}

// std::optional<Data> TreeNode::tryGetData() const
// {
//     int status;

//     mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
//     status = _TreeGetRecord(getTree()->getDBID(), _nid, &xd);
//     if (status == TreeNODATA) {
//         return std::nullopt;
//     }
//     else if (IS_NOT_OK(status)) {
//         throwException(status);
//     }

//     return Data(std::move(xd));
// }

Data TreeNode::getRecord() const
{
    int status;

    mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
    status = _TreeGetRecord(getTree()->getDBID(), _nid, &xd);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
    void putSegment(const Data& data, int index = -1);

    return Data(std::move(xd), getTree());
}

void TreeNode::putRecord(const Data& data) const
{
    int status;

    mdsdsc_t * dsc = data.getDescriptor();
    status = _TreePutRecord(getTree()->getDBID(), _nid, dsc, 0);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

std::vector<TreeNode> TreeNode::getMembersAndChildren(bool sortedNIDs /*= true*/) const
{
    int status;

    uint32_t numberOfMembers = 0;
    uint32_t numberOfChildren = 0;
    nci_itm countItemList[] = {
        { sizeof(numberOfMembers), NciNUMBER_OF_MEMBERS, &numberOfMembers, nullptr },
        { sizeof(numberOfChildren), NciNUMBER_OF_CHILDREN, &numberOfChildren, nullptr },
        { 0, NciEND_OF_LIST, nullptr, nullptr },
    };

    status = _TreeGetNci(getTree()->getDBID(), _nid, countItemList);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    std::vector<int> nids(numberOfMembers + numberOfChildren);

    int16_t sizeInBytesMembers = sizeof(int) * numberOfMembers;
    int16_t sizeInBytesChildren = sizeof(int) * numberOfChildren;
    nci_itm itemList[] = {
        { sizeInBytesMembers, NciMEMBER_NIDS, (void *)&nids[0], nullptr },
        { sizeInBytesChildren, NciCHILDREN_NIDS, (void *)&nids[numberOfMembers], nullptr },
        { 0, NciEND_OF_LIST, nullptr, nullptr },
    };

    status = _TreeGetNci(getTree()->getDBID(), _nid, itemList);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    if (sortedNIDs) {
        std::sort(nids.begin(), nids.end());
    }

    std::vector<TreeNode> nodes;
    nodes.reserve(nids.size());

    for (const int& nid : nids) {
        nodes.emplace_back(_tree, nid);
    }

    return nodes;
}

std::string TreeNode::_getStringNCI(nci_t code, int16_t size) const
{
    int status;

    std::string buffer;
    buffer.resize(size);

    int retlen = 0;
    nci_itm itemList[] = {
        { size, code, (void *)buffer.data(), &retlen },
        { 0, NciEND_OF_LIST, nullptr, nullptr },
    };

    status = _TreeGetNci(getTree()->getDBID(), _nid, itemList);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    buffer.shrink_to_fit();

    return buffer;
}

std::vector<int> TreeNode::_getNIDArrayNCI(nci_t code, nci_t codeForNumberOf) const
{
    int status;

    uint32_t numberOfNodes = _getNCI<uint32_t>(codeForNumberOf);
    std::vector<int> nids(numberOfNodes);

    int16_t sizeInBytes = sizeof(int) * numberOfNodes;
    nci_itm itemList[] = {
        { sizeInBytes, code, (void *)nids.data(), nullptr },
        { 0, NciEND_OF_LIST, nullptr, nullptr },
    };

    status = _TreeGetNci(getTree()->getDBID(), _nid, itemList);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return nids;
}

std::vector<TreeNode> TreeNode::_getTreeNodeArrayNCI(nci_t code, nci_t codeForNumberOf) const
{
    const auto& nids = _getNIDArrayNCI(code, codeForNumberOf);

    std::vector<TreeNode> nodes;
    nodes.reserve(nids.size());

    for (const int& nid : nids) {
        nodes.emplace_back(_tree, nid);
    }

    return nodes;
}

Tree::Tree()
{ }

Tree::Tree(std::string_view treename, int shot, Mode mode /*= Mode::Normal*/)
    : _treename(treename)
    , _shot(shot)
    , _mode(mode)
{
    open();
}

Tree::Tree(std::string_view treename, int shot, void * dbid)
    : _treename(treename)
    , _shot(shot)
    , _mode(Mode::View)
    , _dbid(dbid)
{ }

Tree::~Tree()
{
    if (_mode != Mode::View) {
        TreeFreeDbid(_dbid);
        _dbid = nullptr;
    }
}

void Tree::open(std::string_view treename, int shot, Mode mode /*= Mode::Normal*/)
{
    _treename = treename;
    _shot = shot;
    _mode = mode;

    return open();
}

void Tree::open()
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

void Tree::close()
{
    int status;
    
    status = _TreeClose(&_dbid, _treename.c_str(), _shot);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    // Cleanup TreeNode
    _tree = nullptr;
    _nid = -1;

    _open = false;
}

void Tree::write()
{
    int status;

    status = _TreeWriteTree(&_dbid, _treename.c_str(), _shot);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

void Tree::setDefaultNode(const TreeNode& node) const
{
    int status = _TreeSetDefaultNid(getDBID(), node.getNID());
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

TreeNode Tree::getDefaultNode() const
{
    int nid;
    int status = _TreeGetDefaultNid(getDBID(), &nid);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(getTree(), nid);
}

} // namespace mdsplus