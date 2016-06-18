
#include "org/javia/arity/CSymbols.h"
#include "org/javia/arity/CCompiler.h"
#include "org/javia/arity/CSymbol.h"
#include "org/javia/arity/VM.h"
#include "org/javia/arity/CFunctionAndName.h"
#include <elastos/core/Math.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/etl/Vector.h>

using Org::Javia::Arity::EIID_ISymbols;
using Elastos::Core::Autolock;
using Elastos::Utility::Etl::Vector;

namespace Org {
namespace Javia {
namespace Arity {

//==========================================================
// CSymbols::StaticInitializer
//==========================================================

CSymbols::StaticInitializer::StaticInitializer()
{
    Vector<AutoPtr<ISymbol> > vect = new Vector();
    Int32 arity;
    for (Int32 i = 0; i < VM::BUILTINS->GetLength(); ++i) {
        Byte b = (*VM::BUILTINS)[i];
        vect.PushBack(CSymbol::MakeVmOp((*VM::OPCODENAME)[b], b));
    }

    AutoPtr<ArrayOf<String> > IMPLICIT_ARGS = ArrayOf<String>::Alloc(3);
    (*IMPLICIT_ARGS)[0] = String("x");
    (*IMPLICIT_ARGS)[0] = String("y");
    (*IMPLICIT_ARGS)[0] = String("z");
    for (Int32 i = 0; i < IMPLICIT_ARGS->GetLength(); ++i) {
        vect.PushBack(CSymbol::MakeArg((*IMPLICIT_ARGS)[i], (Byte)i));
    }

    AutoPtr<ISymbol> s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12;
    CSymbol::New(String("pi"), Elastos::Core::Math::PI, TRUE, (ISymbol**)&s1);
    vect.PushBack(s1);
    CSymbol::New(String("\u03c0"), Elastos::Core::Math::PI, TRUE, (ISymbol**)&s2);
    vect.PushBack(s2);
    CSymbol::New(String("e"), Elastos::Core::Math::E, TRUE, (ISymbol**)&s3);
    vect.PushBack(s3);

    Double infinity = Elastos::Core::Math::DOUBLE_POSITIVE_INFINITY;
    CSymbol::New(String("Infinity"), infinity, TRUE, (ISymbol**)&s4);
    vect.PushBack(s4);
    CSymbol::New(String("infinity"), infinity, TRUE, (ISymbol**)&s5);
    vect.PushBack(s5);
    CSymbol::New(String("Inf"), infinity, TRUE, (ISymbol**)&s6);
    vect.PushBack(s6);
    CSymbol::New(String("inf"), infinity, TRUE, (ISymbol**)&s7);
    vect.PushBack(s7);
    CSymbol::New(String("\u221e"), infinity, TRUE, (ISymbol**)&s8);
    vect.PushBack(s8);
    CSymbol::New(String("NaN"), Elastos::Core::Math::DOUBLE_NAN, TRUE, (ISymbol**)&s9);
    vect.PushBack(s9);
    CSymbol::New(String("nan"), Elastos::Core::Math::DOUBLE_NAN, TRUE, (ISymbol**)&s10);
    vect.PushBack(s10);

    CSymbol::New(String("i"), 0, 1, TRUE, (ISymbol**)&s11);
    vect.PushBack(s11);
    CSymbol::New(String("j"), 0, 1, FALSE, (ISymbol**)&s12);
    vect.PushBack(s12);

    Int32 size = vect->GetSize();
    CSymbols::sBuiltin = ArrayOf<ISymbol*>::Alloc[size];
    Vector<AutoPtr<ISymbol> >::Iterator it = vect.Begin();
    for (Int32 i = 0; it != vect.End(); ++it, ++i) {
        CSymbols::sBuiltin->Set(i, *it);
    }
}


//==========================================================
// CSymbols
//==========================================================

AutoPtr<ArrayOf<ISymbol*> > CSymbols::sBuiltin;

static AutoPtr<ISymbol> InitShell()
{
    AutoPtr<ISymbol> symbol;
    CSymbol::New(NULL, 0, FALSE, (ISymbol**)&symbol);
    return symbol;
}
const AutoPtr<ISymbol> CSymbols::SHELL = InitShell();

AutoPtr<ArrayOf<String> > InitDefines()
{
    AutoPtr<ArrayOf<String> > defines = ArrayOf<String>::Alloc(20);
    (*defines)[0] = String("log(x)=ln(x)*0.43429448190325182765");
    (*defines)[1] = String("log10(x)=log(x)");
    (*defines)[2] = String("lg(x)=log(x)");
    (*defines)[3] = String("log2(x)=ln(x)*1.4426950408889634074");
    (*defines)[4] = String("lb(x)=log2(x)");
    (*defines)[5] = String("log(base,x)=ln(x)/ln(base)");
    (*defines)[6] = String("gamma(x)=(x-1)!");
    (*defines)[7] = String("deg=0.017453292519943295");
    (*defines)[8] = String("indeg=57.29577951308232");
    (*defines)[9] = String("sind(x)=sin(x deg)");
    (*defines)[10] = String("cosd(x)=cos(x deg)");
    (*defines)[11] = String("tand(x)=tan(x deg)");
    (*defines)[12] = String("asind(x)=asin(x) indeg");
    (*defines)[13] = String("acosd(x)=acos(x) indeg");
    (*defines)[14] = String("atand(x)=atan(x) indeg");
    (*defines)[15] = String("tg(x)=tan(x)");
    (*defines)[16] = String("tgd(x)=tand(x)");
    (*defines)[17] = String("rnd(max)=rnd()*max");
    (*defines)[18] = String("re(x)=real(x)");
    (*defines)[19] = String("im(x)=imag(x)");
    return defines;
}
const AutoPtr<ArrayOf<String> > CSymbols::DEFINES = InitDefines();
CSymbols::StaticInitializer CSymbols::sInitializer;

CSymbols::CSymbols()
{
    mCompiler = new Compiler();
}

CAR_OBJ_IMPL(CSymbols)

CAR_INTERFACE_DECL(CSymbols, Object, IComplex)

ECode CSymbols::constructor()
{
    for (Int32 i = 0; i < sBuiltin->GetLength(); ++i) {
        Add((*sBuiltin)[i]);
        /*
            Symbol s = builtin[i];
            symbols.put(s, s);
        */
    }
    // try {
    for (Int32 i = 0; i < DEFINES->GetLength(); ++i) {
        AutoPtr<IFunctionAndName> fan;
        FAIL_RETURN(CompileWithName((*DEFINES)[i], (IFunctionAndName**)&fan))
        Define(fan);
    }
    // } catch (SyntaxException e) {
    //     throw new Error(""+e); //never
    // }
    return NOERROR;
}

Boolean CSymbols::IsDefinition(
    /* [in] */ const String& source)
{
    return source.IndexOf('=') != -1;
}

ECode CSymbols::Eval(
    /* [in] */ const String& expression,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    Autolock Lock(this);
    AutoPtr<IFunction> func;
    FAIL_RETURN(mCompiler->CompileSimple((ISymbols*)this, expression, (IFunction**)&func))
    return func->Eval(result);
}

ECode CSymbols::EvalComplex(
    /* [in] */ const String& expression,
    /* [out] */ IComplex** complex)
{
    VALIDATE_NOT_NULL(complex)
    Autolock Lock(this);
    *complex = NULL;
    FAIL_RETURN(mCompiler->CompileSimple((ISymbols*)this, expression, (IFunction**)&func))
    return func->EvalComplex(complex);
}

ECode CSymbols::CompileWithName(
    /* [in] */ const String& source,
    /* [out] */ IFunctionAndName** fan)
{
    VALIDATE_NOT_NULL(fan)
    Autolock Lock(this);
    return mCompiler->CompileWithName((ISymbols)this, source, fan);
}

ECode CSymbols::Compile(
    /* [in] */ const String& source,
    /* [out] */ IFunction** func)
{
    VALIDATE_NOT_NULL(func)
    Autolock Lock(this);
    return mCompiler->Compile((ISymbols)this, source, func);
}

ECode CSymbols::Define(
    /* [in] */ const String& name,
    /* [in] */ IFunction* function)
{
    Autolock Lock(this);
    if (IConstant::Probe(function) != NULL) {
        Double value;
        function->Eval(&value);
        Define(name, value);
    }
    else {
        AutoPtr<ISymbol> s;
        CSymbol::New(name, function, (ISymbol**)&s);
        Add(s);
    }
    return NOERROR;
}

ECode CSymbols::Define(
    /* [in] */ IFunctionAndName* funAndName)
{
    Autolock Lock(this);
    AutoPtr<CFunctionAndName> obj = (CFunctionAndName*)funAndName;
    if (!obj-mName.IsNull()) {
        Define(obj->mName, obj->mFunction);
    }
    return NOERROR;
}

ECode CSymbols::Define(
    /* [in] */ const String& name,
    /* [in] */ Double value)
{
    Autolock Lock(this);
    AutoPtr<ISymbol> s;
    CSymbol::New(name, value, 0, FALSE, (ISymbol**)&s);
    Add(s);
    return NOERROR;
}

ECode CSymbols::Define(
    /* [in] */ const String& name,
    /* [in] */ IComplex* value)
{
    Autolock Lock(this);
    AutoPtr<CComplex> obj = (CComplex*)value;
    AutoPtr<ISymbol> s;
    CSymbol::New(name, obj->mRe, obj->mIm, FALSE, (ISymbol**)&s);
    Add(s);
    return NOERROR;
}

ECode CSymbols::PushFrame()
{
    Autolock Lock(this);
    mFrames.Push(mDelta);
    mDelta = NULL;
    return NOERROR;
}

ECode CSymbols::PopFrame()
{
    Autolock Lock(this);
    if (mDelta != NULL) {
        HashSet<AutoPtr<ISymbol> >::Iterator it = mDelta->Begin();
        for (; it != mDelta->End(); ++it) {
            AutoPtr<ISymbol> previous = *it;
            Boolean isEmpty;
            if (previous->IsEmpty(&isEmpty), isEmpty) {
                mSymbols.Erase(previous);
            }
            else {
                mSymbols[previous] = previous;
            }
        }
    }
    mDelta = mFrames.GetTop();
    mFrames.Pop();
}

ECode CSymbols::GetTopFrame(
    /* [out, callee] */ ArrayOf<ISymbol*>** topFrame)
{
    VALIDATE_NOT_NULL(topFrame)
    AutoPtr<ArrayOf<ISymbol*> > frames = ArrayOf<ISymbol*>::Alloc(0);
    if (mDelta != NULL) {
        frames = ArrayOf<ISymbol*>::Alloc(mDelta->GetSize());
        HashSet<AutoPtr<ISymbol> >::Iterator it = mDelta->Begin();
        for (Int32 i = 0; it != mDelta->End(); ++it, ++i) {
            frames->Set(i, *it);
        }
    }
    *topFrame= frames;
    REFCOUNT_ADD(*topFrame)
    return NOERROR;
}

ECode CSymbols::GetAllSymbols(
    /* [out, callee] */ ArrayOf<ISymbol*>** symbols)
{
    VALIDATE_NOT_NULL(symbols)
    Int32 size = mSymbols.GetSize();
    *symbols = ArrayOf<ISymbol*>::Alloc(size);
    Hashtable<AutoPtr<ISymbol>, AutoPtr<ISymbol> >::Iterator it = mSymbols.Begin();
    for (Int32 i = 0; it != mSymbols.End(); ++it, ++i) {
        (*symbols)->Set(i, it->mFirst);
    }
    REFCOUNT_ADD(*symbols)
    return NOERROR;
}

ECode CSymbols::GetDictionary(
    /* [out, callee] */ ArrayOf<String>** dic)
{
    VALIDATE_NOT_NULL(dic)
    AutoPtr<ArrayOf<ISymbol*> > syms;
    GetAllSymbols((ArrayOf<ISymbol*>**)&syms);
    Int32 size = syms->GetLength();
    AutoPtr<ArrayOf<String> > strings = ArrayOf<String>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        (*syms)[i]->GetName(&(*strings)[i]);
    }
    *dic = strings;
    REFCOUNT_ADD(*dic)
    return NOERROR;
}

void CSymbols::AddArguments(
    /* [in] */ ArrayOf<String>* args)
{
    for (Int32 i = 0; i < args->GetLength(); ++i) {
        Add(CSymbol::MakeArg((*args)[i], (Byte)i));
    }
}

void CSymbols::Add(
    /* [in] */ ISymbol* s);

AutoPtr<ISymbol> CSymbols::Lookup(
    /* [in] */ const String& name,
    /* [in] */ Int32 arity)
{
    Autolock Lock(this);
}

AutoPtr<ISymbol> CSymbols::LookupConst(
    /* [in] */ const String& name);

} // namespace Calculator2
} // namespace Droid
} // namespace Elastos