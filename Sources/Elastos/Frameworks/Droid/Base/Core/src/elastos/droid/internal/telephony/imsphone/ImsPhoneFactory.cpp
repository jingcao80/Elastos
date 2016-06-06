
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneFactory.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

//=====================================================================
//                           ImsPhoneFactory
//=====================================================================
AutoPtr<IImsPhone> ImsPhoneFactory::MakePhone(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* phoneNotifier,
    /* [in] */ IPhone* defaultPhone)
{
    // ==================before translated======================
    //
    // try {
    //     return new ImsPhone(context, phoneNotifier, defaultPhone);
    // } catch (Exception e) {
    //     Rlog.e("VoltePhoneFactory", "makePhone", e);
    //     return null;
    // }
    assert(0);
    AutoPtr<IImsPhone> empty;
    return empty;
}

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos


