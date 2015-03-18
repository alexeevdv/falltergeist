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

// C++ standard includes

// Falltergeist includes
#include "../Exception.h"
#include "../Graphics/Texture.h"
#include "../ResourceManager.h"
#include "../UI/Image.h"

// Third party includes

namespace Falltergeist
{

Image::Image(std::string filename) : ActiveUI()
{
    setTexture(ResourceManager::texture(filename));
}

Image::Image(Image* image) : ActiveUI()
{
    // @fixme: we should use "clone" feature here
    setTexture(new Texture(image->texture()->width(), image->texture()->height()));
    unsigned int* pixels = (unsigned int*)image->texture()->sdlSurface()->pixels;
    _texture->loadFromRGBA(pixels);
}

Image::Image(unsigned int width, unsigned int height) : ActiveUI()
{
    setTexture(new Texture(width, height));
}

Image::Image(Texture* texture) : ActiveUI()
{
    setTexture(texture);
}

Image::Image(std::shared_ptr<libfalltergeist::Frm::File> frm, unsigned int direction)
{
    if (direction >= frm->directions()->size())
    {
        throw Exception("Image::Image(frm, direction) - direction not found: " + std::to_string(direction));
    }

    // direction texture
    setTexture(new Texture(frm->width(direction), frm->height(direction)));

    // full frm texture
    Texture* texture = new Texture(frm->width(), frm->height());
    texture->loadFromRGBA(frm->rgba(ResourceManager::palFileType("color.pal")));

    // direction offset in full texture
    unsigned int y = 0;
    for (unsigned int i = 0; i != direction; ++i)
    {
        y += frm->height(direction);
    }

    texture->copyTo(_texture, 0, 0, 0, y, frm->width(direction), frm->height(direction));
    delete texture;

    setXOffset(frm->offsetX(direction) + frm->shiftX(direction));
    setYOffset(frm->offsetY(direction) + frm->shiftY(direction));
}

Image::~Image()
{
}

unsigned int Image::width()
{
    return texture()->width();
}

unsigned int Image::height()
{
    return texture()->height();
}

}
