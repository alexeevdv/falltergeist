#include <algorithm>
#include <cstdlib>
#include <list>
#include <memory>
#include "../State/Location.h"
#include "../Audio/Mixer.h"
#include "../Exception.h"
#include "../Format/Msg/File.h"
#include "../Format/Txt/MapsFile.h"
#include "../Format/Gam/File.h"
#include "../functions.h"
#include "../Game/ContainerItemObject.h"
#include "../Game/Defines.h"
#include "../Game/DoorSceneryObject.h"
#include "../Game/ExitMiscObject.h"
#include "../Game/Game.h"
#include "../Game/LadderSceneryObject.h"
#include "../Game/StairsSceneryObject.h"
#include "../Game/Location.h"
#include "../Game/LocationElevation.h"
#include "../Game/ObjectFactory.h"
#include "../Game/SpatialObject.h"
#include "../Game/WeaponItemObject.h"
#include "../Helpers/GameLocationHelper.h"
#include "../Helpers/GameObjectHelper.h"
#include "../LocationCamera.h"
#include "../Logger.h"
#include "../PathFinding/Hexagon.h"
#include "../PathFinding/HexagonGrid.h"
#include "../ResourceManager.h"
#include "../Settings.h"
#include "../State/CursorDropdown.h"
#include "../State/WorldMap.h"
#include "../UI/Animation.h"
#include "../UI/AnimationFrame.h"
#include "../UI/AnimationQueue.h"
#include "../UI/PlayerPanel.h"
#include "../UI/SmallCounter.h"
#include "../UI/TextArea.h"
#include "../UI/Tile.h"
#include "../UI/TileMap.h"

namespace Falltergeist
{
    using Helpers::GameLocationHelper;
    using Helpers::GameObjectHelper;

    namespace State
    {
        using namespace Base;

        const int Location::DROPDOWN_DELAY = 350;
        const int Location::KEYBOARD_SCROLL_STEP = 35;

        Location::Location(
            std::shared_ptr<Game::DudeObject> player,
            std::shared_ptr<Input::Mouse> mouse,
            std::shared_ptr<Settings> settings,
            std::shared_ptr<Graphics::Renderer> renderer,
            std::shared_ptr<Audio::Mixer> audioMixer,
            std::shared_ptr<Game::Time> gameTime,
            std::shared_ptr<UI::IResourceManager> resourceManager,
            std::shared_ptr<ILogger> logger
        ) : State(),
            player(std::move(player)),
            mouse(std::move(mouse)),
            settings(std::move(settings)),
            renderer(std::move(renderer)),
            audioMixer(std::move(audioMixer)),
            gameTime(std::move(gameTime))
        {
            this->resourceManager = std::move(resourceManager);
            this->logger = std::move(logger);
        }

        void Location::init()
        {
            if (initialized()) {
                return;
            }
            State::init();

            mouse->setState(Input::Mouse::Cursor::ACTION);

            _camera = std::make_unique<LocationCamera>(renderer->size(), Point(0, 0));

            _hexagonInfo = std::make_unique<UI::TextArea>("", renderer->width() - 135, 25);
            _hexagonInfo->setWidth(135);
            _hexagonInfo->setHorizontalAlign(UI::TextArea::HorizontalAlign::RIGHT);

            setFullscreen(true);
            setModal(true);

            _objects.clear();
            _flatObjects.clear();
            _spatials.clear();

            _hexagonGrid = std::make_unique<HexagonGrid>();

            initializeLightmap();

            auto elevation = _location->elevations()->at(_elevation);
            // Set camera position on default
            camera()->setCenter(hexagonGrid()->at(_location->defaultPosition())->position());

            // @todo remove old objects from hexagonal grid
            for (auto &object : *elevation->objects()) {

                auto hexagon = hexagonGrid()->at(object->position());
                moveObjectToHexagon(object, hexagon, false);

                if (object->ui()) {
                    object->ui()->mouseDownHandler().add(
                        std::bind(
                            &Location::onObjectMouseEvent,
                            this,
                            std::placeholders::_1,
                            object
                        )
                    );
                    object->ui()->mouseClickHandler().add(
                        std::bind(
                            &Location::onObjectMouseEvent,
                            this,
                            std::placeholders::_1,
                            object
                        )
                    );
                    object->ui()->mouseInHandler().add(
                        std::bind(
                            &Location::onObjectHover,
                            this,
                            std::placeholders::_1,
                            object
                        )
                    );
                    // TODO: get rid of mousemove handler?
                    object->ui()->mouseMoveHandler().add(
                        std::bind(
                            &Location::onObjectHover,
                            this,
                            std::placeholders::_1,
                            object
                        )
                    );
                    object->ui()->mouseOutHandler().add(
                        std::bind(
                            &Location::onObjectHover,
                            this,
                            std::placeholders::_1,
                            object
                        )
                    );
                }

                if (auto spatial = dynamic_cast<Game::SpatialObject*>(object)) {
                    _spatials.push_back(spatial);
                    continue;
                }

                // flat objects are like tiles. they don't think (but has handlers) and rendered first.
                if (object->flat()) {
                    _flatObjects.emplace_back(object);
                    continue;
                }

                _objects.emplace_back(object);
            }

            initializePlayerTestAppareance(player);
            player->setOrientation(_location->defaultOrientation());

            // Player script
            player->setScript(new VM::Script(ResourceManager::getInstance()->intFileType(0), player.get()));

            auto hexagon = hexagonGrid()->at(_location->defaultPosition());
            _objects.emplace_back(player);
            moveObjectToHexagon(player.get(), hexagon);

            elevation->floor()->init();
            elevation->roof()->init();

            //loadAmbient(name);

            initLight();

            _playerPanel = makeUI<UI::PlayerPanel>(logger);

            _mouseDownHandler.add(std::bind(&Location::onMouseDown, this, std::placeholders::_1));
            _mouseUpHandler.add(std::bind(&Location::onMouseUp, this, std::placeholders::_1));
            _mouseMoveHandler.add(std::bind(&Location::onMouseMove, this, std::placeholders::_1));

            // action cursor stuff
            _actionCursorTimer.setInterval((unsigned) DROPDOWN_DELAY);
            _actionCursorTimer.tickHandler().add([this](Event::Event *) {
                if (!_objectUnderCursor) {
                    return;
                }

                if (_actionCursorButtonPressed || mouse->state() == Input::Mouse::Cursor::ACTION) {
                    if (!_actionCursorButtonPressed && (_actionCursorLastObject != _objectUnderCursor)) {
                        _objectUnderCursor->look_at_p_proc();
                        _actionCursorLastObject = _objectUnderCursor;
                    }
                    auto icons = getCursorIconsForObject(_objectUnderCursor);
                    if (!icons.empty()) {
                        // TODO delegate state manipulation to some kind of state manager
                        if (dynamic_cast<CursorDropdown *>(Game::Game::getInstance()->topState()) != nullptr) {
                            Game::Game::getInstance()->popState();
                        }
                        auto state = new CursorDropdown(resourceManager, std::move(icons), !_actionCursorButtonPressed);
                        state->setObject(_objectUnderCursor);
                        Game::Game::getInstance()->pushState(state);
                    }
                }
                _actionCursorButtonPressed = false;
            });

            _locationScriptTimer.start(10000.0f, true);
            _locationScriptTimer.tickHandler().add([this](Event::Event*) {
                if (_location->script()) {
                    _location->script()->call("map_update_p_proc");
                }
                for (auto &object : _objects) {
                    object->map_update_p_proc();
                }
                player->map_update_p_proc();
            });
        }

