
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_WAPPUSHOVERSMS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_WAPPUSHOVERSMS_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Google.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.CoreLibrary.IO.h"
#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Google::Mms::Pdu::IGenericPdu;
using Elastos::Droid::Google::Mms::Pdu::INotificationInd;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBinder;

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * WAP push handler class.
 *
 * @hide
 */
class WapPushOverSms
    : public Object
    , public IServiceConnection
    , public IWapPushOverSms
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnServiceConnected(
        /* [in] */ IComponentName* name,
        /* [in] */ IBinder* service);

    //@Override
    CARAPI OnServiceDisconnected(
        /* [in] */ IComponentName* name);

    CARAPI Dispose();

    /**
     * Dispatches inbound messages that are in the WAP PDU format. See
     * wap-230-wsp-20010705-a section 8 for details on the WAP PDU format.
     *
     * @param pdu The WAP PDU, made up of one or more SMS PDUs
     * @param address The originating address
     * @return a result code from {@link android.provider.Telephony.Sms.Intents}, or
     *         {@link Activity#RESULT_OK} if the message has been broadcast
     *         to applications
     */
    CARAPI DispatchWapPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ IBroadcastReceiver* receiver,
        /* [in] */ IInboundSmsHandler* handler,
        /* [in] */ const String& address,
        /* [out] */ Int32* result);

private:
    CARAPI_(void) WriteInboxMessage(
        /* [in] */ Int64 subId,
        /* [in] */ ArrayOf<Byte>* pushData);

    static CARAPI_(Int64) GetDeliveryOrReadReportThreadId(
        /* [in] */ IContext* context,
        /* [in] */ IGenericPdu* pdu);

    static CARAPI_(Boolean) IsDuplicateNotification(
        /* [in] */ IContext* context,
        /* [in] */ INotificationInd* nInd);

private:
    static const String TAG;
    static const Boolean DBG;

    AutoPtr<IContext> mContext;

    /** Assigned from ServiceConnection callback on main threaad. */
    /* volatile */ AutoPtr<IIWapPushManager> mWapPushManager;

    static const String THREAD_ID_SELECTION;

    static const String LOCATION_SELECTION;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_WAPPUSHOVERSMS_H__
