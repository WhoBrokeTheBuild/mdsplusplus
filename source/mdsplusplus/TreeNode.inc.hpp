#ifndef MDSPLUS_TREE_NODE_INC_HPP
#define MDSPLUS_TREE_NODE_INC_HPP

#include "TreeNode.hpp"
#include "Tree.hpp"
#include "DataView.hpp"

#include <algorithm>

namespace mdsplus {

inline std::string to_string(const Usage& usage)
{
    switch (usage) {
    case Usage::Any: return "Usage::Any";
    case Usage::Structure: return "Usage::Structure";
    case Usage::Action: return "Usage::Action";
    case Usage::Device: return "Usage::Device";
    case Usage::Dispatch: return "Usage::Dispatch";
    case Usage::Numeric: return "Usage::Numeric";
    case Usage::Signal: return "Usage::Signal";
    case Usage::Task: return "Usage::Task";
    case Usage::Text: return "Usage::Text";
    case Usage::Window: return "Usage::Window";
    case Usage::Axis: return "Usage::Axis";
    case Usage::Subtree: return "Usage::Subtree";
    case Usage::CompoundData: return "Usage::CompoundData";
    }

    return "?";
}

inline std::string to_string(const TreeNodeFlags& flags)
{
    std::string str;

    if (flags.CompressSegments) {
        str += "CompressSegments | ";
    }

    if (flags.IncludeInPulse) {
        str += "IncludeInPulse | ";
    }

    if (flags.NidReference) {
        str += "NidReference | ";
    }

    if (flags.PathReference) {
        str += "PathReference | ";
    }

    if (flags.NoWriteShot) {
        str += "NoWriteShot | ";
    }

    if (flags.NoWriteModel) {
        str += "NoWriteModel | ";
    }

    if (flags.CompressOnPut) {
        str += "CompressOnPut | ";
    }

    if (flags.DoNotCompress) {
        str += "DoNotCompress | ";
    }

    if (flags.Compressible) {
        str += "Compressible | ";
    }

    if (flags.WriteOnce) {
        str += "WriteOnce | ";
    }

    if (flags.SetupInformation) {
        str += "SetupInformation | ";
    }

    if (flags.Segmented) {
        str += "Segmented | ";
    }

    if (flags.Versions) {
        str += "Versions | ";
    }

    if (flags.Cached) {
        str += "Cached | ";
    }

    if (flags.Essential) {
        str += "Essential | ";
    }

    if (flags.ParentState) {
        str += "ParentState | ";
    }

    if (flags.State) {
        str += "State | ";
    }

    // Strip trailing " | "
    if (!str.empty()) {
        str.resize(str.size() - 3);
    }

    return str;
}

inline std::string to_string(const TreeNode * node)
{
    assert(node);

    std::string str = "TreeNode(";
    str += to_string(node->getTree());
    str += ", ";
    str += std::to_string(node->getNID());
    str += ")";

    return str;

    // return std::format(
    //     "TreeNode({}, {})",
    //     to_string(node->getTree()),
    //     node->getNID()
    // );
}

inline std::string to_string(const Mode& mode)
{
    switch (mode) {
    case Mode::Normal: return "Mode::Normal";
    case Mode::ReadOnly: return "Mode::ReadOnly";
    case Mode::Edit: return "Mode::Edit";
    case Mode::New: return "Mode::New";

    case Mode::View: return "Mode::View";
    }

    return "?";
}

inline std::string to_string(const Tree * tree)
{
    assert(tree);

    std::string str = "Tree(\"";
    str += tree->getTreeName();
    str += "\", ";
    str += std::to_string(tree->getShot());
    str += ", ";
    str += to_string(tree->getMode());
    str += ")";

    return str;

    // return std::format(
    //     "Tree(\"{}\", {}, {})",
    //     tree->getTreeName(),
    //     tree->getShot(),
    //     to_string(tree->getMode())
    // );
}

[[nodiscard]]
inline void * TreeNode::getDBID() const {
    return getTree()->getDBID();
}

[[nodiscard]]
inline void * TreeNode::getContext() const {
    return getTree()->getContext();
}

inline TreeNode TreeNode::getNode(const std::string& path) const
{
    int nid = 0;

    int status = _TreeFindNodeRelative(getTree()->getDBID(), path.data(), _nid, &nid);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(_tree, nid);
}

inline TreeNode TreeNode::addNode(const std::string& path, Usage usage) const
{
    int nid = 0;

    int status = _TreeAddNode(getTree()->getDBID(), path.data(), &nid, static_cast<unsigned char>(usage));
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(_tree, nid);
}

inline TreeNode TreeNode::addDevice(const std::string& path, const std::string& model) const
{
    int nid = 0;

    int status = _TreeAddConglom(getTree()->getDBID(), path.data(), model.data(), &nid);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(_tree, nid);
}

inline std::string TreeNode::getNodeName() const
{
    std::string name = _getStringNCI(NciNODE_NAME, 64);

    // Trim trailing spaces
    size_t it = name.find(' ');
    if (it != std::string::npos) {
        name[it] = '\0';
    }

    return name;
}

template <typename ValueType>
inline void TreeNode::setExtendedAttribute(const std::string& name, ValueType value) const
{
    DataView argValue(value);
    int status = _TreeSetXNci(getDBID(), _nid, name.c_str(), argValue.getDescriptor());
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

template <typename ResultType /*= Data*/>
ResultType TreeNode::getExtendedAttribute(const std::string& name)
{
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    int status = _TreeGetXNci(getDBID(), _nid, name.c_str(), &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out), getTree()).releaseAndConvert<ResultType>();
}

inline std::vector<std::string> TreeNode::getTags() const
{
    std::vector<std::string> tags;

    int outnid = _nid;
    void * findContext = nullptr;
    while (true)
    {
        char * tag = _TreeFindNodeTags(getDBID(), _nid, &findContext);
        if (!tag) {
            break;
        }

        tags.push_back("\\" + std::string(tag));
    }

    TreeFindTagEnd(&findContext);

    return tags;
}

inline Data TreeNode::getRecord() const
{
    mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
    int status = _TreeGetRecord(getTree()->getDBID(), _nid, &xd);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
    void putSegment(const Data& data, int index = -1);

    return Data(std::move(xd), getTree());
}

inline void TreeNode::putRecord(const Data& data) const
{
    mdsdsc_t * dsc = data.getDescriptor();
    int status = _TreePutRecord(getTree()->getDBID(), _nid, dsc, 0);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

inline std::vector<TreeNode> TreeNode::getMembersAndChildren(bool sortedNIDs /*= true*/) const
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

template <typename ResultType>
inline ResultType TreeNode::_getNCI(nci_t code) const
{
    ResultType value = {};
    nci_itm itm_lst[] = {
        { sizeof(value), code, &value, nullptr },
        { 0, NciEND_OF_LIST, nullptr, nullptr },
    };

    int status = _TreeGetNci(_tree->getDBID(), _nid, itm_lst);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return value;
}

inline std::string TreeNode::_getStringNCI(nci_t code, int16_t size) const
{
    std::string buffer;
    buffer.resize(size);

    int retlen = 0;
    nci_itm itemList[] = {
        { size, code, (void *)buffer.data(), &retlen },
        { 0, NciEND_OF_LIST, nullptr, nullptr },
    };

    int status = _TreeGetNci(getTree()->getDBID(), _nid, itemList);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    buffer.resize(retlen);

    return buffer;
}

inline std::vector<int> TreeNode::_getNIDArrayNCI(nci_t code, nci_t codeForNumberOf) const
{
    uint32_t numberOfNodes = _getNCI<uint32_t>(codeForNumberOf);
    std::vector<int> nids(numberOfNodes);

    int16_t sizeInBytes = sizeof(int) * numberOfNodes;
    nci_itm itemList[] = {
        { sizeInBytes, code, (void *)nids.data(), nullptr },
        { 0, NciEND_OF_LIST, nullptr, nullptr },
    };

    int status = _TreeGetNci(getTree()->getDBID(), _nid, itemList);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return nids;
}

inline std::vector<TreeNode> TreeNode::_getTreeNodeArrayNCI(nci_t code, nci_t codeForNumberOf) const
{
    const auto& nids = _getNIDArrayNCI(code, codeForNumberOf);

    std::vector<TreeNode> nodes;
    nodes.reserve(nids.size());

    for (const int& nid : nids) {
        nodes.emplace_back(_tree, nid);
    }

    return nodes;
}

template <typename ValueType>
void TreeNode::putRow(int segmentLength, ValueType value, int64_t timestamp)
{
    DataView argValue(value);

    int status = _TreePutRow(
        getTree()->getDBID(),
        getNID(),
        segmentLength,
        &timestamp,
        (mdsdsc_a_t *)argValue.getDescriptor()
    );
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

template <typename ValueArrayType>
void TreeNode::putSegment(ValueArrayType values, int index /*= -1*/)
{
    DataView argValues(values);

    int status = _TreePutSegment(
        getTree()->getDBID(),
        getNID(),
        index,
        (mdsdsc_a_t *)argValues.getDescriptor()
    );
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

template <typename ValueArrayType>
void TreeNode::putTimestampedSegment(int64_t * timestamps, ValueArrayType values)
{
    DataView argValues(values);

    int status = _TreePutTimestampedSegment(
        getTree()->getDBID(),
        getNID(),
        timestamps,
        (mdsdsc_a_t *)argValues.getDescriptor()
    );
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

template <typename DataType /*= Data*/>
[[nodiscard]]
inline DataType TreeNode::getData() const {
    return getRecord().releaseAndConvert<DataType>();
}

template <typename DataType /*= Data*/, typename UnitsType /*= Data*/>
[[nodiscard]]
inline std::tuple<DataType, UnitsType> TreeNode::getDataWithUnits() const {
    auto data = getRecord();
    auto units = data.getUnits<UnitsType>();
    return { data.releaseAndConvert<DataType>(), std::move(units) };
}

inline int _getMaxRowsFilled(mdsdsc_a_t * dsc)
{
    // TODO: Investigate, taken from TreeNode:makeSegment() in tree.py
    if (dsc->class_ == CLASS_R) {
        return 1;
    }
    else if (dsc->class_ == CLASS_A) {
        // TODO: Verify
        if (dsc->aflags.coeff) {
            array_coeff * dscCoeff = (array_coeff *)dsc;
            return dscCoeff->m[dscCoeff->dimct - 1];
        }
        else {
            return (dsc->arsize / dsc->length);
        }
    }

    return -1;
}

template <
    typename StartIndexType,
    typename EndIndexType,
    typename DimensionType,
    typename ValueArrayType
>
void TreeNode::makeSegment(
    StartIndexType startIndex,
    EndIndexType endIndex,
    DimensionType dimension,
    ValueArrayType values,
    int index /*= -1*/,
    int rowsFilled /*= -1*/
) const
{
    DataView argStartIndex(startIndex);
    DataView argEndIndex(endIndex);
    DataView argDimension(dimension);
    DataView argValues(values);

    mdsdsc_a_t * dscValues = (mdsdsc_a_t *)argValues.getDescriptor();
    if (rowsFilled < 0) {
        rowsFilled = _getMaxRowsFilled(dscValues);
    }

    int status = _TreeMakeSegment(
        getTree()->getDBID(),
        getNID(),
        argStartIndex.getDescriptor(),
        argEndIndex.getDescriptor(),
        argDimension.getDescriptor(),
        dscValues,
        index,
        rowsFilled
    );
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

template <
    typename StartIndexType,
    typename EndIndexType,
    typename DimensionType,
    typename ValueArrayType
>
void TreeNode::makeSegmentResampled(
    StartIndexType startIndex,
    EndIndexType endIndex,
    DimensionType dimension,
    ValueArrayType values,
    const TreeNode& resampleNode,
    int resampleFactor,
    int index /*= -1*/,
    int rowsFilled /*= -1*/
) const
{
    DataView argStartIndex(startIndex);
    DataView argEndIndex(endIndex);
    DataView argDimension(dimension);
    DataView argValues(values);

    mdsdsc_a_t * dscValues = (mdsdsc_a_t *)argValues.getDescriptor();
    if (rowsFilled < 0) {
        rowsFilled = _getMaxRowsFilled(dscValues);
    }

    int status = _TreeMakeSegmentResampled(
        getTree()->getDBID(),
        getNID(),
        argStartIndex.getDescriptor(),
        argEndIndex.getDescriptor(),
        argDimension.getDescriptor(),
        dscValues,
        index,
        rowsFilled,
        resampleNode.getNID(),
        resampleFactor
    );
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

// template <
//     typename StartIndexType,
//     typename EndIndexType,
//     typename DimensionType,
//     typename ValueArrayType
// >
// void TreeNode::makeSegmentMinMax(
//     StartIndexType startIndex,
//     EndIndexType endIndex,
//     DimensionType dimension,
//     ValueArrayType values,
//     const TreeNode& resampleNode,
//     int resampleFactor,
//     int index /*= -1*/,
//     int rowsFilled /*= -1*/
// ) const
// {
//     Argument argStartIndex(startIndex);
//     Argument argEndIndex(endIndex);
//     Argument argDimension(dimension);
//     Argument argValues(values);

//     mdsdsc_a_t * dscValues = (mdsdsc_a_t *)argValues.getDescriptor();
//     if (rowsFilled < 0) {
//         rowsFilled = _getMaxRowsFilled(dscValues);
//     }

//     int status = _TreeMakeSegmentMinMax(
//         getTree()->getDBID(),
//         getNID(),
//         argStartIndex.getDescriptor(),
//         argEndIndex.getDescriptor(),
//         argDimension.getDescriptor(),
//         dscValues,
//         index,
//         rowsFilled,
//         resampleNode.getNID(),
//         resampleFactor
//     );
//     if (IS_NOT_OK(status)) {
//         throwException(status);
//     }
// }

template <typename ValueArrayType>
void TreeNode::makeTimestampedSegment(
    int64_t * timestamps,
    ValueArrayType values,
    int index /*= -1*/,
    int rowsFilled /*= -1*/
) const
{
    DataView argValues(values);

    mdsdsc_a_t * dscValues = (mdsdsc_a_t *)argValues.getDescriptor();
    if (rowsFilled < 0) {
        rowsFilled = _getMaxRowsFilled(dscValues);
    }

    int status = _TreeMakeTimestampedSegment(
        getTree()->getDBID(),
        getNID(),
        timestamps,
        dscValues,
        index,
        rowsFilled
    );
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

} // namespace mdsplus

#endif // MDSPLUS_TREE_NODE_INC_HPP