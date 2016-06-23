
#ifndef _ORG_JAVIA_ARITY_TOKEN_H__
#define _ORG_JAVIA_ARITY_TOKEN_H__

#include "_Org.Javia.Arity.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

namespace Org {
namespace Javia {
namespace Arity {

class Token : public Object
{
public:
    Token(
        /* [in] */ Int32 id,
        /* [in] */ Int32 priority,
        /* [in] */ Int32 assoc,
        /* [in] */ Int32 vmop);

    CARAPI_(void) SetPos(
        /* [in] */ Int32 pos);

    CARAPI_(void) SetValue(
        /* [in] */ Double value);

    CARAPI_(void) SetAlpha(
        /* [in] */ const String& alpha);

    CARAPI_(Boolean) IsDerivative();

    CARAPI ToString(
        /* [out] */ String* str);

public:
    //kind
    static const Int32 PREFIX = 1;
    static const Int32 LEFT   = 2;
    static const Int32 RIGHT  = 3;
    static const Int32 SUFIX  = 4;

    Int32 mPriority;
    Int32 mAssoc;
    Int32 mId;
    Byte mVmop;

    Double mValue;    //for NUMBER only
    String mName; //for CONST & CALL
    Int32 mArity;
    Int32 mPosition;        //pos inside expression
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_TOKEN_H__