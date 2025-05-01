#ifndef MDSPLUS_TREE_HPP
#define MDSPLUS_TREE_HPP

#include "Data.hpp"
#include "MDSplusException.hpp"

#ifdef __cpp_lib_optional
    #include <optional>
#endif

extern "C" {

    #include <treeshr.h>
    #include <ncidef.h>
    #include <dbidef.h>

    // #include <tdishr.h>

    int _TdiIntrinsic(void **ctx, opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr);
    int TdiIntrinsic(opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr);

} // extern "C"

namespace mdsplus {

enum class Usage : uint8_t
{
    Any = TreeUSAGE_ANY,
    Structure = TreeUSAGE_STRUCTURE,
    Action = TreeUSAGE_ACTION,
    Device = TreeUSAGE_DEVICE,
    Dispatch = TreeUSAGE_DISPATCH,
    Numeric = TreeUSAGE_NUMERIC,
    Signal = TreeUSAGE_SIGNAL,
    Task = TreeUSAGE_TASK,
    Text = TreeUSAGE_TEXT,
    Window = TreeUSAGE_WINDOW,
    Axis = TreeUSAGE_AXIS,
    Subtree = TreeUSAGE_SUBTREE,
    CompoundData = TreeUSAGE_COMPOUND_DATA,

}; // enum class Usage

std::string to_string(const Usage& usage);

struct TreeNodeFlags
{
    uint32_t State : 1;
    uint32_t ParentState : 1;
    uint32_t Essential : 1;
    uint32_t Cached : 1;
    uint32_t Versions : 1;
    uint32_t Segmented : 1;
    uint32_t SetupInformation : 1;
    uint32_t WriteOnce : 1;
    uint32_t Compressible : 1;
    uint32_t DoNotCompress : 1;
    uint32_t CompressOnPut : 1;
    uint32_t NoWriteModel : 1;
    uint32_t NoWriteShot : 1;
    uint32_t PathReference : 1;
    uint32_t NidReference : 1;
    uint32_t IncludeInPulse : 1;
    uint32_t CompressSegments : 1;
    uint32_t : 15;

}; // struct TreeNodeFlags

static_assert(sizeof(TreeNodeFlags) == sizeof(uint32_t));

std::string to_string(const TreeNodeFlags& flags);

class Tree;
class Data;

class TreeNode
{
public:

    TreeNode();

    TreeNode(Tree * tree, int nid);

    virtual ~TreeNode();

    [[nodiscard]]
    inline Tree * getTree() const {
        return _tree;
    }

    [[nodiscard]]
    inline int getNID() const {
        return _nid;
    }

    TreeNode getNode(std::string_view path) const;

// #ifdef __cpp_lib_optional
//     std::optional<TreeNode> tryGetNode(std::string_view path) const; // noexcept
// #endif

    TreeNode addNode(std::string_view path, Usage usage) const;

    TreeNode addDevice(std::string_view path, std::string_view model) const;

    [[nodiscard]]
    inline uint64_t getTimeInserted() const {
        return _getNCI<uint64_t>(NciTIME_INSERTED);
    }

    [[nodiscard]]
    inline uint32_t getOwnerID() const {
        return _getNCI<uint32_t>(NciOWNER_ID);
    }

    [[nodiscard]]
    inline class_t getClass() const {
        return _getNCI<class_t>(NciCLASS);
    }

    [[nodiscard]]
    inline dtype_t getDType() const {
        return _getNCI<dtype_t>(NciDTYPE);
    }

    [[nodiscard]]
    inline uint32_t getLength() const {
        return _getNCI<uint32_t>(NciLENGTH);
    }

    [[nodiscard]]
    inline uint32_t getStatus() const {
        return _getNCI<uint32_t>(NciSTATUS);
    }

    [[nodiscard]]
    inline uint16_t getConglomorateElements() const {
        return _getNCI<uint16_t>(NciCONGLOMERATE_ELT);
    }

