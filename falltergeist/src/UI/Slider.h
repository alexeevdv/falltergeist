/*
 * Copyright 2012-2018 Falltergeist Developers.
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

#ifndef FALLTERGEIST_UI_SLIDER_H
#define FALLTERGEIST_UI_SLIDER_H

// C++ standard includes
#include <string>

// Falltergeist includes
#include "../UI/Base.h"
#include "../UI/ImageList.h"

// Third party includes

namespace Falltergeist
{
namespace Event
{
    class Mouse;
}
namespace UI
{

class Slider : public Falltergeist::UI::Base
{
public:
    Slider(const Point& pos);
    Slider(int x, int y);
    ~Slider() override;

    void handle(Event::Event* event) override;


    virtual Size size() const override;

    virtual bool opaque(const Point &pos) override;

    virtual void render(bool eggTransparency) override;

    double minValue() const;
    void setMinValue(double value);

    double maxValue() const;
    void setMaxValue(double value);

    double value() const;
    void setValue(double value);

    Event::Handler& changeHandler();

protected:
    ImageList _imageList;
    double _minValue = 0.0;
    double _maxValue = 1.0;
    double _value = 0.0;
    std::string _downSound;
    std::string _upSound;
    Event::Handler _changeHandler;

    void _onDrag(Event::Mouse* event);
    void _onLeftButtonDown(Event::Mouse* event);
    void _onLeftButtonUp(Event::Mouse* event);
};

}
}
#endif // FALLTERGEIST_UI_SLIDER_H
