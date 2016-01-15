
#ifndef __CFOOBAR_H__
#define __CFOOBAR_H__

#include "_Reflection_CFooBar.h"
#include <elastos/core/Object.h>


namespace Reflection {

CarClass(CFooBar)
    , public Object
    , public IFooBar
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CFooBar();

    CARAPI constructor();

    CARAPI GetValue(
        /* [out] */ String* str);

    CARAPI SetValue(
        /* [in] */ const String& str);

    CARAPI GetValue(
        /* [out] */ Int32* value);

    CARAPI SetValue(
        /* [in] */ Int32 value);

    CARAPI GetValue(
        /* [out] */ Int64* value);

    CARAPI SetValue(
        /* [in] */ Int64 value);

    CARAPI SetValue(
        /* [in] */ Int32 value0,
        /* [in] */ const String& str,
        /* [in] */ Int64 value1);

    CARAPI GetValue(
        /* [out] */ Int32* value0,
        /* [out] */ String* str,
        /* [out] */ Int64* value1);

private:
    Int32 mValue0;
    String mString;
    Int64 mValue1;
};

} // Reflection

#endif //__CFOOBAR_H__
