
#ifndef __ELASTOS_SECURITY_CSIGNATUREHELPER_H__
#define __ELASTOS_SECURITY_CSIGNATUREHELPER_H__

#include <elastos/core/Singleton.h>
#include "_Elastos_Security_CSignatureHelper.h"

using Elastos::Security::IProvider;

namespace Elastos {
namespace Security {

CarClass(CSignatureHelper)
    , public Singleton
    , public ISignatureHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ ISignature** instance);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ ISignature** instance);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ ISignature** instance);
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_CSIGNATUREHELPER_H__
