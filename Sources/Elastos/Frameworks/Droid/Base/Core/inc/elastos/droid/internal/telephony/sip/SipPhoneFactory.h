#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_SIPPHONEFACTORY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_SIPPHONEFACTORY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/internal/telephony/sip/SipPhone.h"

// import android.net.sip.SipProfile;
// import android.telephony.Rlog;
// import java.text.ParseException;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IPhoneNotifier;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Sip {

/**
  * {@hide}
  */
class SipPhoneFactory
    : public Object
    , public ISipPhoneFactory
{
public:
    CAR_INTERFACE_DECL();

    /**
      * Makes a {@link SipPhone} object.
      * @param sipUri the local SIP URI the phone runs on
      * @param context {@code Context} needed to create a Phone object
      * @param phoneNotifier {@code PhoneNotifier} needed to create a Phone
      *      object
      * @return the {@code SipPhone} object or null if the SIP URI is not valid
      */
    static CARAPI_(AutoPtr<SipPhone>) MakePhone(
        /* [in] */ const String& sipUri,
        /* [in] */ IContext* context,
        /* [in] */ IPhoneNotifier* phoneNotifier);
};

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_SIPPHONEFACTORY_H__
