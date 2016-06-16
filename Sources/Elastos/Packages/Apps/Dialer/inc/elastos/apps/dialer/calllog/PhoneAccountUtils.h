
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_PHONEACCOUNTUTILS_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_PHONEACCOUNTUTILS_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Telecomm.h"

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccount;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

/**
 * Methods to help extract {@code PhoneAccount} information from database and Telecomm sources
 */
class PhoneAccountUtils
    : public Object
{
public:
    /**
     * Generate account info from data in Telecomm database
     */
    static CARAPI_(AutoPtr<IPhoneAccountHandle>) GetAccount(
        /* [in] */ const String& componentString,
        /* [in] */ const String& accountId);

    /**
     * Generate account icon from data in Telecomm database
     */
    static CARAPI_(AutoPtr<IDrawable>) GetAccountIcon(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneAccountHandle* phoneAccount);

    /**
     * Generate account label from data in Telecomm database
     */
    static CARAPI_(String) GetAccountLabel(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneAccountHandle* phoneAccount);

private:
    /**
     * Retrieve the account metadata, but if the account does not exist or the device has only a
     * single registered and enabled account, return null.
     */
    static CARAPI_(AutoPtr<IPhoneAccount>) GetAccountOrNull(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneAccountHandle* phoneAccount);
};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_PHONEACCOUNTUTILS_H__
