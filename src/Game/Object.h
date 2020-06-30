#pragma once

#include <memory>
#include <string>
#include "../Event/EventTarget.h"
#include "../Format/Enums.h"
#include "../Game/Orientation.h"
#include "../Graphics/TransFlags.h"
#include "../UI/Base.h"

namespace Falltergeist
{
    namespace Event
    {
        class Event;
    }
    namespace Graphics
    {
        class Texture;
    }
    namespace UI
    {
        class AnimationQueue;
        class Base;
        class Image;
        class TextArea;
    }
    namespace VM
    {
        class Script;
    }

    class Hexagon;
    class Location;

    namespace Game
    {
        class CritterObject;

        class Object : public Event::EventTarget
        {
            public:
                // Object type as defined in prototype
                enum class Type
                {
                    ITEM = 0,
                    CRITTER,
                    SCENERY,
                    WALL,
                    TILE,
                    MISC,
                    DUDE
                };


                Object();
                virtual ~Object();

                // whether this object is transparent in terms of walking through it by a critter
                virtual bool canWalkThru() const;
                virtual void setCanWalkThru(bool value);

                // whether this object is transparent to the light
                virtual bool canLightThru() const;
                virtual void setCanLightThru(bool value);

                // whether this object is transparent to projectiles
                virtual bool canShootThru() const;
                virtual void setCanShootThru(bool value);

                virtual bool wallTransEnd() const;
                virtual void setWallTransEnd(bool value);

                // object type
                Type type() const;

                // object prototype ID - refers to numeric ID as used in original game
                int PID() const;
                void setPID(int value);

                // object base frame ID
                int FID() const;
                virtual void setFID(int value);

                // script ID
                int SID() const;
                virtual void setSID(int value);

                // object current elevation index on the map (0-based)
                int elevation() const;
                void setElevation(int value);

                // returns facing direction (0 - 5)
                Orientation orientation() const;
                // changes object facing direction (0 - 5)
                virtual void setOrientation(Orientation value);

                // object name, as defined in proto msg file
                std::string name() const;
                void setName(const std::string& value);

                // object description, as defined in proto msg file
                std::string description() const;
                void setDescription(const std::string& value);

                // object name from scrname.msg
                virtual std::string scrName() const;
                void setScrName(const std::string& value);


                // script entity associated with the object
                VM::Script* script() const;
                void setScript(VM::Script* script);

                /**
                 * @brief Handles OS events coming from the State::handle().
                 * Used in Event Capturing process.
                 * This method is called first in the main loop (before think() and render()).
                 */
                virtual void handle(Event::Event* event);
                /**
                 * @brief Process any real-time actions of the game object (like calling critter_p_proc).
                 * This method is called after handle() but before render() in the main loop.
                 */
                virtual void think(const float &deltaTime);
                /**
                 * @brief Render this object, if it has visible UI elements.
                 * This method is called last in the main loop (after handle() and think()).
                 */
                virtual void render();

                /**
                * @brief Render object outline, if it has visible UI elements.
                */
                virtual void renderOutline(int type);

                /**
                 * @brief Render floater text above object, if any.
                 */
                virtual void renderText();

                // ActiveUI used to display object on screen and capture mouse events
                UI::Base* ui() const;
                void setUI(UI::Base* ui);

                // Hexagon of object current position
                const std::shared_ptr<Hexagon> &hexagon() const;
                void setHexagon(const std::shared_ptr<Hexagon> &hexagon);

                // TextArea, currently floating above the object
                UI::TextArea* floatMessage() const;
                void setFloatMessage(std::unique_ptr<UI::TextArea> floatMessage);

                // is object currently being rendered
                bool inRender() const;
                void setInRender(bool value);

                // object translucency mode
                Graphics::TransFlags::Trans trans() const;
                // sets object translucency mode
                void setTrans(Graphics::TransFlags::Trans value);

                // request description of the object to console, may call "description_p_proc" procedure of underlying script entity
                virtual void description_p_proc();
                // call "destroy_p_proc" procedure of underlying script entity (use this just before killing critter or destroying the object)
                virtual void destroy_p_proc();
                // request brief description of the object to console, may call "look_at_p_proc" of the script
                virtual void look_at_p_proc();
                // call "map_enter_p_proc" of the script entity (use this when dude travels to another map via exit grid or worldmap)
                virtual void map_enter_p_proc();
                // call "map_exit_p_proc" of the script entity (use this when dude travels out of current map via exit grid or worldmap)
                virtual void map_exit_p_proc();
                // call "map_update_p_proc" when map is updating (once every N frames, after times skip in pipboy)
                virtual void map_update_p_proc();
                // call "pickup_p_proc" of the script entity (when picking up item object)
                virtual void pickup_p_proc(const std::shared_ptr<CritterObject> &pickedUpBy);
                // perform "use" action, may call "use_p_proc" of the underlying script
                virtual void use_p_proc(const std::shared_ptr<CritterObject> &usedBy);
                // perform "use object on" action, may call "use_obj_on_p_proc" procedure
                virtual void use_obj_on_p_proc(const std::shared_ptr<Object> &objectUsed, const std::shared_ptr<CritterObject> &usedBy);
                // perform "use skill on" action, may call "use_skill_on_p_proc" procedure
                virtual void use_skill_on_p_proc(SKILL skill, const std::shared_ptr<Object> &objectUsed, const std::shared_ptr<CritterObject> &usedBy);



                virtual void onUseAnimationActionFrame(Event::Event* event, const std::shared_ptr<CritterObject> &critter);
                virtual void onUseAnimationEnd(Event::Event* event, const std::shared_ptr<CritterObject> &critter);

                unsigned short lightOrientation() const;
                virtual void setLightOrientation(Orientation orientation);

                unsigned int lightIntensity() const;
                virtual void setLightIntensity(unsigned int intensity);

                unsigned int lightRadius() const;
                virtual void setLightRadius(unsigned int radius);

                virtual void setFlags(unsigned int flags);

                bool flat() const;
                virtual void setFlat(bool value);

                unsigned int defaultFrame();
                virtual void setDefaultFrame(unsigned int frame);

                int position() const;
                void setPosition(int position);

            protected:
                bool _canWalkThru = true;
                bool _canLightThru = false;
                bool _canShootThru = true;
                bool _wallTransEnd = false;
                bool _flat = false;
                Type _type;
                int _PID = -1;
                int _FID = -1;
                int _SID = -1;
                int _elevation = 0;
                Orientation _orientation;
                int _position = -1;
                std::string _name;
                std::string _scrName;
                std::string _description;
                std::unique_ptr<VM::Script> _script;
                std::unique_ptr<UI::Base> _ui;
                virtual void _generateUi();
                std::unique_ptr<UI::TextArea> _floatMessage;
                bool _inRender = false;
                Graphics::TransFlags::Trans _trans = Graphics::TransFlags::Trans::DEFAULT;
                Orientation _lightOrientation;
                unsigned int _lightIntensity = 0;
                unsigned int _lightRadius = 0;
                unsigned int _defaultFrame;
        };
    }
}