    [[nodiscard]]
    inline TreeNodeFlags getFlags() const {
        return _getNCI<TreeNodeFlags>(NciGET_FLAGS);
    }

    inline void setFlagsOn(TreeNodeFlags flags) const {
        
    }

    inline void setFlagsOff(TreeNodeFlags flags) const {
        
    }

    inline void clearFlags(TreeNodeFlags flags) const {
        setFlagsOff(flags);
    }


    [[nodiscard]]
    inline uint32_t getFlagsInt() const {
        return _getNCI<uint32_t>(NciGET_FLAGS);
    }

    [[nodiscard]]
    inline std::string getNodeName() const {
        std::string name = _getStringNCI(NciNODE_NAME, 64);

        // Trim trailing spaces
        size_t it = name.find(' ');
        if (it != std::string::npos) {
            name[it] = '\0';
        }

        return name;
    }

    [[nodiscard]]
    inline std::string getPath() const {
        return _getStringNCI(NciPATH, 1024);
    }

    [[nodiscard]]
    inline uint32_t getDepth() const {
        return _getNCI<uint32_t>(NciDEPTH);
    }

    [[nodiscard]]
    inline int getParentNID() const {
        return _getNCI<uint32_t>(NciPARENT);
    }

    [[nodiscard]]
    inline TreeNode getParent() const {
        int nid = getParentNID();
        if (nid == 0) {
            throw TreeNodeNotFound();
        }
        return TreeNode(_tree, nid);
    }

#ifdef __cpp_lib_optional
    [[nodiscard]]
    inline std::optional<TreeNode> tryGetParent() const {
        int nid = getParentNID();
        if (nid == 0) {
            return std::nullopt;
        }
        return TreeNode(_tree, nid);
    }
#endif

    [[nodiscard]]
    inline int getBrotherNID() const {
        return _getNCI<uint32_t>(NciPARENT);
    }

    [[nodiscard]]
    inline TreeNode getBrother() const {
        int nid = getBrotherNID();
        if (nid == 0) {
            throw TreeNodeNotFound();
        }
        return TreeNode(_tree, nid);
    }

#ifdef __cpp_lib_optional
    [[nodiscard]]
    inline std::optional<TreeNode> tryGetBrother() const {
        int nid = getBrotherNID();
        if (nid == 0) {
            return std::nullopt;
        }
        return TreeNode(_tree, nid);
    }
#endif

    [[nodiscard]]
    inline int getMemberID() const {
        return _getNCI<uint32_t>(NciPARENT);
    }

    [[nodiscard]]
    inline TreeNode getMember() const {
        int nid = getMemberID();
        if (nid == 0) {
            throw TreeNodeNotFound();
        }
        return TreeNode(_tree, nid);
    }

#ifdef __cpp_lib_optional
    [[nodiscard]]
    inline std::optional<TreeNode> tryGetMember() const {
        int nid = getMemberID();
        if (nid == 0) {
            return std::nullopt;
        }
        return TreeNode(_tree, nid);
    }
#endif

    [[nodiscard]]
    inline int getChildNID() const {
        return _getNCI<uint32_t>(NciPARENT);
    }

    [[nodiscard]]
    inline TreeNode getChild() const {
        int nid = getChildNID();
        if (nid == 0) {
            throw TreeNodeNotFound();
        }
        return TreeNode(_tree, nid);
    }

#ifdef __cpp_lib_optional
    [[nodiscard]]
    inline std::optional<TreeNode> tryGetChild() const {
        int nid = getChildNID();
        if (nid == 0) {
            return std::nullopt;
        }
        return TreeNode(_tree, nid);
    }
#endif

    [[nodiscard]]
    inline ncik_t getParentRelationship() const {
        return _getNCI<ncik_t>(NciPARENT_RELATIONSHIP);
    }

    [[nodiscard]]
    inline std::vector<int> getConglomerateElementNIDs() const {
        return _getNIDArrayNCI(NciCONGLOMERATE_NIDS, NciNUMBER_OF_ELTS);
    }

