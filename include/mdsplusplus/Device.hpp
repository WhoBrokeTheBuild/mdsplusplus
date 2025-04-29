#ifndef MDSPLUS_DEVICE_HPP
#define MDSPLUS_DEVICE_HPP

#include "Data.hpp"
#include "Scalar.hpp"
#include "Array.hpp"
#include "Record.hpp"
#include "Tree.hpp"
#include "MDSplusException.hpp"

#include <functional>
#include <map>
#include <memory>

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

#define MDSPLUS_DEVICE(LibraryName, DeviceClass, DeviceClassLower) \
    extern "C" int DeviceClassLower##__add(mdsdsc_t * name_dsc, mdsdsc_t * dummy_dsc, int * nid_ptr) \
    { \
        (void)dummy_dsc; \
        int head_nid = 0; \
        static DESCRIPTOR(library_dsc, LibraryName); \
        static DESCRIPTOR(model_dsc, #DeviceClass); \
        static DESCRIPTOR_CONGLOM(device_dsc, &library_dsc, &model_dsc, 0, 0); \
        std::vector<mdsplus::DevicePart> parts = DeviceClass::GetParts(); \
        TreeStartConglomerate(parts.size() + 1); \
        std::string name(name_dsc->pointer, name_dsc->length); \
        TreeAddNode(name.c_str(), &head_nid, TreeUSAGE_DEVICE); \
        TreePutRecord(head_nid, (mdsdsc_t *)&device_dsc, 0); \
        Tree tree = mdsplus::Tree::GetActive(); \
        DeviceClass device(&tree, head_nid); \
        device.addParts(std::move(parts)); \
        TreeEndConglomerate(); \
        if (nid_ptr) { \
            *nid_ptr = head_nid; \
        } \
        return TreeSUCCESS; \
    }

#define MDSPLUS_DEVICE_METHOD(DeviceClassLower, DeviceClass, MethodName) \
    extern "C" int DeviceClassLower##__##MethodName(mdsdsc_t * nid) \
    { \
        Tree tree = mdsplus::Tree::GetActive(); \
        DeviceClass device(&tree, *(int *)nid->pointer); \
        try { \
            device.MethodName(); \
        } catch (mdsplus::MDSplusException& e) { \
            return e.getStatus(); \
        } \
        return TreeSUCCESS; \
    }

class SineWave : public Device
{
public:

    MDSPLUS_DEVICE_BOOTSTRAP(SineWave)

    static std::vector<DevicePart> GetParts() {
        std::vector<DevicePart> parts = {
            {
                .Path = ":COMMENT",
                .Usage = Usage::Any,
                .Value = new String("Test 123"),
                .Flags = {
                    .WriteOnce = true,
                    .NoWriteShot = true,
                },
                .XNCI = {
                    { "tooltip", new String("A comment") },
                }
            },
            {
                .Path = ":FREQUENCY",
                .Usage = Usage::Numeric,
                .Value = new WithUnits(1000.0f, "Hz"),
                .Flags = {
                    .NoWriteShot = true,
                },
                .Callback = [](TreeNode& node) {
                    node.setTheseFlags(TreeNodeFlags{
                        .Cached = true,
                    });
                    // node.setExtendedAttribute("tooltip", "The Frequency in Hz");
                },
                // .XNCI = {
                //     { "tooltip", String("The Frequency in Hz") },
                // }
            }
        };

        for (int i = 0; i < 32; ++i) {
            std::string path = ":INPUTS:INPUT_" + std::to_string(i);
            parts.push_back({
                .Path = path,
                // ...
            });
        }

        return parts;
    }

};

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