
///Bits

template<class T>
Bits<T>::Bits(T* data, std::size_t pos) :
    g_data(data),
    g_pos(pos)
{
}

template<class T>
Bits<T>& Bits<T>::setPos(std::size_t pos)
{
    this->g_pos = pos;
    return *this;
}
template<class T>
std::size_t Bits<T>::getPos() const
{
    return this->g_pos;
}

template<class T>
bool Bits<T>::getBit(std::size_t pos) const
{
    return ((*this->g_data) & (static_cast<T>(1)<<pos)) != 0;
}
template<class T>
bool Bits<T>::getBit() const
{
    return ((*this->g_data) & (static_cast<T>(1)<<this->g_pos)) != 0;
}

template<class T>
void Bits<T>::setData(T* data)
{
    this->g_data = data;
}

template<class T>
const T* Bits<T>::getData() const
{
    return this->g_data;
}
template<class T>
T* Bits<T>::getData()
{
    return this->g_data;
}

template<class T>
Bits<T>::operator bool() const
{
    return ((*this->g_data) & (static_cast<T>(1)<<this->g_pos)) != 0;
}

template<class T>
bool Bits<T>::operator=(const Bits<T>& _bit)
{
    (*this->g_data) &=~ (1<<this->g_pos);
    if (_bit)
    {
        (*this->g_data) |= (1<<this->g_pos);
    }
    return *this;
}
template<class T>
bool Bits<T>::operator^=(const Bits<T>& _bit)
{
    (*this->g_data) ^= ((_bit?1:0)<<this->g_pos);
    return this->operator bool();
}
template<class T>
bool Bits<T>::operator|=(const Bits<T>& _bit)
{
    (*this->g_data) |= ((_bit?1:0)<<this->g_pos);
    return this->operator bool();
}
template<class T>
bool Bits<T>::operator&=(const Bits<T>& _bit)
{
    (*this->g_data) &= ((_bit?1:0)<<this->g_pos);
    return this->operator bool();
}

template<class T>
bool Bits<T>::operator=(bool _bit)
{
    (*this->g_data) &=~ (1<<this->g_pos);
    if (_bit)
    {
        (*this->g_data) |= (1<<this->g_pos);
    }
    return *this;
}
template<class T>
bool Bits<T>::operator^=(bool _bit)
{
    (*this->g_data) ^= ((_bit?1:0)<<this->g_pos);
    return this->operator bool();
}
template<class T>
bool Bits<T>::operator|=(bool _bit)
{
    (*this->g_data) |= ((_bit?1:0)<<this->g_pos);
    return this->operator bool();
}
template<class T>
bool Bits<T>::operator&=(bool _bit)
{
    (*this->g_data) &= ((_bit?1:0)<<this->g_pos);
    return this->operator bool();
}

///BitsBank

template<class T>
void BitBank<T>::clear()
{
    this->g_data.clear();
}

template<class T>
void BitBank<T>::reserve(std::size_t n)
{
    this->g_data.reserve(n);
}
template<class T>
void BitBank<T>::setSize(std::size_t n)
{
    this->g_data.resize(n);
}
template<class T>
std::size_t BitBank<T>::getSize() const
{
    return this->g_data.size();
}

template<class T>
Bits<T>& BitBank<T>::getBit(std::size_t bitIndex)
{
    return this->g_data[bitIndex/(sizeof(T)*8)].setPos(bitIndex%(sizeof(T)*8));
}
template<class T>
Bits<T>& BitBank<T>::getData(std::size_t index)
{
    return this->g_data[index];
}
template<class T>
Bits<T>& BitBank<T>::addData(const Bits<T>& bit)
{
    this->g_data.push_back(bit);
    return this->g_data.back();
}