    [[nodiscard]]
    inline std::vector<TreeNode> getConglomerateElements() const {
        return _getTreeNodeArrayNCI(NciCONGLOMERATE_NIDS, NciNUMBER_OF_ELTS);
    }

    [[nodiscard]]
    inline std::string getOriginalPartName() const {
        return _getStringNCI(NciORIGINAL_PART_NAME, 1024);
    }

    [[nodiscard]]
    inline uint32_t getNumberOfMembers() const {
        return _getNCI<uint32_t>(NciNUMBER_OF_MEMBERS);
    }

    [[nodiscard]]
    inline uint32_t getNumberOfChildren() const {
        return _getNCI<uint32_t>(NciNUMBER_OF_CHILDREN);
    }

    [[nodiscard]]
    inline std::vector<int> getMemberNIDs() const {
        return _getNIDArrayNCI(NciMEMBER_NIDS, NciNUMBER_OF_MEMBERS);
    }

    [[nodiscard]]
    inline std::vector<TreeNode> getMembers() const {
        return _getTreeNodeArrayNCI(NciMEMBER_NIDS, NciNUMBER_OF_MEMBERS);
    }

    [[nodiscard]]
    inline std::vector<int> getChildrenNIDs() const {
        return _getNIDArrayNCI(NciCHILDREN_NIDS, NciNUMBER_OF_CHILDREN);
    }

    [[nodiscard]]
    inline std::vector<TreeNode> getChildren() const {
        return _getTreeNodeArrayNCI(NciCHILDREN_NIDS, NciNUMBER_OF_CHILDREN);
    }

    [[nodiscard]]
    std::vector<TreeNode> getMembersAndChildren(bool sortedNIDs = true) const;

    [[nodiscard]]
    inline std::string getFullPath() const {
        return _getStringNCI(NciFULLPATH, 1024);
    }
    
    [[nodiscard]]
    inline std::string getMinPath() const {
        return _getStringNCI(NciMINPATH, 1024);
    }

    [[nodiscard]]
    inline usage_t getUsage() const {
        return _getNCI<usage_t>(NciUSAGE);
    }

    // setUsage
    
    [[nodiscard]]
    inline std::string getParentTreeName() const {
        return _getStringNCI(NciPARENT_TREE, 64);
    }

    [[nodiscard]]
    inline uint32_t getRecordLength() const {
        return _getNCI<uint32_t>(NciRLENGTH);
    }

    [[nodiscard]]
    inline uint32_t getNumberOfElements() const {
        return _getNCI<uint32_t>(NciNUMBER_OF_ELTS);
    }

    [[nodiscard]]
    inline bool hasDataInNCI() const {
        return _getNCI<bool>(NciDATA_IN_NCI);
    }

    [[nodiscard]]
    inline bool hasErrorOnPut() const {
        return _getNCI<bool>(NciERROR_ON_PUT);
    }

    [[nodiscard]]
    inline uint64_t getRFA() const {
        return _getNCI<uint64_t>(NciRFA);
    }

    [[nodiscard]]
    inline uint32_t getIOStatus() const {
        return _getNCI<uint32_t>(NciIO_STATUS);
    }

    [[nodiscard]]
    inline uint32_t getIOSTV() const {
        return _getNCI<uint32_t>(NciIO_STV);
    }
    
    [[nodiscard]]
    inline std::string getDTypeString() const {
        return _getStringNCI(NciDTYPE_STR, 64);
    }
    
    [[nodiscard]]
    inline std::string getUsageString() const {
        return _getStringNCI(NciUSAGE_STR, 64);
    }
    
    [[nodiscard]]
    inline std::string getClassString() const {
        return _getStringNCI(NciCLASS_STR, 64);
    }

    [[nodiscard]]
    inline uint32_t getVersion() const {
        return _getNCI<uint32_t>(NciVERSION);
    }

    [[nodiscard]]
    inline uint8_t getCompressionMethod() const {
        return _getNCI<uint8_t>(NciCOMPRESSION_METHOD);
    }

