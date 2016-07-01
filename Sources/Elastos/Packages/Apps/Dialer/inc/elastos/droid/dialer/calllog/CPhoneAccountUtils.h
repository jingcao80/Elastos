
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CPHONEACCOUNTUTILS_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CPHONEACCOUNTUTILS_H__

#include "_Elastos_Droid_Dialer_CallLog_CPhoneAccountUtils.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Graphics.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CarClass(CPhoneAccountUtils)
    , public Singleton
    , public IPhoneAccountUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Generate account info from data in Telecomm database
     */
    CARAPI GetAccount(
        /* [in] */ String componentString,
        /* [in] */ String accountId,
        /* [out] */ IPhoneAccountHandle** handle);

    /**
     * Generate account icon from data in Telecomm database
     */
    CARAPI GetAccountIcon(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneAccountHandle* phoneAccount,
        /* [out] */ IDrawable** drawable);

    /**
     * Generate account label from data in Telecomm database
     */
    CARAPI GetAccountLabel(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneAccountHandle* phoneAccount,
        /* [out] */ String* lable);
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CPHONEACCOUNTUTILS_H__
