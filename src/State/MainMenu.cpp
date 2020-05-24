#include <sstream>
#include "../State/MainMenu.h"
#include "../Audio/Mixer.h"
#include "../Event/State.h"
#include "../functions.h"
#include "../Game/Game.h"
#include "../Graphics/Renderer.h"
#include "../Input/Mouse.h"
#include "../Logger.h"
#include "../ResourceManager.h"
#include "../State/Credits.h"
#include "../State/LoadGame.h"
#include "../State/Movie.h"
#include "../State/NewGame.h"
#include "../State/SettingsMenu.h"
#include "../UI/Animation.h"
#include "../UI/Factory/ImageButtonFactory.h"
#include "../UI/Image.h"
#include "../UI/ImageButton.h"
#include "../UI/TextArea.h"

namespace Falltergeist
{
    using ImageButtonType = UI::Factory::ImageButtonFactory::Type;

    namespace State
    {
        MainMenu::MainMenu(std::shared_ptr<UI::IResourceManager> resourceManager) : State()
        {
            this->resourceManager = resourceManager;
            imageButtonFactory = std::make_unique<UI::Factory::ImageButtonFactory>(resourceManager);
        }

        MainMenu::~MainMenu()
        {
            if (Game::getInstance()->mixer()) {
                Game::getInstance()->mixer()->stopMusic();
            }
        }

        void MainMenu::init()
        {
            if (_initialized) return;
            State::init();

            setModal(true);
            setFullscreen(true);

            auto renderer = Game::getInstance()->renderer();
            setPosition((renderer->size() - Point(640, 480)) / 2);

            addUI("background", resourceManager->getImage("art/intrface/mainmenu.frm"));

            // intro button
            auto introButton = addUI(imageButtonFactory->getByType(ImageButtonType::MENU_RED_CIRCLE, {30, 19}));
            introButton->mouseClickHandler().add(std::bind(&MainMenu::onIntroButtonClick, this, std::placeholders::_1));

            // new game button
            auto newGameButton = addUI(imageButtonFactory->getByType(ImageButtonType::MENU_RED_CIRCLE, {30, 19 + 41}));
            newGameButton->mouseClickHandler().add(std::bind(&MainMenu::onNewGameButtonClick, this, std::placeholders::_1));

            // load game button
            auto loadGameButton = addUI(imageButtonFactory->getByType(ImageButtonType::MENU_RED_CIRCLE, {30, 19 + 41 * 2}));
            loadGameButton->mouseClickHandler().add(std::bind(&MainMenu::onLoadGameButtonClick, this, std::placeholders::_1));

            // settings button
            auto settingsButton = addUI(imageButtonFactory->getByType(ImageButtonType::MENU_RED_CIRCLE, {30, 19 + 41 * 3}));
            settingsButton->mouseClickHandler().add(std::bind(&MainMenu::onSettingsButtonClick, this, std::placeholders::_1));

            // credits button
            auto creditsButton = addUI(imageButtonFactory->getByType(ImageButtonType::MENU_RED_CIRCLE, {30, 19 + 41 * 4}));
            creditsButton->mouseClickHandler().add(std::bind(&MainMenu::onCreditsButtonClick, this, std::placeholders::_1));

            // exit button
            auto exitButton = addUI(imageButtonFactory->getByType(ImageButtonType::MENU_RED_CIRCLE, {30, 19 + 41 * 5}));
            exitButton->mouseClickHandler().add(std::bind(&MainMenu::onExitButtonClick, this, std::placeholders::_1));

            auto font4 = ResourceManager::getInstance()->font("font4.aaf");
            SDL_Color color = {0xb8, 0x9c, 0x28, 0xff};

            // "Intro" label
            auto introButtonLabel = new UI::TextArea("INTRO", 50, 20);
            introButtonLabel->setFont(font4, color);
            introButtonLabel->setWidth(150);
            introButtonLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

            // "New Game" label
            auto newGameButtonLabel = new UI::TextArea("NEW GAME", 50, 20 + 41);
            newGameButtonLabel->setFont(font4, color);
            newGameButtonLabel->setWidth(150);
            newGameButtonLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

            // "Load Game" label
            auto loadGameButtonLabel = new UI::TextArea("LOAD GAME", 50, 20 + 41*2);
            loadGameButtonLabel->setFont(font4, color);
            loadGameButtonLabel->setWidth(150);
            loadGameButtonLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

            // "Options" label
            auto optionsButtonLabel = new UI::TextArea("OPTIONS", 50, 20 + 41*3);
            optionsButtonLabel->setFont(font4, color);
            optionsButtonLabel->setWidth(150);
            optionsButtonLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

            // "Credits" label
            auto creditsButtonLabel = new UI::TextArea("CREDITS", 50, 20 + 41*4);
            creditsButtonLabel->setFont(font4, color);
            creditsButtonLabel->setWidth(150);
            creditsButtonLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

            // "Exit" label
            auto exitButtonLabel = new UI::TextArea("EXIT", 50, 20 + 41*5);
            exitButtonLabel->setFont(font4, color);
            exitButtonLabel->setWidth(150);
            exitButtonLabel->setHorizontalAlign(UI::TextArea::HorizontalAlign::CENTER);

            // Text areas
            addUI(introButtonLabel);
            addUI(newGameButtonLabel);
            addUI(loadGameButtonLabel);
            addUI(optionsButtonLabel);
            addUI(creditsButtonLabel);
            addUI(exitButtonLabel);
        }

