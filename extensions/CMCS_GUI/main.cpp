#include "CMCS_API.hpp"
#include "C_pin.hpp"

#include "C_text.hpp"
#include "C_sprite.hpp"
#include <replxx.hxx>

#include <string>
#include <vector>
#include <thread>

#define NUM_OF_PORTS 3

using namespace cmcs;

ShareableData* gCoreData;
std::thread* gThread;
bool gRunning;

void ThreadUpdate()
{
    replxx::Replxx* terminal;
    {
        auto terminalData = gCoreData->get("terminal")->acquirePointer<replxx::Replxx>();
        terminal = terminalData.second;
    }

    terminal->print(CMCS_EXT_PRINT_FORMAT("CMCS_GUI", "info", "hi from CMCS_GUI extension !\n"));

    auto* window = SDL_CreateWindow("CMCS - GUI", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, 512,512, SDL_WINDOW_OPENGL);
    auto* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT("CMCS_GUI", "error", "bad renderer: %s\n"), SDL_GetError());
    }

    draw::Texture textureLedOff;
    draw::Texture textureLedOn;
    draw::Texture textureSwitchOff;
    draw::Texture textureSwitchOn;

    if (!textureLedOff.loadBmp(renderer, "data/libGUI/images/LedOff.bmp"))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT("CMCS_GUI", "error", "bad texture: %s\n"), SDL_GetError());
    }
    if (!textureLedOn.loadBmp(renderer, "data/libGUI/images/LedOn.bmp"))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT("CMCS_GUI", "error", "bad texture: %s\n"), SDL_GetError());
    }
    if (!textureSwitchOff.loadBmp(renderer, "data/libGUI/images/SwitchOff.bmp"))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT("CMCS_GUI", "error", "bad texture: %s\n"), SDL_GetError());
    }
    if (!textureSwitchOn.loadBmp(renderer, "data/libGUI/images/SwitchOn.bmp"))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT("CMCS_GUI", "error", "bad texture: %s\n"), SDL_GetError());
    }

    draw::Font font;
    if (!font.loadFont(CMCS_MAIN_FONT_PATH, 16))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT("CMCS_GUI", "error", "bad font: %s\n"), SDL_GetError());
    }

    draw::Text textPort(renderer, font, "X");
    draw::Text textBits(renderer, font, "MSB                                                  LSB");
    draw::Text textAuthor(renderer, font, "Author: Guillaume G.");
    draw::Text textVersion(renderer, font, "Version: "+(std::string)CMCS_CORE_VERSION);

    textBits.setPosition({0.0f, 0.0f});
    textAuthor.setPosition({0.0f, 480.0f});
    textVersion.setPosition({0.0f, 496.0f});

    auto deviceData = gCoreData->get("C8051_data");

    auto data = deviceData->acquirePointer<device::C8051_data>();

    uint8_t* dataPorts[NUM_OF_PORTS] =
    {
        &data.second->_dataRam[0x80],
        &data.second->_dataRam[0x90],
        &data.second->_dataRam[0xA0]
    };
    uint8_t* dataPortOutputModes[NUM_OF_PORTS] =
    {
        &data.second->_dataRam[0xA4],
        &data.second->_dataRam[0xA5],
        &data.second->_dataRam[0xA6]
    };
    data.first.unlock();

    std::vector<std::vector<Pin> > pins;

    float posx=0.0f;
    float posy=20.0f;
    pins.resize(NUM_OF_PORTS);
    for (unsigned int i=0; i<NUM_OF_PORTS; ++i)
    {
        pins[i].reserve(8);
        for (unsigned int a=0; a<8; ++a)
        {
            pins[i].emplace_back(SDL_FPoint{posx, posy}, dataPorts[i], 7-a, dataPortOutputModes[i], 7-a);
            pins[i].back().setLedTextures(textureLedOn, textureLedOff);
            pins[i].back().setSwitchTextures(textureSwitchOn, textureSwitchOff);
            posx+=32.0f;
        }
        posy+=40.0f;
        posx=0.0f;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    uint64_t lastTime;
    uint64_t currentTime;

    while (gRunning)
    {
        SDL_Event event;
        while ( SDL_PollEvent(&event) )
        {
            if (event.type == SDL_QUIT)
            {
                gRunning=false;
            }
            for (unsigned int i=0; i<NUM_OF_PORTS; ++i)
            {
                for (unsigned int a=0; a<8; ++a)
                {
                    pins[i][a].update(event);
                }
            }
        }

        currentTime = SDL_GetTicks64();
        uint32_t remainingTime = std::clamp<int64_t>(static_cast<int64_t>(1000/60) - static_cast<int64_t>(currentTime-lastTime), 0, 1000/60);
        lastTime = currentTime;
        SDL_Delay(remainingTime);

        SDL_RenderClear(renderer);

        for (unsigned int i=0; i<pins.size(); ++i)
        {
            for (unsigned int a=0; a<pins[i].size(); ++a)
            {
                pins[i][a].draw(renderer);
            }

            textPort.setPosition({pins[i].back().getPosition().x + 32, pins[i].back().getPosition().y});
            textPort.setString(renderer, "P"+std::to_string(i));
            textPort.draw(renderer);
        }

        textBits.draw(renderer);
        textAuthor.draw(renderer);
        textVersion.draw(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

CMCS_API void CMCSlib_Init(ShareableData& data)
{
    gRunning = true;
    gCoreData = &data;
    gThread = new std::thread(ThreadUpdate);
}

CMCS_API ExtensionIdentity CMCSlib_retrieveExtensionIdentity()
{
    return {"CMCS_GUI", "V1.0", "Guillaume G."};
}

CMCS_API bool CMCSlib_Update()
{
    return true;
}

CMCS_API void CMCSlib_Uninit()
{
    gRunning = false;
    if (gThread->joinable())
    {
        gThread->join();
    }
    delete gThread;
}
