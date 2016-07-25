
#include "org/javia/arity/CSymbols.h"
#include "org/javia/arity/Compiler.h"
#include "org/javia/arity/VM.h"
#include "org/javia/arity/CFunctionAndName.h"
#include "org/javia/arity/CComplex.h"
#include <elastos/core/Math.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/etl/Vector.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

using Org::Javia::Arity::EIID_ISymbols;
using Elastos::Core::AutoLock;
using Elastos::Utility::CHashTable;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CStack;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Etl::Vector;

namespace Org {
namespace Javia {
namespace Arity {

//==========================================================
// CSymbols::StaticInitializer
//==========================================================

CSymbols::StaticInitializer::StaticInitializer()
{
    Vector<AutoPtr<ISymbol> > vect;
    for (Int32 i = 0; i < VM::BUILTINS->GetLength(); ++i) {
        Byte b = (*VM::BUILTINS)[i];
        vect.PushBack(Symbol::MakeVmOp((*VM::OPCODENAME)[b], b));
    }

    AutoPtr<ArrayOf<String> > IMPLICIT_ARGS = ArrayOf<String>::Alloc(3);
    (*IMPLICIT_ARGS)[0] = String("x");
    (*IMPLICIT_ARGS)[0] = String("y");
    (*IMPLICIT_ARGS)[0] = String("z");
    for (Int32 i = 0; i < IMPLICIT_ARGS->GetLength(); ++i) {
        vect.PushBack(Symbol::MakeArg((*IMPLICIT_ARGS)[i], (Byte)i));
    }

    AutoPtr<ISymbol> s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12;
    s1 = new Symbol(String("pi"), Elastos::Core::Math::PI, TRUE);
    vect.PushBack(s1);
    s2 = new Symbol(String("\u03c0"), Elastos::Core::Math::PI, TRUE);
    vect.PushBack(s2);
    s3 = new Symbol(String("e"), Elastos::Core::Math::E, TRUE);
    vect.PushBack(s3);

    Double infinity = Elastos::Core::Math::DOUBLE_POSITIVE_INFINITY;
    s4 = new Symbol(String("Infinity"), infinity, TRUE);
    vect.PushBack(s4);
    s5 = new Symbol(String("infinity"), infinity, TRUE);
    vect.PushBack(s5);
    s6 = new Symbol(String("Inf"), infinity, TRUE);
    vect.PushBack(s6);
    s7 = new Symbol(String("inf"), infinity, TRUE);
    vect.PushBack(s7);
    s8 = new Symbol(String("\u221e"), infinity, TRUE);
    vect.PushBack(s8);
    s9 = new Symbol(String("NaN"), Elastos::Core::Math::DOUBLE_NAN, TRUE);
    vect.PushBack(s9);
    s10 = new Symbol(String("nan"), Elastos::Core::Math::DOUBLE_NAN, TRUE);
    vect.PushBack(s10);

    s11 = new Symbol(String("i"), 0, 1, TRUE);
    vect.PushBack(s11);
    s12 = new Symbol(String("j"), 0, 1, FALSE);
    vect.PushBack(s12);

    Int32 size = vect.GetSize();
    CSymbols::sBuiltin = ArrayOf<ISymbol*>::Alloc(size);
    Vector<AutoPtr<ISymbol> >::Iterator it = vect.Begin();
    for (Int32 i = 0; it != vect.End(); ++it, ++i) {
        CSymbols::sBuiltin->Set(i, *it);
    }
}


//==========================================================
// CSymbols
//==========================================================

AutoPtr<ArrayOf<ISymbol*> > CSymbols::sBuiltin;

static AutoPtr<Symbol> InitShell()
{
    return new Symbol(String(NULL), 0, FALSE);
}
const AutoPtr<Symbol> CSymbols::SHELL = InitShell();

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
    CHashTable::New((IHashTable**)&mSymbols);
    CStack::New((IStack**)&mFrames);
    mCompiler = new Compiler();
}

CAR_OBJECT_IMPL(CSymbols)

CAR_INTERFACE_IMPL(CSymbols, Object, ISymbols)

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
    AutoLock lock(this);
    AutoPtr<IFunction> func;
    FAIL_RETURN(mCompiler->CompileSimple(this, expression, (IFunction**)&func))
    return func->Eval(result);
}

ECode CSymbols::EvalComplex(
    /* [in] */ const String& expression,
    /* [out] */ IComplex** complex)
{
    VALIDATE_NOT_NULL(complex)
    AutoLock lock(this);
    *complex = NULL;
    AutoPtr<IFunction> func;
    FAIL_RETURN(mCompiler->CompileSimple(this, expression, (IFunction**)&func))
    return func->EvalComplex(complex);
}

ECode CSymbols::CompileWithName(
    /* [in] */ const String& source,
    /* [out] */ IFunctionAndName** fan)
{
    VALIDATE_NOT_NULL(fan)
    AutoLock lock(this);
    return mCompiler->CompileWithName(this, source, fan);
}

ECode CSymbols::Compile(
    /* [in] */ const String& source,
    /* [out] */ IFunction** func)
{
    VALIDATE_NOT_NULL(func)
    AutoLock lock(this);
    return mCompiler->Compile(this, source, func);
}

ECode CSymbols::Define(
    /* [in] */ const String& name,
    /* [in] */ IFunction* function)
{
    AutoLock lock(this);
    if (IConstant::Probe(function) != NULL) {
        Double value;
        function->Eval(&value);
        Define(name, value);
    }
    else {
        AutoPtr<ISymbol> s = new Symbol(name, function);
        Add(s);
    }
    return NOERROR;
}

ECode CSymbols::Define(
    /* [in] */ IFunctionAndName* funAndName)
{
    AutoLock lock(this);
    AutoPtr<CFunctionAndName> obj = (CFunctionAndName*)funAndName;
    if (!obj->mName.IsNull()) {
        Define(obj->mName, obj->mFunction);
    }
    return NOERROR;
}

ECode CSymbols::Define(
    /* [in] */ const String& name,
    /* [in] */ Double value)
{
    AutoLock lock(this);
    AutoPtr<ISymbol> s = new Symbol(name, value, 0, FALSE);
    Add(s);
    return NOERROR;
}

ECode CSymbols::Define(
    /* [in] */ const String& name,
    /* [in] */ IComplex* value)
{
    AutoLock lock(this);
    AutoPtr<CComplex> obj = (CComplex*)value;
    AutoPtr<ISymbol> s = new Symbol(name, obj->mRe, obj->mIm, FALSE);
    Add(s);
    return NOERROR;
}

ECode CSymbols::PushFrame()
{
    AutoLock lock(this);
    mFrames->Push(mDelta);
    mDelta = NULL;
    return NOERROR;
}

ECode CSymbols::PopFrame()
{
    AutoLock lock(this);
    if (mDelta != NULL) {
        AutoPtr<IIterator> it;
        mDelta->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> next;
            it->GetNext((IInterface**)&next);
            AutoPtr<ISymbol> previous = ISymbol::Probe(next);
            if (((Symbol*)previous.Get())->IsEmpty()) {
                mSymbols->Remove(previous);
            }
            else {
                mSymbols->Put(previous, previous);
            }
        }
    }
    AutoPtr<IInterface> pop;
    mFrames->Pop((IInterface**)&pop);
    mDelta = IHashSet::Probe(pop);
    return NOERROR;
}