        void MainMenu::doExit()
        {
            fadeDoneHandler().clear();
            fadeDoneHandler().add([this](Event::Event* event){ this->onExitStart(dynamic_cast<Event::State*>(event)); });
            Game::getInstance()->renderer()->fadeOut(0,0,0,1000);
        }

        void MainMenu::doNewGame()
        {
            fadeDoneHandler().clear();
            fadeDoneHandler().add([this](Event::Event* event){ this->onNewGameStart(dynamic_cast<Event::State*>(event)); });
            Game::getInstance()->renderer()->fadeOut(0,0,0,1000);
        }

        void MainMenu::doLoadGame()
        {
            fadeDoneHandler().clear();
            fadeDoneHandler().add([this](Event::Event* event){ this->onLoadGameStart(dynamic_cast<Event::State*>(event)); });
            Game::getInstance()->renderer()->fadeOut(0,0,0,1000);
        }

        void MainMenu::doSettings()
        {
            Game::getInstance()->pushState(std::make_shared<SettingsMenu>(resourceManager));
        }

        void MainMenu::doIntro()
        {
            fadeDoneHandler().clear();
            fadeDoneHandler().add([this](Event::Event* event){ this->onIntroStart(dynamic_cast<Event::State*>(event)); });
            Game::getInstance()->renderer()->fadeOut(0,0,0,1000);
        }

        void MainMenu::doCredits()
        {
            fadeDoneHandler().clear();
            fadeDoneHandler().add([this](Event::Event* event){ this->onCreditsStart(dynamic_cast<Event::State*>(event)); });
            Game::getInstance()->renderer()->fadeOut(0,0,0,1000);
        }

        void MainMenu::onExitButtonClick(Event::Mouse* event)
        {
            doExit();
        }

        void MainMenu::onExitStart(Event::State* event)
        {
            fadeDoneHandler().clear();
            Game::getInstance()->mixer()->stopMusic();
            Game::getInstance()->quit();
        }

        void MainMenu::onNewGameButtonClick(Event::Mouse* event)
        {
            doNewGame();
        }

        void MainMenu::onNewGameStart(Event::State* event)
        {
            fadeDoneHandler().clear();
            Game::getInstance()->pushState(std::make_shared<NewGame>(resourceManager));
        }

        void MainMenu::onLoadGameButtonClick(Event::Mouse* event)
        {
            doLoadGame();
        }

        void MainMenu::onLoadGameStart(Event::State* event)
        {
            fadeDoneHandler().clear();
            Game::getInstance()->pushState(std::make_shared<LoadGame>(resourceManager));
        }

        void MainMenu::onSettingsButtonClick(Event::Mouse* event)
        {
            doSettings();
        }

        void MainMenu::onIntroButtonClick(Event::Mouse* event)
        {
            doIntro();
        }

        void MainMenu::onIntroStart(Event::State* event)
        {
            fadeDoneHandler().clear();
            Game::getInstance()->pushState(std::make_shared<Movie>(17));
            Game::getInstance()->pushState(std::make_shared<Movie>(1));
        }

        void MainMenu::onCreditsButtonClick(Event::Mouse* event)
        {
            doCredits();
        }

        void MainMenu::onCreditsStart(Event::State* event)
        {
            fadeDoneHandler().clear();
            Game::getInstance()->pushState(std::make_shared<Credits>());
        }

        void MainMenu::onKeyDown(Event::Keyboard* event)
        {
            switch (event->keyCode())
            {
                case SDLK_e:
                case SDLK_ESCAPE:
                    doExit();
                    break;
                case SDLK_n:
                    doNewGame();
                    break;
                case SDLK_l:
                    doLoadGame();
                    break;
                case SDLK_i:
                    doIntro();
                    break;
                case SDLK_c:
                    doCredits();
                    break;
                case SDLK_o:
                    doSettings();
                    break;
            }
        }

        void MainMenu::onStateActivate(Event::State* event)
        {
            Game::getInstance()->mouse()->setState(Input::Mouse::Cursor::BIG_ARROW);
            Game::getInstance()->mixer()->playACMMusic("07desert.acm",true);
            Game::getInstance()->renderer()->fadeIn(0,0,0,1000);
        }
    }
}
