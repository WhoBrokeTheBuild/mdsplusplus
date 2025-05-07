#ifndef MDSPLUS_CONNECTION_HPP
#define MDSPLUS_CONNECTION_HPP

#include <string>
#include <vector>

#include "Data.hpp"
#include "DataView.hpp"
#include "Scalar.hpp"
#include "Array.hpp"
#include "APD.hpp"

namespace mdsplus {

class GetMany;
class PutMany;

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

    template <typename ResultType = Data, typename ...ArgTypes>
    ResultType get(const std::string& expression, const ArgTypes& ...args) const
    {
        if (_local) {
            return Data::Execute<ResultType>(expression, args...);
        }
        else {
            std::vector<DataView> argList({ DataView(args)... });
            return _get(expression, std::move(argList)).releaseAndConvert<ResultType>();
        }
    }

    template <typename ResultType = Data, typename ...ArgTypes>
    ResultType getObject(const std::string& expression, ArgTypes ...args) const
    {
        Int8Array serializedData = get<Int8Array>("SerializeOut(`(" + expression + ";))", args...);
        return serializedData.deserialize<ResultType>();
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
    inline void put(const std::string& node, const std::string& expression, const Args& ...args) const
    {
        std::string putExpression = "TreePut($,$";
        for (size_t i = 0; i < sizeof...(args) - 2; ++i) {
            putExpression += ",$";
        }
        putExpression += ")";

        get(putExpression, node, expression, args...);
    }

    GetMany getMany() const;

    // putMany

private:

    Data _get(const std::string& expression, std::vector<DataView>&& xdArgs) const;

    int _id = InvalidConnectionID;

    std::string _hostspec;

    bool _local = false;

}; // class Connection

class GetMany
{
public:

    GetMany(const Connection * conn)
        : _conn(conn)
    { }

    template <typename ...ArgTypes>
    void append(const std::string& name, const std::string& expression, const ArgTypes& ...args)
    {
        // _queries.emplace_back(std::unordered_map<std::string, Data>{
        //     { "name", String(name) },
        //     { "exp", String(expression), },
        //     { "args", List(args...) },
        // });
        _queries.emplace_back(Dictionary(
            "name", name,
            "exp", expression,
            "args", List(args...)
        ));
    }

    const Dictionary& execute()
    {
        printf("%s\n", to_string(List(_queries).serialize()).c_str());
        Data result = _conn->get("GetManyExecute($)", List(_queries).serialize());

        if (result.getClass() == Class::S) {
            if (result.getDType() == DType::T) {
                // TODO: Read error
                throw MDSplusException();
            }
            else if (result.getDType() == DType::L) {
                int status = result.releaseAndConvert<Int32>().getValue();
                throwException(status);
            }
        }

        _result = result.releaseAndConvert<UInt8Array>().deserialize<Dictionary>();
        return _result;
    }

private:

    const Connection * _conn;

    // std::vector<std::unordered_map<std::string, Data>> _queries;

    std::vector<Dictionary> _queries;

    Dictionary _result;

}; // class GetMany

inline GetMany Connection::getMany() const {
    return GetMany(this);
}

} // namespace mdsplus

#endif // MDSPLUS_CONNECTION_HPP