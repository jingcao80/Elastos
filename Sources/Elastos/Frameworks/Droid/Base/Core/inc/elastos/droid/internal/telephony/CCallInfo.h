
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLINFO_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLINFO_H__

#include "_Elastos_Droid_Internal_Telephony_CCallInfo.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CCallInfo)
    , public Object
    , public ICallInfo
    , public IParcelable
{
public:
    CCallInfo();

    virtual ~CCallInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& handle);

    CARAPI GetHandle(
        /* [out] */ String* handle);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    String mHandle;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLINFO_H__