        void Location::onStateActivate(Event::State *event)
        {
            // correct position of "red hexagon" after popups
            auto hexagon = hexagonGrid()->hexagonAt(mouse->position() + _camera->topLeft());
            if (mouse->state() == Input::Mouse::Cursor::HEXAGON_RED && hexagon) {
                mouse->ui()->setPosition(hexagon->position() - _camera->topLeft());
            }
        }

        void Location::onStateDeactivate(Event::State *event)
        {
            _objectUnderCursor = nullptr;
            _actionCursorTimer.stop();
        }

        void Location::setLocation(std::shared_ptr<Game::Location> location)
        {
            _location = std::move(location);
        }

        void Location::loadAmbient(const std::string &name)
        {
            auto maps = ResourceManager::getInstance()->mapsTxt()->maps();
            auto mapShortName = path_basename(name, true);
            auto it = find_if(maps.begin(), maps.end(), [&](const Format::Txt::Map &map) {
                return map.name == mapShortName;
            });

            if (it != maps.end()) {
                _currentMap = static_cast<unsigned>(it - maps.begin());

                if (!it->music.empty() && settings->musicVolume() > 0.0001) {
                    Logger::info("Location") << "Playing music " << it->music << std::endl;
                    audioMixer->playACMMusic(it->music + ".acm");
                } else {
                    Logger::info("Location") << "Map " << mapShortName << " has no music." << std::endl;
                }
                _ambientSfx = it->ambientSfx;
                if (!_ambientSfx.empty()) {
                    _ambientSfxTimer.tickHandler().add([this, mapShortName](Event::Event *evt) {
                        unsigned char rnd = rand() % 100, sum = 0;
                        auto it = _ambientSfx.cbegin();
                        while (it != _ambientSfx.cend() && (sum + it->second) < rnd) {
                            sum += it->second;
                            ++it;
                        }
                        if (it != _ambientSfx.cend()) {
                            Logger::info("Location") << "Playing ambient sfx " << it->first << std::endl;
                            audioMixer->playACMSound("sound/sfx/" + it->first + ".acm");
                        } else {
                            Logger::error("Location") << "Could not match ambient sfx for map " << mapShortName
                                                      << " with " << rnd << std::endl;
                        }
                        _ambientSfxTimer.start(rand() % 10000 + 20000);
                    });
                    _ambientSfxTimer.start(10000);
                } else {
                    Logger::error("Location") << "No ambient sfx for map " << mapShortName << std::endl;
                }
            }
        }

        void Location::initializeLightmap()
        {
            std::vector<glm::vec2> _vertices;
            for (auto hex : _hexagonGrid->hexagons()) {
                _vertices.push_back(glm::vec2(hex->position().x(), hex->position().y()));
            }
            std::vector<GLuint> indexes;
            for (auto hexagon : _hexagonGrid->hexagons()) {
                bool doup = true;
                bool dodown = true;
                if (hexagon->number() % 200 == 0) {
                    doup = false;
                }
                if ((hexagon->number() + 1) % 200 == 0) {
                    dodown = false;
                }
                if (hexagon->number() < 200) {
                    doup = false;
                }
                if (hexagon->number() >= 39800) {
                    dodown = false;
                }
                if (dodown) {
                    indexes.push_back(hexagon->number());
                    if (hexagon->number() % 2) // odd
                    {
                        indexes.push_back(hexagon->number() + 200);
                        indexes.push_back(hexagon->number() + 1);
                    } else {
                        indexes.push_back(hexagon->number() + 200);
                        indexes.push_back(hexagon->number() + 201);
                    }
                }
                if (doup) {
                    indexes.push_back(hexagon->number());
                    if (hexagon->number() % 2) // odd
                    {
                        indexes.push_back(hexagon->number() - 200);
                        indexes.push_back(hexagon->number() - 201);
                    } else {
                        indexes.push_back(hexagon->number() - 200);
                        indexes.push_back(hexagon->number() - 1);
                    }
                }
            }
            _lightmap = new Graphics::Lightmap(_vertices, indexes);
        }

        void Location::initializePlayerTestAppareance(std::shared_ptr<Game::DudeObject> player) const
        {
            Game::ObjectFactory objectFactory(logger);

            static bool equipped;
            if (equipped) {
                return;
            }

            equipped = true;
            player->setArmorSlot(nullptr);
            auto powerArmor = (Game::ItemObject*) objectFactory.createObjectByPID(PID_POWERED_ARMOR);
            auto leatherJacket = (Game::ItemObject*) objectFactory.createObjectByPID(PID_LEATHER_JACKET);
            auto combatArmor = (Game::ItemObject*) objectFactory.createObjectByPID(PID_COMBAT_ARMOR);
            auto purpleRobe = (Game::ItemObject*) objectFactory.createObjectByPID(PID_PURPLE_ROBE);
            purpleRobe->setAmount(5);
            auto miniGun = (Game::WeaponItemObject*) objectFactory.createObjectByPID(PID_MINIGUN);
            auto spear = (Game::WeaponItemObject*) objectFactory.createObjectByPID(PID_SPEAR);

            player->inventory()->push_back(powerArmor);
            player->inventory()->push_back(leatherJacket);
            player->inventory()->push_back(combatArmor);
            player->inventory()->push_back(purpleRobe);
            player->setLeftHandSlot(miniGun);
            player->setRightHandSlot(spear);
            player->setActionAnimation("aa")->stop();
            player->setPID(0x01000001);
        }

        std::vector<Input::Mouse::Icon> Location::getCursorIconsForObject(Game::Object *object)
        {
            std::vector<Input::Mouse::Icon> icons;
            if (object->script() && object->script()->hasFunction("use_p_proc")) {
                icons.push_back(Input::Mouse::Icon::USE);
            } else if (dynamic_cast<Game::DoorSceneryObject *>(object)) {
                icons.push_back(Input::Mouse::Icon::USE);
            } else if (dynamic_cast<Game::ContainerItemObject *>(object)) {
                icons.push_back(Input::Mouse::Icon::USE);
            } else if (dynamic_cast<Game::LadderSceneryObject *>(object)) {
                icons.push_back(Input::Mouse::Icon::USE);
            } else if (dynamic_cast<Game::StairsSceneryObject *>(object)) {
                icons.push_back(Input::Mouse::Icon::USE);
            }

            if (object->type() == Game::Object::Type::DUDE) {
                icons.push_back(Input::Mouse::Icon::ROTATE);
            }
            if (object->type() == Game::Object::Type::CRITTER) {
                icons.push_back(Input::Mouse::Icon::TALK);
            }

            icons.push_back(Input::Mouse::Icon::LOOK);
            icons.push_back(Input::Mouse::Icon::INVENTORY);
            icons.push_back(Input::Mouse::Icon::SKILL);
            icons.push_back(Input::Mouse::Icon::CANCEL);
            return icons;
        }


        void Location::onObjectMouseEvent(Event::Mouse *event, Game::Object *object)
        {
            if (!object) {
                return;
            }
            if (event->button() == Event::Mouse::Button::LEFT) {
                if (event->name() == "mousedown") {
                    _objectUnderCursor = object;
                    _actionCursorTimer.start();
                    _actionCursorButtonPressed = true;
                } else if (event->name() == "mouseclick") {
                    auto icons = getCursorIconsForObject(object);
                    if (!icons.empty()) {
                        // Move
                        movePlayerToObject(object);
                        // Use
                        handleAction(object, icons.front());
                        _actionCursorButtonPressed = false;
                    }
                }
            }
        }

        void Location::onObjectHover(Event::Mouse *event, Game::Object *object)
        {
            if (event->name() == "mouseout") {
                if (_objectUnderCursor == object) {
                    _objectUnderCursor = nullptr;
                }
            } else {
                if (_objectUnderCursor == nullptr || event->name() == "mousein") {
                    _objectUnderCursor = object;
                    _actionCursorButtonPressed = false;
                }

                _actionCursorTimer.start();
            }
        }

        void Location::onBackgroundClick(Event::Mouse *event)
        {
        }

