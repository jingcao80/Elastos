
#ifndef __ELASTOS_SECURITY_CKEYPAIRGENERATORHELPER_H__
#define __ELASTOS_SECURITY_CKEYPAIRGENERATORHELPER_H__

#include "_Elastos_Security_CKeyPairGeneratorHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Security {

CarClass(CKeyPairGeneratorHelper)
    , public Singleton
    , public IKeyPairGeneratorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IKeyPairGenerator** result);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IKeyPairGenerator** result);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ IKeyPairGenerator** result);
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_CKEYPAIRGENERATORHELPER_H__
