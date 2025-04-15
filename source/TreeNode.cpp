#include <algorithm>
#include <mdsplus.hpp>

#include <tdishr.h>
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

    status = _TreeFindNodeRelative(_tree->getDBID(), path.data(), _nid, &nid);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(_tree, nid);
}

std::optional<TreeNode> TreeNode::tryGetNode(std::string_view path) const
{
    int status;
    int nid = 0;

    status = _TreeFindNodeRelative(_tree->getDBID(), path.data(), _nid, &nid);
    if (status == TreeNNF) {
        return std::nullopt;
    }
    else if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(_tree, nid);
}

TreeNode TreeNode::addNode(std::string_view path, Usage usage) const
{
    int status;
    int nid = 0;

    status = _TreeAddNode(_tree->getDBID(), path.data(), &nid, static_cast<unsigned char>(usage));
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(_tree, nid);
}

TreeNode TreeNode::addDevice(std::string_view path, std::string_view model) const
{
    int status;
    int nid = 0;

    status = _TreeAddConglom(_tree->getDBID(), path.data(), model.data(), &nid);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(_tree, nid);
}

// std::optional<Data> TreeNode::tryGetData() const
// {
//     int status;

//     mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
//     status = _TreeGetRecord(_tree->getDBID(), _nid, &xd);
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
    status = _TreeGetRecord(_tree->getDBID(), _nid, &xd);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(xd));
}

void TreeNode::putRecord(const Data& data) const
{
    int status;

    mdsdsc_t * dsc = data.getDescriptor();
    status = _TreePutRecord(_tree->getDBID(), _nid, dsc, 0);
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

    status = _TreeGetNci(_tree->getDBID(), _nid, countItemList);
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

    status = _TreeGetNci(_tree->getDBID(), _nid, itemList);
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

    status = _TreeGetNci(_tree->getDBID(), _nid, itemList);
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

    status = _TreeGetNci(_tree->getDBID(), _nid, itemList);
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

} // namespace mdsplus