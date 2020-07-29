#include <sstream>
#include "../State/Inventory.h"
#include "../Event/Event.h"
#include "../Event/Mouse.h"
#include "../functions.h"
#include "../Game/ArmorItemObject.h"
#include "../Game/CritterObject.h"
#include "../Game/DudeObject.h"
#include "../Game/Game.h"
#include "../Game/Object.h"
#include "../Game/ObjectFactory.h"
#include "../Game/WeaponItemObject.h"
#include "../Graphics/CritterAnimationFactory.h"
#include "../Graphics/Size.h"
#include "../Helpers/CritterHelper.h"
#include "../Input/Mouse.h"
#include "../Logger.h"
#include "../ResourceManager.h"
#include "../State/State.h"
#include "../State/GameMenu.h"
#include "../State/InventoryDragItem.h"
#include "../State/Location.h"
#include "../State/CursorDropdown.h"
#include "../UI/Animation.h"
#include "../UI/Factory/ImageButtonFactory.h"
#include "../UI/Image.h"
#include "../UI/ImageButton.h"
#include "../UI/ImageList.h"
#include "../UI/InventoryItem.h"
#include "../UI/ItemsList.h"
#include "../UI/PlayerPanel.h"
#include "../UI/Rectangle.h"
#include "../UI/TextArea.h"
#include "../UI/Panel.h"

namespace Falltergeist
{
    using ImageButtonType = UI::Factory::ImageButtonFactory::Type;

    namespace State
    {
        Inventory::Inventory(std::shared_ptr<UI::IResourceManager> resourceManager, std::shared_ptr<ILogger> logger) : State()
        {
            this->resourceManager = std::move(resourceManager);
            this->logger = std::move(logger);
            imageButtonFactory = std::make_unique<UI::Factory::ImageButtonFactory>(this->resourceManager);

            pushHandler().add([](Event::State* ev) {
                Game::Game::getInstance()->mouse()->pushState(Input::Mouse::Cursor::ACTION);
            });
            popHandler().add([](Event::State* ev) {
                // If hand cursor now
                if (Game::Game::getInstance()->mouse()->state() == Input::Mouse::Cursor::HAND) {
                    Game::Game::getInstance()->mouse()->popState();
                }
                Game::Game::getInstance()->mouse()->popState();
            });
        }

