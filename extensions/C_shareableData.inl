
///SharedData

inline SharedData::SharedData(void* ptr, std::type_info const& typeInfo) :
        g_ptr(ptr),
        g_typeIndex(typeInfo)
{}

template<class T>
std::pair<std::unique_lock<std::mutex>, T*> SharedData::acquirePointer()
{
    std::unique_lock<std::mutex> lock{this->g_mutex};
    if (std::type_index{typeid(T)} == this->g_typeIndex)
    {
        return {std::move(lock), reinterpret_cast<T*>(this->g_ptr)};
    }
    return {std::unique_lock<std::mutex>{}, nullptr};
}

template<class T>
bool SharedData::acquirePointerAndThen(std::function<void(T*)> const& function)
{
    std::scoped_lock<std::mutex> lock{this->g_mutex};
    if (std::type_index{typeid(T)} == this->g_typeIndex)
    {
        if (function)
        {
            function(reinterpret_cast<T*>(this->g_ptr));
            return true;
        }
    }
    return false;
}

template<class T, class ...TArgs>
bool SharedData::acquirePointerAndCall(TArgs... args)
{
    std::scoped_lock<std::mutex> lock{this->g_mutex};
    if (std::type_index{typeid(T)} == this->g_typeIndex)
    {
        auto* function = reinterpret_cast<T*>(this->g_ptr);

        if (*function)
        {
            (*function)(args...);
            return true;
        }
    }
    return false;
}

inline void SharedData::invalidate()
{
    std::scoped_lock<std::mutex> lock{this->g_mutex};
    this->g_ptr = nullptr;
    this->g_typeIndex = typeid(std::nullptr_t);
}
inline bool SharedData::isValid() const
{
    std::scoped_lock<std::mutex> lock{this->g_mutex};
    return this->g_ptr != nullptr;
}

///ShareableData

inline void ShareableData::clear()
{
    std::scoped_lock<std::mutex> lock(this->g_mutex);
    this->g_data.clear();
}

template<typename T>
std::shared_ptr<SharedData> ShareableData::add(T* data, std::string name)
{
    std::scoped_lock<std::mutex> lock(this->g_mutex);

    auto tuple = this->g_data.insert({std::move(name), std::make_shared<SharedData>(data, typeid(T))});
    if (tuple.second)
    {
        this->g_cv.notify_all();
        return tuple.first->second;
    }
    return nullptr;
}

inline std::shared_ptr<SharedData> ShareableData::get(std::string_view name)
{
    std::scoped_lock<std::mutex> lock(this->g_mutex);
    auto it = this->g_data.find(name);
    if (it != this->g_data.end())
    {
        return it->second;
    }
    return nullptr;
}
inline std::shared_ptr<SharedData> ShareableData::waitFor(std::string_view name, std::chrono::milliseconds timeout)
{
    std::unique_lock<std::mutex> lock(this->g_mutex);
    bool result = this->g_cv.wait_for(lock, timeout, [&](){
        auto it = this->g_data.find(name);
        if (it != this->g_data.end())
        {
            return true;
        }
        return false;
    });

    if (result)
    {
        return this->g_data.find(name)->second;
    }
    return nullptr;
}

inline void ShareableData::remove(const std::string_view& name)
{
    std::scoped_lock<std::mutex> lock(this->g_mutex);
    auto it = this->g_data.find(name);
    if (it != this->g_data.end())
    {
        it->second->invalidate();
        this->g_data.erase(it);
    }
}

inline std::size_t ShareableData::getSize() const
{
    std::scoped_lock<std::mutex> lock(this->g_mutex);
    return this->g_data.size();
}
