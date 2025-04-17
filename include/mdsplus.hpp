#ifndef MDSPLUS_HPP
#define MDSPLUS_HPP

#include <cstring>
#include <optional>
// #include <mdspan>
#include <string>
#include <vector>
#include <cassert>
#include <numeric>

#if __has_include(<span>)
#include <span>
#else
#include <gsl/span>
namespace std {
    using gsl::span;
}
#endif

#include <MDSplusException.hpp>

extern "C" {

#include <mdsshr.h>
#include <mdsdescrip.h>
#include <ncidef.h>
#include <treeshr.h>
#include <usagedef.h>

extern int TdiConvert(mdsdsc_a_t * dsc, mdsdsc_a_t * convert);
extern int TdiIntrinsic(opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr);
extern int _TdiIntrinsic(void **ctx, opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr);
extern int TdiCompile(mdsdsc_t * first, ...);
extern int TdiExecute(mdsdsc_t * first, ...);

int TdiCall(dtype_t rtype, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr);

// #include <tdishr.h>

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

enum class Class : uint8_t
{
    Missing = CLASS_MISSING,
    S = CLASS_S,
    D = CLASS_D,
    A = CLASS_A,
    XD = CLASS_XD,
    XS = CLASS_XS,
    R = CLASS_R,
    CA = CLASS_CA,
    APD = CLASS_APD,

}; // enum class Class

std::string to_string(const Class& class_);

enum class DType : uint8_t
{
    Missing = DTYPE_MISSING,

    BU = DTYPE_BU,
    WU = DTYPE_WU,
    LU = DTYPE_LU,
    QU = DTYPE_QU,
    B = DTYPE_B,
    W = DTYPE_W,
    L = DTYPE_L,
    Q = DTYPE_Q,
    F = DTYPE_F,
    D = DTYPE_D,
    FC = DTYPE_FC,
    DC = DTYPE_DC,
    T = DTYPE_T,
    DSC = DTYPE_DSC,
    OU = DTYPE_OU,
    O = DTYPE_O,
    G = DTYPE_G,
    H = DTYPE_H,
    GC = DTYPE_GC,
    HC = DTYPE_HC,
    FS = DTYPE_FS,
    FT = DTYPE_FT,
    FSC = DTYPE_FSC,
    FTC = DTYPE_FTC,
    C = DTYPE_C,

    Float = DTYPE_FLOAT,
    Double = DTYPE_DOUBLE,

    Pointer = DTYPE_POINTER,
    Ident = DTYPE_IDENT,
    NID = DTYPE_NID,
    Path = DTYPE_PATH,
    Param = DTYPE_PARAM,
    Signal = DTYPE_SIGNAL,
    Dimension = DTYPE_DIMENSION,
    Window = DTYPE_WINDOW,
    Slope = DTYPE_SLOPE,
    Function = DTYPE_FUNCTION,
    Conglom = DTYPE_CONGLOM,
    Range = DTYPE_RANGE,
    Action = DTYPE_ACTION,
    Dispatch = DTYPE_DISPATCH,
    Program = DTYPE_PROGRAM,
    Routine = DTYPE_ROUTINE,
    Procedure = DTYPE_PROCEDURE,
    Method = DTYPE_METHOD,
    Dependency = DTYPE_DEPENDENCY,
    Condition = DTYPE_CONDITION,
    Event = DTYPE_EVENT,
    WithUnits = DTYPE_WITH_UNITS,
    Call = DTYPE_CALL,
    WithError = DTYPE_WITH_ERROR,
    List = DTYPE_LIST,
    Tuple = DTYPE_TUPLE,
    Dictionary = DTYPE_DICTIONARY,
    Opaque = DTYPE_OPAQUE,

}; // enum class DType

std::string to_string(const DType& dtype);

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

    std::optional<TreeNode> tryGetNode(std::string_view path) const; // noexcept

    TreeNode addNode(std::string_view path, Usage usage) const;

    TreeNode addDevice(std::string_view path, std::string_view model) const;

    // SET_FLAGS
    // CLEAR_FLAGS

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
    inline std::optional<TreeNode> getParent() const {
        int nid = getParentNID();
        if (nid == 0) {
            return std::nullopt;
        }
        return TreeNode(_tree, nid);
    }

    [[nodiscard]]
    inline int getBrotherNID() const {
        return _getNCI<uint32_t>(NciPARENT);
    }

    [[nodiscard]]
    inline std::optional<TreeNode> getBrother() const {
        int nid = getBrotherNID();
        if (nid == 0) {
            return std::nullopt;
        }
        return TreeNode(_tree, nid);
    }

    [[nodiscard]]
    inline int getMemberID() const {
        return _getNCI<uint32_t>(NciPARENT);
    }

    [[nodiscard]]
    inline std::optional<TreeNode> getMember() const {
        int nid = getMemberID();
        if (nid == 0) {
            return std::nullopt;
        }
        return TreeNode(_tree, nid);
    }

    [[nodiscard]]
    inline int getChildNID() const {
        return _getNCI<uint32_t>(NciPARENT);
    }

    [[nodiscard]]
    inline std::optional<TreeNode> getChild() const {
        int nid = getChildNID();
        if (nid == 0) {
            return std::nullopt;
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

    [[nodiscard]]
    inline std::string getCompressionMethodString() const {
        return _getStringNCI(NciCOMPRESSION_METHOD_STR, 64);
    }

    [[nodiscard]]
    Data getRecord() const;

    void putRecord(const Data& data) const;

    template <typename T = Data>
    [[nodiscard]]
    T getData() const;

    template <typename T = Data, typename U = Data>
    [[nodiscard]]
    std::tuple<T, U> getDataWithUnits() const;

    template <typename T>
    void setData(T value) const;

    template <typename T, typename U>
    void setDataWithUnits(T value, U units) const;

    template <typename T>
    void setArrayData(const std::span<const T> values, const std::vector<uint32_t>& dims = {}) const;

    template <typename T, typename U>
    void setArrayDataWithUnits(const std::span<const T> values, U units, const std::vector<uint32_t>& dims = {}) const;

    template <typename T>
    inline void setArrayData(std::vector<T>& values, const std::vector<uint32_t>& dims = {}) const {
        return setArrayData(std::span<T>(values.begin(), values.end()), dims);
    }

    template <typename T, typename U>
    inline void setArrayDataWithUnits(const std::vector<T>& values, U units, const std::vector<uint32_t>& dims = {}) const {
        return setArrayDataWithUnits(std::span<const T>(values.begin(), values.end()), units, dims);
    }

    template <typename T>
    inline void setArrayData(const T * values, const std::vector<uint32_t>& dims) const {
        assert(!dims.empty());

        size_t size = std::accumulate(dims.begin(), dims.end(), 1.0, std::multiplies());
        return setArrayData(std::span(values, values + size), dims);
    }

    template <typename T, typename U>
    inline void setArrayDataWithUnits(const T * values, U units, const std::vector<uint32_t>& dims) const {
        assert(!dims.empty());

        size_t size = std::accumulate(dims.begin(), dims.end(), 1.0, std::multiplies());
        return setArrayDataWithUnits(std::span(values, values + size), units, dims);
    }

protected:

    Tree * _tree = nullptr;

    int _nid = -1;

    template <typename T>
    T _getNCI(nci_t code) const;

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
};

std::string to_string(const Mode& mode);

class Tree : public TreeNode
{
public:

    Tree();

    Tree(std::string_view treename, int shot, Mode mode = Mode::Normal);

    virtual ~Tree();

    [[nodiscard]]
    inline void * getDBID() const {
        return _dbid;
    }

    [[nodiscard]]
    inline void ** getContext() {
        return &_dbid;
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

class Data
{
public:

    static constexpr Class __class = Class::Missing;
    static constexpr DType __dtype = DType::Missing;

    template <typename ...Args>
    static Data Compile(std::string_view expression, Args... args);

    template <typename ...Args>
    static Data Execute(std::string_view expression, Args... args);

    template <typename T>
    static Data FromScalar(T value);

    template <typename T>
    static Data FromArray(const std::span<T> values, const std::vector<uint32_t>& dims = {});

    template <typename T>
    static Data FromArray(const std::vector<T>& values, const std::vector<uint32_t>& dims = {});

    template <typename ...Args>
    static inline Data FromExpression(std::string_view expression, Args... args) {
        return Execute(expression, args...);
    }

    inline Data() = default;

    inline Data(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : _xd(xd)
        , _tree(tree)
    {
        xd = MDSDSC_XD_INITIALIZER;
    }

    inline virtual ~Data() {
        MdsFree1Dx(&_xd, nullptr);
    }

    // Disallow copy and assign
    Data(const Data&) = delete;
    Data& operator=(Data&) = delete;

    // Enable move operators
    inline Data(Data&& other) {
        _xd = other._xd;
        other._xd = MDSDSC_XD_INITIALIZER;
        _tree = other._tree;
        other._tree = nullptr;
    }

    Data& operator=(Data&& other) {
        _xd = other._xd;
        other._xd = MDSDSC_XD_INITIALIZER;
        _tree = other._tree;
        other._tree = nullptr;
        return *this;
    }

    template <typename T, std::enable_if_t<std::is_base_of_v<Data, T>, bool> = true>
    inline bool operator==(const T& other) const {
        int status;

        bool equal = false;
        mdsdsc_s_t out = {
            .length = 1,
            .dtype = DTYPE_BU,
            .class_ = CLASS_S,
            .pointer = (char *)&equal
        };

        // TODO: _tree != other._tree?

        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        if (hasTree()) {
            status = _TdiIntrinsic(getTree()->getContext(), OPC_EQ, 2, args, (mdsdsc_xd_t *)&out);
        }
        else {
            status = TdiIntrinsic(OPC_EQ, 2, args, (mdsdsc_xd_t *)&out);
        }
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return equal;
    }

    template <typename T, std::enable_if_t<std::is_base_of_v<Data, T>, bool> = true>
    inline bool operator!=(const T& other) const {
        return !(*this == other);
    }

    inline void setTree(Tree * tree) {
        _tree = tree;
    }

    inline Tree * getTree() const {
        return _tree;
    }

    inline bool hasTree() const {
        return (_tree != nullptr);
    }

    [[nodiscard]]
    inline mdsdsc_xd_t release() {
        mdsdsc_xd_t tmp = std::move(_xd);
        _xd = MDSDSC_XD_INITIALIZER;
        return tmp;
    }

    [[nodiscard]]
    inline Data clone() const {
        return _clone<Data>();
    }

    template <typename T>
    [[nodiscard]]
    T convert();

    [[nodiscard]]
    inline mdsdsc_t * getDescriptor() const {
        return _xd.pointer;
    }

    [[nodiscard]]
    inline length_t getLength() const {
        mdsdsc_t * dsc = getDescriptor();
        return (dsc ? dsc->length : 0);
    }

    [[nodiscard]]
    inline Class getClass() const {
        mdsdsc_t * dsc = getDescriptor();
        return (dsc ? Class(dsc->class_) : Class::Missing);
    }

    [[nodiscard]]
    inline DType getDType() const {
        mdsdsc_t * dsc = getDescriptor();
        return (dsc ? DType(dsc->dtype) : DType::Missing);
    }

    [[nodiscard]]
    inline void * getPointer() const {
        mdsdsc_t * dsc = getDescriptor();
        return (dsc ? dsc->pointer : nullptr);
    }

    template <typename T = Data>
    T getData() const;

    template <typename U = Data>
    U getUnits() const;

    template <typename T = Data, typename U = Data>
    std::tuple<T, U> getDataWithUnits() const;

    template <typename T = Data>
    T getError() const;

    template <typename T = Data>
    T getHelp() const;

    template <typename T = Data>
    T getValidation() const;

    std::string decompile() const;

    // UInt8Array serialize() const;

protected:

    mdsdsc_xd_t _xd = MDSDSC_XD_INITIALIZER;

    Tree * _tree = nullptr;

    template <typename T>
    inline T _clone() const {
        int status;

        mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
        status = MdsCopyDxXd(getDescriptor(), &xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return T(std::move(xd), getTree());
    }

}; // class Data

static const Data EmptyData;

std::string to_string(const Data * data);

inline std::string to_string(const Data& data) {
    return to_string(&data);
}

// TODO: Move
template <typename T>
struct is_std_vector : std::false_type { };

template <typename T, typename A>
struct is_std_vector<std::vector<T, A>> : std::true_type { };

template <typename T>
inline constexpr bool is_std_vector_v = is_std_vector<T>::value;

// TODO: Rename
template <typename T>
inline mdsdsc_xd_t _getArgument(const T& value)
{
    if constexpr (std::is_base_of_v<Data, T>) {
        return mdsdsc_xd_t{
            .length = 0,
            .dtype = DTYPE_DSC,
            .class_ = CLASS_S,
            .pointer = value.getDescriptor(),
            .l_length = 0,
        };
    }
    else if constexpr (is_std_vector_v<T>) {
        return Data::FromArray(value).release();
    }
    else {
        return Data::FromScalar(value).release();
    }
}

// TODO: Rename
inline void _freeArgument(mdsdsc_xd_t dsc) {
    if (dsc.class_ == CLASS_XD) {
        MdsFree1Dx(&dsc, nullptr);
    }
}

// TODO: WTF?
#define MdsEND_ARG ((void *)(intptr_t)1)
#define MDS_END_ARG , MdsEND_ARG

template <typename ...Args>
Data Data::Compile(std::string_view expression, Args... args)
{
    int status;
    mdsdsc_t dscExp = {
        .length = length_t(expression.length()),
        .dtype = DTYPE_T,
        .class_ = CLASS_S,
        .pointer = const_cast<char *>(expression.data()),
    };

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;

    std::vector<mdsdsc_xd_t> xdArgs;
    xdArgs.reserve(sizeof...(args));

    auto handleArgument =
        [&](mdsdsc_xd_t xd) -> mdsdsc_t * {
            xdArgs.push_back(xd);
            return xd.pointer;
        };
    
    status = TdiCompile(&dscExp, handleArgument(_getArgument(args))..., &out MDS_END_ARG);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    for (const auto& xd : xdArgs) {
        _freeArgument(xd);
    }

    return Data(std::move(out));
}

template <typename ...Args>
Data Data::Execute(std::string_view expression, Args... args)
{
    int status;
    mdsdsc_t dscExp = {
        .length = length_t(expression.length()),
        .dtype = DTYPE_T,
        .class_ = CLASS_S,
        .pointer = const_cast<char *>(expression.data()),
    };

    std::vector<mdsdsc_xd_t> xdArgs;
    xdArgs.reserve(sizeof...(args));

    auto handleArgument =
        [&](mdsdsc_xd_t xd) -> mdsdsc_t * {
            xdArgs.push_back(xd);
            return xd.pointer;
        };
    
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    status = TdiExecute(&dscExp, handleArgument(_getArgument(args))..., &out MDS_END_ARG);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    for (const auto& xd : xdArgs) {
        _freeArgument(xd);
    }

    return Data(std::move(out));
}

template <>
inline Data Data::FromScalar(Data value) {
    return value;
}

template <>
inline Data Data::FromScalar(std::nullptr_t) {
    return Data();
}

template <typename CT, DType DT>
class Scalar : public Data
{
public:

    typedef CT __ctype;
    static constexpr Class __class = Class::S;
    static constexpr DType __dtype = DT;

    Scalar() = default;

    Scalar(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Data(std::move(xd), tree)
    { }

    Scalar(__ctype value, Tree * tree = nullptr) {
        setValue(value);
        setTree(tree);
    }

    Scalar(Scalar&&) = default;
    Scalar& operator=(Scalar&&) = default;

    [[nodiscard]]
    inline Scalar clone() const {
        return _clone<Scalar>();
    }

    // TODO: Math?

    inline void setValue(__ctype value)
    {
        int status;

        mdsdsc_s_t dsc = {
            .length = sizeof(__ctype),
            .dtype = dtype_t(__dtype),
            .class_ = class_t(__class),
            .pointer = (char *)&value,
        };

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    [[nodiscard]]
    inline __ctype getValue() const {
        mdsdsc_t * dsc = getDescriptor();
        if (dsc && dsc->pointer) {
            return *(__ctype *)dsc->pointer;
        }

        // TODO: throw something?
        return __ctype{};
    }

}; // class Scalar<>

typedef Scalar<int8_t, DType::B> Int8;
typedef Scalar<uint8_t, DType::BU> UInt8;
typedef Scalar<int16_t, DType::W> Int16;
typedef Scalar<uint16_t, DType::WU> UInt16;
typedef Scalar<int32_t, DType::L> Int32;
typedef Scalar<uint32_t, DType::LU> UInt32;
typedef Scalar<int64_t, DType::Q> Int64;
typedef Scalar<uint64_t, DType::QU> UInt64;
typedef Scalar<float, DType::Float> Float32;
typedef Scalar<double, DType::Double> Float64;

template <>
inline Data Data::FromScalar(Int8::__ctype value) {
    return Int8(value);
}

template <>
inline Data Data::FromScalar(UInt8::__ctype value) {
    return UInt8(value);
}

template <>
inline Data Data::FromScalar(Int16::__ctype value) {
    return Int16(value);
}

template <>
inline Data Data::FromScalar(UInt16::__ctype value) {
    return UInt16(value);
}

template <>
inline Data Data::FromScalar(Int32::__ctype value) {
    return Int32(value);
}

template <>
inline Data Data::FromScalar(UInt32::__ctype value) {
    return UInt32(value);
}

template <>
inline Data Data::FromScalar(Int64::__ctype value) {
    return Int64(value);
}

template <>
inline Data Data::FromScalar(UInt64::__ctype value) {
    return UInt64(value);
}

template <>
inline Data Data::FromScalar(Float32::__ctype value) {
    return Float32(value);
}

template <>
inline Data Data::FromScalar(Float64::__ctype value) {
    return Float64(value);
}

template <>
inline Int8::__ctype Data::getData() const {
    return getData().convert<Int8>().getValue();
}

template <>
inline UInt8::__ctype Data::getData() const {
    return getData().convert<UInt8>().getValue();
}

template <>
inline Int16::__ctype Data::getData() const {
    return getData().convert<Int16>().getValue();
}

template <>
inline UInt16::__ctype Data::getData() const {
    return getData().convert<UInt16>().getValue();
}

template <>
inline Int32::__ctype Data::getData() const {
    return getData().convert<Int32>().getValue();
}

template <>
inline UInt32::__ctype Data::getData() const {
    return getData().convert<UInt32>().getValue();
}

template <>
inline Int64::__ctype Data::getData() const {
    return getData().convert<Int64>().getValue();
}

template <>
inline UInt64::__ctype Data::getData() const {
    return getData().convert<UInt64>().getValue();
}

template <>
inline Float32::__ctype Data::getData() const {
    return getData().convert<Float32>().getValue();
}

template <>
inline Float64::__ctype Data::getData() const {
    return getData().convert<Float64>().getValue();
}

template <typename CT, typename ST, DType DT>
class Array : public Data
{
public:

    typedef CT __ctype;
    static constexpr Class __class = Class::A;
    static constexpr DType __dtype = DT;

    Array() = default;

    inline Array(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Data(std::move(xd), tree)
    {
        mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
        if (dsc && dsc->length == 0) {
            dsc->length = sizeof(__ctype);
        }
    }

    inline Array(std::initializer_list<__ctype> values, const std::vector<uint32_t>& dims = {}, Tree * tree = nullptr) {
        std::vector<__ctype> tmp(values);
        setValues(tmp, dims);
        setTree(tree);
    }

    inline Array(const std::span<const __ctype> values, const std::vector<uint32_t>& dims = {}, Tree * tree = nullptr) {
        setValues(values, dims);
        setTree(tree);
    }

    Array(Array&&) = default;
    Array& operator=(Array&&) = default;

    [[nodiscard]]
    inline const __ctype& operator[](size_t index) const {
        return getValueAt(index);
    }

    [[nodiscard]]
    inline Array clone() const {
        return _clone<Array>();
    }

    void setValues(const std::span<const __ctype> values, const std::vector<uint32_t>& dims)
    {
        int status;
        
        array_coeff dsc = {
            .length = sizeof(__ctype),
            .dtype = dtype_t(__dtype),
            .class_ = CLASS_A,
            .pointer = (char *)values.data(),
            .scale = 0,
            .digits = 0,
            .aflags = aflags_t{
                .binscale = false,
                .redim = true,
                .column = true,
                .coeff = true,
                .bounds = false,
            },
            .dimct = dimct_t(dims.size()),
            .arsize = arsize_t(values.size_bytes()),
            .a0 = (char *)values.data(),
            .m = { 0 },
        };

        if (dims.empty()) {
            dsc.dimct = 1;
            dsc.m[0] = values.size();
        }
        else {
            for (size_t i = 0; i < dims.size(); ++i) {
                dsc.m[i] = dims[i];
            }
        }

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    [[nodiscard]]
    inline std::vector<__ctype> getValues() const {
        mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
        if (dsc) {
            return std::vector<__ctype>((__ctype *)dsc->pointer, (__ctype *)dsc->pointer + size());
        }
        return {};
    }

    [[nodiscard]]
    inline const __ctype& getValueAt(size_t index) const {
        mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
        if (dsc && index < size()) {
            return *((__ctype *)dsc->pointer + index);
        }

        throw TdiBadIndex();
    }

    [[nodiscard]]
    inline size_t getSize() const {
        mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
        if (dsc) {
            return dsc->arsize / dsc->length;
        }
        return 0;
    }

    // STL Compatability

    [[nodiscard]]
    inline size_t size() const {
        return getSize();
    }

    [[nodiscard]]
    inline size_t empty() const {
        return getSize() == 0;
    }

    [[nodiscard]]
    inline const __ctype& at(size_t index) const {
        return getValueAt(index);
    }

    [[nodiscard]]
    inline __ctype * begin() const {
        mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
        return (dsc ? (__ctype *)dsc->pointer : nullptr);
    }

    [[nodiscard]]
    inline __ctype * end() const {
        mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
        return (dsc ? (__ctype *)(dsc->pointer + dsc->arsize) : nullptr);
    }

}; // class Array<>

typedef Array<int8_t, Int8, DType::B> Int8Array;
// typedef Array<uint8_t, UInt8, DType::BU> UInt8Array;
typedef Array<int16_t, Int16, DType::W> Int16Array;
typedef Array<uint16_t, UInt16, DType::WU> UInt16Array;
typedef Array<int32_t, Int32, DType::L> Int32Array;
typedef Array<uint32_t, UInt32, DType::LU> UInt32Array;
typedef Array<int64_t, Int64, DType::Q> Int64Array;
typedef Array<uint64_t, UInt64, DType::QU> UInt64Array;
typedef Array<float, Float32, DType::Float> Float32Array;
typedef Array<double, Float64, DType::Double> Float64Array;

class UInt8Array : public Array<uint8_t, UInt8, DType::BU>
{
public:

    UInt8Array() = default;

    inline UInt8Array(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Array(std::move(xd), tree)
    { }

    inline UInt8Array(std::initializer_list<__ctype> values, const std::vector<uint32_t>& dims = {}, Tree * tree = nullptr)
        : Array(values, dims, tree)
    { }

    inline UInt8Array(const std::span<const __ctype> values, const std::vector<uint32_t>& dims = {}, Tree * tree = nullptr)
        : Array(values, dims, tree)
    { }

    template <typename T>
    T deserialize()
    {
        int status;

        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        status = MdsSerializeDscIn(getDescriptor()->pointer, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out)).convert<T>();
    }
};

template <>
inline Data Data::FromArray(std::span<Int8Array::__ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Int8Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<UInt8Array::__ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return UInt8Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<Int16Array::__ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Int16Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<UInt16Array::__ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return UInt16Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<Int32Array::__ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Int32Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<UInt32Array::__ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return UInt32Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<Int64Array::__ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Int64Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<UInt64Array::__ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return UInt64Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<Float32Array::__ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Float32Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<Float64Array::__ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Float64Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<Int8Array::__ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Int8Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<UInt8Array::__ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return UInt8Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<Int16Array::__ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Int16Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<UInt16Array::__ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return UInt16Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<Int32Array::__ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Int32Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<UInt32Array::__ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return UInt32Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<Int64Array::__ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Int64Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<UInt64Array::__ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return UInt64Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<Float32Array::__ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Float32Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<Float64Array::__ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Float64Array(values, dims);
}

template <>
inline std::vector<Int8Array::__ctype> Data::getData() const {
    return getData().convert<Int8Array>().getValues();
}

template <>
inline std::vector<UInt8Array::__ctype> Data::getData() const {
    return getData().convert<UInt8Array>().getValues();
}

template <>
inline std::vector<Int16Array::__ctype> Data::getData() const {
    return getData().convert<Int16Array>().getValues();
}

template <>
inline std::vector<UInt16Array::__ctype> Data::getData() const {
    return getData().convert<UInt16Array>().getValues();
}

template <>
inline std::vector<Int32Array::__ctype> Data::getData() const {
    return getData().convert<Int32Array>().getValues();
}

template <>
inline std::vector<UInt32Array::__ctype> Data::getData() const {
    return getData().convert<UInt32Array>().getValues();
}

template <>
inline std::vector<Int64Array::__ctype> Data::getData() const {
    return getData().convert<Int64Array>().getValues();
}

template <>
inline std::vector<UInt64Array::__ctype> Data::getData() const {
    return getData().convert<UInt64Array>().getValues();
}

template <>
inline std::vector<Float32Array::__ctype> Data::getData() const {
    return getData().convert<Float32Array>().getValues();
}

template <>
inline std::vector<Float64Array::__ctype> Data::getData() const {
    return getData().convert<Float64Array>().getValues();
}

class String : public Data
{
public:

    static constexpr Class __class = Class::S;
    static constexpr DType __dtype = DType::T;

    String() = default;

    String(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Data(std::move(xd), tree)
    { }

    inline String(std::string_view value, Tree * tree = nullptr) {
        setValue(value);
        setTree(tree);
    }

    String(String&&) = default;
    String& operator=(String&&) = default;

    [[nodiscard]]
    inline String clone() const {
        return _clone<String>();
    }

    void setValue(std::string_view value);

    std::string getValue() const {
        mdsdsc_t * dsc = getDescriptor();
        return std::string(dsc->pointer, dsc->length);
    }

}; // class String

template <>
inline Data Data::FromScalar(const char * value) {
    return String(value);
}

template <>
inline Data Data::FromScalar(std::string value) {
    return String(value);
}

template <>
inline Data Data::FromScalar(std::string_view value) {
    return String(value);
}

template <>
inline std::string Data::getData() const {
    return getData().convert<String>().getValue();
}

class StringArray : public Data
{
public:

    static constexpr Class __class = Class::A;
    static constexpr DType __dtype = DType::T;

    StringArray() = default;

    inline StringArray(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Data(std::move(xd), tree)
    { }

    inline StringArray(std::span<std::string> values, const std::vector<uint32_t>& dims = {}, Tree * tree = nullptr) {
        setValues(values, dims);
        setTree(tree);
    }

    inline StringArray(std::initializer_list<std::string> values, const std::vector<uint32_t>& dims = {}, Tree * tree = nullptr) {
        std::vector<std::string> tmp(values);
        setValues(tmp, dims);
        setTree(tree);
    }

    StringArray(StringArray&&) = default;
    StringArray& operator=(StringArray&&) = default;

    [[nodiscard]]
    inline StringArray clone() const {
        return _clone<StringArray>();
    }

    void setValues(std::span<std::string> values, const std::vector<uint32_t>& dims = {});

    [[nodiscard]]
    inline std::vector<std::string> getValues() const {
        mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
        if (dsc) {
            std::vector<std::string> values;
            for (size_t i = 0; i < size(); ++i) {
                values.emplace_back(dsc->pointer + (i * dsc->length), dsc->length);
            }
            return values;
        }
        return {};
    }

    [[nodiscard]]
    inline std::string getValueAt(size_t index) const {
        mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
        if (dsc && index < size()) {
            return std::string(dsc->pointer + (index * dsc->length), dsc->length);
        }

        throw TdiBadIndex();
    }

    [[nodiscard]]
    inline size_t getSize() const {
        mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
        if (dsc) {
            return dsc->arsize / dsc->length;
        }
        return 0;
    }

    // STL Compatability

    [[nodiscard]]
    inline size_t size() const {
        return getSize();
    }

    [[nodiscard]]
    inline size_t empty() const {
        return getSize() == 0;
    }

    [[nodiscard]]
    inline std::string at(size_t index) const {
        return getValueAt(index);
    }

    // TODO: StringArrayIterator

    // [[nodiscard]]
    // inline __ctype * begin() const {
    //     mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
    //     return (dsc ? (__ctype *)dsc->pointer : nullptr);
    // }

    // [[nodiscard]]
    // inline __ctype * end() const {
    //     mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
    //     return (dsc ? (__ctype *)(dsc->pointer + dsc->arsize) : nullptr);
    // }

}; // class StringArray

template <DType DT>
class Record : public Data
{
public:

    static constexpr Class __class = Class::R;
    static constexpr DType __dtype = DT;

    Record() = default;

    inline Record(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Data(std::move(dsc), tree)
    { }

    Record(Record&&) = default;
    Record& operator=(Record&&) = default;

    virtual ~Record() = default;

    inline size_t getNumDescriptors() const {
        mdsdsc_r_t * dsc = (mdsdsc_r_t *)getDescriptor();
        return (dsc ? dsc->ndesc : 0);
    }

    inline bool hasDescriptorAt(size_t index) const {
        mdsdsc_r_t * dsc = (mdsdsc_r_t *)getDescriptor();
        return (dsc && index < dsc->ndesc && dsc->dscptrs[index] != nullptr);
    }

    template <typename T = Data>
    inline T getDescriptorAt(size_t index) const {
        mdsdsc_r_t * dsc = (mdsdsc_r_t *)getDescriptor();

        if (dsc && index < dsc->ndesc) {
            mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
            int status = MdsCopyDxXd(dsc->dscptrs[index], &xd);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            mdsdsc_t * newDsc = xd.pointer;
            if (Class(newDsc->class_) == T::__class && DType(newDsc->dtype) == T::__dtype) {
                return T(std::move(xd), getTree());
            }

            return Data(std::move(xd), getTree()).convert<T>();
        }

        return T();
    }

}; // class Record<>

class Param : public Record<DType::Param>
{
public:

    Param() = default;

    inline Param(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Record(std::move(dsc), tree)
    { }

    template <typename _Value = Data, typename _Help = Data, typename _Validation = Data>
    Param(const _Value& value, const _Help& help, const _Validation& validation)
    {
        int status;

        mdsdsc_xd_t dscValue = _getArgument(value);
        mdsdsc_xd_t dscUnits = _getArgument(help);
        mdsdsc_xd_t dscValidation = _getArgument(validation);

        DESCRIPTOR_PARAM(dsc, dscValue.pointer, dscUnits.pointer, dscValidation.pointer);

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        _freeArgument(dscValue);
        _freeArgument(dscUnits);
        _freeArgument(dscValidation);
    }

    [[nodiscard]]
    inline Param clone() const {
        return _clone<Param>();
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getValue() const {
        return getDescriptorAt<T>(0);
    }

    // getHelp and getValidation are defined in Data

}; // class Param

class Signal : public Record<DType::Signal>
{
public:

    Signal() = default;

    inline Signal(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Record(std::move(dsc), tree)
    { }

    template <typename _Value = Data, typename _Raw = Data, typename _Dimension = Data>
    Signal(const _Value& value, const _Raw& raw, const _Dimension& dimension = {})
    {
        int status;

        mdsdsc_xd_t dscValue = _getArgument(value);
        mdsdsc_xd_t dscRaw = _getArgument(raw);
        mdsdsc_xd_t dscDimension = _getArgument(dimension);

        DESCRIPTOR_SIGNAL_1(dsc,
            dscValue.pointer,
            dscRaw.pointer,
            dscDimension.pointer
        );

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
        
        _freeArgument(dscValue);
        _freeArgument(dscRaw);
        _freeArgument(dscDimension);
    }

    template <typename _Value = Data, typename _Raw = Data, typename ..._Dimensions>
    Signal(const _Value& value, const _Raw& raw, const _Dimensions& ...dimensions)
    {
        int status;

        mdsdsc_xd_t dscValue = _getArgument(value);
        mdsdsc_xd_t dscRaw = _getArgument(raw);
        std::vector<mdsdsc_xd_t> dscDimensions = { _getArgument(dimensions)... };

        DESCRIPTOR_SIGNAL(dsc, sizeof...(dimensions), dscValue.pointer, dscRaw.pointer);

        for (size_t i = 0; i < dscDimensions.size(); ++i) {
            dsc.dimensions[i] = dscDimensions[i].pointer;
        }

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        _freeArgument(dscValue);
        _freeArgument(dscRaw);

        for (size_t i = 0; i < dscDimensions.size(); ++i) {
            _freeArgument(dscDimensions[i]);
        }
    }

    [[nodiscard]]
    inline Signal clone() const {
        return _clone<Signal>();
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getValue() const {
        return getDescriptorAt<T>(0);
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getRaw() const {
        return getDescriptorAt<T>(1);
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getDimensionAt(size_t index = 0) const {
        return getDescriptorAt<T>(2 + index);
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getDimensions() const {
        std::vector<T> dims;
        size_t count = getNumDescriptors() - 2;
        for (size_t i = 0; i < count; ++i) {
            dims.push_back(getDimensionAt(i));
        }
        return dims;
    }

}; // class Signal

class Dimension : public Record<DType::Dimension>
{
public:

    Dimension() = default;

    Dimension(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Record(std::move(dsc), tree)
    { }

    template <typename _Window, typename _Axis>
    Dimension(const _Window& window, const _Axis& axis)
    {
        int status;

        mdsdsc_xd_t tmpWindow = _getArgument(window);
        mdsdsc_xd_t tmpAxis = _getArgument(axis);

        DESCRIPTOR_DIMENSION(dsc, tmpWindow.pointer, tmpAxis.pointer);

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        _freeArgument(tmpWindow);
        _freeArgument(tmpAxis);
    }

    [[nodiscard]]
    inline Dimension clone() const {
        return _clone<Dimension>();
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getWindow() const {
        return getDescriptorAt<T>(0);
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getAxis() const {
        return getDescriptorAt<T>(1);
    }

}; // class Dimension

class Window : public Record<DType::Window>
{
public:

    Window() = default;

    Window(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Record(std::move(dsc), tree)
    { }

    template <typename _StartIndex, typename _EndIndex, typename _ValueAtIndex0>
    Window(const _StartIndex& startIndex, const _EndIndex& endIndex, const _ValueAtIndex0& valueAtIndex0)
    {
        int status;

        mdsdsc_xd_t tmpStartIndex = _getArgument(startIndex);
        mdsdsc_xd_t tmpEndIndex = _getArgument(endIndex);
        mdsdsc_xd_t tmpValueAtIndex0 = _getArgument(valueAtIndex0);

        DESCRIPTOR_WINDOW(dsc, tmpStartIndex.pointer, tmpEndIndex.pointer, tmpValueAtIndex0.pointer);

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        _freeArgument(tmpStartIndex);
        _freeArgument(tmpEndIndex);
        _freeArgument(tmpValueAtIndex0);
    }

    [[nodiscard]]
    inline Window clone() const {
        return _clone<Window>();
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getStartIndex() const {
        return getDescriptorAt<T>(0);
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getEndIndex() const {
        return getDescriptorAt<T>(1);
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getValueAtIndex0() const {
        return getDescriptorAt<T>(2);
    }

}; // class Window

class Function : public Record<DType::Function>
{
public:

    Function() = default;

    Function(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Record(std::move(dsc), tree)
    { }

    template <typename ..._Args>
    Function(opcode_t opcode, const _Args& ...args)
    {
        int status;

        // TODO: #define MAX_ARGS 255 ?
        static_assert(sizeof...(args) <= 255);

        std::vector<mdsdsc_xd_t> tmpArgs = { _getArgument(args)... };

        DESCRIPTOR_FUNCTION(dsc, &opcode, sizeof...(args));

        for (size_t i = 0; i < tmpArgs.size(); ++i) {
            dsc.arguments[i] = tmpArgs[i].pointer;
        }

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        for (const auto& tmpArg : tmpArgs) {
            _freeArgument(tmpArg);
        }
    }

    Data call() const;

}; // class Function

class Range : public Record<DType::Range>
{
public:

    Range() = default;

    Range(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Record(std::move(dsc), tree)
    { }

    template <typename _Begin = Data, typename _Ending = Data, typename _Delta = Data>
    Range(const _Begin& begin, const _Ending& ending, const _Delta& delta)
    {
        int status;

        mdsdsc_xd_t tmpBegin = _getArgument(begin);
        mdsdsc_xd_t tmpEnding = _getArgument(ending);
        mdsdsc_xd_t tmpDelta = _getArgument(delta);

        DESCRIPTOR_RANGE(dsc, tmpBegin.pointer, tmpEnding.pointer, tmpDelta.pointer);

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        _freeArgument(tmpBegin);
        _freeArgument(tmpEnding);
        _freeArgument(tmpDelta);
    }

    [[nodiscard]]
    inline Range clone() const {
        return _clone<Range>();
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getBegin() const {
        return getDescriptorAt<T>(0);
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getEnding() const {
        return getDescriptorAt<T>(1);
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getDelta() const {
        return getDescriptorAt<T>(2);
    }

    // TODO: isFinite() ?
    // TODO: begin/end/size

}; // class Range

class WithUnits : public Record<DType::WithUnits>
{
public:

    WithUnits() = default;

    inline WithUnits(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Record(std::move(dsc), tree)
    { }

    template <typename _Value = Data, typename _Units = Data>
    WithUnits(const _Value& value, const _Units& units)
    {
        int status;

        mdsdsc_xd_t dscValue = _getArgument(value);
        mdsdsc_xd_t dscUnits = _getArgument(units);

        DESCRIPTOR_WITH_UNITS(dsc, dscValue.pointer, dscUnits.pointer);

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        _freeArgument(dscValue);
        _freeArgument(dscUnits);
    }

    [[nodiscard]]
    inline WithUnits clone() const {
        return _clone<WithUnits>();
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getValue() const {
        return getDescriptorAt<T>(0);
    }

    // getUnits is defined in Data

}; // class WithUnits

class WithError : public Record<DType::WithError>
{
public:

    WithError() = default;

    inline WithError(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Record(std::move(dsc), tree)
    { }

    template <typename _Value = Data, typename _Error = Data>
    WithError(const _Value& value, const _Error& error)
    {
        int status;

        mdsdsc_xd_t dscValue = _getArgument(value);
        mdsdsc_xd_t dscError = _getArgument(error);

        DESCRIPTOR_WITH_ERROR(dsc, dscValue.pointer, dscError.pointer);

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        _freeArgument(dscValue);
        _freeArgument(dscError);
    }

    [[nodiscard]]
    inline WithError clone() const {
        return _clone<WithError>();
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getValue() const {
        return getDescriptorAt<T>(0);
    }

    // getError is defined in Data

}; // class WithError

class Connection
{
public:

    static constexpr int InvalidConnectionID = -1;

    inline Connection(const std::string& hostspec) {
        open(hostspec);
    }

    inline virtual ~Connection() {
        close();
    }

    void open(const std::string& hostspec);

    void close();

    void reconnect();

    void openTree(const std::string& tree, int shot) const;

    void closeTree(const std::string& tree, int shot) const;

    int closeAllTrees() const;

    template <typename T = Data, typename ...Args>
    T get(const std::string& expression, const Args& ...args) const
    {
        std::vector<mdsdsc_xd_t> dataArgs{ _getArgument(args)... };
        return _get(expression, std::move(dataArgs)).convert<T>();
    }

    template <typename T = Data, typename ...Args>
    T getObject(const std::string& expression, const Args& ...args) const
    {
        UInt8Array serializedData = get<UInt8Array>("serializeout(`(" + expression + ";))", args...);
        return serializedData.deserialize<T>();
    }

private:

    Data _get(const std::string& expression, std::vector<mdsdsc_xd_t>&& xdArgs) const;

    int _id = InvalidConnectionID;

    std::string _hostspec;

}; // class Connection



// class Event
// {
// public:

//     static void SetEventRaw(std::string name, size_t size, const uint8_t * buffer);

//     inline static void SetEvent(std::string name, const Data& data) {
//         // data.
//         return SetEventRaw(name, 0, nullptr);
//     }

//     inline static void SetEvent(std::string name) {
//         return SetEventRaw(name, 0, nullptr);
//     }

//     Event(std::string name)
//         : _name(name)
//     { }

//     virtual ~Event();

//     inline std::string getName() const {
//         return _name;
//     }

//     template <typename T = Data>
//     [[nodiscard]]
//     inline T getData() const {

//     }

// private:

//     std::string _name;

// }; // class Event



// struct DevicePart
// {
// public:

//     std::string Name;
//     Usage Usage = Usage::Any;
//     Data Value = Data();
//     std::string ValueExpr = {};
//     uint32_t SetFlags = 0;
//     std::map<std::string, Data> XNCI = {};
// };

// class Device : public TreeNode
// {
// public:

//     Device(Tree * tree, int nid)
//         : TreeNode(tree, nid)
//     { }

//     virtual ~Device() = default;

//     static int GetNumParts();
//     void AddParts();

//     virtual std::vector<DevicePart> GetParts() = 0;

// }; // class Device

// class SineDevice : public Device
// {
// public:

//     std::vector<DevicePart> GetParts() override {
//         return {
//             {
//                 .Name = "COMMENT",
//                 .Usage = Usage::Text,
//                 .Value = String("Hello, World!"),
//                 .SetFlags = NciM_NO_WRITE_SHOT | NciM_DO_NOT_COMPRESS,
//                 .XNCI = {
//                     { "tooltip", String("Figure it out") },
//                 }
//             }
//         };
//     }

//     int GetNumParts()
//     {
//         return 1;
//     }

//     void AddParts()
//     {
//         addNode("COMMENT", Usage::Text)
//             .setData("Hello, World!")
//             .setNoWriteShot()
//             .setDoNotCompress()
//             .setXNCI("tooltip", String("Figure it out"));
//     }

// };

// #define MDSPLUS_DEVICE(LibraryName, DeviceClassLower, DeviceClass, NodeCount)

// TODO: getActiveTree() (Tree::getGlobalTree() ?)
// TODO: arguments?
// #define MDSPLUS_DEVICE_METHOD(DeviceClassLower, DeviceClass, MethodName) \
//     extern "C" int DeviceClassLower##__##MethodName(mdsdsc_t * nid) \
//     { \
//         DeviceClass device(*(int *)nid->pointer, getActiveTree()); \
//         device.MethodName(); \
//         return TreeSUCCESS; \
//     }

///

template <typename T /*= Data*/>
[[nodiscard]]
inline T TreeNode::getData() const {
    return getRecord().convert<T>();
}

template <typename T /*= Data*/, typename U /*= Data*/>
[[nodiscard]]
inline std::tuple<T, U> TreeNode::getDataWithUnits() const {
    auto data = getRecord();
    auto units = data.getUnits<U>();
    return { data.convert<T>(), std::move(units) };
}

template <typename T>
inline void TreeNode::setData(T value) const
{
    putRecord(Data::FromScalar(value));
}

template <typename T, typename U>
inline void TreeNode::setDataWithUnits(T value, U units) const
{
    putRecord(WithUnits(Data::FromScalar(value), Data::FromScalar(units)));
}

template <typename T>
inline void TreeNode::setArrayData(const std::span<const T> values, const std::vector<uint32_t>& dims /*= {}*/) const
{
    putRecord(Data::FromArray(values, dims));
}

template <typename T, typename U>
inline void TreeNode::setArrayDataWithUnits(const std::span<const T> values, U units, const std::vector<uint32_t>& dims /*= {}*/) const
{
    putRecord(WithUnits(Data::FromArray(values, dims), Data::FromScalar(units)));
}

template <typename T>
inline T TreeNode::_getNCI(nci_t code) const
{
    T value = {};
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

template <typename T>
T Data::convert()
{
    int status;

    mdsdsc_xd_t dscData = MDSDSC_XD_INITIALIZER;

    mdsdsc_t * dsc = getDescriptor();
    Class dscClass = Class(dsc->class_);
    DType dscDType = DType(dsc->dtype);

    if constexpr (std::is_same_v<Data, T>) {
        return T(release(), getTree());
    }
    else if constexpr (std::is_base_of_v<Data, T>) {

        if (dscClass == T::__class && dscDType == T::__dtype) {
            return T(release(), getTree());
        }

        // TODO: Check edge cases for when we need to call DATA()
        if (dscClass == Class::R) {
            if (hasTree()) {
                status = _TdiIntrinsic(getTree()->getContext(), OPC_DATA, 1, &dsc, &dscData);
            }
            else {
                status = TdiIntrinsic(OPC_DATA, 1, &dsc, &dscData);
            }
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            dscClass = Class(dscData.pointer->class_);
            dscDType = DType(dscData.pointer->dtype);

            if (dscClass == T::__class && dscDType == T::__dtype) {
                return T(std::move(dscData));
            }

            dsc = (mdsdsc_t *)dscData.pointer;
        }

        // TODO: Is there a better way to convert to a string?
        if constexpr (std::is_same_v<String, T>) {
            mdsdsc_xd_t decoXD = MDSDSC_XD_INITIALIZER;
            if (hasTree()) {
                status = _TdiIntrinsic(getTree()->getContext(), OPC_DECOMPILE, 1, &dsc, &decoXD);
            }
            else {
                status = TdiIntrinsic(OPC_DECOMPILE, 1, &dsc, &decoXD);
            }
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            MdsFree1Dx(&dscData, nullptr);
            return String(std::move(decoXD));
        }
        else if constexpr (T::__class == Class::S) {
            typename T::__ctype value = {};

            mdsdsc_t convert = {
                .length = sizeof(typename T::__ctype),
                .dtype = dtype_t(T::__dtype),
                .class_ = CLASS_S,
                .pointer = (char *)&value,
            };
    
            status = TdiConvert((mdsdsc_a_t *)dsc, (mdsdsc_a_t *)&convert);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }
    
            MdsFree1Dx(&dscData, nullptr);
            return T(value, getTree());
        }
        else if constexpr (T::__class == Class::A && T::__dtype != DType::T) {
            array_coeff * dscArray;

            mdsdsc_a_t arrayOfOne = {
                .length = sizeof(typename T::__ctype),
                .dtype = dtype_t(T::__dtype),
                .class_ = CLASS_A,
                .pointer = dsc->pointer,
                .scale = 0,
                .digits = 0,
                .aflags = {
                    .binscale = false,
                    .redim = true,
                    .column = true,
                    .coeff = false,
                    .bounds = false,
                },
                .dimct = 1,
                .arsize = sizeof(typename T::__ctype),
            };

            if (dsc->class_ == CLASS_A) {
                dscArray = (array_coeff *)dsc;
            }
            else {
                arrayOfOne.length = dsc->length;
                dscArray = (array_coeff *)&arrayOfOne;
            }

            size_t size = (dscArray->arsize / dscArray->length);
            std::vector<typename T::__ctype> values(size);
            array_coeff convert = {
                .length = sizeof(typename T::__ctype),
                .dtype = dtype_t(T::__dtype),
                .class_ = CLASS_A,
                .pointer = (char *)values.data(),
                .scale = dscArray->scale,
                .digits = dscArray->digits,
                .aflags = dscArray->aflags,
                .dimct = dscArray->dimct,
                .arsize = arsize_t(size * sizeof(typename T::__ctype)),
                .a0 = (char *)values.data(),
                .m = { 0 },
            };

            std::vector<uint32_t> dims(dscArray->dimct);
        
            if (dscArray->aflags.coeff) {
                for (dimct_t i = 0; i < dscArray->dimct; ++i) {
                    convert.m[i] = dscArray->m[i];
                    dims[i] = dscArray->m[i];
                }
            }
        
            status = TdiConvert((mdsdsc_a_t *)dscArray, (mdsdsc_a_t *)&convert);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }
    
            MdsFree1Dx(&dscData, nullptr);
            return T(values, dims, getTree());
        }
    }

    throw TdiInvalidDataType();
}

template <typename T /*= Data*/>
inline T Data::getData() const
{
    return clone().convert<T>();
}

template <typename U /*= Data*/>
inline U Data::getUnits() const
{
    int status;

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    if (hasTree()) {
        status = _TdiIntrinsic(getTree()->getContext(), OPC_UNITS_OF, 1, &dsc, &out);
    }
    else {
        status = TdiIntrinsic(OPC_UNITS_OF, 1, &dsc, &out);
    }
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).convert<U>();
}

template <typename T /*= Data*/, typename U /*= Data*/>
std::tuple<T, U> Data::getDataWithUnits() const
{
    return { getData<T>(), getUnits<U>() };
}

template <typename T /*= Data*/>
inline T Data::getError() const
{
    int status;

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    if (hasTree()) {
        status = _TdiIntrinsic(getTree()->getContext(), OPC_ERROR_OF, 1, &dsc, &out);
    }
    else {
        status = TdiIntrinsic(OPC_ERROR_OF, 1, &dsc, &out);
    }
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).convert<T>();
}

template <typename T /*= Data*/>
inline T Data::getHelp() const
{
    int status;

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    if (hasTree()) {
        status = _TdiIntrinsic(getTree()->getContext(), OPC_HELP_OF, 1, &dsc, &out);
    }
    else {
        status = TdiIntrinsic(OPC_HELP_OF, 1, &dsc, &out);
    }
    if (status == TdiINVDTYDSC) {
        // Unlike UNITS_OF and ERROR_OF, this throws an error when there is no value
        return T();
    }
    else if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).convert<T>();
}

template <typename T /*= Data*/>
inline T Data::getValidation() const
{
    int status;

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    if (hasTree()) {
        status = _TdiIntrinsic(getTree()->getContext(), OPC_VALIDATION_OF, 1, &dsc, &out);
    }
    else {
        status = TdiIntrinsic(OPC_VALIDATION_OF, 1, &dsc, &out);
    }
    if (status == TdiINVDTYDSC) {
        // Unlike UNITS_OF and ERROR_OF, this throws an error when there is no value
        return T();
    }
    else if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).convert<T>();
}

} // namespace mdsplus

#endif // MDSPLUS_HPP