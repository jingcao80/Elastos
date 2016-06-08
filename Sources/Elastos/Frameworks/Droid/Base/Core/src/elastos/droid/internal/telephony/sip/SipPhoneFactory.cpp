
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/sip/SipPhoneFactory.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Sip {

//=====================================================================
//                           SipPhoneFactory
//=====================================================================
CAR_INTERFACE_IMPL(SipPhoneFactory, Object, ISipPhoneFactory);

AutoPtr<SipPhone> SipPhoneFactory::MakePhone(
    /* [in] */ const String& sipUri,
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* phoneNotifier)
{
    // ==================before translated======================
    // try {
    //     SipProfile profile = new SipProfile.Builder(sipUri).build();
    //     return new SipPhone(context, phoneNotifier, profile);
    // } catch (ParseException e) {
    //     Rlog.w("SipPhoneFactory", "makePhone", e);
    //     return null;
    // }
    assert(0);
    AutoPtr<SipPhone> empty;
    return empty;
}

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
