#include "simul_ext.h"
#include "simul_core.hpp"

#include <vector>
#include <chrono>
#include <cmath>
#include <string>
#include <windows.h>
#include <FastEngine/fastengine_includes.h>

#define KMCS_EXT_MAXSIGNAL 4
/// JAUNE BLEU "ROSE" VERT

using namespace std;

typedef std::chrono::duration<double, std::ratio<1ll, 1000ll> > millisec_t;

namespace
{
    vector<bool> _osc_isset(KMCS_EXT_MAXSIGNAL);
    vector<unsigned char> _osc_portcible(KMCS_EXT_MAXSIGNAL);
    vector<unsigned char> _osc_bitcible(KMCS_EXT_MAXSIGNAL);
    vector<double> _osc_th_before(KMCS_EXT_MAXSIGNAL);
    vector<double> _osc_tb_before(KMCS_EXT_MAXSIGNAL);
    vector<bool> _osc_before(KMCS_EXT_MAXSIGNAL);

    vector<chrono::high_resolution_clock::time_point> _osc_time(KMCS_EXT_MAXSIGNAL);
    vector<sf::Color> _signal_color(KMCS_EXT_MAXSIGNAL);

    sf::Clock _si_osc_clk;
    sf::Texture _tex_temp_signal;
    sf::Image _si_osc_signal;
    sf::Sprite objo_spr_signal;

    double GetFloatPrecision(double value, double precision)
    {
        return (floor((value * pow(10, precision) + 0.5)) / pow(10, precision));
    }
    std::string ValueToScience (float value)
    {
        unsigned int i;
        char down[5] = {'m', 'u', 'n', 'p', '?'};
        char up[5] = {'k', 'M', 'G', 'T', '?'};
        if (value < 1)
        {
            value*=1000;
            for (i=0;i<4;i++)
            {
                if (value < 1)
                {
                    value*=1000;
                    continue;
                }
                break;
            }
            return fge::string::ToStr( GetFloatPrecision(value, 6) ) + '['+down[i];
        }
        if (value > 999)
        {
            value/=1000;
            for (i=0;i<4;i++)
            {
                if (value > 999)
                {
                    value/=1000;
                    continue;
                }
                break;
            }
            return fge::string::ToStr(GetFloatPrecision(value, 6))+'['+up[i];
        }
        return fge::string::ToStr(GetFloatPrecision(value, 6))+'[';
    }
}

sf::Color simulext_signalcolour(unsigned char _pcible , unsigned char _bcible)
{
    unsigned int i;
    for (i=0;i<_osc_portcible.size();++i)
    {
        if ((_osc_portcible[i] == _pcible) && (_osc_bitcible[i] == _bcible))
        {
            return _signal_color[i];
        }
    }
    return sf::Color(255,255,255,0);
}

bool simulext_iscible(unsigned char _pcible , unsigned char _bcible)
{
    unsigned int i;
    for (i=0;i<_osc_portcible.size();++i)
    {
        if ((_osc_portcible[i] == _pcible) && (_osc_bitcible[i] == _bcible))
        {
            return true;
        }
    }
    return false;
}

bool simulext_delcible(unsigned char _pcible , unsigned char _bcible)
{
    unsigned int i;
    for (i=0;i<_osc_portcible.size();++i)
    {
        if ((_osc_portcible[i] == _pcible) && (_osc_bitcible[i] == _bcible))
        {
            _osc_portcible[i] = 0;
            _osc_bitcible[i] = 8;
            _osc_isset[i] = false;
            return true;
        }
    }
    return false;
}

bool simulext_setcible(unsigned char _pcible , unsigned char _bcible)
{
    unsigned int i;

    if (simulext_iscible(_pcible ,_bcible))
    {
        return false;
    }

    for (i=0;i<_osc_portcible.size();++i)
    {
        if (!_osc_isset[i])
        {
            _osc_isset[i] = true;
            _osc_portcible[i] = _pcible;
            _osc_bitcible[i] = _bcible;
            return true;
        }
    }
    return false;
}

