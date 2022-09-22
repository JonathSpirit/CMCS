#include "CMCS_API.hpp"
#include "C_pin.hpp"

#include "C_text.hpp"
#include "C_sprite.hpp"
#include <replxx.hxx>

#include <string>
#include <vector>
#include <thread>
#include <functional>

#define NUM_OF_PORTS 3

#define EXTENSION_NAME "CMCS_GUI"

using namespace cmcs;

ShareableData* gCoreData;
std::thread* gThread;
bool gRunning;
std::shared_ptr<SharedData> gPinPressedCallbacks;
using gPinPressedCallbacks_t = std::vector<std::function<void(uint8_t*, std::size_t, uint8_t*, std::size_t, SDL_MouseButtonEvent)> >;

void ThreadUpdate()
{
    replxx::Replxx* terminal;
    {
        auto terminalData = gCoreData->get("terminal")->acquirePointer<replxx::Replxx>();
        terminal = terminalData.second;
    }

    terminal->print(CMCS_EXT_PRINT_FORMAT(EXTENSION_NAME, "info", "hi from CMCS_GUI extension !\n"));

    auto* window = SDL_CreateWindow("CMCS - GUI", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, 512,512, SDL_WINDOW_OPENGL);
    auto* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT(EXTENSION_NAME, "error", "bad renderer: %s\n"), SDL_GetError());
    }

    draw::Texture textureLedOff;
    draw::Texture textureLedOn;
    draw::Texture textureSwitchOff;
    draw::Texture textureSwitchOn;

    if (!textureLedOff.loadBmp(renderer, "data/libGUI/images/LedOff.bmp"))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT(EXTENSION_NAME, "error", "bad texture: %s\n"), SDL_GetError());
    }
    if (!textureLedOn.loadBmp(renderer, "data/libGUI/images/LedOn.bmp"))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT(EXTENSION_NAME, "error", "bad texture: %s\n"), SDL_GetError());
    }
    if (!textureSwitchOff.loadBmp(renderer, "data/libGUI/images/SwitchOff.bmp"))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT(EXTENSION_NAME, "error", "bad texture: %s\n"), SDL_GetError());
    }
    if (!textureSwitchOn.loadBmp(renderer, "data/libGUI/images/SwitchOn.bmp"))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT(EXTENSION_NAME, "error", "bad texture: %s\n"), SDL_GetError());
    }

    draw::Font font;
    if (!font.loadFont(CMCS_MAIN_FONT_PATH, 16))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT(EXTENSION_NAME, "error", "bad font: %s\n"), SDL_GetError());
    }

    draw::Text textPort(renderer, font, "X");
    draw::Text textBits(renderer, font, "MSB                                                  LSB");
    draw::Text textAuthor(renderer, font, "Author: Guillaume G.");
    draw::Text textVersion(renderer, font, "Version: "+(std::string)CMCS_CORE_VERSION);

    textBits.setPosition({0.0f, 0.0f});
    textAuthor.setPosition({0.0f, 480.0f});
    textVersion.setPosition({0.0f, 496.0f});

    std::mutex extensionsCallbackMutex;

    auto deviceData = gCoreData->get("C8051_data");
    if (!deviceData)
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT(EXTENSION_NAME, "error", "can't retrieve device data !\n"));
        std::terminate();
    }

    std::array<uint8_t*, NUM_OF_PORTS> dataPorts = {nullptr};
    std::array<uint8_t*, NUM_OF_PORTS> dataPortOutputModes = {nullptr};

    deviceData->acquirePointerAndThen<device::C8051_data>([&](device::C8051_data* data){
        dataPorts = {
            &data->_dataRam[0x80],
            &data->_dataRam[0x90],
            &data->_dataRam[0xA0]
        };
        dataPortOutputModes = {
            &data->_dataRam[0xA4],
            &data->_dataRam[0xA5],
            &data->_dataRam[0xA6]
        };
    });

    std::vector<std::vector<Pin> > pins;

    float posx=0.0f;
    float posy=20.0f;
    pins.resize(NUM_OF_PORTS);
    for (std::size_t i=0; i<NUM_OF_PORTS; ++i)
    {
        pins[i].reserve(8);
        for (std::size_t a=0; a<8; ++a)
        {
            pins[i].emplace_back(SDL_FPoint{posx, posy}, dataPorts[i], 7-a, dataPortOutputModes[i], 7-a);
            pins[i].back().setLedTextures(textureLedOn, textureLedOff);
            pins[i].back().setSwitchTextures(textureSwitchOn, textureSwitchOff);
            posx+=32.0f;
        }
        posy+=40.0f;
        posx=0.0f;
    }

    std::function<void(Bits<uint8_t>, SDL_Color)> functionSetPinColor = [&](Bits<uint8_t> pin, SDL_Color color){
        std::scoped_lock<std::mutex> lock{extensionsCallbackMutex};
        for (std::size_t i=0; i<pins.size(); ++i)
        {
            for (std::size_t a=0; a<pins[i].size(); ++a)
            {
                if (pins[i][a].getBitsStat() == pin)
                {
                    pins[i][a].setColor(color);
                    return;
                }
            }
        }
    };
    std::function<void()> functionResetAllPinColor = [&](){
        std::scoped_lock<std::mutex> lock{extensionsCallbackMutex};
        for (std::size_t i=0; i<pins.size(); ++i)
        {
            for (std::size_t a=0; a<pins[i].size(); ++a)
            {
                pins[i][a].setColor({255,255,255,255});
            }
        }
    };

    gCoreData->add(&functionSetPinColor, "CMCS_GUI:functionSetPinColor");
    gCoreData->add(&functionResetAllPinColor, "CMCS_GUI:functionResetAllPinColor");

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

            extensionsCallbackMutex.lock();
            for (std::size_t i=0; i<pins.size(); ++i)
            {
                for (auto& pin : pins[i])
                {
                    pin.update(event);

                    if (pin.isMouseOn() && event.type == SDL_MOUSEBUTTONDOWN)
                    {//The button has been pressed
                        gPinPressedCallbacks->acquirePointerAndThen<gPinPressedCallbacks_t>([&](gPinPressedCallbacks_t* callbacks){
                            for (auto& callback : *callbacks)
                            {
                                auto& stat = pin.getBitsStat();
                                auto& outputMod = pin.getBitsOutputMod();

                                extensionsCallbackMutex.unlock();
                                callback(stat.getData(), stat.getPos(), outputMod.getData(), outputMod.getPos(), event.button);
                                extensionsCallbackMutex.lock();
                            }
                        });
                    }
                }
            }
            extensionsCallbackMutex.unlock();
        }

        currentTime = SDL_GetTicks64();
        uint32_t remainingTime = std::clamp<int64_t>(static_cast<int64_t>(1000/60) - static_cast<int64_t>(currentTime-lastTime), 0, 1000/60);
        lastTime = currentTime;
        SDL_Delay(remainingTime);

        SDL_RenderClear(renderer);

        extensionsCallbackMutex.lock();
        for (std::size_t i=0; i<pins.size(); ++i)
        {
            for (auto& pin : pins[i])
            {
                pin.draw(renderer);
            }

            textPort.setPosition({pins[i].back().getPosition().x + 32, pins[i].back().getPosition().y});
            textPort.setString(renderer, "P"+std::to_string(i));
            textPort.draw(renderer);
        }
        extensionsCallbackMutex.unlock();

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
    //Shared data with others extensions
    static gPinPressedCallbacks_t pinPressedCallbacks;
    gPinPressedCallbacks = data.add(&pinPressedCallbacks, "CMCS_GUI:pinPressedCallbacks");

    gRunning = true;
    gCoreData = &data;
    gThread = new std::thread(ThreadUpdate);
}

CMCS_API ExtensionIdentity CMCSlib_retrieveExtensionIdentity()
{
    return {EXTENSION_NAME, "V1.0", "Guillaume G."};
}

CMCS_API bool CMCSlib_Update()
{
    return true;
}

CMCS_API void CMCSlib_Uninit()
{
    gCoreData->remove("CMCS_GUI:pinPressedCallbacks");
    gCoreData->remove("CMCS_GUI:functionSetPinColor");
    gCoreData->remove("CMCS_GUI:functionResetAllPinColor");

    gRunning = false;
    if (gThread->joinable())
    {
        gThread->join();
    }
    delete gThread;
}
