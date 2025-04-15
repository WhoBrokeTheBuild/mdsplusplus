#include <mdsplus.hpp>

#include <treeshr.h>

namespace mdsplus {

Tree::Tree()
{ }

Tree::Tree(std::string_view treename, int shot, Mode mode /*= Mode::Normal*/)
    : _treename(treename)
    , _shot(shot)
    , _mode(mode)
{
    open();
}

Tree::~Tree()
{
    TreeFreeDbid(_dbid);
    _dbid = nullptr;
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

} // namespace mdsplus