        void Location::render()
        {
            auto elevation = _location->elevations()->at(_elevation);
            elevation->floor()->render();
            _lightmap->render(_camera->topLeft());
            renderCursor();
            renderObjects();
            elevation->roof()->render();
            renderObjectsText();
            renderCursorOutline();
            renderTestingOutline();
            if (active()) {
                _hexagonInfo->render();
            }
            State::render();
        }

        void Location::renderTestingOutline() const
        {
            // just for testing
            if (settings->targetHighlight()) {
                for (auto &object: _objects) {
                    if (dynamic_cast<Game::CritterObject *>(object.get())) {
                        if (!dynamic_cast<Game::DudeObject *>(object.get())) {
                            object->renderOutline(1);
                        }
                    }
                }
            }
        }

        // render hex object outline
        void Location::renderCursorOutline() const
        {
            if (mouse->state() == Input::Mouse::Cursor::HEXAGON_RED) {
                mouse->renderOutline();
            }
        }

        //render only flat objects first
        void Location::renderObjects() const
        {
            for (auto &object: _flatObjects) {
                object->render();
            }

            for (auto &object: _objects) {
                object->render();
            }
        }

        void Location::renderObjectsText() const
        {
            for (auto &object: _objects) {
                object->renderText();
            }
        }

        // render hex cursor
        void Location::renderCursor() const
        {
            if (mouse->state() == Input::Mouse::Cursor::HEXAGON_RED) {
                mouse->render();
            }
        }

        void Location::think(const float &deltaTime)
        {
            gameTime->think(deltaTime);
            thinkObjects(deltaTime);
            player->think(deltaTime);
            performScrolling(deltaTime);
            if (_locationEnter) {
                _locationEnter = false;
                firstLocationEnter(deltaTime);
            }
            processTimers(deltaTime);
            State::think(deltaTime);
        }

        // timers processing
        void Location::processTimers(const float &deltaTime)
        {
            _locationScriptTimer.think(deltaTime);
            _actionCursorTimer.think(deltaTime);
            _ambientSfxTimer.think(deltaTime);

            for (auto it = _timerEvents.begin(); it != _timerEvents.end();) {
                it->timer.think(deltaTime);
                if (!it->timer.enabled()) {
                    it = _timerEvents.erase(it);
                } else ++it;
            }
        }

        void Location::firstLocationEnter(const float &deltaTime) const
        {
            if (_location->script()) {
                _location->script()->initialize();
            }

            for (auto &object : _objects) {
                if (object->script()) {
                    object->script()->initialize();
                }
            }
            for (auto &spatial: _spatials) {
                if (spatial->script()) {
                    spatial->script()->initialize();
                }
            }
            if (player->script()) {
                player->script()->initialize();
            }

            if (_location->script()) {
                _location->script()->call("map_enter_p_proc");
            }

            // By some reason we need to use reverse iterator to prevent scripts problems
            // If we use normal iterators, some exported variables are not initialized on the moment
            // when script is called
            player->map_enter_p_proc();
            for (auto it = _objects.rbegin(); it != _objects.rend(); ++it) {
                (*it)->map_enter_p_proc();
            }
        }

        void Location::performScrolling(const float &deltaTime)
        {
            float scrollSpeed = 5.0f /* pixels */ / 10.0f /* ms */;
            int scrollDelta = scrollSpeed * deltaTime;

            Point pScrollDelta = Point(
                this->_scrollLeft ? -scrollDelta : (this->_scrollRight ? scrollDelta : 0),
                this->_scrollTop ? -scrollDelta : (this->_scrollBottom ? scrollDelta : 0)
            );
            this->_camera->setCenter(this->_camera->center() + pScrollDelta);

            // if scrolling is active
            if (this->_scrollLeft || this->_scrollRight || this->_scrollTop || this->_scrollBottom) {
                Input::Mouse::Cursor state;
                if (this->_scrollLeft) {
                    state = Input::Mouse::Cursor::SCROLL_W;
                }
                if (this->_scrollRight) {
                    state = Input::Mouse::Cursor::SCROLL_E;
                }
                if (this->_scrollTop) {
                    state = Input::Mouse::Cursor::SCROLL_N;
                }
                if (this->_scrollBottom) {
                    state = Input::Mouse::Cursor::SCROLL_S;
                }
                if (this->_scrollLeft && this->_scrollTop) {
                    state = Input::Mouse::Cursor::SCROLL_NW;
                }
                if (this->_scrollLeft && this->_scrollBottom) {
                    state = Input::Mouse::Cursor::SCROLL_SW;
                }
                if (this->_scrollRight && this->_scrollTop) {
                    state = Input::Mouse::Cursor::SCROLL_NE;
                }
                if (this->_scrollRight && this->_scrollBottom) {
                    state = Input::Mouse::Cursor::SCROLL_SE;
                }
                if (mouse->state() != state) {
                    if (mouse->scrollState()) {
                        mouse->popState();
                    }
                    mouse->pushState(state);
                }
            } else {
                if (mouse->scrollState()) {
                    mouse->popState();
                }
            }
        }

        void Location::thinkObjects(const float &deltaTime) const
        {
            for (auto &object : _objects) {
                object->think(deltaTime);
            }
        }

        void Location::toggleCursorMode()
        {
            // Just for testing. This case should never happen in real life
            if (mouse->state() == Input::Mouse::Cursor::NONE) {
                mouse->pushState(Input::Mouse::Cursor::ACTION);
                return;
            }

            if (mouse->state() == Input::Mouse::Cursor::ACTION) {
                auto hexagon = hexagonGrid()->hexagonAt(mouse->position() + _camera->topLeft());
                if (!hexagon) {
                    return;
                }
                mouse->pushState(Input::Mouse::Cursor::HEXAGON_RED);
                mouse->ui()->setPosition(hexagon->position() - _camera->topLeft());
                _objectUnderCursor = nullptr;
                return;
            }

            if (mouse->state() == Input::Mouse::Cursor::HEXAGON_RED) {
                mouse->popState();
                return;
            }

            if (mouse->state() == Input::Mouse::Cursor::USE) {
                mouse->setState(Input::Mouse::Cursor::ACTION);
                return;
            }
        }

        void Location::handle(Event::Event *event)
        {
            State::handle(event);
            if (event->handled()) {
                return;
            }

            if (auto mouseEvent = dynamic_cast<Event::Mouse *>(event)) {
                using Mouse = Event::Mouse;

                if (mouseEvent->originalType() == Mouse::Type::BUTTON_DOWN) {
                    emitEvent(std::make_unique<Event::Mouse>(*mouseEvent), _mouseDownHandler);
                }

                if (mouseEvent->originalType() == Mouse::Type::BUTTON_UP) {
                    emitEvent(std::make_unique<Event::Mouse>(*mouseEvent), _mouseUpHandler);
                }

                if (mouseEvent->originalType() == Mouse::Type::MOVE) {
                    emitEvent(std::make_unique<Event::Mouse>(*mouseEvent), _mouseMoveHandler);
                }

                // let event fall down to all objects when using action cursor and within active view
                if (!mouseEvent->handled() &&
                    (mouse->state() == Input::Mouse::Cursor::ACTION || mouse->state() == Input::Mouse::Cursor::NONE)) {
                    auto elevation = _location->elevations()->at(_elevation);
                    if (!elevation->roof()->opaque(mouse->position())) {
                        handleByGameObjects(mouseEvent);
                    }
                }
            }
        }

        void Location::handleByGameObjects(Event::Mouse *event)
        {
            for (auto it = _objects.rbegin(); it != _objects.rend(); ++it) {
                auto object = (*it).get();
                if (event->handled()) {
                    return;
                }
                if (!object->inRender()) {
                    continue;
                }
                object->handle(event);
            }

            // sadly, flat objects do handle events.
            for (auto it = _flatObjects.rbegin(); it != _flatObjects.rend(); ++it) {
                auto object = (*it).get();
                if (event->handled()) {
                    return;
                }
                if (!object->inRender()) {
                    continue;
                }
                object->handle(event);
            }
            /*
                auto hexagons = _hexagonGrid->hexagons();
                for (auto it = hexagons.rbegin(); it != hexagons.rend(); ++it)
                {
                    Hexagon* hexagon = *it;
                    if (!hexagon->inRender()) continue;
                    auto objects = hexagon->objects();
                    for (auto itt = objects->rbegin(); itt != objects->rend(); ++itt)
                    {
                        if (event->handled()) return;
                        auto object = *itt;
                        if (!object->inRender()) continue;
                        object->handle(event);
                    }
                }
            */
        }

        void Location::onMouseDown(Event::Mouse *event)
        {
            if (event->rightButton()) {
                toggleCursorMode();
                event->setHandled(true);
            }
        }

        void Location::onMouseUp(Event::Mouse *event)
        {
            // Player movement
            if (event->leftButton()) {
                if (mouse->state() == Input::Mouse::Cursor::HEXAGON_RED) {
                    // Here goes the movement
                    auto hexagon = hexagonGrid()->hexagonAt(mouse->position() + _camera->topLeft());
                    if (hexagon) {
                        auto path = hexagonGrid()->findPath(player->hexagon(), hexagon);
                        if (path.size()) {
                            player->stopMovement();
                            player->setRunning((_lastClickedTile != 0 && hexagon->number() == _lastClickedTile) ||
                                               (event->shiftPressed() != settings->running()));
                            for (auto pathHexagon : path) {
                                player->movementQueue()->push_back(pathHexagon);
                            }
                        }
                        event->setHandled(true);
                        _lastClickedTile = hexagon->number();
                    }
                }

                // Using a skill
                if (mouse->state() == Input::Mouse::Cursor::USE) {
                    auto object = getGameObjectUnderCursor();

                    if (!object) return;

                    // Move
                    movePlayerToObject(object);

                    // TODO: Animate

                    // Use
                    object->use_skill_on_p_proc(skillInUse(), object, player.get());
                    mouse->setState(Input::Mouse::Cursor::ACTION);
                }
            }
        }

