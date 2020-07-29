#pragma once

#include "../UI/Base.h"
#include <map>

namespace Falltergeist
{
    namespace Event
    {
        class Keyboard;
        class Mouse;
    }

    namespace UI
    {
        class TextArea;
        class Panel : public UI::Base
        {
            public:

                Panel(const Point& pos = Point(0, 0));
                void setVisible(bool value) override;
                void handle(Event::Event *event) override;
                void think(const float &deltaTime) override;
                void render(bool eggTransparency) override;
                UI::Base* addUI(UI::Base* ui);
                UI::Base* addUI(const std::string& name, UI::Base* ui);
                void addUI(const std::vector<UI::Base*>& uis);
                UI::Base* getUI(const std::string& name);
                UI::TextArea* getTextArea(const std::string& name);

            private:
                Size _size;
                std::vector<std::unique_ptr<UI::Base>> _children;
                std::vector<std::unique_ptr<UI::Base>> _childrenToDelete;
                std::map<std::string, UI::Base*> _childrenLabeledUI;
        };
    }
}