    // setCompressionMethod?

    [[nodiscard]]
    inline std::string getCompressionMethodString() const {
        return _getStringNCI(NciCOMPRESSION_METHOD_STR, 64);
    }

    // setCompressionMethodString?

    // template <typename T = Data>
    // [[nodiscard]]
    // inline T getExtendedAttribute(std::string_view name) {
    //     _TreeSetXNci(void *dbid, int nid, const char *xnciname, mdsdsc_t *value)
    // }

    // void setExtendedAttribute(std::string_view name, const Data& data);

    [[nodiscard]]
    Data getRecord() const;

    void putRecord(const Data& data) const;

    template <typename DataType = Data>
    [[nodiscard]]
    DataType getData() const;

    template <typename DataType = Data, typename UnitsType = Data>
    [[nodiscard]]
    std::tuple<DataType, UnitsType> getDataWithUnits() const;

    template <typename DataType>
    inline void setData(DataType value) const {
        putRecord(Data::FromScalar(value));
    }

    template <typename DataType, typename UnitsType>
    inline void setDataWithUnits(DataType value, UnitsType units) const {
        putRecord(WithUnits(Data::FromScalar(value), Data::FromScalar(units)));
    }

#ifdef __cpp_lib_span
    template <typename DataType>
    void setArrayData(const std::span<const DataType> values, const std::vector<uint32_t>& dims = {}) const {
        if (dims.empty()) {
            return setArrayData(values.data(), { values.size() });
        }
        return setArrayData(values.data(), dims);
    }

    template <typename DataType, typename UnitsType>
    void setArrayDataWithUnits(const std::span<const DataType> values, UnitsType units, const std::vector<uint32_t>& dims = {}) const {
        if (dims.empty()) {
            return setArrayDataWithUnits(values.data(), units, { values.size() });
        }
        return setArrayDataWithUnits(values.data(), units, dims);
    }
#endif

    template <typename DataType>
    inline void setArrayData(std::vector<DataType>& values, const std::vector<uint32_t>& dims = {}) const {
        if (dims.empty()) {
            return setArrayData(values.data(), { values.size() });
        }
        return setArrayData(values.data(), dims);
    }

    template <typename DataType, typename UnitsType>
    inline void setArrayDataWithUnits(const std::vector<DataType>& values, UnitsType units, const std::vector<uint32_t>& dims = {}) const {
        if (dims.empty()) {
            return setArrayDataWithUnits(values.data(), units, { values.size() });
        }
        return setArrayDataWithUnits(values.data(), units, dims);
    }

    template <typename DataType>
    inline void setArrayData(const DataType * values, const std::vector<uint32_t>& dims) const {
        putRecord(Data::FromArray(values, dims));
    }

    template <typename DataType, typename UnitsType>
    inline void setArrayDataWithUnits(const DataType * values, UnitsType units, const std::vector<uint32_t>& dims) const {
        putRecord(WithUnits(Data::FromArray(values, dims), Data::FromScalar(units)));
    }

    template <typename ValueType>
    void putRow(int segmentLength, ValueType value, int64_t timestamp);

    template <typename ValueArrayType>
    void putSegment(ValueArrayType values, int index = -1);

    template <typename ValueArrayType>
    inline void putTimestampedSegment(std::vector<int64_t> timestamps, ValueArrayType values) {
        putTimestampedSegment(timestamps.data(), values);
    }

    template <typename ValueArrayType>
    void putTimestampedSegment(int64_t * timestamps, ValueArrayType values);

    template <
        typename StartIndexType,
        typename EndIndexType,
        typename DimensionType,
        typename ValueArrayType
    >
    void makeSegment(
        StartIndexType startIndex,
        EndIndexType endIndex,
        DimensionType dimension,
        ValueArrayType values,
        int index = -1,
        int rowsFilled = -1
    ) const;

