#include <mdsplusplus/Device.hpp>

namespace mdsplus {

void Device::addParts(std::vector<DevicePart>&& parts) const
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

        for (auto& [key, value] : part.XNCI) {
            if (value) {
                delete value;
                value = nullptr;
            }
        }
    }

    getTree()->setDefaultNode(defaultNode);
}

} // namespace mdsplus