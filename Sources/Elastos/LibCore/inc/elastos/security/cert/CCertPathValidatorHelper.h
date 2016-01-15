
#ifndef __ELASTOS_SECURITY_CERT_CCERTPATHVALIDATORHELPER_H__
#define __ELASTOS_SECURITY_CERT_CCERTPATHVALIDATORHELPER_H__

#include "_Elastos_Security_Cert_CCertPathValidatorHelper.h"

using Elastos::Security::IProvider;

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CCertPathValidatorHelper)
    , public Singleton
    , public ICertPathValidatorHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ ICertPathValidator **validator);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ ICertPathValidator **validator);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider *provider,
        /* [out] */ ICertPathValidator **validator);

    CARAPI GetDefaultType(
        /* [out] */ String *type);
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CCERTPATHVALIDATORHELPER_H__