    template <
        typename StartIndexType,
        typename EndIndexType,
        typename DimensionType,
        typename ValueArrayType
    >
    void makeSegmentResampled(
        StartIndexType startIndex,
        EndIndexType endIndex,
        DimensionType dimension,
        ValueArrayType values,
        const TreeNode& resampleNode,
        int resampleFactor,
        int index = -1,
        int rowsFilled = -1
    ) const;

    // template <typename StartIndexType, typename EndIndexType, typename DimensionType, typename ValueArrayType>
    // void makeSegmentMinMax(
    //     StartIndexType startIndex,
    //     EndIndexType endIndex,
    //     DimensionType dimension,
    //     ValueArrayType values,
    //     const TreeNode& resampleNode,
    //     int resampleFactor,
    //     int index = -1,
    //     int rowsFilled = -1
    // ) const;

    // template <typename TimestampArrayType, typename ValueArrayType>
    // inline void makeTimestampedSegment(
    //     TimestampArrayType timestamps,
    //     ValueArrayType values,
    //     int index = -1,
    //     int rowsFilled = -1
    // ) const;

    // template <typename ValueArrayType>
    // inline void makeTimestampedSegment(
    //     Int64Array timestamps,
    //     ValueArrayType values,
    //     int index = -1,
    //     int rowsFilled = -1
    // ) const;

    template <typename ValueArrayType>
    inline void makeTimestampedSegment(
        std::vector<int64_t> timestamps,
        ValueArrayType values,
        int index = -1,
        int rowsFilled = -1
    ) const
    {
        makeTimestampedSegment(timestamps.data(), values, index, rowsFilled);
    }

    template <typename ValueArrayType>
    void makeTimestampedSegment(
        int64_t * timestamps,
        ValueArrayType values,
        int index = -1,
        int rowsFilled = -1
    ) const;

protected:

    Tree * _tree = nullptr;

    int _nid = -1;

    template <typename ResultType>
    ResultType _getNCI(nci_t code) const;

    std::string _getStringNCI(nci_t code, int16_t size) const;

    std::vector<int> _getNIDArrayNCI(nci_t code, nci_t codeForNumberOf) const;

    std::vector<TreeNode> _getTreeNodeArrayNCI(nci_t code, nci_t codeForNumberOf) const;

}; // class TreeNode

std::string to_string(const TreeNode * node);

inline std::string to_string(const TreeNode& node) {
    return to_string(&node);
}

enum class Mode
{
    Normal,
    ReadOnly,
    Edit,
    New,

    // TODO: Rename
    View,
};

std::string to_string(const Mode& mode);

class Tree : public TreeNode
{
public:

    static Tree GetActive() {
        char name[64];
        int shot;
        int retNameLength;

        DBI_ITM itmList[] = {
            { sizeof(name), DbiNAME, name, &retNameLength, },
            { sizeof(shot), DbiSHOTID, &shot, nullptr, },
            { 0, DbiEND_OF_LIST, nullptr, nullptr, },
        };
        int status = TreeGetDbi(itmList);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Tree(name, shot, TreeDbid());
    }

    inline void setActive() {
        TreeSwitchDbid(getDBID());
    }

    Tree();

    Tree(std::string_view treename, int shot, Mode mode = Mode::Normal);

    Tree(std::string_view treename, int shot, void * dbid);

    virtual ~Tree();

    [[nodiscard]]
    inline void * getDBID() const {
        return _dbid;
    }

    [[nodiscard]]
    inline void ** getContext() const {
        return const_cast<void **>(&_dbid);
    }

    [[nodiscard]]
    inline std::string getTreeName() const {
        return _treename;
    }

    [[nodiscard]]
    inline int getShot() const {
        return _shot;
    }

    [[nodiscard]]
    inline Mode getMode() const {
        return _mode;
    }

    void open(std::string_view treename, int shot, Mode mode = Mode::Normal);

    void open();

    inline void reopen() { open(); }

    void close();

    void write();

    inline void setDefaultNode(std::string_view path) const {
        setDefaultNode(getNode(path));
    }

