#ifndef MDSPLUS_DEVICE_INC_HPP
#define MDSPLUS_DEVICE_INC_HPP

#include "Device.hpp"

namespace mdsplus {

inline void Device::addParts(std::vector<DevicePart>&& parts) const
{
    auto defaultNode = getTree()->getDefaultNode();
    getTree()->setDefaultNode(*this);

    for (const auto& part : parts)
    {
        printf("Adding %s\n", part.Path.c_str());
        auto node = addNode(part.Path, part.Usage);
        
        if (!part.ValueExpression.empty()) {
            node.putRecord(getTree()->compileData(part.ValueExpression));
        }
        else {
            node.putRecord(*part.Value);
        }

        node.setFlagsOn(part.Flags);
    }

    for (auto& part : parts)
    {
        if (part.Value) {
            delete part.Value;
            part.Value = nullptr;
        }

        for (auto& it : part.XNCI) {
            if (it.second) {
                delete it.second;
                it.second = nullptr;
            }
        }
    }

    getTree()->setDefaultNode(defaultNode);
}

} // namespace mdsplus

#endif // MDSPLUS_DEVICE_INC_HPP