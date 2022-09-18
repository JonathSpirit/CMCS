#ifndef _SHAREABLEDATA_HPP_INCLUDED
#define _SHAREABLEDATA_HPP_INCLUDED

#include <string>
#include <typeinfo>
#include <mutex>
#include <map>
#include <memory>

class SharedData
{
public:
    SharedData(void* ptr, const std::type_info& typeInfo);

    template<class T>
    std::pair<std::unique_lock<std::mutex>, T*> acquirePointer();

private:
    void* g_ptr;
    std::string g_typeName;
    std::size_t g_typeHash;
    std::mutex g_mutex;
};

class ShareableData
{
public:
    ShareableData() = default;
    ~ShareableData() = default;

    inline void clear();

    template<typename T>
    bool add(T* data, std::string name);

    std::shared_ptr<SharedData> get(const std::string_view& name);

    inline void remove(const std::string_view& name);

    inline std::size_t getSize() const;

private:
    std::map<std::string, std::shared_ptr<SharedData>, std::less<>> g_data;
    mutable std::mutex g_mutex;
};

#include "C_shareableData.inl"

#endif // _SHAREABLEDATA_H_INCLUDED
