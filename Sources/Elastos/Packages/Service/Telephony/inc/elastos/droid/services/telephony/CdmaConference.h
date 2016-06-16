#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_CDMACONFERENCE_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_CDMACONFERENCE_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"

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
        /* [in] */ IConnection* connection);

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
        /* [in] */ IConnection* connection);

    CARAPI_(AutoPtr<ICall>) GetOriginalCall();

    CARAPI_(AutoPtr<com.android.internal.telephony.Connection>) GetOriginalConnection(
        /* [in] */ IConnection* connection);

    CARAPI_(AutoPtr<ICdmaConnection>) GetFirstConnection();


private:
    Int32 mCapabilities = PhoneCapabilities::MUTE;
};

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_TELEPHONY_CDMACONFERENCE_H__