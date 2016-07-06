
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/sip/CSipPhone.h"
#include "elastos/droid/internal/telephony/sip/SipPhoneFactory.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Sip {

//=====================================================================
//                           SipPhoneFactory
//=====================================================================

AutoPtr<ISipPhone> SipPhoneFactory::MakePhone(
    /* [in] */ const String& sipUri,
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* phoneNotifier)
{
    // try {
// TODO: Need SipProfile
    // AutoPtr<ISipProfile> profile = new SipProfile.Builder(sipUri).build();
    AutoPtr<ISipPhone> sipPhone;
    CSipPhone::New(context, phoneNotifier, NULL/*TODO: profile*/, (ISipPhone**)&sipPhone);
    // } catch (ParseException e) {
    //     Rlog.w("SipPhoneFactory", "makePhone", e);
    //     return null;
    // }
    return sipPhone;
}

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
