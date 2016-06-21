#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMASMSMESSAGEHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMASMSMESSAGEHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_CCdmaSmsMessageHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Internal::Telephony::Cdma::Sms::IUserData;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CarClass(CCdmaSmsMessageHelper)
    , public Singleton
    , public ISmsMessageHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI CreateFromPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [out] */ ISmsMessage** result);

    CARAPI NewFromParcel(
        /* [in] */ IParcel* p,
        /* [out] */ ISmsMessage** result);

    CARAPI CreateFromEfRecord(
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ ISmsMessage** result);

    CARAPI GetTPLayerLengthForPDU(
        /* [in] */ const String& pdu,
        /* [out] */ Int32* result);

    CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddr,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ ISmsHeader* smsHeader,
        /* [out] */ ISmsMessageSubmitPdu** result);

    CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddr,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ ISmsHeader* smsHeader,
        /* [in] */ Int32 priority,
        /* [out] */ ISmsMessageSubmitPdu** result);

    CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddr,
        /* [in] */ const String& destAddr,
        /* [in] */ Int32 destPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Boolean statusReportRequested,
        /* [out] */ ISmsMessageSubmitPdu** result);

    CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddr,
        /* [in] */ const String& destAddr,
        /* [in] */ Int32 destPort,
        /* [in] */ Int32 origPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Boolean statusReportRequested,
        /* [out] */ ISmsMessageSubmitPdu** result);

    CARAPI GetSubmitPdu(
        /* [in] */ const String& destAddr,
        /* [in] */ IUserData* userData,
        /* [in] */ Boolean statusReportRequested,
        /* [out] */ ISmsMessageSubmitPdu** result);

    CARAPI GetSubmitPdu(
        /* [in] */ const String& destAddr,
        /* [in] */ IUserData* userData,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ Int32 priority,
        /* [out] */ ISmsMessageSubmitPdu** result);

    CARAPI CalculateLength(
        /* [in] */ ICharSequence* messageBody,
        /* [in] */ Boolean use7bitOnly,
        /* [out] */ IGsmAlphabetTextEncodingDetails** result);

};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMASMSMESSAGEHELPER_H__
