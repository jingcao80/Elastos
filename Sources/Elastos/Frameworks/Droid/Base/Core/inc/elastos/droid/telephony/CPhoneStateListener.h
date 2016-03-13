
#ifndef __ELASTOS_DROID_TELEPHONY_CPHONESTATELISTENER_H__
#define __ELASTOS_DROID_TELEPHONY_CPHONESTATELISTENER_H__

#include "_Elastos_Droid_Telephony_CPhoneStateListener.h"
#include "elastos/droid/telephony/PhoneStateListener.h"

using Elastos::Droid::Internal::Telephony::IIPhoneStateListener;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CPhoneStateListener)
    , public PhoneStateListener
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CPHONESTATELISTENER_H__
