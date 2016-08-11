
#ifndef __ELASTOS_DROID_TELEPHONY_CSMSBROADCASTUNDELIVERED_H__
#define __ELASTOS_DROID_TELEPHONY_CSMSBROADCASTUNDELIVERED_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Internal_Telephony_CSmsBroadcastUndelivered.h"
#include <elastos/core/Object.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Core::IRunnable;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaInboundSmsHandler;
using Elastos::Droid::Internal::Telephony::Gsm::IGsmInboundSmsHandler;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSmsBroadcastUndelivered)
    , public Object
    , public IRunnable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IGsmInboundSmsHandler* gsmInboundSmsHandler,
        /* [in] */ ICdmaInboundSmsHandler* cdmaInboundSmsHandler);

    CARAPI Run();

private:
    class ECO_LOCAL SmsReferenceKey : public Object
    {
    public:
        SmsReferenceKey(IInboundSmsTracker* tracker);

        AutoPtr<ArrayOf<String> > GetDeleteWhereArgs();

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        CARAPI Equals(
            /* [in] */ IInterface* entry,
            /* [out] */ Boolean* result);
    public:
        String mAddress;
        Int32 mReferenceNumber;
        Int32 mMessageCount;

    };
    /**
     * Scan the raw table for complete SMS messages to broadcast, and old PDUs to delete.
     */
    void ScanRawTable();

    /**
     * Send tracker to appropriate (3GPP or 3GPP2) inbound SMS handler for broadcast.
     */
    void BroadcastSms(IInboundSmsTracker* tracker);

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitPDU_PENDING_MESSAGE();

private:
    static const String TAG;
    static const Boolean DBG;

    /**
     * List of ISO codes for countries that can have an offset of
     * GMT+0 when not in daylight savings time.  This ignores some
     * small places such as the Canary Islands (Spain) and
     * Danmarkshavn (Denmark).  The list must be sorted by code.
    */
    static const AutoPtr<ArrayOf<String> > PDU_PENDING_MESSAGE_PROJECTION;

    /** URI for raw table from SmsProvider. */
    static const AutoPtr<IUri> sRawUri;

    /** Content resolver to use to access raw table from SmsProvider. */
    AutoPtr<IContentResolver> mResolver;

    /** Handler for 3GPP-format Messages (may be NULL). */
    AutoPtr<IGsmInboundSmsHandler> mGsmInboundSmsHandler;

    /** Handler for 3GPP2-format Messages (may be NULL). */
    AutoPtr<ICdmaInboundSmsHandler> mCdmaInboundSmsHandler;

    /** Use context get bool config resource from framework. */
    AutoPtr<IContext> mContext;

};

} //namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CSMSBROADCASTUNDELIVERED_H__
