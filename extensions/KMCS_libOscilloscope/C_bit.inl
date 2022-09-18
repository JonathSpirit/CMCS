#ifndef C_BIT_H_INCLUDED
#define C_BIT_H_INCLUDED

#include <vector>

namespace simul
{

template<class T>
class Bit
{
public:
    Bit();
    Bit(T* data, unsigned char pos);
    ~Bit();

    Bit<T>& setBit(unsigned char pos);
    void setData(T* data);
    T* getData();

    operator bool() const;

    bool operator=(const Bit<T>& _bit);
    bool operator^=(const Bit<T>& _bit);
    bool operator|=(const Bit<T>& _bit);
    bool operator&=(const Bit<T>& _bit);

    bool operator=(bool _bit);
    bool operator^=(bool _bit);
    bool operator|=(bool _bit);
    bool operator&=(bool _bit);

private:
    T* g_data;
    unsigned char g_pos;
};

template<class T>
class BitBank
{
public:
    BitBank();
    ~BitBank();

    void clear();

    void reserve(unsigned int newSize);
    void setSize(unsigned int newSize);
    unsigned int getSize() const;

    Bit<T>& get(unsigned int bitIndex);
    Bit<T>& getData(unsigned int index);
    Bit<T>& addData();

private:
    std::vector<Bit<T> > g_data;
    unsigned char g_defaultData;
};

}//end simul

namespace simul
{

///Bit

template<class T>
Bit<T>::Bit()
{
    this->g_data = nullptr;
    this->g_pos = 0;
}
template<class T>
Bit<T>::Bit(T* data, unsigned char pos)
{
    this->g_data = data;
    this->g_pos = pos;
}
template<class T>
Bit<T>::~Bit()
{
}

template<class T>
Bit<T>& Bit<T>::setBit(unsigned char pos)
{
    this->g_pos = pos;
    return *this;
}
template<class T>
void Bit<T>::setData(T* data)
{
    this->g_data = data;
}
template<class T>
T* Bit<T>::getData()
{
    return this->g_data;
}

template<class T>
Bit<T>::operator bool() const
{
    return ((*this->g_data) & (1<<this->g_pos)) > 0;
}

template<class T>
bool Bit<T>::operator=(const Bit<T>& _bit)
{
    (*this->g_data) &=~ (1<<this->g_pos);
    if (_bit)
    {
        (*this->g_data) |= (1<<this->g_pos);
    }
    return *this;
}
template<class T>
bool Bit<T>::operator^=(const Bit<T>& _bit)
{
    (*this->g_data) ^= ((_bit?1:0)<<this->g_pos);
    return this->operator bool();
}
template<class T>
bool Bit<T>::operator|=(const Bit<T>& _bit)
{
    (*this->g_data) |= ((_bit?1:0)<<this->g_pos);
    return this->operator bool();
}
template<class T>
bool Bit<T>::operator&=(const Bit<T>& _bit)
{
    (*this->g_data) &= ((_bit?1:0)<<this->g_pos);
    return this->operator bool();
}

template<class T>
bool Bit<T>::operator=(bool _bit)
{
    (*this->g_data) &=~ (1<<this->g_pos);
    if (_bit)
    {
        (*this->g_data) |= (1<<this->g_pos);
    }
    return *this;
}
template<class T>
bool Bit<T>::operator^=(bool _bit)
{
    (*this->g_data) ^= ((_bit?1:0)<<this->g_pos);
    return this->operator bool();
}
template<class T>
bool Bit<T>::operator|=(bool _bit)
{
    (*this->g_data) |= ((_bit?1:0)<<this->g_pos);
    return this->operator bool();
}
template<class T>
bool Bit<T>::operator&=(bool _bit)
{
    (*this->g_data) &= ((_bit?1:0)<<this->g_pos);
    return this->operator bool();
}

///BitBank
template<class T>
BitBank<T>::BitBank()
{

}
template<class T>
BitBank<T>::~BitBank()
{

}

template<class T>
void BitBank<T>::clear()
{
    this->g_data.clear();
}

template<class T>
void BitBank<T>::reserve(unsigned int newSize)
{
    this->g_data.reserve(newSize);
}
template<class T>
void BitBank<T>::setSize(unsigned int newSize)
{
    this->g_data.resize(newSize);
}
template<class T>
unsigned int BitBank<T>::getSize() const
{
    return this->g_data.size();
}

template<class T>
Bit<T>& BitBank<T>::get(unsigned int bitIndex)
{
    return this->g_data[bitIndex/8].setBit(bitIndex%8);
}
template<class T>
Bit<T>& BitBank<T>::getData(unsigned int index)
{
    return this->g_data[index];
}
template<class T>
Bit<T>& BitBank<T>::addData()
{
    this->g_data.push_back(Bit<T>());
    return this->g_data.back();
}


}//end simul

#endif // C_BIT_H_INCLUDED
