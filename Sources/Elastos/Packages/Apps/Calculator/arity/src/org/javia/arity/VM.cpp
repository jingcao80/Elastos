
#include "org/javia/arity/VM.h"

namespace Org {
namespace Javia {
namespace Arity {

const Byte VM::RESERVED;
const Byte VM::_CONST;
const Byte VM::CALL;
const Byte VM::ADD;
const Byte VM::SUB;
const Byte VM::MUL;
const Byte VM::DIV;
const Byte VM::MOD;
const Byte VM::RND;
const Byte VM::UMIN;
const Byte VM::POWER;
const Byte VM::FACT;
const Byte VM::PERCENT;
const Byte VM::SQRT;
const Byte VM::CBRT;
const Byte VM::EXP;
const Byte VM::LN;
const Byte VM::SIN;
const Byte VM::COS;
const Byte VM::TAN;
const Byte VM::ASIN;
const Byte VM::ACOS;
const Byte VM::ATAN;
const Byte VM::SINH;
const Byte VM::COSH;
const Byte VM::TANH;
const Byte VM::ASINH;
const Byte VM::ACOSH;
const Byte VM::ATANH;
const Byte VM::ABS;
const Byte VM::FLOOR;
const Byte VM::CEIL;
const Byte VM::SIGN;
const Byte VM::MIN;
const Byte VM::MAX;
const Byte VM::GCD;
const Byte VM::COMB;
const Byte VM::PERM;
const Byte VM::LOAD0;
const Byte VM::LOAD1;
const Byte VM::LOAD2;
const Byte VM::LOAD3;
const Byte VM::LOAD4;
const Byte VM::REAL;
const Byte VM::IMAG;

static AutoPtr<ArrayOf<String> > InitOpcodeName()
{
    AutoPtr<ArrayOf<String> > names = ArrayOf<String>::Alloc(45);
    (*names)[0] = String("reserved");
    (*names)[1] = String("const");
    (*names)[2] = String("call");
    (*names)[3] = String("add");
    (*names)[4] = String("sub");
    (*names)[5] = String("mul");
    (*names)[6] = String("div");
    (*names)[7] = String("mod");
    (*names)[8] = String("rnd");
    (*names)[9] = String("umin");
    (*names)[10] = String("power");
    (*names)[11] = String("fact");
    (*names)[12] = String("percent");
    (*names)[13] = String("sqrt");
    (*names)[14] = String("cbrt");
    (*names)[15] = String("exp");
    (*names)[16] = String("ln");
    (*names)[17] = String("sin");
    (*names)[18] = String("cos");
    (*names)[19] = String("tan");
    (*names)[20] = String("asin");
    (*names)[21] = String("acos");
    (*names)[22] = String("atan");
    (*names)[23] = String("sinh");
    (*names)[24] = String("cosh");
    (*names)[25] = String("tanh");
    (*names)[26] = String("asinh");
    (*names)[27] = String("acosh");
    (*names)[28] = String("atanh");
    (*names)[29] = String("abs");
    (*names)[30] = String("floor");
    (*names)[31] = String("ceil");
    (*names)[32] = String("sign");
    (*names)[33] = String("min");
    (*names)[34] = String("max");
    (*names)[35] = String("gcd");
    (*names)[36] = String("comb");
    (*names)[37] = String("perm");
    (*names)[38] = String("load0");
    (*names)[39] = String("load1");
    (*names)[40] = String("load2");
    (*names)[41] = String("load3");
    (*names)[42] = String("load4");
    (*names)[43] = String("real");
    (*names)[44] = String("imag");
    return names;
}
const AutoPtr<ArrayOf<String> > VM::OPCODENAME = InitOpcodeName();

static AutoPtr<ArrayOf<Byte> > InitArity()
{
    AutoPtr<ArrayOf<Byte> > aritys = ArrayOf<Byte>::Alloc(45);
    (*aritys)[0] = 0;
    (*aritys)[1] = 0;
    (*aritys)[2] = -1;
    (*aritys)[3] = 2;
    (*aritys)[4] = 2;
    (*aritys)[5] = 2;
    (*aritys)[6] = 2;
    (*aritys)[7] = 2;
    (*aritys)[8] = 0;
    (*aritys)[9] = 1;
    (*aritys)[10] = 2;
    (*aritys)[11] = 1;
    (*aritys)[12] = 1;
    (*aritys)[13] = 1;
    (*aritys)[14] = 1;
    (*aritys)[15] = 1;
    (*aritys)[16] = 1;
    (*aritys)[17] = 1;
    (*aritys)[18] = 1;
    (*aritys)[19] = 1;
    (*aritys)[20] = 1;
    (*aritys)[21] = 1;
    (*aritys)[22] = 1;
    (*aritys)[23] = 1;
    (*aritys)[24] = 1;
    (*aritys)[25] = 1;
    (*aritys)[26] = 1;
    (*aritys)[27] = 1;
    (*aritys)[28] = 1;
    (*aritys)[29] = 1;
    (*aritys)[30] = 1;
    (*aritys)[31] = 1;
    (*aritys)[32] = 1;
    (*aritys)[33] = 2;
    (*aritys)[34] = 2;
    (*aritys)[35] = 2;
    (*aritys)[36] = 2;
    (*aritys)[37] = 2;
    (*aritys)[38] = 0;
    (*aritys)[39] = 0;
    (*aritys)[40] = 0;
    (*aritys)[41] = 0;
    (*aritys)[42] = 0;
    (*aritys)[43] = 1;
    (*aritys)[44] = 1;
    return aritys;
}
const AutoPtr<ArrayOf<Byte> > VM::ARITY = InitArity();

static AutoPtr<ArrayOf<Byte> > InitBuiltins()
{
    AutoPtr<ArrayOf<Byte> > builtins = ArrayOf<Byte>::Alloc(29);
    (*builtins)[0] = VM::RND;
    (*builtins)[1] = VM::SQRT;
    (*builtins)[2] = VM::CBRT;
    (*builtins)[3] = VM::SIN;
    (*builtins)[4] = VM::COS;
    (*builtins)[5] = VM::TAN;
    (*builtins)[6] = VM::ASIN;
    (*builtins)[7] = VM::ACOS;
    (*builtins)[8] = VM::ATAN;
    (*builtins)[9] = VM::SINH;
    (*builtins)[10] = VM::COSH;
    (*builtins)[11] = VM::TANH;
    (*builtins)[12] = VM::ASINH;
    (*builtins)[13] = VM::ACOSH;
    (*builtins)[14] = VM::ATANH;
    (*builtins)[15] = VM::EXP;
    (*builtins)[16] = VM::LN;
    (*builtins)[17] = VM::ABS;
    (*builtins)[18] = VM::FLOOR;
    (*builtins)[19] = VM::CEIL;
    (*builtins)[20] = VM::SIGN;
    (*builtins)[21] = VM::MIN;
    (*builtins)[22] = VM::MAX;
    (*builtins)[23] = VM::GCD;
    (*builtins)[24] = VM::COMB;
    (*builtins)[25] = VM::PERM;
    (*builtins)[26] = VM::MOD;
    (*builtins)[27] = VM::REAL;
    (*builtins)[28] = VM::IMAG;
    return builtins;
}
const AutoPtr<ArrayOf<Byte> > VM::BUILTINS = InitBuiltins();

} // namespace Arity
} // namespace Javia
} // namespace Org