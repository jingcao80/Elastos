
#ifndef __ELASTOS_SECURITY_CALGORITHMPARAMETERSHELPER_H__
#define __ELASTOS_SECURITY_CALGORITHMPARAMETERSHELPER_H__

#include <elastos/core/Singleton.h>
#include "_Elastos_Security_CAlgorithmParametersHelper.h"

using Elastos::Security::IProvider;

namespace Elastos {
namespace Security {

CarClass(CAlgorithmParametersHelper)
    , public Singleton
    , public IAlgorithmParametersHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IAlgorithmParameters** result);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IAlgorithmParameters** result);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ IAlgorithmParameters** result);
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_CALGORITHMPARAMETERSHELPER_H__
