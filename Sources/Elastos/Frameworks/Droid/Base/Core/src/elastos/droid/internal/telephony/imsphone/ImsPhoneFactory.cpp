
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneFactory.h"
#include "elastos/droid/internal/telephony/imsphone/CImsPhone.h"

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
    // try {
    AutoPtr<IImsPhone> phone;
    CImsPhone::New(context, phoneNotifier, defaultPhone, (IImsPhone**)&phone);
    return phone;
    // } catch (Exception e) {
    //     Rlog.e("VoltePhoneFactory", "makePhone", e);
    //     return null;
    // }
}

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos


