
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_PHONENUMBERUTILSWRAPPER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_PHONENUMBERUTILSWRAPPER_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/core/Object.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/utility/etl/Set.h>

using Elastos::Utility::Etl::Set;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

class PhoneNumberUtilsWrapper : public Object
{
public:
    /** Returns true if it is possible to place a call to the given number. */
    static CARAPI_(Boolean) CanPlaceCallsTo(
        /* [in] */ ICharSequence* number,
        /* [in] */ Int32 presentation);

    /**
     * Returns true if it is possible to send an SMS to the given number.
     */
    CARAPI_(Boolean) CanSendSmsTo(
        /* [in] */ ICharSequence* number,
        /* [in] */ Int32 presentation);

    /**
     * Returns true if the given number is the number of the configured voicemail. To be able to
     * mock-out this, it is not a static method.
     */
    CARAPI_(Boolean) IsVoicemailNumber(
        /* [in] */ ICharSequence* number);

    /**
     * Returns true if the given number is a SIP address. To be able to mock-out this, it is not a
     * static method.
     */
    CARAPI_(Boolean) IsSipNumber(
        /* [in] */ ICharSequence* number);

    static CARAPI_(Boolean) IsUnknownNumberThatCanBeLookedUp(
        /* [in] */ ICharSequence* number,
        /* [in] */ Int32 presentation);


    static CARAPI_(Boolean) IsLegacyUnknownNumbers(
        /* [in] */ ICharSequence* number);

public:
    static const AutoPtr<PhoneNumberUtilsWrapper> INSTANCE;

private:
    static const AutoPtr< Set<String> > LEGACY_UNKNOWN_NUMBERS;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_PHONENUMBERUTILSWRAPPER_H__