    void setDefaultNode(const TreeNode& node) const;

    TreeNode getDefaultNode() const;

    template <typename ResultType = Data, typename ...ArgTypes>
    ResultType compileData(std::string_view expression, ArgTypes... args) const;

    template <typename ResultType = Data, typename ...ArgTypes>
    ResultType executeData(std::string_view expression, ArgTypes... args) const;

private:

    void * _dbid = nullptr;

    bool _open = false;

    std::string _treename;

    int _shot;

    Mode _mode = Mode::Normal;

};

std::string to_string(const Tree * tree);

inline std::string to_string(const Tree& tree) {
    return to_string(&tree);
}

template <typename ResultType /*= Data*/, typename ...ArgTypes>
ResultType Tree::compileData(std::string_view expression, ArgTypes... args) const
{
    int status;
    mdsdsc_t dscExp = StringViewToDescriptor(expression);

    std::vector<Argument> argList = { Argument(args)... };
    std::vector<mdsdsc_t *> dscList = { &dscExp };
    for (const auto& arg : argList) {
        dscList.push_back(arg.getDescriptor());
    }

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    status = TdiIntrinsic(OPC_COMPILE, dscList.size(), dscList.data(), &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    for (auto& arg : argList) {
        arg.free();
    }

    return Data(std::move(out), getTree()).releaseAndConvert<ResultType>();
}

template <typename ResultType /*= Data*/, typename ...ArgTypes>
ResultType Tree::executeData(std::string_view expression, ArgTypes... args) const
{
    int status;
    mdsdsc_t dscExp = StringViewToDescriptor(expression);

    std::vector<Argument> argList = { Argument(args)... };
    std::vector<mdsdsc_t *> dscList = { &dscExp };
    for (const auto& arg : argList) {
        dscList.push_back(arg.getDescriptor());
    }

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    status = _TdiIntrinsic(getContext(), OPC_EXECUTE, dscList.size(), dscList.data(), &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    for (auto& arg : argList) {
        arg.free();
    }

    return Data(std::move(out), getTree()).releaseAndConvert<ResultType>();
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

template <typename ValueType>
void TreeNode::putRow(int segmentLength, ValueType value, int64_t timestamp)
{
    Argument argValue(value);

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

    argValue.free();
}

template <typename ValueArrayType>
void TreeNode::putSegment(ValueArrayType values, int index /*= -1*/)
{
    Argument argValues(values);

    int status = _TreePutSegment(
        getTree()->getDBID(),
        getNID(),
        index,
        (mdsdsc_a_t *)argValues.getDescriptor()
    );
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    argValues.free();
}

template <typename ValueArrayType>
void TreeNode::putTimestampedSegment(int64_t * timestamps, ValueArrayType values)
{
    Argument argValues(values);

    int status = _TreePutTimestampedSegment(
        getTree()->getDBID(),
        getNID(),
        timestamps,
        (mdsdsc_a_t *)argValues.getDescriptor()
    );
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    argValues.free();
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
    Argument argStartIndex(startIndex);
    Argument argEndIndex(endIndex);
    Argument argDimension(dimension);
    Argument argValues(values);

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

    argStartIndex.free();
    argEndIndex.free();
    argDimension.free();
    argValues.free();
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
    Argument argStartIndex(startIndex);
    Argument argEndIndex(endIndex);
    Argument argDimension(dimension);
    Argument argValues(values);

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

    argStartIndex.free();
    argEndIndex.free();
    argDimension.free();
    argValues.free();
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

//     argStartIndex.free();
//     argEndIndex.free();
//     argDimension.free();
//     argValues.free();
// }

template <typename ValueArrayType>
void TreeNode::makeTimestampedSegment(
    int64_t * timestamps,
    ValueArrayType values,
    int index /*= -1*/,
    int rowsFilled /*= -1*/
) const
{
    Argument argValues(values);

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

    argValues.free();
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

} // namespace mdsplus

#endif // MDSPLUS_TREE_HPP