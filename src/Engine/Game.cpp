/*
 * Copyright 2012-2014 Falltergeist Developers.
 *
 * This file is part of Falltergeist.
 *
 * Falltergeist is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Falltergeist is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Falltergeist.  If not, see <http://www.gnu.org/licenses/>.
 */

// C++ standard includes
#include <sstream>
#include <string>

// Falltergeist includes
#include "../Engine/Audio/AudioMixer.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Event/StateEvent.h"
#include "../Engine/Exception.h"
#include "../Engine/Game.h"
#include "../Engine/GameTime.h"
#include "../Engine/Graphics/AnimatedPalette.h"
#include "../Engine/Graphics/OpenGLRenderer.h"
#include "../Engine/Graphics/SDLRenderer.h"
#include "../Engine/Input/Mouse.h"
#include "../Engine/Logger.h"
#include "../Engine/ResourceManager.h"
#include "../Engine/Settings/Settings.h"
#include "../Engine/State.h"
#include "../States/LocationState.h"
#include "../UI/FpsCounter.h"
#include "../UI/TextArea.h"

// Third patry includes

namespace Falltergeist
{

class GameDudeObject;
bool Game::_instanceFlag = false;
Game* Game::_instance = NULL;

Game* Game::getInstance()
{
    if(!_instanceFlag)
     {
         _instance = new Game();
         _instanceFlag = true;
         _instance->_initialize();
         return _instance;
     }
     else
     {
         return _instance;
     }
}

void Game::_initialize()
{
    if (_initialized) return;
    _initialized = true;

    _engineSettings = new EngineSettings();
    auto width = _engineSettings->screenWidth();
    auto height = _engineSettings->screenHeight();

    switch (_engineSettings->renderer())
    {
        case EngineSettings::Renderer::SDL:
            _renderer = new SDLRenderer(width, height);
            break;
        case EngineSettings::Renderer::OPENGL:
            _renderer = new OpenGLRenderer(width, height);
            break;
    }

    Logger::info("GAME") << CrossPlatform::getVersion() << std::endl;
    Logger::info("GAME") << "Opensource Fallout 2 game engine" << std::endl;

    putenv(strdup("SDL_VIDEO_CENTERED=1"));

    _resourceManager = new ResourceManager();

    renderer()->init();

    std::string version = CrossPlatform::getVersion();
    renderer()->setCaption(version.c_str());

    _mixer = new AudioMixer();
    _mouse = new Mouse();
    _fpsCounter = new FpsCounter(renderer()->width() - 42, 2);

    version += " " + std::to_string(renderer()->width()) + "x" + std::to_string(renderer()->height());
    version += " " + renderer()->name();

    _falltergeistVersion = new TextArea(version, 3, renderer()->height() - 10);
    _mousePosition = new TextArea("", renderer()->width() - 55, 14);
    _animatedPalette = new AnimatedPalette();
    _gameTime = new GameTime();
    _currentTime = new TextArea(renderer()->width() - 150, renderer()->height() - 10);
}

Game::~Game()
{
    _instanceFlag = false;
    delete _mouse;
    delete _fpsCounter;
    delete _mousePosition;
    delete _falltergeistVersion;
    delete _mixer;
    delete _resourceManager;
    while (!_states.empty()) popState();
    delete _renderer;
    delete _engineSettings;
    delete _gameTime;
    delete _currentTime;
}

void Game::pushState(State* state)
{
    // if new state is "modal", we need to deactivate previous state
    if (_states.size() > 0 && state->modal())
    {
        auto event = new StateEvent("deactivate");
        _states.back()->emitEvent(event);
        delete event;
    }

    auto event = new StateEvent("activate");
    state->emitEvent(event);
    delete event;

    _states.push_back(state);
    
    if (!state->initialized()) state->init();
}

void Game::popState()
{
    if (_states.size() == 0) return;

    auto state = _states.back();
    _states.pop_back();
    _statesForDelete.push_back(state);

    auto event = new StateEvent("deactivate");
    state->emitEvent(event);
    delete event;

    // if poped state was "modal" we need to "activate" previous state
    if (_states.size() > 0 && state->modal())
    {
        auto event = new StateEvent("activate");
        _states.back()->emitEvent(event);
        delete event;
    }
}

void Game::setState(State* state)
{
    while (!_states.empty()) popState();
    pushState(state);
}

void Game::run()
{
    Logger::info("GAME") << "Starting main loop" << std::endl;
    while (!_quit)
    {
        handle();
        think();
        render();
        SDL_Delay(1);
        while (!_statesForDelete.empty())
        {
            delete _statesForDelete.back();
            _statesForDelete.pop_back();
        }
    }
    Logger::info("GAME") << "Stopping main loop" << std::endl;
}

void Game::quit()
{
    _quit = true;
}

void Game::setPlayer(GameDudeObject* player)
{
    _player = player;
}

GameDudeObject* Game::player()
{
    return _player;
}

Mouse* Game::mouse()
{
    return _mouse;
}

LocationState* Game::locationState()
{
    for (auto state : _states)
    {
        auto locationState = dynamic_cast<LocationState*>(state);
        if (locationState)
        {
            return locationState;
        }
    }
    return 0;
}

void Game::setGVAR(unsigned int number, int value)
{
    _initGVARS();
    if (number >= _GVARS.size())
    {
        throw Exception("Game::setGVAR(num, value) - num out of range: " + std::to_string(number));
    }
    _GVARS.at(number) = value;
}

int Game::GVAR(unsigned int number)
{
    _initGVARS();
    if (number >= _GVARS.size())
    {
        throw Exception("Game::GVAR(num) - num out of range: " + std::to_string(number));
    }
    return _GVARS.at(number);
}

void Game::_initGVARS()
{
    if (_GVARS.size() > 0) return;
    auto gam = ResourceManager::gamFileType("data/vault13.gam");
    for (auto gvar : *gam->GVARS())
    {
        _GVARS.push_back(gvar.second);
    }
}

std::vector<State*>* Game::states()
{
    return &_states;
}

std::vector<State*>* Game::statesForRender()
{
    // we must render all states from last fullscreen state to the top of stack
    _statesForRender.clear();
    auto it = _states.end();
    do
    {
        --it;
    }
    while(it != _states.begin() && !(*it)->fullscreen());

    for (; it != _states.end(); ++it)
    {
        if (*it) _statesForRender.push_back(*it);
    }
    return &_statesForRender;
}

std::vector<State*>* Game::statesForThinkAndHandle()
{
    // we must handle all states from top to bottom of stack
    _statesForThinkAndHandle.clear();

    for (auto it = _states.rbegin(); it != _states.rend(); ++it)
    {
        _statesForThinkAndHandle.push_back(*it);
        if ((*it)->modal() || (*it)->fullscreen()) break;
    }

    return &_statesForThinkAndHandle;
}

Renderer* Game::renderer()
{
    return _renderer;
}

EngineSettings* Game::engineSettings()
{
    return _engineSettings;
}

void Game::handle()
{
    while(SDL_PollEvent(&_event))
    {
        if (_event.type == SDL_QUIT)
        {
            _quit = true;
        }
        else
        {
            MouseEvent* mouseEvent = 0;
            KeyboardEvent* keyboardEvent = 0;
            switch (_event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                {
                    mouseEvent = new MouseEvent("mousedown");
                    mouseEvent->setX(_event.button.x);
                    mouseEvent->setY(_event.button.y);
                    mouseEvent->setLeftButton(_event.button.button == SDL_BUTTON_LEFT);
                    mouseEvent->setRightButton(_event.button.button == SDL_BUTTON_RIGHT);
                    for (auto state : *statesForThinkAndHandle()) state->handle(mouseEvent);
                    break;
                }
                case SDL_MOUSEBUTTONUP:
                {
                    mouseEvent = new MouseEvent("mouseup");
                    mouseEvent->setX(_event.button.x);
                    mouseEvent->setY(_event.button.y);
                    mouseEvent->setLeftButton(_event.button.button == SDL_BUTTON_LEFT);
                    mouseEvent->setRightButton(_event.button.button == SDL_BUTTON_RIGHT);
                    for (auto state : *statesForThinkAndHandle()) state->handle(mouseEvent);
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    mouseEvent = new MouseEvent("mousemove");
                    mouseEvent->setX(_event.motion.x);
                    mouseEvent->setY(_event.motion.y);
                    mouseEvent->setXOffset(_event.motion.xrel);
                    mouseEvent->setYOffset(_event.motion.yrel);
                    for (auto state : *statesForThinkAndHandle()) state->handle(mouseEvent);
                    break;
                }
                case SDL_KEYDOWN:
                {
                    keyboardEvent = new KeyboardEvent("keydown");
                    keyboardEvent->setKeyCode(_event.key.keysym.sym);
                    keyboardEvent->setShiftPressed(_event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT));
                    for (auto state : *statesForThinkAndHandle()) state->handle(keyboardEvent);
                    break;
                }
                case SDL_KEYUP:
                {
                    keyboardEvent = new KeyboardEvent("keyup");
                    keyboardEvent->setKeyCode(_event.key.keysym.sym);
                    keyboardEvent->setShiftPressed(_event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT));
                    for (auto state : *statesForThinkAndHandle()) state->handle(keyboardEvent);

                    if (!keyboardEvent->handled())
                    {
                        if (keyboardEvent->keyCode() == SDLK_F10)
                        {
                            _quit = true;
                        }

                        if (keyboardEvent->keyCode() == SDLK_F11)
                        {
                            Texture* texture = renderer()->screenshot();
                            SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(texture->data(), texture->width(), texture->height(), 32, texture->width()*4, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);                            
                            std::string name = std::to_string(SDL_GetTicks()) +  ".bmp";
                            SDL_SaveBMP(surface, name.c_str());
                            SDL_FreeSurface(surface);                            
                            delete texture;
                            Logger::info("GAME") << "Screenshot saved to " + name << std::endl;

                        }
                    }
                    break;
                }
            }
            delete keyboardEvent;
            delete mouseEvent;
        }
    }
}

