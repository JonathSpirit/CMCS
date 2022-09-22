#ifndef _CMCS_C_BIT_HPP_INCLUDED
#define _CMCS_C_BIT_HPP_INCLUDED

#include <vector>

namespace cmcs
{

template<class T>
class Bits
{
public:
    Bits(T* data=nullptr, std::size_t pos=0);
    ~Bits() = default;

    Bits<T>& setPos(std::size_t pos);
    [[nodiscard]] std::size_t getPos() const;

    [[nodiscard]] bool getBit(std::size_t pos) const;
    [[nodiscard]] bool getBit() const;

    void setData(T* data);

    const T* getData() const;
    T* getData();

    bool operator==(const Bits<T>& r) const;

    operator bool() const;

    Bits<T>& operator=(bool _bit);
    Bits<T>& operator^=(bool _bit);
    Bits<T>& operator|=(bool _bit);
    Bits<T>& operator&=(bool _bit);

private:
    T* g_data;
    std::size_t g_pos;
};

template<class T>
class BitBank
{
    static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
public:
    BitBank() = default;
    ~BitBank() = default;

    void clear();

    void reserve(std::size_t n);

    void setSize(std::size_t n);
    [[nodiscard]] std::size_t getSize() const;

    Bits<T>& getBit(std::size_t bitIndex);
    Bits<T>& getData(std::size_t index);
    Bits<T>& addData(const Bits<T>& bit);

private:
    std::vector<Bits<T> > g_data;
};

#include "C_bit.inl"

}//end cmcs

#endif // _CMCS_C_BIT_HPP_INCLUDED
