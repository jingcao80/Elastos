
#ifndef __ELASTOS_DROID_CONTENT_PM_CSIGNATUREHELPER_H__
#define __ELASTOS_DROID_CONTENT_PM_CSIGNATUREHELPER_H__

#include "_Elastos_Droid_Content_Pm_CSignatureHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Security::Cert::ICertificateFactory;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CSignatureHelper)
    , public Singleton
    , public ISignatureHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI AreExactMatch(
        /* [in] */ ArrayOf<ISignature*>* a,
        /* [in] */ ArrayOf<ISignature*>* b,
        /* [out] */ Boolean* match);

    CARAPI AreEffectiveMatch(
        /* [in] */ ArrayOf<ISignature*>* a,
        /* [in] */ ArrayOf<ISignature*>* b,
        /* [out] */ Boolean* match);

    CARAPI Bounce(
        /* [in] */ ICertificateFactory* cf,
        /* [in] */ ISignature* s,
        /* [out] */ ISignature** sig);
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CSIGNATUREHELPER_H__
