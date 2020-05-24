#include "../State/GameMenu.h"
#include "../Audio/Mixer.h"
#include "../functions.h"
#include "../Game/Game.h"
#include "../Graphics/Renderer.h"
#include "../Input/Mouse.h"
#include "../ResourceManager.h"
#include "../State/ExitConfirm.h"
#include "../State/LoadGame.h"
#include "../State/Location.h"
#include "../State/SaveGame.h"
#include "../State/SettingsMenu.h"
#include "../UI/Factory/ImageButtonFactory.h"
#include "../UI/Image.h"
#include "../UI/ImageButton.h"
#include "../UI/PlayerPanel.h"
#include "../UI/TextArea.h"

namespace Falltergeist
{
    using ImageButtonType = UI::Factory::ImageButtonFactory::Type;

    namespace State
    {
        GameMenu::GameMenu(std::shared_ptr<UI::IResourceManager> resourceManager) : State()
        {
            this->resourceManager = resourceManager;
            imageButtonFactory = std::make_unique<UI::Factory::ImageButtonFactory>(resourceManager);
        }

        void GameMenu::init()
        {
            if (_initialized) return;
            State::init();

            setModal(true);
            setFullscreen(false);

            auto background = resourceManager->getImage("art/intrface/opbase.frm");
            auto panelHeight = Game::getInstance()->locationState()->playerPanel()->size().height();

            auto backgroundPos = (Game::getInstance()->renderer()->size() - background->size() - Point(0, panelHeight)) / 2;
            int backgroundX = backgroundPos.x();
            int backgroundY = backgroundPos.y();

            auto saveGameButton    = imageButtonFactory->getByType(ImageButtonType::OPTIONS_BUTTON, {backgroundX + 14, backgroundY + 18});
            auto loadGameButton    = imageButtonFactory->getByType(ImageButtonType::OPTIONS_BUTTON, {backgroundX + 14, backgroundY + 18 + 37});
            auto preferencesButton = imageButtonFactory->getByType(ImageButtonType::OPTIONS_BUTTON, {backgroundX + 14, backgroundY + 18 + 37 * 2});
            auto exitGameButton    = imageButtonFactory->getByType(ImageButtonType::OPTIONS_BUTTON, {backgroundX + 14, backgroundY + 18 + 37 * 3});
            auto doneButton        = imageButtonFactory->getByType(ImageButtonType::OPTIONS_BUTTON, {backgroundX + 14, backgroundY + 18 + 37 * 4});

            preferencesButton->mouseClickHandler().add([this](Event::Event* event){ this->doPreferences(); });
            exitGameButton->mouseClickHandler().add(   [this](Event::Event* event){ this->doExit(); });
            doneButton->mouseClickHandler().add(       [this](Event::Event* event){ this->closeMenu(); });

            auto font = ResourceManager::getInstance()->font("font3.aaf");
            SDL_Color color = {0xb8, 0x9c, 0x28, 0xff};

            // label: save game
            auto saveGameButtonLabel = new UI::TextArea(_t(MSG_OPTIONS, 0), backgroundX+8, backgroundY+26);
            saveGameButtonLabel->setFont(font, color);
            saveGameButtonLabel->setSize({150, 0});
            saveGameButtonLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);
            saveGameButton->mouseClickHandler().add([this](Event::Event* event){ this->doSaveGame(); });

            // label: load game
            auto loadGameButtonLabel = new UI::TextArea(_t(MSG_OPTIONS, 1), backgroundX+8, backgroundY+26+37);
            loadGameButtonLabel->setFont(font, color);
            loadGameButtonLabel->setSize({150, 0});
            loadGameButtonLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);
            loadGameButton->mouseClickHandler().add([this](Event::Event* event){ this->doLoadGame(); });

            // label: preferences
            auto preferencesButtonLabel = new UI::TextArea(_t(MSG_OPTIONS, 2), backgroundX+8, backgroundY+26+37*2);
            preferencesButtonLabel->setFont(font, color);
            preferencesButtonLabel->setSize({150, 0});
            preferencesButtonLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

            // label: exit game
            auto exitGameButtonLabel = new UI::TextArea(_t(MSG_OPTIONS, 3), backgroundX+8, backgroundY+26+37*3);
            exitGameButtonLabel->setFont(font, color);
            exitGameButtonLabel->setSize({150, 0});
            exitGameButtonLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

            // label: done
            auto doneButtonLabel = new UI::TextArea(_t(MSG_OPTIONS, 4), backgroundX+8, backgroundY+26+37*4);
            doneButtonLabel->setFont(font, color);
            doneButtonLabel->setSize({150, 0});
            doneButtonLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

            background->setPosition(backgroundPos);

            addUI(background);
            addUI(saveGameButton);
            addUI(loadGameButton);
            addUI(preferencesButton);
            addUI(exitGameButton);
            addUI(doneButton);
            addUI(saveGameButtonLabel);
            addUI(loadGameButtonLabel);
            addUI(preferencesButtonLabel);
            addUI(exitGameButtonLabel);
            addUI(doneButtonLabel);
        }

        void GameMenu::doSaveGame()
        {
            Game::getInstance()->pushState(std::make_unique<SaveGame>(resourceManager));
        }

        void GameMenu::doLoadGame()
        {
            Game::getInstance()->pushState(std::make_unique<LoadGame>(resourceManager));
        }

        void GameMenu::doPreferences()
        {
            Game::getInstance()->pushState(std::make_unique<SettingsMenu>(resourceManager));
        }

        void GameMenu::doExit()
        {
            Game::getInstance()->pushState(std::make_unique<ExitConfirm>(resourceManager));
        }

        void GameMenu::closeMenu()
        {
            Game::getInstance()->popState();
        }

        void GameMenu::onStateActivate(Event::State* event)
        {
            Game::getInstance()->mouse()->pushState(Input::Mouse::Cursor::BIG_ARROW);
        }

        void GameMenu::onStateDeactivate(Event::State* event)
        {
            Game::getInstance()->mouse()->popState();
        }

        void GameMenu::onKeyDown(Event::Keyboard* event)
        {
            switch (event->keyCode())
            {
                case SDLK_ESCAPE:
                case SDLK_d:
                    closeMenu();
                    break;
                case SDLK_s:
                    doSaveGame();
                    break;
                case SDLK_l:
                    doLoadGame();
                    break;
                case SDLK_p:
                    doPreferences();
                    break;
                case SDLK_e:
                    doExit();
                    break;
            }
        }
    }
}
