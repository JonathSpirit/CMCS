#ifndef _CMCS_SHAREABLEDATA_HPP_INCLUDED
#define _CMCS_SHAREABLEDATA_HPP_INCLUDED

#include <string>
#include <typeinfo>
#include <typeindex>
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
    SharedData(void* ptr, std::type_info const& typeInfo);
    SharedData(SharedData const& r) = delete;
    SharedData(SharedData&& r) noexcept = delete;
    ~SharedData() = default;

    SharedData& operator=(SharedData const& r) = delete;
    SharedData& operator=(SharedData&& r) noexcept = delete;

    template<class T>
    std::pair<std::unique_lock<std::mutex>, T*> acquirePointer();

    template<class T>
    bool acquirePointerAndThen(std::function<void(T*)> const& function);

    template<class T, class ...TArgs>
    bool acquirePointerAndCall(TArgs... args);

    void invalidate();
    bool isValid() const;

private:
    void* g_ptr;
    std::type_index g_typeIndex;
    mutable std::mutex g_mutex;
};

class ShareableData
{
public:
    ShareableData() = default;
    ShareableData(ShareableData const& r) = delete;
    ShareableData(ShareableData&& r) noexcept = delete;
    ~ShareableData() = default;

    ShareableData& operator=(ShareableData const& r) = delete;
    ShareableData& operator=(ShareableData&& r) noexcept = delete;

    void clear();

    template<typename T>
    std::shared_ptr<SharedData> add(T* data, std::string name);

    std::shared_ptr<SharedData> get(std::string_view name);
    std::shared_ptr<SharedData> waitFor(std::string_view name, std::chrono::milliseconds timeout=std::chrono::milliseconds{2000});

    void remove(const std::string_view& name);

    std::size_t getSize() const;

private:
    std::map<std::string, std::shared_ptr<SharedData>, std::less<>> g_data;
    mutable std::mutex g_mutex;
    mutable std::condition_variable g_cv;
};

#include "C_shareableData.inl"

}//end cmcs

#endif // _CMCS_SHAREABLEDATA_HPP_INCLUDED
