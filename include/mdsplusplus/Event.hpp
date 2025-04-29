





// class Event
// {
// public:

//     static void SetEventRaw(std::string name, size_t size, const uint8_t * buffer);

//     inline static void SetEvent(std::string name, const Data& data) {
//         // data.
//         return SetEventRaw(name, 0, nullptr);
//     }

//     inline static void SetEvent(std::string name) {
//         return SetEventRaw(name, 0, nullptr);
//     }

//     Event(std::string name)
//         : _name(name)
//     { }

//     virtual ~Event();

//     inline std::string getName() const {
//         return _name;
//     }

//     template <typename T = Data>
//     [[nodiscard]]
//     inline T getData() const {

//     }

// private:

//     std::string _name;

// }; // class Event
