#ifndef _CMCS_SHAREABLEDATA_HPP_INCLUDED
#define _CMCS_SHAREABLEDATA_HPP_INCLUDED

#include <string>
#include <typeinfo>
#include <mutex>
#include <map>
#include <memory>
#include <functional>
#include <condition_variable>

namespace cmcs
{

class SharedData
{
public:
    SharedData(void* ptr, const std::type_info& typeInfo);

    template<class T>
    std::pair<std::unique_lock<std::mutex>, T*> acquirePointer();

    template<class T>
    bool acquirePointerAndThen(const std::function<void(T*)>& function);

    void invalidate();
    bool isValid() const;

private:
    void* g_ptr;
    std::string g_typeName;
    std::size_t g_typeHash;
    mutable std::mutex g_mutex;
};

class ShareableData
{
public:
    ShareableData() = default;

    ~ShareableData() = default;

    inline void clear();

    template<typename T>
    std::shared_ptr<SharedData> add(T* data, std::string name);

    std::shared_ptr<SharedData> get(const std::string_view& name);
    std::shared_ptr<SharedData> waitFor(const std::string_view& name, const std::chrono::milliseconds& timeout=std::chrono::milliseconds{2000});

    inline void remove(const std::string_view& name);

    inline std::size_t getSize() const;

private:
    std::map<std::string, std::shared_ptr<SharedData>, std::less<>> g_data;
    mutable std::mutex g_mutex;
    mutable std::condition_variable g_cv;
};

#include "C_shareableData.inl"

}//end cmcs

#endif // _CMCS_SHAREABLEDATA_HPP_INCLUDED
