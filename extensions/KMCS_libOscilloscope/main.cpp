#include <SFML/Graphics.hpp>
#include <FastEngine/C_event.h>
#include <FastEngine/extra_random.h>
#include <FastEngine/texture_manager.h>
#include "C_bit.inl"

#include <iostream>
#include <chrono>

#define API __declspec(dllexport)

#define KMCS_EXT_MAXSIGNAL 2

#define KMCS_MAIN_FONT_PATH "data/fonts/arial.ttf"
#define KMCS_MAIN_FONT "default"
#define KMCS_CORE_VERSION "V0.3"
#define KMCS_CORE_MAXPORTS 11

using namespace std;

struct ExtensionIdentity
{
    std::string _name;
    std::string _version;
    std::string _author;
};

struct ExtensionData
{
    std::string* _C8051_device;
    std::vector<unsigned char>* _C8051_dataflash;
    std::vector<unsigned char>* _C8051_dataram;
    simul::BitBank<unsigned char>* _C8051_databit;
    std::vector<unsigned char>* _C8051_dataxram;
    unsigned short* _C8051_programCounter; //PC
    unsigned short* _C8051_cursor;

    unsigned char** _C8051_stackPointer; //SP
    unsigned short** _C8051_dptr; //DPTR
    unsigned char** _C8051_registerBank; //Rn
    unsigned char** _C8051_accumulator; //A
    unsigned char** _C8051_b; //B
    simul::Bit<unsigned char>* _C8051_psw; //PSW
};

typedef void (*CoreFunc_SetPortNum)(unsigned char);
typedef unsigned char& (*CoreFunc_GetPort)(unsigned char);
typedef unsigned char& (*CoreFunc_GetPortType)(unsigned char);
typedef unsigned char& (*CoreFunc_GetPortHide)(unsigned char);

typedef ExtensionIdentity (*FunctionInitCall)( const ExtensionData& );
typedef void (*FunctionUninitCall)(void);
typedef bool (*FunctionUpdateCall)( sf::RenderWindow&, fge::Event&);

ExtensionData _coreData;
sf::RenderWindow _myScreen;

struct OscSignal
{
    std::string _name;
    sf::Color _color;
    simul::Bit<unsigned char> _data;
    unsigned int _posy;

    bool _valueBefore;

    sf::Clock _timing;
    sf::Time _timingUp;
    sf::Time _timingDown;
};

sf::Texture osc_graphTexture;
sf::Image osc_graphImg;
//osc_graphImg.create(320,340, sf::Color::Transparent);
sf::Vector2u osc_graphPos;
std::vector<OscSignal> osc_target;

sf::Sprite osc_spriteBackground;
//osc_spriteBackground.setTexture( fge::texture::Get("oscilloscope") );
sf::Sprite osc_spritePower;
//osc_spritePower.setTexture( fge::texture::Get("power_on") );
//osc_spritePower.setPosition(278,348);

sf::Text osc_textTime;
//osc_textTime.setCharacterSize(11);

ExtensionIdentity API KMCSlib_Init( const ExtensionData& data )
{
    _myScreen.create( sf::VideoMode(320,340), "Oscilloscope", sf::Style::Close );
    _myScreen.setActive(false);
    fge::texture::Load("oscilloscope", "libOscilloscope/osc.png");
    fge::texture::Load("power_on", "libOscilloscope/PowerOn.png");

    _coreData = data;
    osc_graphImg.create(320,340, sf::Color::Transparent);
    osc_spriteBackground.setTexture( fge::texture::Get("oscilloscope") );
    osc_spritePower.setTexture( fge::texture::Get("power_on") );
    osc_spritePower.setPosition(278,348);

    osc_target.push_back( {"test1", sf::Color::Red, simul::Bit<unsigned char>(&((*_coreData._C8051_dataram)[0xA0]), 4), 64} );
    osc_target.back()._valueBefore = osc_target.back()._data;

    return {"Oscilloscope", "V1", "Guillaume G."};
}

bool API KMCSlib_Update( sf::RenderWindow& main_screen, fge::Event& evt )
{
    _myScreen.setActive(true);

    for (unsigned int i=0; i<osc_target.size(); ++i)
    {
        bool actualValue = osc_target[i]._data;

        osc_textTime.setString("Wouaouh");
        //osc_textTime.setString( (std::string)"Th : "+/*fge::string::ToStr(_osc_th_before[pi])+*/"[ms] Tb : "+/*fge::string::ToStr(_osc_tb_before[pi])+*/"[ms] Freq : "+/*ValueToScience(1/(_osc_th_before[pi]*pow(10,-3)+_osc_tb_before[pi]*pow(10,-3)+0.01f))+*/"Hz]" );
        //osc_textTime.setPosition(0,260+22*pi);
        //osc_textTime.setFillColor(_signal_color[pi]);

        for (unsigned int y=0; y<340; ++y)
        {
            for (unsigned int x=0; x<6; ++x)
            {
                osc_graphImg.setPixel((osc_graphPos.x+x)%320, y, sf::Color::Transparent);
            }
        }

        if (actualValue)
        {//haut
            if (osc_target[i]._valueBefore)
            {//ligne droite haut
                osc_graphImg.setPixel(osc_graphPos.x, osc_target[i]._posy-20, osc_target[i]._color);
            }
            else
            {//passage vers le haut
                //auto timeclk_now = chrono::_V2::high_resolution_clock::now();
                //millisec_t clkDuration(chrono::duration_cast<millisec_t>(timeclk_now - _osc_time[pi]));

                /*_osc_tb_before[pi] = clkDuration.count();
                _osc_time[pi] = std::chrono::_V2::high_resolution_clock::now();*/
                for (unsigned int y=0; y<20; ++y)
                {
                    osc_graphImg.setPixel(osc_graphPos.x, osc_target[i]._posy-y, osc_target[i]._color);
                }
            }
        }
        else
        {//bas
            if (!osc_target[i]._valueBefore)
            {//ligne droite bas
                osc_graphImg.setPixel(osc_graphPos.x, osc_target[i]._posy, osc_target[i]._color);
            }
            else
            {//passage vers le bas
                //auto timeclk_now = chrono::_V2::high_resolution_clock::now();
                //millisec_t clkDuration(chrono::duration_cast<millisec_t>(timeclk_now - _osc_time[pi]));

                /*_osc_th_before[pi] = clkDuration.count();
                _osc_time[pi] = std::chrono::_V2::high_resolution_clock::now();*/
                for (unsigned int y=0; y<20; ++y)
                {
                    osc_graphImg.setPixel(osc_graphPos.x, osc_target[i]._posy-20+y, osc_target[i]._color);
                }
            }
        }

        osc_target[i]._valueBefore = actualValue;
    }
    ++osc_graphPos.x;
    osc_graphPos.x = osc_graphPos.x%320;

    //_myScreen.setPosition(_myScreen.getPosition() - sf::Vector2i(330,0));

    _myScreen.clear();

    _myScreen.draw(osc_spriteBackground);
    osc_graphTexture.loadFromImage(osc_graphImg);
    sf::Sprite osc_graphSprite;
    osc_graphSprite.setTexture(osc_graphTexture);
    _myScreen.draw(osc_graphSprite);

    _myScreen.display();

    _myScreen.setActive(false);
    return true;
}

void API KMCSlib_Uninit()
{
    _myScreen.close();
}