ECode CSymbols::GetTopFrame(
    /* [out, callee] */ ArrayOf<ISymbol*>** topFrame)
{
    VALIDATE_NOT_NULL(topFrame)

    if (mDelta != NULL) {
        AutoPtr<ArrayOf<IInterface*> > temp = ArrayOf<IInterface*>::Alloc(0);
        AutoPtr<ArrayOf<IInterface*> > out;
        ISet::Probe(mDelta)->ToArray(temp, (ArrayOf<IInterface*>**)&out);
        *topFrame = ArrayOf<ISymbol*>::Alloc(out->GetLength());
        REFCOUNT_ADD(*topFrame)

        for (Int32 i = 0; i < out->GetLength(); ++i) {
            (*topFrame)->Set(i, ISymbol::Probe((*out)[i]));
        }
    }
    else {
        *topFrame = ArrayOf<ISymbol*>::Alloc(0);
        REFCOUNT_ADD(*topFrame)
    }
    return NOERROR;
}

ECode CSymbols::GetAllSymbols(
    /* [out, callee] */ ArrayOf<ISymbol*>** symbols)
{
    VALIDATE_NOT_NULL(symbols)
    Int32 size;
    mSymbols->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > ret = ArrayOf<IInterface*>::Alloc(size);
    *symbols = ArrayOf<ISymbol*>::Alloc(size);
    REFCOUNT_ADD(*symbols)

    AutoPtr<ISet> set;
    mSymbols->GetKeySet((ISet**)&set);
    AutoPtr<ArrayOf<IInterface*> > out;
    set->ToArray(ret, (ArrayOf<IInterface*>**)&out);
    for (Int32 i = 0; i < out->GetLength(); ++i) {
        (*symbols)->Set(i, ISymbol::Probe((*out)[i]));
    }
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
        Add(Symbol::MakeArg((*args)[i], (Byte)i));
    }
}

void CSymbols::Add(
    /* [in] */ ISymbol* s)
{
    AutoPtr<IInterface> temp;
    mSymbols->Put(s, s, (IInterface**)&temp);
    AutoPtr<ISymbol> previous = ISymbol::Probe(temp);
    AutoPtr<Symbol> obj = (Symbol*)previous.Get();
    if (previous != NULL && obj->mIsConst) {
        mSymbols->Put(previous, previous);
        return;
    }
    if (mDelta == NULL) {
        CHashSet::New((IHashSet**)&mDelta);
    }
    Boolean contains;
    if (mDelta->Contains(s, &contains), !contains) {
        mDelta->Add(previous != NULL ? previous : Symbol::NewEmpty(s));
    }
}

AutoPtr<ISymbol> CSymbols::Lookup(
    /* [in] */ const String& name,
    /* [in] */ Int32 arity)
{
    AutoLock lock(this);
    SHELL->SetKey(name, arity);
    AutoPtr<IInterface> value;
    mSymbols->Get((ISymbol*)SHELL.Get(), (IInterface**)&value);
    return ISymbol::Probe(value);
}

AutoPtr<ISymbol> CSymbols::LookupConst(
    /* [in] */ const String& name)
{
    return Lookup(name, Symbol::CONST_ARITY);
}

} // namespace Arity
} // namespace Javia
} // namespace Org