void Game::think()
{
    _fpsCounter->think();
    _mouse->think();

    _animatedPalette->think();

    _mousePosition->setText(std::to_string(mouse()->x()) + " : " + std::to_string(mouse()->y()));

    std::string time = std::to_string(_gameTime->year()) + "-" + std::to_string(_gameTime->month()) + "-" + std::to_string(_gameTime->day()) + " ";
    time += std::to_string(_gameTime->hours()) + ":" + std::to_string(_gameTime->minutes()) + ":" + std::to_string(_gameTime->seconds()) + " ";
    time += std::to_string(_gameTime->ticks());
    _currentTime->setText(time);

    for (auto state : *statesForThinkAndHandle())
    {
        state->think();
    }
}

void Game::render()
{
    renderer()->beginFrame();
    for (auto state : *statesForRender())
    {
        state->render();
    }
    if (engineSettings()->displayFps()) _fpsCounter->render();
    _falltergeistVersion->render();
    if (engineSettings()->displayMousePosition()) _mousePosition->render();
    _currentTime->render();
    _mouse->render();
    renderer()->endFrame();
}

AnimatedPalette* Game::animatedPalette()
{
    return _animatedPalette;
}

GameTime* Game::gameTime()
{
    return _gameTime;
}

AudioMixer* Game::mixer()
{
    return _mixer;
}


}