        void Inventory::init()
        {
            if (_initialized) return;
            State::init();

            _mouseDownHandler.add(std::bind(&Inventory::onMouseDown, this, std::placeholders::_1));

            // action cursor stuff
            _actionCursorTimer.setInterval((unsigned) 350);
            _actionCursorTimer.tickHandler().add([this](Event::Event *) {
                if (!_item) {
                    return;
                }

                if (_actionCursorButtonPressed || Game::Game::getInstance()->mouse()->state() == Input::Mouse::Cursor::ACTION) {
                    if (!_actionCursorButtonPressed && (_actionCursorLastItem != _item)) {
                        _item->look_at_p_proc();
                        _actionCursorLastItem = _item;
                    }
                    auto icons = getCursorIconsForItem(_item);
                    if (!icons.empty()) {
                        auto inventory = dynamic_cast<Inventory *>(Game::Game::getInstance()->topState(0));
                        inventory->_screenShow(_item->PID());
                        // TODO delegate state manipulation to some kind of state manager
                        if (dynamic_cast<CursorDropdown *>(Game::Game::getInstance()->topState()) != nullptr) {
                            Game::Game::getInstance()->popState();
                        }

                        auto state = new CursorDropdown(resourceManager, std::move(icons), !_actionCursorButtonPressed);
                        state->setObject(_item);
                        Game::Game::getInstance()->pushState(state);
                    }
                }
                _actionCursorButtonPressed = false;
            });

            setModal(true);
            setFullscreen(false);

            auto game = Game::Game::getInstance();
            auto panelHeight = Game::Game::getInstance()->locationState()->playerPanel()->size().height();

            setPosition((game->renderer()->size() - Point(499, 377 + panelHeight)) / 2); // 499x377 = art/intrface/invbox.frm

            statsPanel = new UI::Panel(position() + Point(300, 47));

            addUI("background", resourceManager->getImage("art/intrface/invbox.frm"));
            getUI("background")->mouseClickHandler().add(std::bind(&Inventory::backgroundRightClick, this, std::placeholders::_1));

            addUI("button_up",   imageButtonFactory->getByType(ImageButtonType::INVENTORY_UP_ARROW,   {128, 40}));
            addUI("button_down", imageButtonFactory->getByType(ImageButtonType::INVENTORY_DOWN_ARROW, {128, 65}));
            auto buttonDownDisabled = resourceManager->getImage("art/intrface/invdnds.frm");
            auto buttonUpDisabled = resourceManager->getImage("art/intrface/invupds.frm");
            buttonUpDisabled->setPosition(Point(128, 40));
            buttonDownDisabled->setPosition(Point(128, 65));
            addUI("button_up_disabled", buttonUpDisabled);
            addUI("button_down_disabled", buttonDownDisabled);
            addUI("button_done", imageButtonFactory->getByType(ImageButtonType::SMALL_RED_CIRCLE, {438, 328}));

            getUI("button_done")->mouseClickHandler().add(std::bind(&Inventory::onDoneButtonClick, this, std::placeholders::_1));
            getUI("button_up")->mouseClickHandler().add(  std::bind(&Inventory::onScrollUpButtonClick, this, std::placeholders::_1));
            getUI("button_down")->mouseClickHandler().add(std::bind(&Inventory::onScrollDownButtonClick, this, std::placeholders::_1));

            // screen
            auto screenX = 0;
            auto screenY = 0;
            auto player = Game::Game::getInstance()->player();

            statsPanel->addUI("player_name", new UI::TextArea(player->name(), Point(screenX, screenY)));

            auto line1 = new UI::Rectangle(Point(screenX, screenY+16), Graphics::Size(142, 1), {0x3f, 0xf8, 0x00, 0xff} );

            std::string statsLabels;
            for (unsigned i = (unsigned)STAT::STRENGTH; i <= (unsigned)STAT::LUCK; i++)
            {
                statsLabels += _t(MSG_INVENTORY, i) + "\n";
            }
            statsPanel->addUI("label_stats", new UI::TextArea(statsLabels, screenX, screenY + 10*2));

            std::string statsValues;
            for (unsigned i = (unsigned)STAT::STRENGTH; i <= (unsigned)STAT::LUCK; i++)
            {
                statsValues += std::to_string(player->stat((STAT)i)) + "\n";
            }
            statsPanel->addUI("label_stats_values", new UI::TextArea(statsValues, screenX + 22, screenY + 20));

            std::stringstream ss;
            for (unsigned int i=7; i<14; i++)
            {
                ss << _t(MSG_INVENTORY, i) << "\n";
            }
            auto textLabel = new UI::TextArea(ss.str(), screenX+40, screenY+20);

            // label: hit points
            ss.str("");
            ss << player->hitPoints();
            ss << "/";
            ss << player->hitPointsMax();
            auto hitPointsLabel = new UI::TextArea(ss.str(), screenX+94, screenY+20);
            hitPointsLabel->setWidth(46);
            hitPointsLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::RIGHT);
            // label: armor class
            ss.str("");
            ss << player->armorClass();
            auto armorClassLabel = new UI::TextArea(ss.str(), screenX+94, screenY+30);
            armorClassLabel->setWidth(46);
            armorClassLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::RIGHT);

            // armorSlot, leftHand, rightHand
            Game::ArmorItemObject* armorSlot = player->armorSlot();
            Game::ItemObject* leftHand = player->leftHandSlot();
            Game::ItemObject* rightHand = player->rightHandSlot();


