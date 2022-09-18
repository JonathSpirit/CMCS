#ifndef __PIN_HPP__
#define __PIN_HPP__

#include "C_sprite.hpp"
#include "C_bit.hpp"

class Pin
{
public:
    Pin(uint8_t* dataStat, std::size_t posStat,
        uint8_t* dataOutputMod, std::size_t posOutputMod);
    Pin(const SDL_FPoint& pos, uint8_t* dataStat, std::size_t posStat,
        uint8_t* dataOutputMod, std::size_t posOutputMod);

    void update(const SDL_Event& event);
    void draw(SDL_Renderer* renderer) const;

    void setPosition(const SDL_FPoint& pos);
    SDL_FPoint getPosition() const;

    void setLedTextures(const cmcs::draw::Texture& ledOn, const cmcs::draw::Texture& ledOff);
    void setSwitchTextures(const cmcs::draw::Texture& switchOn, const cmcs::draw::Texture& switchOff);

    void setStat(bool stat);
    bool getStat() const;

    void setOutputMod(bool mod);

private:
    mutable cmcs::draw::Sprite g_sprite;

    cmcs::draw::Texture g_textureLedOn;
    cmcs::draw::Texture g_textureLedOff;
    cmcs::draw::Texture g_textureSwitchOn;
    cmcs::draw::Texture g_textureSwitchOff;

    bool g_statMouseOn = false;
    simul::Bits<uint8_t> g_stat;
    simul::Bits<uint8_t> g_outputMod;

    bool g_flag = false;
};


#endif // __PIN_HPP__
