
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_PHONENUMBERUTILSWRAPPER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_PHONENUMBERUTILSWRAPPER_H__

#include "_Elastos.Apps.Dialer.h"
#include "elastos/core/Object.h"

using Elastos::Utility::ISet;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

class PhoneNumberUtilsWrapper
    : public Object
    , public IPhoneNumberUtilsWrapper
{
public:
    CAR_INTERFACE_DECL()

    /** Returns true if it is possible to place a call to the given number. */
    static CARAPI_(Boolean) CanPlaceCallsTo(
        /* [in] */ ICharSequence* number,
        /* [in] */ Int32 presentation);

    /**
     * Returns true if it is possible to send an SMS to the given number.
     */
    CARAPI CanSendSmsTo(
        /* [in] */ ICharSequence* number,
        /* [in] */ Int32 presentation,
        /* [out] */ Boolean* result);

    /**
     * Returns true if the given number is the number of the configured voicemail. To be able to
     * mock-out this, it is not a static method.
     */
    CARAPI IsVoicemailNumber(
        /* [in] */ ICharSequence* number,
        /* [out] */ Boolean* result);

    /**
     * Returns true if the given number is a SIP address. To be able to mock-out this, it is not a
     * static method.
     */
    CARAPI IsSipNumber(
        /* [in] */ ICharSequence* number,
        /* [out] */ Boolean* result);

    static CARAPI_(Boolean) IsUnknownNumberThatCanBeLookedUp(
        /* [in] */ ICharSequence* number,
        /* [in] */ Int32 presentation);


    static CARAPI_(Boolean) IsLegacyUnknownNumbers(
        /* [in] */ ICharSequence* number);

public:
    static AutoPtr<IPhoneNumberUtilsWrapper> INSTANCE; // = new PhoneNumberUtilsWrapper();

private:
    static AutoPtr<ISet> LEGACY_UNKNOWN_NUMBERS; // = Sets.newHashSet("-1", "-2", "-3");
};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_PHONENUMBERUTILSWRAPPER_H__