        void Location::onMouseMove(Event::Mouse *mouseEvent)
        {
            auto hexagon = hexagonGrid()->hexagonAt(mouse->position() + _camera->topLeft());
            if (mouse->states()->empty()) {
                mouse->setState(Input::Mouse::Cursor::ACTION);
            }
            if (mouse->state() == Input::Mouse::Cursor::HEXAGON_RED && hexagon) {
                mouse->ui()->setPosition(hexagon->position() - _camera->topLeft());
            }

            int scrollArea = 8;
            Point mpos = mouse->position();
            _scrollLeft = (mpos.x() < scrollArea);
            _scrollRight = (mpos.x() > renderer->width() - scrollArea);
            _scrollTop = (mpos.y() < scrollArea);
            _scrollBottom = (mpos.y() > renderer->height() - scrollArea);

            if (hexagon) {
                std::string text = "Hex number: " + std::to_string(hexagon->number()) + "\n";
                text += "Hex position: " + std::to_string(hexagon->number() % 200) + "," +
                        std::to_string((unsigned int) (hexagon->number() / 200)) + "\n";
                text += "Hex coords: " + std::to_string(hexagon->position().x()) + "," +
                        std::to_string(hexagon->position().y()) + "\n";
                auto hex = player->hexagon();
                text += "Hex delta:\n dx=" + std::to_string(hex->cubeX() - hexagon->cubeX()) + "\n dy=" +
                        std::to_string(hex->cubeY() - hexagon->cubeY()) + "\n dz=" +
                        std::to_string(hex->cubeZ() - hexagon->cubeZ()) + "\n";
                text += "Hex light: " + std::to_string(hexagon->light()) + "\n";
                _hexagonInfo->setText(text);
            } else {
                _hexagonInfo->setText("No hex");
            }
        }

        void Location::onKeyDown(Event::Keyboard *event)
        {
            if (event->keyCode() == SDLK_m) {
                toggleCursorMode();
            }

            if (event->keyCode() == SDLK_COMMA) {
                // rotate left
                player->setOrientation(player->orientation() + 5);
            }

            if (event->keyCode() == SDLK_PERIOD) {
                // rotate right
                player->setOrientation(player->orientation() + 1);
            }

            if (event->keyCode() == SDLK_HOME) {
                centerCameraAtHexagon(player->hexagon());
            }

            // @TODO: SDLK_PLUS || SDLK_KP_PLUS - increase brightness
            // @TODO: SDLK_MINUS || SDLK_KP_MINUS - decrease brightness
            // @TODO: SDLK_1 - use skill: sneak
            // @TODO: SDLK_2 - use skill: lockpick
            // @TODO: SDLK_3 - use skill: steal
            // @TODO: SDLK_4 - use skill: traps
            // @TODO: SDLK_5 - use skill: first aid
            // @TODO: SDLK_6 - use skill: doctor
            // @TODO: SDLK_7 - use skill: science
            // @TODO: SDLK_8 - use skill: repair

            if (event->keyCode() == SDLK_LEFT) {
                _camera->setCenter(_camera->center() + Point(-KEYBOARD_SCROLL_STEP, 0));
            }

            if (event->keyCode() == SDLK_RIGHT) {
                _camera->setCenter(_camera->center() + Point(KEYBOARD_SCROLL_STEP, 0));
            }

            if (event->keyCode() == SDLK_UP) {
                _camera->setCenter(_camera->center() + Point(0, -KEYBOARD_SCROLL_STEP));
            }

            if (event->keyCode() == SDLK_DOWN) {
                _camera->setCenter(_camera->center() + Point(0, KEYBOARD_SCROLL_STEP));
            }
        }

        bool Location::movePlayerToObject(Game::Object *object)
        {
            // Find path to object
            auto hexagon = object->hexagon();

            for (auto adjacentHex : hexagon->neighbors()) {
                if (!adjacentHex->canWalkThru()) {
                    continue;
                }

                auto path = hexagonGrid()->findPath(player->hexagon(), adjacentHex);

                if (path.size()) {
                    /* Remove the last hexagon from the path so the player stops on
                    an adjacent tile (rather than on the tile the object occupies) */
                    path.pop_back();

                    player->stopMovement();
                    player->setRunning(true);

                    // Move!
                    for (auto pathHexagon : path) {
                        player->movementQueue()->push_back(pathHexagon);
                    }
                    // The player was able to move to an adjacent tile
                    return true;
                }
            }
            // There wasn't a clear path
            return false;
        }

        LocationCamera *Location::camera()
        {
            return _camera.get();
        }

        void Location::setMVAR(unsigned int number, int value)
        {
            if (number >= _location->MVARS()->size()) {
                throw Exception("Location::setMVAR(num, value) - num out of range: " + std::to_string((int) number));
            }
            _location->MVARS()->at(number) = value;
        }

        int Location::MVAR(unsigned int number)
        {
            if (number >= _location->MVARS()->size()) {
                throw Exception("Location::MVAR(num) - num out of range: " + std::to_string((int) number));
            }
            return _location->MVARS()->at(number);
        }

        std::map<std::string, VM::StackValue> *Location::EVARS()
        {
            return &_EVARS;
        }

