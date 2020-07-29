#include "../Game/ContainerItemObject.h"
#include "../Game/Game.h"
#include "../State/Container.h"
#include "../UI/ResourceManager.h"
#include "../UI/TextArea.h"
#include "../Audio/Mixer.h"
#include "../Event/Event.h"
#include "../State/Location.h"
#include "../UI/Animation.h"
#include "../UI/AnimationQueue.h"
#include "../Logger.h"

namespace Falltergeist
{
    namespace Game
    {
        ContainerItemObject::ContainerItemObject() : ItemObject()
        {
            _subtype = Subtype::CONTAINER;
        }

        std::vector<ItemObject*>* ContainerItemObject::inventory()
        {
            return &_inventory;
        }

        void ContainerItemObject::use_p_proc(CritterObject* usedBy)
        {
            Object::use_p_proc(usedBy);
            if (script() && script()->overrides()) {
                return;
            }

            if (!_locked) {
                if (!opened()) {
                    if (UI::AnimationQueue* queue = dynamic_cast<UI::AnimationQueue*>(this->ui())) {
                        queue->start();
                        queue->animationEndedHandler().add(std::bind(&ContainerItemObject::onOpeningAnimationEnded, this, std::placeholders::_1));
                        if (_soundId) {
                            Game::getInstance()->mixer()->playACMSound(std::string("sound/sfx/iocntnr") + _soundId + ".acm");
                        }
                    }
                } else {
                    if (UI::AnimationQueue* queue = dynamic_cast<UI::AnimationQueue*>(this->ui())) {
                        queue->start();
                        queue->animationEndedHandler().add(std::bind(&ContainerItemObject::onClosingAnimationEnded, this, std::placeholders::_1));
                        if (_soundId) {
                            Game::getInstance()->mixer()->playACMSound(std::string("sound/sfx/iccntnr") + _soundId + ".acm");
                        }
                    }
                }
            } else if (_soundId) {
                Game::getInstance()->mixer()->playACMSound(std::string("sound/sfx/ilcntnr") + _soundId + ".acm");
            }
        }

        void ContainerItemObject::setLocked(bool locked)
        {
            this->_locked = locked;
        }

        bool ContainerItemObject::locked() const
        {
            return _locked;
        }

        void ContainerItemObject::setOpened(bool opened)
        {
            // Don't change if container is locked.
            if (!_locked) {
                _opened = opened;
                setCanLightThru(_opened);

                if (auto queue = dynamic_cast<UI::AnimationQueue*>(this->ui())) {
                    queue->currentAnimation()->setReverse(opened);
                }
            }
        }

        bool ContainerItemObject::opened() const
        {
            return _opened;
        }

        void ContainerItemObject::onOpeningAnimationEnded(Event::Event* event)
        {
            auto queue = (UI::AnimationQueue*)event->target();
            setOpened(true);
            queue->animationEndedHandler().clear();
            queue->stop();
            queue->currentAnimation()->setReverse(true);
            Game::getInstance()->locationState()->initLight();
            Logger::info("") << "Container opened: " << opened() << std::endl;

            auto state = new State::Container(std::make_shared<UI::ResourceManager>());
            state->setObject(this);
            Game::getInstance()->pushState(state);
        }

        void ContainerItemObject::onClosingAnimationEnded(Event::Event* event)
        {
            auto queue = (UI::AnimationQueue*)event->target();
            setOpened(false);
            queue->animationEndedHandler().clear();
            queue->stop();
            queue->currentAnimation()->setReverse(false);
            Game::getInstance()->locationState()->initLight();
            Logger::info("") << "Container opened: " << opened() << std::endl;
        }
    }
}
