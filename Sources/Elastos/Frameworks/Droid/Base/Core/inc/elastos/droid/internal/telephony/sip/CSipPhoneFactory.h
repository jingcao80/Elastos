#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_CSIPPHONEFACTORY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_CSIPPHONEFACTORY_H__

#include "_Elastos_Droid_Internal_Telephony_Sip_CSipPhoneFactory.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Sip {

CarClass(CSipPhoneFactory)
    , public Singleton
    , public ISipPhoneFactory
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI MakePhone(
        /* [in] */ const String& sipUri,
        /* [in] */ IContext* context,
        /* [in] */ IPhoneNotifier* phoneNotifier,
        /* [out] */ ISipPhone** result);

};

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_CSIPPHONEFACTORY_H__
