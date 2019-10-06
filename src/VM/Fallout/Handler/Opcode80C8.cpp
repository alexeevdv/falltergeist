#include "Opcode80C8.h"
#include "VM/Fallout/IContext.h"
#include "VM/Fallout/IStack.h"
#include "VM/Fallout/IStackValue.h"
#include "Game/Object.h"

namespace Falltergeist
{
    namespace VM
    {
        namespace Handler
        {
            void Opcode80C8::applyTo(std::shared_ptr<IContext> context)
            {
                // @TODO: implement
                auto object = context->dataStack()->pop()->asObject();
                Game::Object::Type type = object->type();
                switch (type) {
                    case Game::Object::Type::CRITTER:
                    case Game::Object::Type::DUDE:
                        context->dataStack()->push(1);
                        break;
                    default:
                        context->dataStack()->push((int) type);
                        break;

                }
                //_script->dataStack()->push(object);
            }

            int Opcode80C8::number()
            {
                return 0x80C8;
            }

            std::string Opcode80C8::name()
            {
                return "int obj_type(void* obj)";
            }

            std::string Opcode80C8::notes()
            {
                return "";
            }
        }
    }
}