void simulext_update(sf::RenderWindow* ren_window, sf::RenderWindow* ren_oscillo)
{
    unsigned char pi;
    unsigned int i;
    unsigned int a;
    bool actual_value;
    unsigned int osc_pos_actuel_x=0;

    bool power_button_flag(false);
    bool power_button(true);
    sf::Sprite osc_sprite_power;
    sf::Sprite osc_sprite_back;
    sf::Text osc_text_time;
    sf::Font osc_font;

    vector<unsigned int> signal_pos(KMCS_EXT_MAXSIGNAL);

    osc_font.loadFromFile(KMCS_MAIN_FONT);

    signal_pos[0] = 255;
    signal_pos[1] = 191;
    signal_pos[2] = 127;
    signal_pos[3] = 63;

    _signal_color[0] = sf::Color(0,255,255);
    _signal_color[1] = sf::Color(0,0,255);
    _signal_color[2] = sf::Color(0,255,0);
    _signal_color[3] = sf::Color(255,0,255);

    _si_osc_signal.create(360,256,sf::Color(0,0,0,0)); //320/256

    osc_sprite_power.setTexture( fge::texture::Get("power_on") );
    osc_sprite_power.setPosition(278,348);
    osc_sprite_back.setTexture( fge::texture::Get("oscilloscope") );

    osc_text_time.setCharacterSize(11);
    osc_text_time.setFont(osc_font);

    while (ren_window->isOpen())
    {
        ren_oscillo->clear(sf::Color(50,50,50));

        if ( 1 /**KMCS_IsPressed(2,2,278,348,28,28,false,ren_oscillo)**/ )
        {
            if (!power_button_flag)
            {
                power_button_flag=true;
                power_button^=1;
            }
        }
        else
        {
            power_button_flag=false;
        }

        if (osc_pos_actuel_x >= ren_oscillo->getSize().x)
        {
            osc_pos_actuel_x = 0;
        }

        if (power_button)
        {
            for (i=0;i<=255;++i)
            {
                for (a=0;a<=21;++a)
                {
                    _si_osc_signal.setPixel(osc_pos_actuel_x+a, i, sf::Color(0,0,0,0));
                }
            }
        }

        for (pi=0;pi<_osc_portcible.size();++pi)
        {
            actual_value = (bool)( simul::core::GetPort(_osc_portcible[pi])&(1<<_osc_bitcible[pi]) );

            osc_text_time.setString( "Th : "+fge::string::ToStr(_osc_th_before[pi])+"[ms] Tb : "+fge::string::ToStr(_osc_tb_before[pi])+"[ms] Freq : "+ValueToScience(1/(_osc_th_before[pi]*pow(10,-3)+_osc_tb_before[pi]*pow(10,-3)+0.01f))+"Hz]" );
            osc_text_time.setPosition(0,260+22*pi);
            osc_text_time.setFillColor(_signal_color[pi]);

            if (power_button)
            {
                if (actual_value)
                {//haut
                    if (_osc_before[pi])
                    {//ligne droite haut
                        _si_osc_signal.setPixel(osc_pos_actuel_x, signal_pos[pi]-32, _signal_color[pi]);
                    }
                    else
                    {//passage vers le haut
                        auto timeclk_now = chrono::_V2::high_resolution_clock::now();
                        millisec_t clkDuration(chrono::duration_cast<millisec_t>(timeclk_now - _osc_time[pi]));

                        _osc_tb_before[pi] = clkDuration.count();
                        _osc_time[pi] = std::chrono::_V2::high_resolution_clock::now();
                        for (i=signal_pos[pi];i>=signal_pos[pi]-32;--i)
                        {
                            _si_osc_signal.setPixel(osc_pos_actuel_x, i, _signal_color[pi]);
                        }
                    }
                }
                else
                {//bas
                    if (!_osc_before[pi])
                    {//ligne droite bas
                        _si_osc_signal.setPixel(osc_pos_actuel_x, signal_pos[pi], _signal_color[pi]);
                    }
                    else
                    {//passage vers le bas
                        auto timeclk_now = chrono::_V2::high_resolution_clock::now();
                        millisec_t clkDuration(chrono::duration_cast<millisec_t>(timeclk_now - _osc_time[pi]));

                        _osc_th_before[pi] = clkDuration.count();
                        _osc_time[pi] = std::chrono::_V2::high_resolution_clock::now();
                        for (i=signal_pos[pi]-32;i<=signal_pos[pi];++i)
                        {
                            _si_osc_signal.setPixel(osc_pos_actuel_x, i, _signal_color[pi]);
                        }
                    }
                }
            }

            _osc_before[pi] = actual_value;
            ren_oscillo->draw(osc_text_time);
        }
        osc_pos_actuel_x = power_button ? (osc_pos_actuel_x+1) : 0;

        osc_sprite_power.setTexture((power_button ? fge::texture::Get("power_on") : fge::texture::Get("power_off")));

        _tex_temp_signal.loadFromImage(_si_osc_signal);
        objo_spr_signal.setTexture(_tex_temp_signal);
        ren_oscillo->setPosition(ren_window->getPosition() - sf::Vector2i(330,0));
        ren_oscillo->draw(osc_sprite_power);
        ren_oscillo->draw(osc_sprite_back);
        ren_oscillo->draw(objo_spr_signal);
        ren_oscillo->display();
    }//end while
}
