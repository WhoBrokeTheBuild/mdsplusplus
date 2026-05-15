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

    template <typename DeviceClass,
        std::enable_if_t<std::is_base_of<Device, DeviceClass>::value, bool> = true>
    static DeviceClass Add(Tree * tree, const std::string& name)
    {
        int nid = 0;
        DESCRIPTOR_FROM_CSTRING(dscLibrary, DeviceClass::__library_name);
        DESCRIPTOR_FROM_CSTRING(dscModel, DeviceClass::__class_name);
        static DESCRIPTOR_CONGLOM(dscConglom, &dscLibrary, &dscModel, 0, 0);

        std::vector<mdsplus::DevicePart> parts = DeviceClass::GetParts();

        _TreeStartConglomerate(tree->getDBID(), parts.size() + 1);
        _TreeAddNode(tree->getDBID(), name.c_str(), &nid, TreeUSAGE_DEVICE);
        _TreePutRecord(tree->getDBID(), nid, (mdsdsc_t *)&dscConglom, 0);

        DeviceClass device(tree, nid);
        device.addParts(std::move(parts));

        _TreeEndConglomerate(tree->getDBID());

        return device;
    }

    static std::vector<DevicePart> GetParts() {
        return {};
    }

    void addParts(std::vector<DevicePart>&& parts) const;

};

// TODO: Error handling

#define MDSPLUS_DEVICE_BOOTSTRAP(LibraryName, DeviceClass, DeviceClassLower) \
    static inline const char * __library_name = #LibraryName;                \
    static inline const char * __class_name = #DeviceClass;                  \
    static inline const char * __class_name_lower = #DeviceClassLower;       \
                                                                             \
    DeviceClass(Tree * tree, int nid)                                        \
        : Device(tree, nid)                                                  \
    { }                                                                      \
    virtual ~DeviceClass() = default;                                        \

#define MDSPLUS_DEVICE(DeviceClassLower, DeviceClass)               \
    extern "C" int DeviceClassLower##__add(                         \
        mdsdsc_t * dscName,                                         \
        mdsdsc_t * dscDummy,                                        \
        int * nidOut)                                               \
    {                                                               \
        (void)dscDummy;                                             \
        Tree tree = mdsplus::Tree::GetActive();                     \
        std::string name(dscName->pointer, dscName->length);        \
        const auto& device = Device::Add<DeviceClass>(&tree, name); \
        if (nidOut) {                                               \
            *nidOut = device.getNID();                              \
        }                                                           \
        return TreeSUCCESS;                                         \
    }

#define MDSPLUS_DEVICE_METHOD(DeviceClassLower, DeviceClass, MethodName) \
    extern "C" int DeviceClassLower##__##MethodName(mdsdsc_t * nid)      \
    {                                                                    \
        mdsplus::Tree tree = mdsplus::Tree::GetActive();                 \
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