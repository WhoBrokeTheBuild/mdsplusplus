#ifndef MDSPLUS_TREE_HPP
#define MDSPLUS_TREE_HPP

#include "TreeNode.hpp"

#include <climits>

extern "C" {

    #include <treeshr.h>
    #include <dbidef.h>

    int _TreeFileName(void *, char *, int, struct descriptor_xd *);

} // extern "C"

namespace mdsplus {

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
Mode from_string(const std::string& mode); // TODO: Rename?

// TODO: TreeView class

class Tree : public TreeNode
{
public:

    // TODO: Rename public? global? ~~current~~? 
    static Tree GetActive() {
        return Tree(TreeDbid());
    }
    inline void setActive() {
        TreeSwitchDbid(getDBID());
    }

    static std::vector<int> getShotDB(
        const std::string& treename,
        const std::string& path = "",
        int lower = INT_MIN,
        int higher = INT_MAX
    );

    Tree() = default;

    inline Tree(const std::string& treename, int shot, Mode mode = Mode::Normal, const std::string& path = {})
        : _path(path)
    {
        open(treename, shot, mode);
    }

    inline Tree(void * dbid)
        : _mode(Mode::View)
        , _dbid(dbid)
    {   
        // TODO: Review

        char name[64];
        int retNameLength;
        DBI_ITM itmList[] = {
            { sizeof(name), DbiNAME, name, &retNameLength, },
            { sizeof(_shot), DbiSHOTID, &_shot, nullptr, },
            { 0, DbiEND_OF_LIST, nullptr, nullptr, },
        };

        _treename = std::string(name, retNameLength);

        int status = TreeGetDbi(itmList);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    // Disallow copy constructor and assignment operator
    Tree(const Tree&) = delete;
    Tree& operator=(const Tree&) = delete;

    inline Tree(Tree&& other)
    {
        // TODO: Improve
        std::swap(_treename, other._treename);
        std::swap(_shot, other._shot);
        std::swap(_mode, other._mode);
        std::swap(_dbid, other._dbid);
    }

    inline Tree& operator=(Tree&& other)
    {
        // TODO: Improve
        std::swap(_treename, other._treename);
        std::swap(_shot, other._shot);
        std::swap(_mode, other._mode);
        std::swap(_dbid, other._dbid);
        return *this;
    }

    inline ~Tree()
    {
        if (_mode != Mode::View) {
            TreeFreeDbid(_dbid);
            _dbid = nullptr;
        }
    }

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
        // Return _treename?
        return _getStringDBI(DbiNAME, 64);
    }

    [[nodiscard]]
    inline int getShot() const {
        // Return _shot?
        return _getDBI<int>(DbiSHOTID);
    }

    [[nodiscard]]
    inline Mode getMode() const {
        return _mode;
    }

    inline int64_t getDatafileSize() {
        return _TreeGetDatafileSize(getDBID());
    }

    std::string getFileName(const std::string& subtree = {});

    void open(const std::string& treename, int shot, Mode mode = Mode::Normal);

    inline void reopen() {
        open(getTreeName(), getShot(), getMode());
    }

    void close();

    void write();

    void createPulse(int shot) const;

    // std::vector<TreeNode> findNodeWild(const std::string& wildcard) const;

    std::vector<std::string> findTagWild(const std::string& wildcard) const;

    inline std::vector<std::string> getTags() const override {
        return findTagWild("*");
    }

    bool isOpen() const {
        return (_dbid != nullptr);
    }

    bool isOpenForEdit() const {
        return _getDBI<int>(DbiOPEN_FOR_EDIT);
    }

    bool isModified() const {
        return _getDBI<int>(DbiMODIFIED);
    }

    bool isOpenReadOnly() const {
        return _getDBI<int>(DbiOPEN_READONLY);
    }

    void setReadOnly() {
        if (_dbid != nullptr) {
            _setDBI(DbiREADONLY, true);

            if (_mode != Mode::View) {
                _mode = Mode::ReadOnly;
            }
        }
    }

    bool isVersionsInModelEnabled() const {
        return _getDBI<int>(DbiVERSIONS_IN_MODEL);
    }

    void setVersionsInModelEnabled(bool enabled) const {
        _setDBI(DbiVERSIONS_IN_MODEL, enabled);
    }

    bool isVersionsInPulseEnabled() const {
        return _getDBI<int>(DbiVERSIONS_IN_PULSE);
    }

    void setVersionsInPulseEnabled(bool enabled) const {
        _setDBI(DbiVERSIONS_IN_PULSE, enabled);
    }

    bool isAlternateCompressionEnabled() const {
        return _getDBI<int>(DbiALTERNATE_COMPRESSION);
    }

    void setAlternateCompressionEnabled(bool enabled) const {
        _setDBI(DbiALTERNATE_COMPRESSION, enabled);
    }

    inline void setDefaultNode(const std::string& path) const {
        setDefaultNode(getNode(path));
    }

    void setDefaultNode(const TreeNode& node) const;

    TreeNode getDefaultNode() const;

    template <typename ResultType = Data, typename ...ArgTypes>
    ResultType compileData(const std::string& expression, const ArgTypes&... args) const;

    template <typename ResultType = Data, typename ...ArgTypes>
    ResultType executeData(const std::string& expression, const ArgTypes&... args) const;

private:

    void * _dbid = nullptr;

    std::string _path;

    std::string _treename;

    int _shot;

    Mode _mode = Mode::Normal;

    template <typename ResultType>
    ResultType _getDBI(int16_t code) const;

    std::string _getStringDBI(int16_t code, int16_t size) const;

    void _setDBI(int16_t code, int value) const;

};

std::string to_string(const Tree * tree);

inline std::string to_string(const Tree& tree) {
    return to_string(&tree);
}

} // namespace mdsplus

#endif // MDSPLUS_TREE_HPP