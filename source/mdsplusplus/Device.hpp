#ifndef MDSPLUS_DEVICE_HPP
#define MDSPLUS_DEVICE_HPP

#include "Data.hpp"
#include "Tree.hpp"

#include <functional>
#include <map>

namespace mdsplus {

struct DevicePart
{
    std::string Path;
    mdsplus::Usage Usage = mdsplus::Usage::Any;
    Data * Value = {};
    std::string ValueExpression = {};
    TreeNodeFlags Flags = {};
    std::map<std::string, Data *> XNCI = {};
    std::function<void(TreeNode& node)> Callback;
};

class Device : public TreeNode
{
public:

    Device(Tree * tree, int nid)
        : TreeNode(tree, nid)
    { }

    static std::vector<DevicePart> GetParts() {
        return {};
    }

    void addParts(std::vector<DevicePart>&& parts) const;

};

#define MDSPLUS_DEVICE_BOOTSTRAP(DeviceClass) \
    DeviceClass(Tree * tree, int nid) \
        : Device(tree, nid) \
    { } \
    virtual ~DeviceClass() = default; \

#define MDSPLUS_DEVICE(LibraryName, DeviceClass, DeviceClassLower)               \
    extern "C" int DeviceClassLower##__add(                                      \
        mdsdsc_t * dscName,                                                      \
        mdsdsc_t * dscDummy,                                                     \
        int * nidOut)                                                            \
    {                                                                            \
        (void)dscDummy;                                                          \
        int nidHead = 0;                                                         \
        static DESCRIPTOR(dscLibrary, LibraryName);                              \
        static DESCRIPTOR(dscModel, #DeviceClass);                               \
        mdsdsc_t * dscLibraryPtr = (dscLibrary.pointer ? &dscLibrary : nullptr); \
        static DESCRIPTOR_CONGLOM(dscConglom, dscLibraryPtr, &dscModel, 0, 0);   \
        std::vector<mdsplus::DevicePart> parts = DeviceClass::GetParts();        \
        TreeStartConglomerate(parts.size() + 1);                                 \
        std::string name(dscName->pointer, dscName->length);                     \
        TreeAddNode(name.c_str(), &nidHead, TreeUSAGE_DEVICE);                   \
        TreePutRecord(nidHead, (mdsdsc_t *)&dscConglom, 0);                      \
        Tree tree = mdsplus::Tree::GetActive();                                  \
        DeviceClass device(&tree, nidHead);                                      \
        device.addParts(std::move(parts));                                       \
        TreeEndConglomerate();                                                   \
        if (nidOut) {                                                            \
            *nidOut = nidHead;                                                   \
        }                                                                        \
        return TreeSUCCESS;                                                      \
    }

#define MDSPLUS_DEVICE_METHOD(DeviceClassLower, DeviceClass, MethodName) \
    extern "C" int DeviceClassLower##__##MethodName(mdsdsc_t * nid)      \
    {                                                                    \
        Tree tree = mdsplus::Tree::GetActive();                          \
        DeviceClass device(&tree, *(int *)nid->pointer);                 \
        try {                                                            \
            device.MethodName();                                         \
        } catch (mdsplus::MDSplusException& e) {                         \
            return e.getStatus();                                        \
        }                                                                \
        return TreeSUCCESS;                                              \
    }

} // namespace mdsplus

#endif // MDSPLUS_DEVICE_HPP

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