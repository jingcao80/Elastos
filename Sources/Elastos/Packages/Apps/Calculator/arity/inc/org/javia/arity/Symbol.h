
#ifndef _ORG_JAVIA_ARITY_SYMBOL_H__
#define _ORG_JAVIA_ARITY_SYMBOL_H__

#include "_Org.Javia.Arity.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

namespace Org {
namespace Javia {
namespace Arity {

class Symbol
    : public Object
    , public ISymbol
{
public:
    Symbol(
        /* [in] */ const String& name,
        /* [in] */ IFunction* fun);

    Symbol(
        /* [in] */ const String& name,
        /* [in] */ Double re,
        /* [in] */ Boolean isConst);

    Symbol(
        /* [in] */ const String& name,
        /* [in] */ Double re,
        /* [in] */ Double im,
        /* [in] */ Boolean isConst);

    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<ISymbol>) MakeArg(
        /* [in] */ const String& name,
        /* [in] */ Int32 order);

    static CARAPI_(AutoPtr<ISymbol>) MakeVmOp(
        /* [in] */ const String& name,
        /* [in] */ Int32 op);

    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetArity(
        /* [out] */ Int32* arity);

    static CARAPI_(AutoPtr<ISymbol>) NewEmpty(
        /* [in] */ ISymbol* s);

    CARAPI_(Boolean) IsEmpty();

    CARAPI_(void) SetKey(
        /* [in] */ const String& name,
        /* [in] */ Int32 arity);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

private:
    Symbol(
        /* [in] */ const String& name,
        /* [in] */ Int32 arity,
        /* [in] */ Byte op,
        /* [in] */ Boolean isConst,
        /* [in] */ Int32 dummy);

public:
    static const Int32 CONST_ARITY = -3;
    Byte mOp;
    AutoPtr<IFunction> mFun;
    Double mValueRe, mValueIm;
    Boolean mIsConst;

private:
    String mName;
    Int32 mArity;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_SYMBOL_H__