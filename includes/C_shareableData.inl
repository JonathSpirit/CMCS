
///SharedData

inline SharedData::SharedData(void* ptr, const std::type_info& typeInfo) :
        g_ptr(ptr),
        g_typeName(typeInfo.name()),
        g_typeHash(typeInfo.hash_code())
{}

template<class T>
std::pair<std::unique_lock<std::mutex>, T*> SharedData::acquirePointer()
{
    std::unique_lock<std::mutex> lock{this->g_mutex};
    if (typeid(T).hash_code() == this->g_typeHash && typeid(T).name() == this->g_typeName)
    {
        return {std::move(lock), reinterpret_cast<T*>(this->g_ptr)};
    }
    return {std::unique_lock<std::mutex>{}, nullptr};
}

template<class T>
bool SharedData::acquirePointerAndThen(const std::function<void(T*)>& function)
{
    std::scoped_lock<std::mutex> lock{this->g_mutex};
    if (typeid(T).hash_code() == this->g_typeHash && typeid(T).name() == this->g_typeName)
    {
        if (function)
        {
            function(reinterpret_cast<T*>(this->g_ptr));
            return true;
        }
    }
    return false;
}

inline void SharedData::invalidate()
{
    std::scoped_lock<std::mutex> lock{this->g_mutex};
    this->g_ptr = nullptr;
    this->g_typeName = typeid(nullptr).name();
    this->g_typeHash = typeid(nullptr).hash_code();
}
inline bool SharedData::isValid() const
{
    std::scoped_lock<std::mutex> lock{this->g_mutex};
    return this->g_ptr != nullptr;
}

///ShareableData

void ShareableData::clear()
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

inline std::shared_ptr<SharedData> ShareableData::get(const std::string_view& name)
{
    std::scoped_lock<std::mutex> lock(this->g_mutex);
    auto it = this->g_data.find(name);
    if (it != this->g_data.end())
    {
        return it->second;
    }
    return nullptr;
}
inline std::shared_ptr<SharedData> ShareableData::waitFor(const std::string_view& name, const std::chrono::milliseconds& timeout)
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
