#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_CDMACONFERENCE_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_CDMACONFERENCE_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/telecomm/telecom/Conference.h"
#include "elastos/droid/services/telephony/CdmaConnection.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Telecomm::Telecom::Connection;
using Elastos::Droid::Telecomm::Telecom::Conference;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle;

namespace Elastos {
namespace Droid {
namespace Services {
namespace Telephony {

/**
 * CDMA-based conference call.
 */
class CdmaConference
    : public Conference
{
public:
    TO_STRING_IMPL("CdmaConference")

    CdmaConference(
        /* [in] */ IPhoneAccountHandle* phoneAccount,
        /* [in] */ Int32 capabilities);

    /**
     * Invoked when the Conference and all it's {@link Connection}s should be disconnected.
     */
    //@Override
    CARAPI OnDisconnect();

    //@Override
    CARAPI OnSeparate(
        /* [in] */ Elastos::Droid::Telecomm::Telecom::IConnection* connection);

    //@Override
    CARAPI OnHold();

    /**
     * Invoked when the conference should be moved from hold to active.
     */
    //@Override
    CARAPI OnUnhold();

    //@Override
    CARAPI OnMerge();

    //@Override
    CARAPI OnPlayDtmfTone(
        /* [in] */ Char32 c);

    //@Override
    CARAPI OnStopDtmfTone();

    //@Override
    CARAPI OnSwap();

private:
    CARAPI_(void) UpdateCapabilities();

    CARAPI_(void) SendFlash();

    CARAPI_(AutoPtr<ICall>) GetMultipartyCallForConnection(
        /* [in] */ Elastos::Droid::Telecomm::Telecom::IConnection* connection);

    CARAPI_(AutoPtr<ICall>) GetOriginalCall();

    CARAPI_(AutoPtr<Elastos::Droid::Internal::Telephony::IConnection>) GetOriginalConnection(
        /* [in] */ Elastos::Droid::Telecomm::Telecom::IConnection* connection);

    CARAPI_(AutoPtr<CdmaConnection>) GetFirstConnection();


private:
    Int32 mCapabilities;
};

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_TELEPHONY_CDMACONFERENCE_H__