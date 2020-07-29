#include "../UI/Panel.h"
#include "../UI/Base.h"
#include "../UI/TextArea.h"
#include "../Event/Event.h"
#include "../Event/Mouse.h"
#include "../Graphics/Renderer.h"

namespace Falltergeist
{
    using namespace std;

    namespace UI
    {
        Panel::Panel(const Point& pos) : Base(pos)
        {
        }

        UI::Base* Panel::addUI(UI::Base* ui)
        {
            // Add to UI state position
            ui->setPosition(ui->position() - ui->offset() + position());

            _children.push_back(std::unique_ptr<UI::Base>(ui));
            return ui;
        }

        UI::Base* Panel::addUI(const std::string& name, UI::Base* ui)
        {
            addUI(ui);
            _childrenLabeledUI.insert(std::pair<std::string, UI::Base*>(name, ui));
            return ui;
        }

        void Panel::addUI(const std::vector<UI::Base*>& uis)
        {
            for (auto ui : uis) {
                addUI(ui);
            }
        }

        UI::Base* Panel::getUI(const std::string& name)
        {
            if (_childrenLabeledUI.find(name) != _childrenLabeledUI.end()) {
                return _childrenLabeledUI.at(name);
            }
            return nullptr;
        }

        UI::TextArea* Panel::getTextArea(const std::string& name)
        {
            return dynamic_cast<UI::TextArea*>(getUI(name));
        }

        void Panel::setVisible(bool value)
        {
            _visible = value;
        }

        void Panel::handle(Event::Event *event)
        {
            UI::Base::handle(event);
            if (auto mouseEvent = dynamic_cast<Event::Mouse*>(event))
            {
                mouseEvent->setObstacle(false);
                mouseEvent->setHandled(false);

                if (event->handled()) {
                    return;
                }

                if (_visible) {
                    for (auto it = _children.begin(); it != _children.end(); it++)
                    {
                       (*it)->handle(dynamic_cast<Event::Mouse*>(event));
                    }
                }
            }
        }

        void Panel::think(const float &deltaTime)
        {
            UI::Base::think(deltaTime);
            for (auto it = _children.begin(); it != _children.end(); it++)
            {
                (*it)->think(deltaTime);
            }
        }

        void Panel::render(bool eggTransparency)
        {
            if (_visible) {
                UI::Base::render(eggTransparency);

                for (auto it = _children.begin(); it != _children.end(); it++)
                {
                    if ((*it)->visible()) {
                        (*it)->render(eggTransparency);
                    }
                }
            }
        }
    }
}
