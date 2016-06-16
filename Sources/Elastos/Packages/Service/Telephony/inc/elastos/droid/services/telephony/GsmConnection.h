#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_GSMCONNECTION_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_GSMCONNECTION_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Services {
namespace Telephony {

/**
 * Manages a single phone call handled by GSM.
 */
class GsmConnection
    : public TelephonyConnection
    , public IGsmConnection
{
public:
    TO_STRING_IMPL("GsmConnection")

    CAR_INTERFACE_DECL()

    GsmConnection(
        /* [in] */ IConnection* connection);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnPlayDtmfTone(
        /* [in] */ Char32 digit);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnStopDtmfTone();

    //@Override
    CARAPI PerformConference(
        /* [in] */ ITelephonyConnection* otherConnection);

    //@Override
    CARAPI OnRemovedFromCallService();

protected:
    //@Override
    CARAPI BuildCallCapabilities(
        /* [out] */ Int32* result);
};

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_GSMCONNECTION_H__