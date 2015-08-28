/*
 * Copyright 2012-2015 Falltergeist Developers.
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

#ifndef FALLTERGEIST_STATE_H
#define FALLTERGEIST_STATE_H

// C++ standard includes
#include <map>
#include <memory>
#include <vector>

// Falltergeist includes
#include "../Event/EventTarget.h"
#include "../Event/Keyboard.h"
#include "../Event/Mouse.h"

// Third party includes

namespace Falltergeist
{
namespace Event
{
    class Event;
    class State;
}
namespace Game
{
    class Game;
}
namespace UI
{
    class ImageList;
    class SmallCounter;
    class TextArea;
    class Base;
}

namespace State
{

class State : public Event::EventTarget
{
public:
    State();
    virtual ~State();

    UI::Base* addUI(std::shared_ptr<UI::Base> ui);
    UI::Base* addUI(const std::string& name, std::shared_ptr<UI::Base> ui);
    void addUI(std::vector<std::shared_ptr<UI::Base>> uis);
    void popUI();

    UI::Base* getUI(const std::string& name);

    UI::TextArea* getTextArea(const std::string& name);
    UI::ImageList* getImageList(const std::string& name);
    UI::SmallCounter* getSmallCounter(const std::string& name);

    virtual int x() const;
    virtual void setX(int x);

    virtual int y() const;
    virtual void setY(int y);

    virtual bool fullscreen() const;
    virtual void setFullscreen(bool value);

    virtual bool modal() const;
    virtual void setModal(bool value);

    virtual bool active();
    virtual void setActive(bool value);

    virtual bool initialized();
    virtual void init();

    virtual void think();
    virtual void handle(Event::Event* event);
    virtual void render();

    virtual void onStateActivate(Event::State* event);
    virtual void onStateDeactivate(Event::State* event);
    virtual void onKeyDown(Event::Keyboard* event);

protected:
    std::vector<std::shared_ptr<UI::Base>> _ui;
    std::vector<std::shared_ptr<UI::Base>> _uiToDelete;
    std::map<std::string, std::shared_ptr<UI::Base>> _labeledUI;

    int _x = 0;
    int _y = 0;

    bool _modal = false; // prevents all states before this one to call think() method
    bool _active = false;

    bool _fullscreen = true; // prevents render all states before this one
    bool _initialized = false;

};

}
}

#endif // FALLTERGEIST_STATE_H
