#ifndef MDSPLUS_TREE_HPP
#define MDSPLUS_TREE_HPP

#include "TreeNode.hpp"

extern "C" {

    #include <treeshr.h>
    #include <dbidef.h>

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

        return Tree(std::string(name, retNameLength), shot, TreeDbid());
    }

    inline void setActive() {
        TreeSwitchDbid(getDBID());
    }

    Tree();

    Tree(const std::string& treename, int shot, Mode mode = Mode::Normal)
        : _treename(treename)
        , _shot(shot)
        , _mode(mode)
    {
        open();
    }

    Tree(const std::string& treename, int shot, void * dbid)
        : _treename(treename)
        , _shot(shot)
        , _mode(Mode::View)
        , _dbid(dbid)
    { }

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

    inline void open(const std::string& treename, int shot, Mode mode = Mode::Normal)
    {
        _treename = treename;
        _shot = shot;
        _mode = mode;

        return open();
    }

    void open();

    inline void reopen() { open(); }

    void close();

    void write();

    std::vector<std::string> findTagWild(const std::string& wildcard) const;

    inline std::vector<std::string> getTags() const override {
        return findTagWild("*");
    }

    bool isOpen() const {
        return _open;
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
        if (_open) {
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
    ResultType compileData(const std::string& expression, ArgTypes... args) const;

    template <typename ResultType = Data, typename ...ArgTypes>
    ResultType executeData(const std::string& expression, ArgTypes... args) const;

private:

    void * _dbid = nullptr;

    bool _open = false;

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