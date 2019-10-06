#include "Opcode80C1.h"
#include "VM/Fallout/IContext.h"
#include "VM/Fallout/IStack.h"
#include "VM/Fallout/IStackValue.h"

namespace Falltergeist
{
    namespace VM
    {
        namespace Handler
        {
            void Opcode80C1::applyTo(std::shared_ptr<IContext> context)
            {
                unsigned int num = context->dataStack()->pop()->asInteger();
                while (num >= context->LVARS()->size()) {
                    context->LVARS()->push_back(StackValue(0));
                }
                context->dataStack()->push(context->LVARS()->at(num));
            }

            int Opcode80C1::number()
            {
                return 0x80C1;
            }

            std::string Opcode80C1::name()
            {
                return "LVAR[num]";
            }

            std::string Opcode80C1::notes()
            {
                return "";
            }
        }
    }
}
