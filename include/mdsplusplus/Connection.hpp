#ifndef MDSPLUS_CONNECTION_HPP
#define MDSPLUS_CONNECTION_HPP

#include <string>
#include <vector>

#include "Data.hpp"
#include "Array.hpp"

namespace mdsplus {

class Connection
{
public:

    static constexpr int InvalidConnectionID = -1;

    inline Connection(const std::string& hostspec) {
        connect(hostspec);
    }

    inline virtual ~Connection() {
        disconnect();
    }

    void connect(const std::string& hostspec);

    void disconnect();

    inline void reconnect() {
        disconnect();
        connect(_hostspec);
    }

    template <typename T = Data, typename ...Args>
    T get(const std::string& expression, Args ...args) const
    {
        if (_local) {
            return Data::Execute<T>(expression, args...);
        }
        else {
            std::vector<Argument> argList({ Argument(args)... });
            return _get(expression, std::move(argList)).releaseAndConvert<T>();
        }
    }

    template <typename T = Data, typename ...Args>
    T getObject(const std::string& expression, Args ...args) const
    {
        UInt8Array serializedData = get<UInt8Array>("serializeout(`(" + expression + ";))", args...);
        return serializedData.deserialize<T>();
    }

    inline void openTree(const std::string& tree, int shot) const {
        int status = get<Int32>("TreeOpen($,$)", tree, shot).getValue();
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    inline void closeTree(const std::string& tree, int shot) const {
        int status = get<Int32>("TreeClose($,$)", tree, shot).getValue();
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    inline int closeAllTrees() const {
        return get<Int32>("_i=0;WHILE(IAND(TreeClose(),1)) _i++;_i").getValue();
    }

    inline void setDefault(const std::string& path) {
        get("TreeSetDefault($)", path);
    }

    template <typename ...Args>
    void put(const std::string& node, const std::string& expression, const Args& ...args) const
    {
        std::string putExpression = "TreePut($,$";
        for (size_t i = 0; i < sizeof...(args) - 2; ++i) {
            putExpression += ",$";
        }
        putExpression += ")";
        
        get(putExpression, node, expression, args...);
    }

    // getMany
    // putMany

private:

    Data _get(const std::string& expression, std::vector<Argument>&& xdArgs) const;

    int _id = InvalidConnectionID;

    std::string _hostspec;

    bool _local = false;

}; // class Connection

} // namespace mdsplus

#endif // MDSPLUS_CONNECTION_HPP