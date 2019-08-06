#include "../../VM/Handler/Opcode802A.h"
#include "../../VM/IFalloutStack.h"
#include "../../VM/IFalloutContext.h"

namespace Falltergeist
{
    namespace VM
    {
        namespace Handler
        {
            void Opcode802A::applyTo(std::shared_ptr<IFalloutContext> context)
            {
                while (context->dataStack()->size() > context->dynamicVarStackBase()) {
                    context->dataStack()->pop();
                }
            }

            int Opcode802A::number()
            {
                return 0x802A;
            }

            std::string Opcode802A::name()
            {
                return "op_pop_to_base";
            }

            std::string Opcode802A::notes()
            {
                return "";
            }
        }
    }
}
