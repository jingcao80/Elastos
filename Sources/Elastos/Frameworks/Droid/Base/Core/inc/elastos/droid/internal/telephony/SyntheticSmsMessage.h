
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SYNTHETICSMSMESSAGE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SYNTHETICSMSMESSAGE_H__

#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/SmsMessageBase.h"
#include "elastos/droid/internal/telephony/SmsAddress.h"

using Elastos::Droid::Internal::Telephony::SmsMessageBase;
using Elastos::Droid::Internal::Telephony::SmsAddress;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class SyntheticSmsMessage
    : public SmsMessageBase
    , public ISyntheticSmsMessage
{
public:
    class SyntheticAddress
        : public SmsAddress
    {
    };

public:
    CAR_INTERFACE_DECL()

    static CARAPI_(Boolean) IsSyntheticPdu(
        /* [in] */ ArrayOf<Byte>* pdu);

    static CARAPI_(AutoPtr<ISyntheticSmsMessage>) CreateFromPdu(
        /* [in] */ ArrayOf<Byte>* pdu);

    CARAPI constructor(
        /* [in] */ String originatingAddress,
        /* [in] */ String scAddress,
        /* [in] */ String messageBody,
        /* [in] */ Int64 timestampMillis);

    //@Override
    CARAPI GetMessageClass(
        /* [out] */ MessageClass* result);

    //@Override
    CARAPI GetProtocolIdentifier(
        /* [out] */ Int32* result);

    //@Override
    CARAPI IsReplace(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsCphsMwiMessage(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsMWIClearMessage(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsMWISetMessage(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsMwiDontStore(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetStatus(
        /* [out] */ Int32* result);

    //@Override
    CARAPI IsStatusReportMessage(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsReplyPathPresent(
        /* [out] */ Boolean* result);
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SYNTHETICSMSMESSAGE_H__
