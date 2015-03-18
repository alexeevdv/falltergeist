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

#ifndef FALLTERGEIST_PlayerEditGender_H
#define FALLTERGEIST_PlayerEditGender_H

// C++ standard includes

// Falltergeist includes
#include "State.h"

// Third party includes
#include <libfalltergeist.h>

using namespace libfalltergeist;

namespace Falltergeist
{
class ImageList;

namespace State
{

class PlayerEditGender : public State
{
protected:
    ImageList* _maleImage = 0;
    ImageList* _femaleImage = 0;
    GENDER _gender = GENDER::MALE;
public:
    PlayerEditGender();
    void init();
    void onFemaleButtonPress(MouseEvent* event);
    void onMaleButtonPress(MouseEvent* event);
    void onDoneButtonClick(MouseEvent* event);
    virtual void onKeyDown(KeyboardEvent* event);
    void setGender(GENDER gender);
};

}
}

#endif // FALLTERGEIST_PlayerEditGender_H
