#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONEFACTORY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONEFACTORY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// import android.telephony.Rlog;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IPhoneNotifier;
using Elastos::Droid::Internal::Telephony::IPhone;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

/**
  * {@hide}
  */
class ImsPhoneFactory
    : public Object
{
public:
    CAR_INTERFACE_DECL();

    /**
      * Makes a {@link ImsPhone} object.
      * @param context {@code Context} needed to create a Phone object
      * @param phoneNotifier {@code PhoneNotifier} needed to create a Phone
      *      object
      * @return the {@code ImsPhone} object
      */
    static CARAPI_(AutoPtr<IImsPhone>) MakePhone(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneNotifier* phoneNotifier,
        /* [in] */ IPhone* defaultPhone);
};

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONEFACTORY_H__

