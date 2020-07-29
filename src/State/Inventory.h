#pragma once

#include "../ILogger.h"
#include "../State/State.h"
#include "../Game/Object.h"
#include "../Game/Timer.h"
#include "../Input/Mouse.h"

#include "../UI/IResourceManager.h"

namespace Falltergeist
{
    namespace Game
    {
        class ItemObject;
        class Timer;
    }
    namespace UI
    {
        class Panel;
        namespace Factory
        {
            class ImageButtonFactory;
        }
    }
    namespace State
    {
        class Inventory final : public State
        {
            public:
                Inventory(std::shared_ptr<UI::IResourceManager> resourceManager, std::shared_ptr<ILogger> logger);
                Inventory(const Inventory&) = delete;
                Inventory& operator=(const Inventory&) = delete;
                ~Inventory() override = default;

                void init() override;

                void onDoneButtonClick(Event::Mouse* event);
                void onScrollUpButtonClick(Event::Mouse* event);
                void onScrollDownButtonClick(Event::Mouse* event);
                void onArmorSlotMouseDown(Event::Mouse* event);
                void onLeftHandSlotMouseDown(Event::Mouse* event);
                void onRightHandSlotMouseDown(Event::Mouse* event);
                void enableScrollDownButton(bool enable);
                void enableScrollUpButton(bool enable);
                //void onSlotMouseDown(Event::Mouse* event);
                //void onSlotMouseUp(Event::Mouse* event);
                //void onSlotDrag(Event::Mouse* event);
                void backgroundRightClick(Event::Mouse* event);
                void handle(Event::Event* event) override;
                void onKeyDown(Event::Keyboard* event) override;
                void onMouseDown(Event::Mouse *event);
                void onStateDeactivate(Event::State* event) override;
                void onInventoryModified();
                void toggleCursorMode();
                void _screenShow (unsigned int PID);
                Game::ItemObject* _item = nullptr;
                Game::ItemObject* _actionCursorLastItem = nullptr;
                std::vector<Input::Mouse::Icon> getCursorIconsForItem(Game::ItemObject *item);
                Game::Timer _actionCursorTimer;
                bool _actionCursorButtonPressed = false;
                void think(const float &deltaTime) override;
            protected:
                Event::MouseHandler _mouseDownHandler;
                unsigned int _actionCursorTicks = 0;

            private:
                std::shared_ptr<ILogger> logger;
                std::string _handItemSummary (Game::ItemObject* hand);
                std::shared_ptr<UI::IResourceManager> resourceManager;
                std::unique_ptr<UI::Factory::ImageButtonFactory> imageButtonFactory;
                UI::Panel* statsPanel;
        };
    }
}
