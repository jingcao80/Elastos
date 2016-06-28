
#include "org/javia/arity/Symbol.h"
#include "org/javia/arity/VM.h"
#include "elastos/core/StringBuilder.h"

using Org::Javia::Arity::EIID_ISymbol;
using Elastos::Core::StringBuilder;

namespace Org {
namespace Javia {
namespace Arity {

const Int32 Symbol::CONST_ARITY;

Symbol::Symbol(
    /* [in] */ const String& name,
    /* [in] */ Int32 arity,
    /* [in] */ Byte op,
    /* [in] */ Boolean isConst,
    /* [in] */ Int32 dummy)
    : mOp(op)
    , mValueRe(0)
    , mValueIm(0)
    , mIsConst(isConst)
    , mArity(0)
{
    SetKey(name, arity);
}

Symbol::Symbol(
    /* [in] */ const String& name,
    /* [in] */ IFunction* fun)
    : mOp(0)
    , mFun(fun)
    , mValueRe(0)
    , mValueIm(0)
    , mIsConst(FALSE)
    , mArity(0)
{
    Int32 arity;
    fun->Arity(&arity);
    SetKey(name, arity);
}

Symbol::Symbol(
    /* [in] */ const String& name,
    /* [in] */ Double re,
    /* [in] */ Boolean isConst)
    : mOp(0)
    , mValueRe(re)
    , mValueIm(0)
    , mIsConst(isConst)
    , mArity(0)
{
    SetKey(name, CONST_ARITY);
}

Symbol::Symbol(
    /* [in] */ const String& name,
    /* [in] */ Double re,
    /* [in] */ Double im,
    /* [in] */ Boolean isConst)
    : mOp(0)
    , mValueRe(re)
    , mValueIm(im)
    , mIsConst(isConst)
    , mArity(0)
{
    SetKey(name, CONST_ARITY);
}

CAR_INTERFACE_IMPL(Symbol, Object, ISymbol)

AutoPtr<ISymbol> Symbol::MakeArg(
    /* [in] */ const String& name,
    /* [in] */ Int32 order)
{
    return (ISymbol*)new Symbol(name, CONST_ARITY, (Byte)(VM::LOAD0 + order), FALSE, 0);
}

AutoPtr<ISymbol> Symbol::MakeVmOp(
    /* [in] */ const String& name,
    /* [in] */ Int32 op)
{
    return (ISymbol*)new Symbol(name, (Int32)(*VM::ARITY)[op], (Byte)op, TRUE, 0);
}

ECode Symbol::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("Symbol '");
    sb += mName;
    sb += "' arity ";
    sb += mArity;
    sb += " val ";
    sb += mValueRe;
    sb += " op ";
    sb += mOp;
    *str = sb.ToString();
    return NOERROR;
}

ECode Symbol::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode Symbol::GetArity(
    /* [out] */ Int32* arity)
{
    VALIDATE_NOT_NULL(arity)
    *arity = mArity == CONST_ARITY ? 0 : mArity;
    return NOERROR;
}

AutoPtr<ISymbol> Symbol::NewEmpty(
    /* [in] */ ISymbol* _s)
{
    AutoPtr<Symbol> s = (Symbol*)_s;
    return (ISymbol*)new Symbol(s->mName, s->mArity, (Byte)0, FALSE, 0);
}

Boolean Symbol::IsEmpty()
{
    return mOp == 0 && mFun == NULL && mValueRe == 0 && mValueIm == 0;
}

void Symbol::SetKey(
    /* [in] */ const String& name,
    /* [in] */ Int32 arity)
{
    mName = name;
    mArity = arity;
}

ECode Symbol::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<Symbol> symbol = (Symbol*)(ISymbol*)other;
    *result = mName.Equals(symbol->mName) && mArity == symbol->mArity;
    return NOERROR;
}

ECode Symbol::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = mName.GetHashCode() + mArity;
    return NOERROR;
}

} // namespace Arity
} // namespace Javia
} // namespace Org