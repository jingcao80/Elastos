
#ifndef __ELASTOS_SECURITY_CERT_CCERTIFICATEFACTORYHELPER_H__
#define __ELASTOS_SECURITY_CERT_CCERTIFICATEFACTORYHELPER_H__

#include "_Elastos_Security_Cert_CCertificateFactoryHelper.h"

#include "core/Singleton.h"

using Elastos::Core::Singleton;
using Elastos::Security::IProvider;

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CCertificateFactoryHelper)
    , public Singleton
    , public ICertificateFactoryHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [out] */ ICertificateFactory** factory);

    CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [in] */ const String& provider,
        /* [out] */ ICertificateFactory** factory);

    CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [in] */ IProvider* provider,
        /* [out] */ ICertificateFactory** factory);
};

} // end Cert
} // end Security
} // end Elastos

#endif // __ELASTOS_SECURITY_CERT_CCERTIFICATEFACTORYHELPER_H__
