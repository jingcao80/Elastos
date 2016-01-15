
#ifndef __ELASTOS_DROID_TELEPHONY_CTELEPHONYMANAGERHELPER_H__
#define __ELASTOS_DROID_TELEPHONY_CTELEPHONYMANAGERHELPER_H__

#include "_Elastos_Droid_Telephony_CTelephonyManagerHelper.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Telephony::ITelephonyManager;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CTelephonyManagerHelper)
{
public:

    CARAPI GetDefault(
        /* [out] */ ITelephonyManager** res);

    CARAPI From(
        /* [in] */ IContext* context,
        /* [out] */ ITelephonyManager** res);

    CARAPI GetPhoneType(
        /* [in] */ Int32 networkMode,
        /* [out] */ Int32* val);

    CARAPI GetLteOnCdmaModeStatic(
        /* [out] */ Int32* val);

    CARAPI GetNetworkClass(
        /* [in] */ Int32 networkType,
        /* [out] */ Int32* val);

    CARAPI GetNetworkTypeName(
        /* [in] */ Int32 type,
        /* [out] */ String* res);
};

}
}
}

#endif // __ELASTOS_DROID_TELEPHONY_CTELEPHONYMANAGERHELPER_H__
