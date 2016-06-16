#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_GSMCONFERENCE_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_GSMCONFERENCE_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Services {
namespace Telephony {

/**
 * GSM-based conference call.
 */
class GsmConference
    : public Conference
{
public:
    TO_STRING_IMPL("GsmConference")

    GsmConference(
        /* [in] */ IPhoneAccountHandle* phoneAccount);

    /**
     * Invoked when the Conference and all it's {@link Connection}s should be disconnected.
     */
    //@Override
    CARAPI OnDisconnect();

    /**
     * Invoked when the specified {@link Connection} should be separated from the conference call.
     *
     * @param connection The connection to separate.
     */
    //@Override
    CARAPI OnSeparate(
        /* [in] */ IConnection* connection);

    //@Override
    CARAPI OnMerge(
        /* [in] */ IConnection* connection);

    /**
     * Invoked when the conference should be put on hold.
     */
    //@Override
    CARAPI OnHold();

    /**
     * Invoked when the conference should be moved from hold to active.
     */
    //@Override
    CARAPI OnUnhold();

    //@Override
    CARAPI OnPlayDtmfTone(
        /* [in] */ Char32 c);

    //@Override
    CARAPI OnStopDtmfTone();

private:
    CARAPI_(AutoPtr<ICall>) GetMultipartyCallForConnection(
        /* [in] */ IConnection* connection,
        /* [in] */ const String& tag);

    CARAPI_(AutoPtr<com.android.internal.telephony.Connection>) GetOriginalConnection(
        /* [in] */ IConnection* connection,
        /* [in] */ const String& tag);

    CARAPI_(AutoPtr<IGsmConnection>) GetFirstConnection();

};

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_GSMCONFERENCE_H__