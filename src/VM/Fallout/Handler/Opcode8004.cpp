#include "../../../VM/Fallout/Handler/Opcode8004.h"
#include "../../../VM/Fallout/IContext.h"
#include "../../../VM/Fallout/IStack.h"
#include "../../../VM/Fallout/IStackValue.h"

namespace Falltergeist
{
    namespace VM
    {
        namespace Fallout
        {
            namespace Handler
            {
                void Opcode8004::applyTo(std::shared_ptr<IContext>& context)
                {
                    auto address = context->dataStack()->pop()->asInteger();
                    context->setProgramCounter((unsigned) address);
                }

                int Opcode8004::number()
                {
                    return 0x8004;
                }

                std::string Opcode8004::name()
                {
                    return "void op_jmp(int address)";
                }

                std::string Opcode8004::notes()
                {
                    return "Implemented";
                }
            }
        }
    }
}
