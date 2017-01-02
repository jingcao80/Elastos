//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef _ORG_JAVIA_ARITY_VM_H__
#define _ORG_JAVIA_ARITY_VM_H__

#include <elastos.h>

namespace Org {
namespace Javia {
namespace Arity {

class VM
{
public:
    static const Byte RESERVED = 0;
    static const Byte _CONST = 1;
    static const Byte CALL = 2;
    static const Byte ADD = 3;
    static const Byte SUB = 4;
    static const Byte MUL = 5;
    static const Byte DIV = 6;
    static const Byte MOD = 7;
    static const Byte RND = 8;
    static const Byte UMIN = 9;
    static const Byte POWER = 10;
    static const Byte FACT = 11;
    static const Byte PERCENT = 12;
    static const Byte SQRT = 13;
    static const Byte CBRT = 14;
    static const Byte EXP = 15;
    static const Byte LN = 16;
    static const Byte SIN = 17;
    static const Byte COS = 18;
    static const Byte TAN = 19;
    static const Byte ASIN = 20;
    static const Byte ACOS = 21;
    static const Byte ATAN = 22;
    static const Byte SINH = 23;
    static const Byte COSH = 24;
    static const Byte TANH = 25;
    static const Byte ASINH = 26;
    static const Byte ACOSH = 27;
    static const Byte ATANH = 28;
    static const Byte ABS = 29;
    static const Byte FLOOR = 30;
    static const Byte CEIL = 31;
    static const Byte SIGN = 32;
    static const Byte MIN = 33;
    static const Byte MAX = 34;
    static const Byte GCD = 35;
    static const Byte COMB = 36;
    static const Byte PERM = 37;
    static const Byte LOAD0 = 38;
    static const Byte LOAD1 = 39;
    static const Byte LOAD2 = 40;
    static const Byte LOAD3 = 41;
    static const Byte LOAD4 = 42;
    static const Byte REAL = 43;
    static const Byte IMAG = 44;

    static const AutoPtr<ArrayOf<String> > OPCODENAME;
    static const AutoPtr<ArrayOf<Byte> > ARITY;
    static const AutoPtr<ArrayOf<Byte> > BUILTINS;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_VM_H__