#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_CIMSPHONEFACTORY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_CIMSPHONEFACTORY_H__

#include "_Elastos_Droid_Internal_Telephony_ImsPhone_CImsPhoneFactory.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

CarClass(CImsPhoneFactory)
    , public Singleton
    , public IImsPhoneFactory
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI MakePhone(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneNotifier* phoneNotifier,
        /* [in] */ IPhone* defaultPhone,
        /* [out] */ IImsPhone** result);

};

} // namespace ImsPhone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_CIMSPHONEFACTORY_H__