        void Location::moveObjectToHexagon(Game::Object *object, Hexagon *hexagon, bool update)
        {
            auto elevation = _location->elevations()->at(_elevation);

            auto oldHexagon = object->hexagon();
            if (oldHexagon) {
                if (update) {
                    //_hexagonGrid->initLight(oldHexagon, false);
                }

                for (auto it = oldHexagon->objects()->begin(); it != oldHexagon->objects()->end(); ++it) {
                    if (*it == object) {
                        oldHexagon->objects()->erase(it);
                        break;
                    }
                }

                /* JUST FOR EXIT GRIDS TESTING*/
                if (object->type() == Game::Object::Type::DUDE) {
                    for (auto obj : *hexagon->objects()) {
                        if (auto exitGrid = dynamic_cast<Game::ExitMiscObject *>(obj)) {
                            auto &debug = Logger::critical("LOCATION");
                            debug << " PID: 0x" << std::hex << exitGrid->PID() << std::dec << std::endl;
                            debug << " name: " << exitGrid->name() << std::endl;
                            debug << " exitMapNumber: " << exitGrid->exitMapNumber() << std::endl;
                            debug << " exitElevationNumber: " << exitGrid->exitElevationNumber() << std::endl;
                            debug << " exitHexagonNumber: " << exitGrid->exitHexagonNumber() << std::endl;
                            debug << " exitDirection: " << exitGrid->exitDirection() << std::endl << std::endl;

                            if (exitGrid->exitMapNumber() < 0) {
                                auto worldMapState = new WorldMap(resourceManager);
                                // TODO delegate state manipulation to some kind of state manager
                                Game::Game::getInstance()->setState(worldMapState);
                                return;
                            }

                            auto mapsFile = ResourceManager::getInstance()->mapsTxt();
                            std::string mapName = mapsFile->maps().at(exitGrid->exitMapNumber()).name;

                            GameLocationHelper gameLocationHelper(logger);
                            auto location = gameLocationHelper.getByName(mapName);
                            location->setDefaultPosition(exitGrid->exitHexagonNumber());
                            location->setDefaultOrientation(exitGrid->exitDirection());
                            location->setDefaultElevationIndex(exitGrid->exitElevationNumber());

                            // TODO move this instantiation to StateLocationHelper or some kind of state manager
                            auto state = new Location(player, mouse, settings, renderer, audioMixer, gameTime, resourceManager, logger);
                            state->setLocation(location);
                            // TODO delegate state manipulation to some kind of state manager
                            Game::Game::getInstance()->setState(state);

                            return;
                        }
                    }
                }
            }

            object->setHexagon(hexagon);
            if (hexagon) {
                hexagon->objects()->push_back(object);
            }

            if (object->type() == Game::Object::Type::CRITTER || object->type() == Game::Object::Type::DUDE)
                for (auto &spatial: _spatials) {
                    if (_hexagonGrid->distance(spatial->hexagon(), hexagon) <= spatial->radius()) {
                        spatial->spatial_p_proc(object);
                    }
                }

            // TODO: recreate _objects array for rendering/handling
            if (update) {
                _objects.sort(
                        [](std::shared_ptr<Game::Object> &obj1, std::shared_ptr<Game::Object> &obj2) -> bool {
                            return obj1->hexagon()->number() < obj2->hexagon()->number();
                        }
                );
                _flatObjects.sort(
                        [](std::shared_ptr<Game::Object> &obj1, std::shared_ptr<Game::Object> &obj2) -> bool {
                            return obj1->hexagon()->number() < obj2->hexagon()->number();
                        }
                );
                if (hexagon) {
                    initLight();
                    /*_hexagonGrid->initLight(hexagon, true);
                    std::vector<float> lights;
                    for (auto hex: _hexagonGrid->hexagons())
                    {
                        int lightLevel = 100;
                        unsigned int light = hex->light();
                        if (light<=_lightLevel) light=655;
                        lightLevel = light / ((65536-655)/100);

                        float l = lightLevel/100.0;
                        lights.push_back(l);
                    }
                    _lightmap->update(lights);*/

                }
            }

            if (auto dude = dynamic_cast<Game::DudeObject *>(object)) {
                int x = dude->hexagon()->number() % 200;
                int y = dude->hexagon()->number() / 200;
                x /= 2;
                y /= 2;
                int tilenum = y * 100 + x;

                if (!elevation->roof()->inside() && elevation->roof()->tiles().count(tilenum)) {
                    // we was outside, now are inside
                    elevation->roof()->disable(tilenum);
                    elevation->roof()->setInside(true);
                } else if (elevation->roof()->inside() && !elevation->roof()->tiles().count(tilenum)) {
                    // we was inside, now are outside
                    elevation->roof()->enableAll();
                    elevation->roof()->setInside(false);
                }
            }
        }

        void Location::removeObjectFromMap(Game::Object *object)
        {
            auto hexagon = object->hexagon();

            if (hexagon != nullptr) {
                auto objectsAtHex = hexagon->objects();

                for (auto it = objectsAtHex->begin(); it != objectsAtHex->end(); ++it) {
                    if (*it == object) {
                        objectsAtHex->erase(it);
                        break;
                    }
                }
            } else {
                auto &warning =  Logger::warning("LOCATION");
                warning << "removeObjectFromMap(): object->hexagon() == nullptr for PID = ";
                warning << object->PID() << " (" << object->name() << ")" << std::endl;
            }

            if (_objectUnderCursor == object) {
                _objectUnderCursor = nullptr;
            }
            for (auto it = _objects.begin(); it != _objects.end(); ++it) {
                if ((*it).get() == object) {
                    _objects.erase(it);
                    break;
                }
            }
        }

        void Location::destroyObject(Game::Object *object)
        {
            object->destroy_p_proc();
            removeObjectFromMap(object);
        }

        void Location::centerCameraAtHexagon(Hexagon *hexagon)
        {
            _camera->setCenter(hexagon->position());
        }

        void Location::centerCameraAtHexagon(int tileNum)
        {
            try {
                centerCameraAtHexagon(_hexagonGrid->at((unsigned int) tileNum));
            } catch (const std::out_of_range &) {
                throw Exception(std::string("Tile number out of range: ") + std::to_string(tileNum));
            }
        }

        void Location::handleAction(Game::Object *object, Input::Mouse::Icon action)
        {
            using Input::Mouse;

            switch (action)
            {
                case Mouse::Icon::LOOK:
                    object->description_p_proc();
                    break;


                case Mouse::Icon::USE:
                {
                    auto animation = player->setActionAnimation("al");
                    // Move to object
                    auto playerObject = player;
                    animation->actionFrameHandler().add([object, playerObject](Event::Event *event) {
                        object->onUseAnimationActionFrame(event, playerObject.get());
                    });
                    break;
                }

                case Mouse::Icon::ROTATE:
                {
                    auto dude = dynamic_cast<Game::DudeObject *>(object);

                    if (!dude)
                        throw Exception("Location::handleAction() - only Dude can be rotated");

                    auto orientation = dude->orientation() + 1;

                    if (orientation >= HEX_SIDES)
                        orientation = 0;

                    dude->setOrientation(orientation);
                    break;
                }

                case Mouse::Icon::TALK:
                {
                    auto critter = dynamic_cast<Game::CritterObject *>(object);

                    if (!critter)
                        throw Exception("Location::handleAction() - can talk only with critters!");

                    critter->talk_p_proc();
                    break;
                }

                default:
                    return;
            }
        }

        void Location::displayMessage(const std::string &message)
        {
            _playerPanel->displayMessage(message);
            Logger::info("MESSAGE") << message << std::endl;
        }

        HexagonGrid *Location::hexagonGrid()
        {
            return _hexagonGrid.get();
        }

        UI::PlayerPanel *Location::playerPanel()
        {
            return _playerPanel;
        }

        void Location::addTimerEvent(Game::Object *obj, int ticks, int fixedParam)
        {
            Game::Timer timer(static_cast<float>(ticks) * 100.0f);
            timer.start();
            timer.tickHandler().add([obj, fixedParam](Event::Event *) {
                if (obj) {
                    if (auto vm = obj->script()) {
                        vm->setFixedParam(fixedParam);
                        vm->call("timed_event_p_proc");
                    }
                }
            });
            _timerEvents.emplace_back(TimerEvent {obj, std::move(timer), fixedParam});
        }

        void Location::removeTimerEvent(Game::Object *obj)
        {
            _timerEvents.remove_if([=](Location::TimerEvent &item) { return item.object == obj; });
        }

        void Location::removeTimerEvent(Game::Object *obj, int fixedParam)
        {
            _timerEvents.remove_if(
                [=](Location::TimerEvent &item) {
                    return item.object == obj && item.fixedParam == fixedParam;
                }
            );
        }

        unsigned int Location::lightLevel()
        {
            return _lightLevel;
        }

        void Location::setLightLevel(unsigned int level)
        {
            // TODO: check night vision perk and increase light by 20% (20% of what, maximum or current?)
            if (level > 0x10000) {
                level = 0x10000;
            }
            if (level < 0x4000) {
                level = 0x4000;
            }
            _lightLevel = level;
            initLight();
        }

        void Location::initLight()
        {
            for (auto hex: _hexagonGrid->hexagons()) {
                hex->setLight(655);
            }

            for (auto hex: _hexagonGrid->hexagons()) {
                _hexagonGrid->initLight(hex);
            }

            std::vector<float> lights;
            for (auto hex: _hexagonGrid->hexagons()) {
                int lightLevel;

                unsigned int light = hex->light();

                if (light <= _lightLevel) {
                    light = 655;
                }

                lightLevel = light / ((65536 - 655) / 100);

                auto l = static_cast<float>(lightLevel / 100.0);
                lights.push_back(l);
            }
            _lightmap->update(lights);
        }

        Game::Object *Location::addObject(unsigned int PID, unsigned int position, unsigned int elevation)
        {
            Game::ObjectFactory objectFactory(logger);

            auto object = objectFactory.createObjectByPID(PID);
            _objects.emplace_back(object);
            moveObjectToHexagon(object, hexagonGrid()->at(position));
            object->setElevation(elevation);
            return object;
        }

        unsigned int Location::currentMapIndex()
        {
            return _currentMap;
        }

        std::shared_ptr<Falltergeist::Game::Location> Location::location()
        {
            return _location;
        }

        unsigned int Location::elevation() const
        {
            return _elevation;
        }

        void Location::setElevation(unsigned int elevation)
        {
            _elevation = elevation;
        }

        Game::Object* Location::getGameObjectUnderCursor()
        {
            for (auto it = _objects.rbegin(); it != _objects.rend(); ++it) {
                auto object = (*it).get();
                if (!object->inRender()) {
                    continue;
                }

                Point position = mouse->position() - object->ui()->position() + object->ui()->offset();
                if (object->ui()->opaque(position)) {
                    return object;
                }
            }

            return nullptr;
        }

        SKILL Location::skillInUse() const
        {
            return _skillInUse;
        }

        void Location::setSkillInUse(SKILL skill)
        {
            _skillInUse = skill;
        }
    }
}
