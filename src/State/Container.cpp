#include "../State/Container.h"
#include "../ResourceManager.h"
#include "../Game/ContainerItemObject.h"
#include "../Game/DudeObject.h"
#include "../Game/Game.h"
#include "../Graphics/CritterAnimationFactory.h"
#include "../Graphics/Renderer.h"
#include "../Helpers/CritterHelper.h"
#include "../Input/Mouse.h"
#include "../Logger.h"
#include "../UI/Animation.h"
#include "../UI/AnimationQueue.h"
#include "../UI/Factory/ImageButtonFactory.h"
#include "../UI/Image.h"
#include "../UI/ImageButton.h"
#include "../UI/ItemsList.h"

namespace Falltergeist
{
    namespace State
    {
        using ImageButtonType = UI::Factory::ImageButtonFactory::Type;

        Container::Container(std::shared_ptr<UI::IResourceManager> resourceManager) : State()
        {
            this->resourceManager = resourceManager;
            imageButtonFactory = std::make_unique<UI::Factory::ImageButtonFactory>(resourceManager);
        }

        void Container::init()
        {
            if (_initialized) return;

            setModal(true);
            setFullscreen(false);

            auto game = Game::Game::getInstance();

            setPosition((game->renderer()->size() - Point(537, 376)) / 2);

            addUI("background", resourceManager->getImage("art/intrface/loot.frm"));

            auto dude = Game::Game::getInstance()->player();

            Helpers::CritterHelper critterHelper;
            Graphics::CritterAnimationFactory animationFactory;

            auto dudeCritter = animationFactory.buildStandingAnimation(
                critterHelper.armorFID(dude.get()),
                critterHelper.weaponId(dude.get()),
                Game::Orientation::SC
            );
            dudeCritter->setPosition({56, 47});
            addUI(dudeCritter.release());

            // FIXME: chests and lockers should be shown opened and centered vertically
            auto objectCopy = new Game::Object();
            objectCopy->setFID(object()->FID());
            objectCopy->ui()->setPosition({432, 38});

            if (auto queue = dynamic_cast<UI::AnimationQueue*>(objectCopy->ui())) {
                auto lastIndex = (int)queue->currentAnimation()->frames().size()  - 1;
                queue->currentAnimation()->setCurrentFrame(lastIndex);
            }

            addUI(objectCopy->ui());

            addUI("button_done", imageButtonFactory->getByType(ImageButtonType::SMALL_RED_CIRCLE, {478, 331}));
            getUI("button_done")->mouseClickHandler().add(std::bind(&Container::onDoneButtonClick, this, std::placeholders::_1));

            // TODO: disable buttons if there is nowhere to scroll
            auto scrollUp = [](UI::ItemsList *list) { if (list->canScrollUp()) list->scrollUp(); };
            auto scrollDown = [](UI::ItemsList *list) { if (list->canScrollDown()) list->scrollDown(); };

            auto dudeList = new UI::ItemsList({174, 35});
            dudeList->setItems(Game::Game::getInstance()->player()->inventory());
            addUI(dudeList);

            auto dudeListScrollUpButton = imageButtonFactory->getByType(ImageButtonType::DIALOG_UP_ARROW, {127, 40});
            dudeListScrollUpButton->mouseClickHandler().add([=](...) { scrollUp(dudeList); });
            addUI(dudeListScrollUpButton);

            auto dudeListScrollDownButton = imageButtonFactory->getByType(ImageButtonType::DIALOG_DOWN_ARROW, {127, 66});
            dudeListScrollDownButton->mouseClickHandler().add([=](...) { scrollDown(dudeList); });
            addUI(dudeListScrollDownButton);

            auto containerList = new UI::ItemsList({292, 35});
            containerList->setItems(object()->inventory());
            addUI(containerList);

            auto containerListScrollUpButton = imageButtonFactory->getByType(ImageButtonType::DIALOG_UP_ARROW, {379, 40});
            containerListScrollUpButton->mouseClickHandler().add([=](...) { scrollUp(containerList); });
            addUI(containerListScrollUpButton);

            auto containerListScrollDownButton = imageButtonFactory->getByType(ImageButtonType::DIALOG_DOWN_ARROW, {379, 66});
            containerListScrollDownButton->mouseClickHandler().add([=](...) { scrollDown(containerList); });
            addUI(containerListScrollDownButton);

            auto btnTakeAll = imageButtonFactory->getByType(ImageButtonType::INVENTORY_TAKE_ALL, {432, 203});
            btnTakeAll->mouseClickHandler().add([dudeList, containerList](...) {

                for(const auto &i : *containerList->items()) {
                    dudeList->items()->push_back(i);
                }
                containerList->items()->clear();
                containerList->update();
                dudeList->update();
            });
            addUI(btnTakeAll);

            dudeList->itemDragStopHandler().add([containerList](Event::Mouse* event){ containerList->onItemDragStop(event); });
            containerList->itemDragStopHandler().add([dudeList](Event::Mouse* event){ dudeList->onItemDragStop(event); });
        }

        Game::ContainerItemObject* Container::object()
        {
            return _object;
        }

        void Container::setObject(Game::ContainerItemObject* object)
        {
            _object = object;
        }

        void Container::onDoneButtonClick(Event::Mouse*)
        {
            Game::Game::getInstance()->popState();
        }

        void Container::onStateActivate(Event::State*)
        {
            Game::Game::getInstance()->mouse()->pushState(Input::Mouse::Cursor::BIG_ARROW);
        }

        void Container::onStateDeactivate(Event::State*)
        {
            Game::Game::getInstance()->mouse()->popState();
        }

        void Container::onKeyDown(Event::Keyboard* event)
        {
            if (event->keyCode() == SDLK_ESCAPE)
            {
                Game::Game::getInstance()->popState();
            }
        }
    }
}