            // label: damage treshold levels
            ss.str("");
            if (armorSlot)
            {
                ss << player->damageThreshold(DAMAGE::NORMAL) + armorSlot->damageThreshold(DAMAGE::NORMAL) <<"/\n";
                ss << player->damageThreshold(DAMAGE::LASER) + armorSlot->damageThreshold(DAMAGE::LASER) <<"/\n";
                ss << player->damageThreshold(DAMAGE::FIRE) + armorSlot->damageThreshold(DAMAGE::FIRE) <<"/\n";
                ss << player->damageThreshold(DAMAGE::PLASMA) + armorSlot->damageThreshold(DAMAGE::PLASMA) <<"/\n";
                ss << player->damageThreshold(DAMAGE::EXPLOSIVE) + armorSlot->damageThreshold(DAMAGE::NORMAL) <<"/";
            }
            else
            {
                ss << player->damageThreshold(DAMAGE::NORMAL) <<"/\n";
                ss << player->damageThreshold(DAMAGE::LASER) <<"/\n";
                ss << player->damageThreshold(DAMAGE::FIRE) <<"/\n";
                ss << player->damageThreshold(DAMAGE::PLASMA) <<"/\n";
                ss << player->damageThreshold(DAMAGE::EXPLOSIVE) <<"/";
            }
            auto damageThresholdLabel = new UI::TextArea(ss.str(), screenX+94, screenY+40);
            damageThresholdLabel->setWidth(26);
            damageThresholdLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::RIGHT);

            // label: damage resistance levels
            ss.str("");
            if (armorSlot)
            {
                ss << player->damageResist(DAMAGE::NORMAL) + armorSlot->damageResist(DAMAGE::NORMAL) <<"%\n";
                ss << player->damageResist(DAMAGE::LASER) + armorSlot->damageResist(DAMAGE::LASER) <<"%\n";
                ss << player->damageResist(DAMAGE::FIRE) + armorSlot->damageResist(DAMAGE::FIRE) <<"%\n";
                ss << player->damageResist(DAMAGE::PLASMA) + armorSlot->damageResist(DAMAGE::PLASMA) <<"%\n";
                ss << player->damageResist(DAMAGE::EXPLOSIVE) + armorSlot->damageResist(DAMAGE::NORMAL) <<"%";
            }
            else
            {
                ss << player->damageResist(DAMAGE::NORMAL) <<"%\n";
                ss << player->damageResist(DAMAGE::LASER) <<"%\n";
                ss << player->damageResist(DAMAGE::FIRE) <<"%\n";
                ss << player->damageResist(DAMAGE::PLASMA) <<"%\n";
                ss << player->damageResist(DAMAGE::EXPLOSIVE) <<"%";
            }
            auto damageResistanceLabel = new UI::TextArea(ss.str(), screenX+120, screenY+40);

            auto line2 = new UI::Rectangle(Point(screenX, screenY+94), Graphics::Size(142, 1), {0x3f, 0xf8, 0x00, 0xff} );
            auto line3 = new UI::Rectangle(Point(screenX, screenY+134), Graphics::Size(142, 1), {0x3f, 0xf8, 0x00, 0xff} );

            // label: Total Wt: (20)
            auto weight = player->carryWeight();
            auto weightMax = player->carryWeightMax();

            ss.str("");
            ss << weight;
            auto totalWtLabel = new UI::TextArea(_t(MSG_INVENTORY, 20), screenX+14, screenY+180);
            auto weightLabel = new UI::TextArea(ss.str(), screenX+70, screenY+180);
            weightLabel->setWidth(24);
            weightLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::RIGHT);
            ss.str("");
            ss << "/" << weightMax;
            auto weightMaxLabel = new UI::TextArea(ss.str(), screenX+94, screenY+180);
            if (weight>weightMax)
            {
                weightLabel->setFont("font1.aaf", {0xff, 0x00, 0x00, 0xff});
            }

            // label: left hand
            ss.str("");
            if (leftHand)
            {
                ss << _handItemSummary(leftHand);
            }
            auto leftHandLabel = new UI::TextArea(ss.str(), screenX, screenY+100);

            // label: right hand
            ss.str("");
            if (rightHand)
            {
                ss << _handItemSummary(rightHand);
            }
            auto rightHandLabel = new UI::TextArea(ss.str(), screenX, screenY+140);

            // screen info
            auto screenLabel = new UI::TextArea("", screenX, screenY+20);
            screenLabel->setSize({140, 168}); // screen size
            screenLabel->setVisible(false);
            screenLabel->setWordWrap(true);


            statsPanel->addUI(line1);
            statsPanel->addUI("textLabel", textLabel);
            statsPanel->addUI("hitPointsLabel", hitPointsLabel);
            statsPanel->addUI("armorClassLabel", armorClassLabel);
            statsPanel->addUI("damageThresholdLabel", damageThresholdLabel);
            statsPanel->addUI("damageResistanceLabel", damageResistanceLabel);
            statsPanel->addUI("line2", line2);
            statsPanel->addUI("line3", line3);
            statsPanel->addUI("totalWtLabel", totalWtLabel);
            statsPanel->addUI("weightLabel", weightLabel);
            statsPanel->addUI("weightMaxLabel", weightMaxLabel);
            statsPanel->addUI("leftHandLabel", leftHandLabel);
            statsPanel->addUI("rightHandLabel", rightHandLabel);
            statsPanel->addUI("screenLabel", screenLabel);

            addUI(statsPanel);

            auto inventoryList = new UI::ItemsList(Point(40, 40));
            inventoryList->setItems(game->player()->inventory());
            addUI("inventory_list", inventoryList);

            // TODO: this is a rotating animation in the vanilla engine
            auto dude = Game::Game::getInstance()->player();

            Helpers::CritterHelper critterHelper;
            Graphics::CritterAnimationFactory animationFactory;

            auto dudeCritter = animationFactory.buildStandingAnimation(
                critterHelper.armorFID(dude.get()),
                critterHelper.weaponId(dude.get()),
                Game::Orientation::SC
            );
            dudeCritter->setPosition({188, 52});
            addUI(dudeCritter.release());

            // BIG ICONS
            // icon: armor
            {
                auto inventoryItem = new UI::InventoryItem(armorSlot, {154, 183});
                inventoryItem->setType(UI::InventoryItem::Type::SLOT);
                inventoryItem->itemDragStopHandler().add([inventoryList](Event::Mouse* event){ inventoryList->onItemDragStop(event); });
                inventoryList->itemDragStopHandler().add([inventoryItem](Event::Mouse* event){ inventoryItem->onArmorDragStop(event); });
                
                addUI(inventoryItem);
            }

            // icon: left hand
            {
                auto inventoryItem = new UI::InventoryItem(leftHand, {154, 286});
                inventoryItem->setType(UI::InventoryItem::Type::SLOT);
                inventoryItem->itemDragStopHandler().add([inventoryList](Event::Mouse* event){ inventoryList->onItemDragStop(event, HAND::LEFT); });
                inventoryList->itemDragStopHandler().add([inventoryItem](Event::Mouse* event){ inventoryItem->onHandDragStop(event, HAND::LEFT); });
                addUI(inventoryItem);
            }

            // icon: right hand
            {
                auto inventoryItem = new UI::InventoryItem(rightHand, {247, 286});
                inventoryItem->setType(UI::InventoryItem::Type::SLOT);
                inventoryItem->itemDragStopHandler().add([inventoryList](Event::Mouse* event){ inventoryList->onItemDragStop(event, HAND::RIGHT); });
                inventoryList->itemDragStopHandler().add([inventoryItem](Event::Mouse* event){ inventoryItem->onHandDragStop(event, HAND::RIGHT); });
                addUI(inventoryItem);
            }

            //initialize inventory scroll buttons
            enableScrollUpButton(false);
            if(inventoryList->canScrollDown())
            {
                enableScrollDownButton(true);
            }
            else
            {
                enableScrollDownButton(false);
            }

            inventoryList->itemsListModifiedHandler().add([this](Event::Event* event){ this->onInventoryModified(); });
        }

        void Inventory::onDoneButtonClick(Event::Mouse* event)
        {
            Game::Game::getInstance()->popState();
        }

        void Inventory::onScrollUpButtonClick(Event::Mouse* event)
        {
            auto inventory = dynamic_cast<UI::ItemsList*>(getUI("inventory_list"));
            if(inventory->canScrollUp())
            {
                inventory->scrollUp();
                //enable/disable scroll buttons on upward scroll
                if(!inventory->canScrollUp())
                {
                    enableScrollUpButton(false);
                }
                if(inventory->canScrollDown())
                {
                    enableScrollDownButton(true);
                }
            }
        }

        void Inventory::onScrollDownButtonClick(Event::Mouse* event)
        {
            auto inventory = dynamic_cast<UI::ItemsList*>(getUI("inventory_list"));
            if(inventory->canScrollDown())
            {
                inventory->scrollDown();
                //enable/disable scroll buttons on downward scroll
                if(!inventory->canScrollDown())
                {
                    enableScrollDownButton(false);
                }
                if(inventory->canScrollUp())
                {
                    enableScrollUpButton(true);
                }
            }
        }

        void Inventory::onInventoryModified()
        {
            auto inventory = dynamic_cast<UI::ItemsList*>(getUI("inventory_list"));
            /*
            this would scroll up when an item is removed and you are at the bottom
            of the list to fix the gap, but a bug is causing slotOffset to be crazy number
            if(inventory->items()->size() - inventory->slotOffset() <  inventory->slotsNumber())
            {
                inventory->scrollUp();
            }
            */
            enableScrollDownButton(inventory->canScrollDown());
            enableScrollUpButton(inventory->canScrollUp());
        }

        void Inventory::enableScrollUpButton(bool enable)
        {
            auto scrollUpButton = dynamic_cast<UI::ImageButton*>(getUI("button_up"));
            auto scrollUpButtonDisabled = dynamic_cast<UI::Image*>(getUI("button_up_disabled"));
            scrollUpButtonDisabled->setVisible(!enable);
            scrollUpButton->setEnabled(enable);
        }

        void Inventory::enableScrollDownButton(bool enable)
        {
            auto scrollDownButton = dynamic_cast<UI::ImageButton*>(getUI("button_down"));
            auto scrollDownButtonDisabled = dynamic_cast<UI::Image*>(getUI("button_down_disabled"));
            scrollDownButtonDisabled->setVisible(!enable);
            scrollDownButton->setEnabled(enable);
        }

        void Inventory::onArmorSlotMouseDown(Event::Mouse* event)
        {
            if (Game::Game::getInstance()->mouse()->state() == Input::Mouse::Cursor::HAND)
            {
                auto itemUi = dynamic_cast<UI::ImageList*>(event->target());
                Game::Game::getInstance()->pushState(new InventoryDragItem(itemUi));
            }
            else
            {
                auto itemPID = Game::Game::getInstance()->player()->armorSlot()->PID();
                _screenShow(itemPID);
            }
        }

        void Inventory::onLeftHandSlotMouseDown(Event::Mouse* event)
        {
            if (Game::Game::getInstance()->mouse()->state() == Input::Mouse::Cursor::HAND)
            {
                auto itemUi = dynamic_cast<UI::ImageList*>(event->target());
                Game::Game::getInstance()->pushState(new InventoryDragItem(itemUi));
            }
            else
            {
                auto itemPID = Game::Game::getInstance()->player()->leftHandSlot()->PID();
                _screenShow(itemPID);
            }
        }

        void Inventory::onRightHandSlotMouseDown(Event::Mouse* event)
        {
            if (Game::Game::getInstance()->mouse()->state() == Input::Mouse::Cursor::HAND)
            {
                auto itemUi = dynamic_cast<UI::ImageList*>(event->target());
                Game::Game::getInstance()->pushState(new InventoryDragItem(itemUi));
            }
            else
            {
                auto itemPID = Game::Game::getInstance()->player()->rightHandSlot()->PID();
                _screenShow(itemPID);
            }
        }

        //void Inventory::onSlotMouseDown(MouseEvent* event)
        //{
        //    auto state = dynamic_cast<Inventory*>(event->reciever());
        //    auto itemUi = dynamic_cast<ImageList*>(event->target());
        //    itemUi->setCurrentImage(1);
        //    itemUi->setX(event->x() - itemUi->width()*0.5);
        //    itemUi->setY(event->y() - itemUi->height()*0.5);
        //}

        //void Inventory::onSlotMouseUp(MouseEvent* event)
        //{
        //    auto itemUi = dynamic_cast<ImageList*>(event->target());
        //    itemUi->setCurrentImage(0);
        //    itemUi->setX(event->x() - itemUi->width()*0.5);
        //    itemUi->setY(event->y() - itemUi->height()*0.5);
        //}

        //void Inventory::onSlotDrag(MouseEvent* event)
        //{
        //    //auto item = dynamic_cast<GameItemObject*>(event->reciever());
        //    auto itemUi = dynamic_cast<ImageList*>(event->target());
        //    //auto dragUi = item->inventoryDragUi();
        //    itemUi->setX(itemUi->x() + event->xOffset());
        //    itemUi->setY(itemUi->y() + event->yOffset());
        //    //Game::getInstance()->states()->back()->ui()->push_back(dragUi);
        //}

        std::string Inventory::_handItemSummary (Game::ItemObject* hand)
        {
            Game::ObjectFactory objectFactory(logger);

            std::stringstream ss;
            if (hand)
            {
                ss << hand->name() << "\n";
                // is it weapon
                if (hand->subtype() == Game::ItemObject::Subtype::WEAPON)
                {
                    auto weapon = dynamic_cast<Game::WeaponItemObject*>(hand);
                    ss << "Dmg: " << weapon->damageMin() << "-" << weapon->damageMax() << " ";
                    ss << "Rng: " << weapon->rangePrimary();
                    // has it ammo?
                    if (weapon->ammoType() != 0)
                    {
                        ss << "\nAmmo: /" << weapon->ammoCapacity() << " ";
                        auto ammo = objectFactory.createObjectByPID(weapon->ammoPID());
                        ss << ammo->name();
                    }
                }
            }
            else
            {
                ss << "No item\nUnarmed dmg:";
            }
            return ss.str();
        }

        void Inventory::backgroundRightClick(Event::Mouse* event)
        {
            _screenShow(0);
        }

        void Inventory::_screenShow (unsigned int PID)
        {
            auto player = Game::Game::getInstance()->player();
            auto playerNameLabel = statsPanel->getTextArea("player_name");
            auto statsLabel = statsPanel->getTextArea("label_stats");
            auto statsValuesLabel = statsPanel->getTextArea("label_stats_values");
            auto textLabel = statsPanel->getTextArea("textLabel");
            auto hitPointsLabel = statsPanel->getTextArea("hitPointsLabel");
            auto armorClassLabel = statsPanel->getTextArea("armorClassLabel");
            auto damageThresholdLabel = statsPanel->getTextArea("damageThresholdLabel");
            auto damageResistanceLabel = statsPanel->getTextArea("damageResistanceLabel");
            auto line2 = statsPanel->getUI("line2");
            auto line3 = statsPanel->getUI("line3");
            auto totalWtLabel = statsPanel->getTextArea("totalWtLabel");
            auto weightLabel = statsPanel->getTextArea("weightLabel");
            auto weightMaxLabel = statsPanel->getTextArea("weightMaxLabel");
            auto leftHandLabel = statsPanel->getTextArea("leftHandLabel");
            auto rightHandLabel = statsPanel->getTextArea("rightHandLabel");
            auto screenLabel = statsPanel->getTextArea("screenLabel");

            if (PID == 0)
            {
                playerNameLabel->setText(player->name());
            }
            else
            {
                playerNameLabel->setText(_t(MSG_PROTO_ITEMS, PID*100)); // item name
                screenLabel->setText(_t(MSG_PROTO_ITEMS, PID*100 + 1)); // item dedcription
            }

            screenLabel->setVisible(PID != 0);
            statsLabel->setVisible(PID == 0);
            statsValuesLabel->setVisible(PID == 0);
            textLabel->setVisible(PID == 0);
            hitPointsLabel->setVisible(PID == 0);
            armorClassLabel->setVisible(PID == 0);
            damageThresholdLabel->setVisible(PID == 0);
            damageResistanceLabel->setVisible(PID == 0);
            line2->setVisible(PID == 0);
            line3->setVisible(PID == 0);
            totalWtLabel->setVisible(PID == 0);
            weightLabel->setVisible(PID == 0);
            weightMaxLabel->setVisible(PID == 0);
            leftHandLabel->setVisible(PID == 0);
            rightHandLabel->setVisible(PID == 0);
        }

        void Inventory::handle(Event::Event *event)
        {
            State::handle(event);
            if (auto mouseEvent = dynamic_cast<Event::Mouse *>(event)) {
                using Mouse = Event::Mouse;

                if (mouseEvent->originalType() == Mouse::Type::BUTTON_DOWN) {
                    emitEvent(std::make_unique<Event::Mouse>(*mouseEvent), _mouseDownHandler);
                }
            }
            if (event->handled()) {
                return;
            }
        }

        void Inventory::onKeyDown(Event::Keyboard* event)
        {
            switch (event->keyCode())
            {
                case SDLK_ESCAPE:
                    Game::Game::getInstance()->popState();
                    break;
            }
        }

        void Inventory::onMouseDown(Event::Mouse *event)
        {
            if (event->rightButton()) {
                toggleCursorMode();
            }
        }

        void Inventory::onStateDeactivate(Event::State *event)
        {
            _actionCursorTimer.stop();
        }

        std::vector<Input::Mouse::Icon> Inventory::getCursorIconsForItem(Game::ItemObject *item)
        {
            std::vector<Input::Mouse::Icon> icons;

            icons.push_back(Input::Mouse::Icon::LOOK);
            icons.push_back(Input::Mouse::Icon::CANCEL);
            return icons;
        }

        void Inventory::think(const float &deltaTime)
        {
            _actionCursorTimer.think(deltaTime);
            State::think(deltaTime);
        }

        void Inventory::toggleCursorMode()
        {
            auto mouseState = Game::Game::getInstance()->mouse()->state();

            if (mouseState == Input::Mouse::Cursor::HAND) {
                Game::Game::getInstance()->mouse()->popState();
            } else if (mouseState == Input::Mouse::Cursor::ACTION || mouseState == Input::Mouse::Cursor::ACTION_REVERSE) {
                Game::Game::getInstance()->mouse()->pushState(Input::Mouse::Cursor::HAND);
            }
        }
    }
}
