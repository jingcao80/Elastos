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

#include "org/javia/arity/CompiledFunction.h"
#include "org/javia/arity/CComplex.h"
#include "org/javia/arity/MoreMath.h"
#include "org/javia/arity/VM.h"
#include "org/javia/arity/CCompiledFunction.h"
#include "org/javia/arity/CEvalContext.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>
#include <math.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::CRandom;
using Elastos::Utility::Logging::Slogger;
using Org::Javia::Arity::EIID_ICompiledFunction;

namespace Org {
namespace Javia {
namespace Arity {

//==============================================================================
//                  CompiledFunction::DerivativeFunction
//==============================================================================

ECode CompiledFunction::DerivativeFunction::Arity(
    /* [out] */ Int32* arity)
{
    VALIDATE_NOT_NULL(arity)
    *arity = 1;
    return NOERROR;
}

ECode CompiledFunction::DerivativeFunction::Eval(
    /* [in] */ Double x,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = x > 0 ? 1 : x < 0 ? -1 : 0;
    return NOERROR;
}


//==============================================================================
//                  CompiledFunction
//==============================================================================

static AutoPtr<IRandom> InitRandom()
{
    AutoPtr<IRandom> random;
    CRandom::New((IRandom**)&random);
    return random;
}
const AutoPtr<IRandom> CompiledFunction::RANDOM = InitRandom();

const AutoPtr<ArrayOf<Double> > CompiledFunction::EMPTY_DOUBLE = ArrayOf<Double>::Alloc(0);

const AutoPtr<ArrayOf<IFunction*> > CompiledFunction::EMPTY_FUN = ArrayOf<IFunction*>::Alloc(0);

static AutoPtr<IComplex> InitOneThird()
{
    AutoPtr<IComplex> complex;
    CComplex::New(1/3., 0, (IComplex**)&complex);
    return complex;
}
const AutoPtr<IComplex> CompiledFunction::ONE_THIRD = InitOneThird();

CAR_INTERFACE_IMPL(CompiledFunction, ContextFunction, ICompiledFunction)

ECode CompiledFunction::constructor(
    /* [in] */ Int32 arity,
    /* [in] */ ArrayOf<Byte>* code,
    /* [in] */ ArrayOf<Double>* constsRe,
    /* [in] */ ArrayOf<Double>* constsIm,
    /* [in] */ ArrayOf<IFunction*>* funcs)
{
    mArity    = arity;
    mCode     = code;
    mConstsRe = constsRe;
    mConstsIm = constsIm;
    mFuncs    = funcs;
    return NOERROR;
}

ECode CompiledFunction::MakeOpFunction(
    /* [in] */ Int32 op,
    /* [out] */ IFunction** func)
{
    VALIDATE_NOT_NULL(func)
    *func = NULL;
    if ((*VM::ARITY)[op] != 1) {
        Slogger::E("CompiledFunction", "makeOpFunction expects arity 1, found %d", (*VM::ARITY)[op]);
        return E_ARITY_EXCEPTION;
    }
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(2);
    (*bytes)[0] = VM::LOAD0;
    (*bytes)[1] = (Byte)op;
    AutoPtr<CCompiledFunction> fun;
    CCompiledFunction::NewByFriend((*VM::ARITY)[op], bytes, EMPTY_DOUBLE, EMPTY_DOUBLE, EMPTY_FUN, (CCompiledFunction**)&fun);
    if (op == VM::ABS) {
        AutoPtr<IFunction> func = (IFunction*)new DerivativeFunction();
        fun->SetDerivative(func);
    }
    *func = IFunction::Probe(fun);
    REFCOUNT_ADD(*func)
    return NOERROR;
}

ECode CompiledFunction::Arity(
    /* [out] */ Int32* arity)
{
    VALIDATE_NOT_NULL(arity)
    *arity = mArity;
    return NOERROR;
}

ECode CompiledFunction::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder buf;
    Int32 cpos = 0, fpos = 0;
    if (mArity != 0) {
        buf.Append("arity ");
        buf.Append(mArity);
        buf.Append("; ");
    }
    for (Int32 i = 0; i < mCode->GetLength(); ++i) {
        Byte op = (*mCode)[i];
        buf.Append((*VM::OPCODENAME)[op]);
        if (op == VM::_CONST) {
            buf.Append(' ');
            if (mConstsIm == NULL) {
                buf.Append((*mConstsRe)[cpos]);
            }
            else {
                buf.AppendChar('(');
                buf.Append((*mConstsRe)[cpos]);
                buf.Append(", ");
                buf.Append((*mConstsIm)[cpos]);
                buf.AppendChar(')');
            }
            ++cpos;
        }
        else if (op == VM::CALL) {
            ++fpos;
            //buf.append(" {").append(funcs[fpos++].toString()).append('}');
        }
        buf.Append("; ");
    }
    if (cpos != mConstsRe->GetLength()) {
        buf.Append("\nuses only ");
        buf.Append(cpos);
        buf.Append(" consts out of ");
        buf.Append(mConstsRe->GetLength());
    }
    if (fpos != mFuncs->GetLength()) {
        buf.Append("\nuses only ");
        buf.Append(fpos);
        buf.Append(" funcs out of ");
        buf.Append(mFuncs->GetLength());
    }
    *str = buf.ToString();
    return NOERROR;
}

ECode CompiledFunction::Eval(
    /* [in] */ ArrayOf<Double>* args,
    /* [in] */ IEvalContext* context,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mConstsIm != NULL) {
        return EvalComplexToReal(args, context, result);
    }
    FAIL_RETURN(CheckArity(args->GetLength()))
    AutoPtr<CEvalContext> obj = (CEvalContext*)context;
    obj->mStackRe->Copy(obj->mStackBase, args, 0, args->GetLength());
    // try {
    Int32 value;
    if (FAILED(ExecReal(context, obj->mStackBase + args->GetLength() - 1, &value))) {
        return EvalComplexToReal(args, context, result);
    }
    *result = (*obj->mStackRe)[obj->mStackBase];
    return NOERROR;
    // } catch (IsComplexException e) {
    //     return evalComplexToReal(args, context);
    // }
}

ECode CompiledFunction::EvalComplexToReal(
    /* [in] */ ArrayOf<Double>* args,
    /* [in] */ IEvalContext* context,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    AutoPtr<ArrayOf<IComplex*> > argsC = ToComplex(args, context);
    AutoPtr<IComplex> c;
    FAIL_RETURN(Eval(argsC, context, (IComplex**)&c))
    return c->AsReal(result);
}

ECode CompiledFunction::Eval(
    /* [in] */ ArrayOf<IComplex*>* args,
    /* [in] */ IEvalContext* context,
    /* [out] */ IComplex** complex)
{
    VALIDATE_NOT_NULL(complex)
    *complex = NULL;
    FAIL_RETURN(CheckArity(args->GetLength()))
    AutoPtr<CEvalContext> obj = (CEvalContext*)context;
    AutoPtr<ArrayOf<IComplex*> > stack = obj->mStackComplex;
    Int32 base = obj->mStackBase;
    for (Int32 i = 0; i < args->GetLength(); ++i) {
        (*stack)[i + base]->Set((*args)[i]);
    }
    Int32 result;
    FAIL_RETURN(ExecComplex(context, base + args->GetLength() - 1, &result))
    *complex = (*stack)[base];
    REFCOUNT_ADD(*complex)
    return NOERROR;
}

ECode CompiledFunction::ExecReal(
    /* [in] */ IEvalContext* context,
    /* [in] */ Int32 p,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    Int32 expected = p + 1;
    Int32 value;
    FAIL_RETURN(ExecWithoutCheck(context, p, &value))
    p = value;
    if (p != expected) {
        Slogger::E("CompiledFunction", "Stack pointer after exec: expected %d, got %d", expected, p);
        return E_INVALID_ARGUMENT;
    }
    AutoPtr<CEvalContext> obj = (CEvalContext*)context;
    (*obj->mStackRe)[p - mArity] = (*obj->mStackRe)[p];
    *result = p - mArity;
    return NOERROR;
}

ECode CompiledFunction::ExecComplex(
    /* [in] */ IEvalContext* context,
    /* [in] */ Int32 p,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    Int32 expected = p + 1;
    Int32 value;
    FAIL_RETURN(ExecWithoutCheckComplex(context, p, -2, &value))
    p = value;
    if (p != expected) {
        Slogger::E("CompiledFunction", "Stack pointer after exec: expected %d, got %d", expected, p);
        return E_INVALID_ARGUMENT;
    }
    AutoPtr<CEvalContext> obj = (CEvalContext*)context;
    (*obj->mStackComplex)[p - mArity]->Set((*obj->mStackComplex)[p]);
    *result = p - mArity;
    return NOERROR;
}

ECode CompiledFunction::ExecWithoutCheck(
    /* [in] */ IEvalContext* context,
    /* [in] */ Int32 p,
    /* [out] */ Int32* _result)
{
    VALIDATE_NOT_NULL(_result)
    *_result = 0;
    if (mConstsIm != NULL) {
        return E_IS_COMPLEX_EXCEPTION;
    }

    AutoPtr<CEvalContext> obj = (CEvalContext*)context;
    AutoPtr<ArrayOf<Double> > s = obj->mStackRe;

    Int32 stackBase = p - mArity;
    Int32 constp = 0;
    Int32 funp   = 0;

    Int32 codeLen = mCode->GetLength();
    Int32 percentPC = -2;
    for (Int32 pc = 0; pc < codeLen; ++pc) {
        Int32 opcode = (*mCode)[pc];
        switch (opcode) {
            case VM::_CONST:
                (*s)[++p] = (*mConstsRe)[constp++];
                break;

            case VM::CALL: {
                AutoPtr<IFunction> f = (*mFuncs)[funp++];
                AutoPtr<ICompiledFunction> compiledF = ICompiledFunction::Probe(f);
                if (compiledF != NULL) {
                    ((CompiledFunction*)compiledF.Get())->ExecReal(context, p, &p);
                }
                else {
                    Int32 arity;
                    f->Arity(&arity);
                    p -= arity;
                    Double result;
                    Int32 prevBase = obj->mStackBase;
                    // try {
                    obj->mStackBase = p + 1;
                    switch (arity) {
                        case 0:
                            f->Eval(&result);
                            break;
                        case 1:
                            f->Eval((*s)[p+1], &result);
                            break;
                        case 2:
                            f->Eval((*s)[p+1], (*s)[p+2], &result);
                            break;
                        default:
                            AutoPtr<ArrayOf<Double> > args = ArrayOf<Double>::Alloc(arity);
                            args->Copy(0, s, p+1, arity);
                            f->Eval(args, &result);
                    }
                    // } finally {
                    //     context.stackBase = prevBase;
                    // }
                    obj->mStackBase = prevBase;
                    (*s)[++p] = result;
                    //System.out.println(": " + p + " " + s[0] + " " + s[1] + " " + s[2]);
                }
                break;
            }

            case VM::RND:
                RANDOM->NextDouble(&(*s)[++p]);
                break;

            case VM::ADD: {
                Double a = (*s)[--p];
                Double res = a + (percentPC == pc-1 ? (*s)[p] * (*s)[p+1] : (*s)[p+1]);
                if (Elastos::Core::Math::Abs(res) < Elastos::Core::Math::Ulp(a) * 1024) {
                    // hack for "1.1-1-.1"
                    res = 0;
                }
                (*s)[p] = res;
                break;
            }

            case VM::SUB: {
                Double a = (*s)[--p];
                Double res = a - (percentPC == pc-1 ? (*s)[p] * (*s)[p+1] : (*s)[p+1]);
                if (Elastos::Core::Math::Abs(res) < Elastos::Core::Math::Ulp(a) * 1024) {
                    // hack for "1.1-1-.1"
                    res = 0;
                }
                (*s)[p] = res;
                break;
            }

            case VM::MUL:
                --p;
                (*s)[p] *= (*s)[p+1];
                break;
            case VM::DIV:
                --p;
                (*s)[p] /= (*s)[p+1];
                break;
            case VM::MOD: {
                --p;
                Double v = fmod((*s)[p], (*s)[p+1]);
                (*s)[p] = v;
                break;
            }

            case VM::POWER: {
                Double v = Elastos::Core::Math::Pow((*s)[p], (*s)[p+1]);
                (*s)[--p] = v;
                break;
            }

            case VM::UMIN: (*s)[p] = -(*s)[p]; break;
            case VM::FACT: (*s)[p] = MoreMath::Factorial((*s)[p]); break;
            case VM::PERCENT: (*s)[p] = (*s)[p] * .01; percentPC = pc; break;

            case VM::SIN: (*s)[p] = MoreMath::Sin((*s)[p]); break;
            case VM::COS: (*s)[p] = MoreMath::Cos((*s)[p]); break;
            case VM::TAN: (*s)[p] = MoreMath::Tan((*s)[p]); break;

            case VM::ASIN: {
                Double v = (*s)[p];
                if (v < -1 || v > 1) {
                    return E_IS_COMPLEX_EXCEPTION;
                }
                (*s)[p] = Elastos::Core::Math::Asin(v);
                break;
            }

            case VM::ACOS: {
                Double v = (*s)[p];
                if (v < -1 || v > 1) {
                    return E_IS_COMPLEX_EXCEPTION;
                }
                (*s)[p] = Elastos::Core::Math::Acos(v);
                break;
            }

            case VM::ATAN: (*s)[p] = Elastos::Core::Math::Atan((*s)[p]); break;

            case VM::EXP: (*s)[p] = Elastos::Core::Math::Exp((*s)[p]); break;
            case VM::LN: (*s)[p] = Elastos::Core::Math::Log((*s)[p]); break;

            case VM::SQRT: {
                Double v = (*s)[p];
                if (v < 0) {
                    return E_IS_COMPLEX_EXCEPTION;
                }
                (*s)[p] = Elastos::Core::Math::Sqrt(v);
                break;
            }

            case VM::CBRT: (*s)[p] = Elastos::Core::Math::Cbrt((*s)[p]); break;

            case VM::SINH: (*s)[p] = Elastos::Core::Math::Sinh((*s)[p]); break;
            case VM::COSH: (*s)[p] = Elastos::Core::Math::Cosh((*s)[p]); break;
            case VM::TANH: (*s)[p] = Elastos::Core::Math::Tanh((*s)[p]); break;
            case VM::ASINH: (*s)[p] = MoreMath::Asinh((*s)[p]); break;
            case VM::ACOSH: (*s)[p] = MoreMath::Acosh((*s)[p]); break;
            case VM::ATANH: (*s)[p] = MoreMath::Atanh((*s)[p]); break;

            case VM::ABS:   (*s)[p] = Elastos::Core::Math::Abs((*s)[p]); break;
            case VM::FLOOR: (*s)[p] = Elastos::Core::Math::Floor((*s)[p]); break;
            case VM::CEIL:  (*s)[p] = Elastos::Core::Math::Ceil((*s)[p]); break;
            case VM::SIGN: {
                Double v = (*s)[p];
                (*s)[p] = v > 0 ? 1 : v < 0 ? -1 : v == 0 ? 0 : Elastos::Core::Math::DOUBLE_NAN;
                break;
            }

            case VM::MIN: {
                Double v = Elastos::Core::Math::Min((*s)[p], (*s)[p+1]);
                (*s)[--p] = v;
                break;
            }
            case VM::MAX: {
                Double v = Elastos::Core::Math::Max((*s)[p], (*s)[p+1]);
                (*s)[--p] = v;
                break;
            }
            case VM::GCD: {
                Double v = MoreMath::Gcd((*s)[p], (*s)[p+1]);
                (*s)[--p] = v;
                break;
            }
            case VM::COMB: {
                Double v = MoreMath::Combinations((*s)[p], (*s)[p+1]);
                (*s)[--p] = v;
                break;
            }
            case VM::PERM: {
                Double v = MoreMath::Permutations((*s)[p], (*s)[p+1]);
                (*s)[--p] = v;
                break;
            }

            case VM::LOAD0:
            case VM::LOAD1:
            case VM::LOAD2:
            case VM::LOAD3:
            case VM::LOAD4:
                //System.out.println("base " + stackBase + "; p " + p + "; arity " + arity);
                (*s)[++p] = (*s)[stackBase + opcode - (VM::LOAD0 - 1)];
                break;

            case VM::REAL:
                break; // NOP

            case VM::IMAG:
                if (!Elastos::Core::Math::IsNaN((*s)[p])) {
                    (*s)[p] = 0;
                }
                break;

            default:
                Slogger::E("CompiledFunction", "Unknown opcode %d", opcode);
                return E_INVALID_ARGUMENT;
        }
    }
    *_result = p;
    return NOERROR;
}

ECode CompiledFunction::ExecWithoutCheckComplex(
    /* [in] */ IEvalContext* context,
    /* [in] */ Int32 p,
    /* [in] */ Int32 percentPC,
    /* [out] */ Int32* _result)
{
    VALIDATE_NOT_NULL(_result)
    *_result = 0;
    AutoPtr<CEvalContext> obj = (CEvalContext*)context;
    AutoPtr<ArrayOf<IComplex*> > s = obj->mStackComplex;

    Int32 stackBase = p - mArity;
    Int32 constp = 0;
    Int32 funp = 0;

    Int32 codeLen = mCode->GetLength();
    // System.out.println("exec " + this);
    for (Int32 pc = 0; pc < codeLen; ++pc) {
        Int32 opcode = (*mCode)[pc];
        // System.out.println("+ " + pc + ' ' + opcode + ' ' + p);
        switch (opcode) {
        case VM::_CONST:
            ++p;
            (*s)[p]->Set((*mConstsRe)[constp], mConstsIm == NULL ? 0 : (*mConstsIm)[constp]);
            ++constp;
            break;

        case VM::CALL: {
            AutoPtr<IFunction> f = (*mFuncs)[funp++];
            AutoPtr<ICompiledFunction> compiledF = ICompiledFunction::Probe(f);
            if (compiledF != NULL) {
                ((CompiledFunction*)compiledF.Get())->ExecComplex(context, p, &p);
            }
            else {
                Int32 arity;
                f->Arity(&arity);
                p -= arity;
                AutoPtr<IComplex> result;
                Int32 prevBase = obj->mStackBase;
                // try {
                obj->mStackBase = p + 1;
                switch (arity) {
                    case 0:
                        Double eval;
                        f->Eval(&eval);
                        CComplex::New(eval, 0, (IComplex**)&result);
                        break;
                    case 1:
                        f->Eval((*s)[p+1], (IComplex**)&result);
                        break;
                    case 2:
                        f->Eval((*s)[p+1], (*s)[p+2], (IComplex**)&result);
                        break;
                    default:
                        AutoPtr<ArrayOf<IComplex*> > args = ArrayOf<IComplex*>::Alloc(arity);
                        args->Copy(0, s, p+1, arity);
                        f->Eval(args, (IComplex**)&result);
                }
                // } finally {
                //     context.stackBase = prevBase;
                // }
                obj->mStackBase = prevBase;
                (*s)[++p]->Set(result);
            }
            break;
        }

        case VM::RND: {
            Double d;
            RANDOM->NextDouble(&d);
            (*s)[++p]->Set(d, 0);
            break;
        }

        case VM::ADD: {
            if (percentPC == pc-1) {
                (*s)[p+1]->Mul((*s)[p]);
            }
            IComplex* o = (*s)[p+1];
            (*s)[--p]->Add(o);
            break;
        }
        case VM::SUB: {
            if (percentPC == pc-1) {
                (*s)[p+1]->Mul((*s)[p]);
            }
            IComplex* o = (*s)[p+1];
            (*s)[--p]->Sub(o);
            break;
        }
        case VM::MUL: {
            IComplex* o = (*s)[p+1];
            (*s)[--p]->Mul(o);
            break;
        }
        case VM::DIV: {
            IComplex* o = (*s)[p+1];
            (*s)[--p]->Div(o);
            break;
        }
        case VM::MOD: {
            IComplex* o = (*s)[p+1];
            (*s)[--p]->Mod(o);
            break;
        }
        case VM::POWER: {
            IComplex* o = (*s)[p+1];
            (*s)[--p]->Pow(o);
            break;
        }

        case VM::UMIN: (*s)[p]->Negate();    break;
        case VM::FACT: (*s)[p]->Factorial(); break;
        case VM::PERCENT: (*s)[p]->Mul(.01); percentPC = pc; break;

        case VM::SIN:   (*s)[p]->Sin();   break;
        case VM::COS:   (*s)[p]->Cos();   break;
        case VM::TAN:   (*s)[p]->Tan();   break;
        case VM::SINH:  (*s)[p]->Sinh();  break;
        case VM::COSH:  (*s)[p]->Cosh();  break;
        case VM::TANH:  (*s)[p]->Tanh();  break;

        case VM::ASIN:  (*s)[p]->Asin();  break;
        case VM::ACOS:  (*s)[p]->Acos();  break;
        case VM::ATAN:  (*s)[p]->Atan();  break;
        case VM::ASINH: (*s)[p]->Asinh(); break;
        case VM::ACOSH: (*s)[p]->Acosh(); break;
        case VM::ATANH: (*s)[p]->Atanh(); break;

        case VM::EXP: (*s)[p]->Exp(); break;
        case VM::LN: (*s)[p]->Log(); break;

        case VM::SQRT: (*s)[p]->Sqrt(); break;
        case VM::CBRT: {
            AutoPtr<CComplex> c = (CComplex*)(*s)[p];
            if (c->mIm == 0) {
                c->mRe = Elastos::Core::Math::Cbrt(c->mRe);
            }
            else {
                (*s)[p]->Pow(ONE_THIRD);
            }
            break;
        }

        case VM::ABS: {
            Double abs;
            (*s)[p]->Abs(&abs);
            (*s)[p]->Set(abs, 0);
            break;
        }
        case VM::FLOOR: {
            AutoPtr<CComplex> c = (CComplex*)(*s)[p];
            (*s)[p]->Set(Elastos::Core::Math::Floor(c->mRe), 0);
            break;
        }
        case VM::CEIL: {
            AutoPtr<CComplex> c = (CComplex*)(*s)[p];
            (*s)[p]->Set(Elastos::Core::Math::Ceil(c->mRe), 0);
            break;
        }
        case VM::SIGN: {
            AutoPtr<CComplex> c = (CComplex*)(*s)[p];
            Double a = c->mRe;
            Double b = c->mIm;
            Boolean isNaN;
            if (b == 0) {
                (*s)[p]->Set(a > 0 ? 1 : a < 0 ? -1 : a == 0 ? 0 : Elastos::Core::Math::DOUBLE_NAN, 0);
            }
            else if ((*s)[p]->IsNaN(&isNaN), !isNaN) {
                Double abs;
                (*s)[p]->Abs(&abs);
                (*s)[p]->Set(a / abs, b / abs);
            }
            else {
                (*s)[p]->Set(Elastos::Core::Math::DOUBLE_NAN, 0);
            }
            break;
        }

        case VM::MIN: {
            --p;
            AutoPtr<CComplex> c = (CComplex*)(*s)[p];
            AutoPtr<CComplex> c1 = (CComplex*)(*s)[p+1];
            Boolean isNaN;
            if (c1->mRe < c->mRe || (c1->IsNaN(&isNaN), isNaN)) {
                (*s)[p]->Set((*s)[p+1]);
            }
            break;
        }

        case VM::MAX: {
            --p;
            AutoPtr<CComplex> c = (CComplex*)(*s)[p];
            AutoPtr<CComplex> c1 = (CComplex*)(*s)[p+1];
            Boolean isNaN;
            if (c->mRe < c1->mRe || (c1->IsNaN(&isNaN), isNaN)) {
                (*s)[p]->Set((*s)[p+1]);
            }
            break;
        }

        case VM::GCD: {
            //s[--p] = MoreMath.gcd(s[p], s[p+1]);
            IComplex* o = (*s)[p+1];
            (*s)[--p]->Gcd(o);
            break;
        }

        case VM::COMB: {
            IComplex* o = (*s)[p+1];
            (*s)[--p]->Combinations(o);
            break;
        }

        case VM::PERM: {
            IComplex* o = (*s)[p+1];
            (*s)[--p]->Permutations(o);
            break;
        }

        case VM::LOAD0:
        case VM::LOAD1:
        case VM::LOAD2:
        case VM::LOAD3:
        case VM::LOAD4:
            (*s)[++p]->Set((*s)[stackBase + opcode - (VM::LOAD0 - 1)]);
            break;

        case VM::REAL: {
            Boolean isNaN;
            Double d;
            if ((*s)[p]->IsNaN(&isNaN), isNaN) {
                d = Elastos::Core::Math::DOUBLE_NAN;
            }
            else {
                d = ((CComplex*)(*s)[p])->mRe;
            }
            (*s)[p]->Set(d, 0);
            break;
        }

        case VM::IMAG: {
            Boolean isNaN;
            Double d;
            if ((*s)[p]->IsNaN(&isNaN), isNaN) {
                d = Elastos::Core::Math::DOUBLE_NAN;
            }
            else {
                d = ((CComplex*)(*s)[p])->mIm;
            }
            (*s)[p]->Set(d, 0);
            break;
        }

        default:
            Slogger::E("CompiledFunction", "Unknown opcode %d", opcode);
            return E_INVALID_ARGUMENT;
        }
    }
    *_result = p;
    return NOERROR;
}

} // namespace Arity
} // namespace Javia
} // namespace Org