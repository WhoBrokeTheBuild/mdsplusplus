#ifndef MDSPLUS_TREE_NODE_HPP
#define MDSPLUS_TREE_NODE_HPP

#include "Data.hpp"
#include "Exceptions.hpp"

#ifdef __cpp_lib_optional
    #include <optional>
#endif

extern "C" {

    #include <treeshr.h>
    #include <ncidef.h>
    #include <dbidef.h>

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

static_assert(sizeof(TreeNodeFlags) == sizeof(uint32_t),
    "sizeof(TreeNodeFlags) != sizeof(uint32_t)");

std::string to_string(const TreeNodeFlags& flags);

class Tree;
class DataView;

class TreeNode
{
    friend class DataView;

public:

    TreeNode() = default;

    inline TreeNode(Tree * tree, int nid)
        : _tree(tree)
        , _nid(nid)
    { }

    virtual ~TreeNode() = default;

    [[nodiscard]]
    inline Tree * getTree() const {
        return _tree;
    }

    [[nodiscard]]
    void * getDBID() const;

    [[nodiscard]]
    void * getContext() const;

    [[nodiscard]]
    inline int getNID() const {
        return _nid;
    }

    TreeNode getNode(const std::string& path) const;

    TreeNode addNode(const std::string& path, Usage usage) const;

    TreeNode addDevice(const std::string& path, const std::string& model) const;

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
    std::string getNodeName() const;

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

    template <typename ValueType>
    void setExtendedAttribute(const std::string& name, ValueType value) const;

    template <typename ResultType = Data>
    ResultType getExtendedAttribute(const std::string& name);

    virtual std::vector<std::string> getTags() const;

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

    // TODO: Move
    #ifdef __cpp_lib_optional

        [[nodiscard]]
        std::optional<TreeNode> tryGetNode(const std::string& path) const
        {
            int nid = 0;

            int status = _TreeFindNodeRelative(getDBID(), path.data(), _nid, &nid);
            if (status == TreeNNF) {
                return std::nullopt;
            }
            else if (IS_NOT_OK(status)) {
                throwException(status);
            }

            return TreeNode(_tree, nid);
        }

        [[nodiscard]]
        inline std::optional<TreeNode> tryGetParent() const
        {
            int nid = getParentNID();
            if (nid == 0) {
                return std::nullopt;
            }
            return TreeNode(_tree, nid);
        }

        [[nodiscard]]
        inline std::optional<TreeNode> tryGetBrother() const
        {
            int nid = getBrotherNID();
            if (nid == 0) {
                return std::nullopt;
            }
            return TreeNode(_tree, nid);
        }

        [[nodiscard]]
        inline std::optional<TreeNode> tryGetMember() const
        {
            int nid = getMemberID();
            if (nid == 0) {
                return std::nullopt;
            }
            return TreeNode(_tree, nid);
        }

        [[nodiscard]]
        inline std::optional<TreeNode> tryGetChild() const
        {
            int nid = getChildNID();
            if (nid == 0) {
                return std::nullopt;
            }
            return TreeNode(_tree, nid);
        }

        [[nodiscard]]
        std::optional<Data> tryGetData() const
        {
            mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
            int status = _TreeGetRecord(getDBID(), _nid, &xd);
            if (status == TreeNODATA) {
                return std::nullopt;
            }
            else if (IS_NOT_OK(status)) {
                throwException(status);
            }

            return Data(std::move(xd));
        }

    #endif

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

} // namespace mdsplus

#endif // MDSPLUS_TREE_NODE_HPP