#pragma once

#include "VM/Fallout/IOpcode.h"
#include "Exception.h"

namespace Falltergeist {
    namespace VM {
        namespace Handler {
            class OpcodeComparison : virtual public IOpcode {
            public:
                enum class Type {
                    EQUAL = 1,
                    NOT_EQUAL,
                    LESS,
                    LESS_EQUAL,
                    GREATER,
                    GREATER_EQUAL
                };

                OpcodeComparison(std::shared_ptr<VM::Script> script, Type cmpType);

            private:
                Type _cmpType;

                void _run();

                const char *_cmpOpcodeName();

                template<class T1, class T2>
                int _compare(T1 arg1, T2 arg2) {
                    switch (_cmpType) {
                        case Type::EQUAL: {
                            return arg1 == arg2;
                        }
                        case Type::NOT_EQUAL: {
                            return arg1 != arg2;
                        }
                        case Type::LESS: {
                            return arg1 < arg2;
                        }
                        case Type::LESS_EQUAL: {
                            return arg1 <= arg2;
                        }
                        case Type::GREATER: {
                            return arg1 > arg2;
                        }
                        case Type::GREATER_EQUAL: {
                            return arg1 >= arg2;
                        }
                        default: {
                            throw Exception("Invalid compare type!");
                        }
                    }
                }
            };
        }
    }
}
