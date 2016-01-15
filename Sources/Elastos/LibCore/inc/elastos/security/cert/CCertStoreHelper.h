
#ifndef __ELASTOS_SECURITY_CERT_CCERTSTOREHELPER_H__
#define __ELASTOS_SECURITY_CERT_CCERTSTOREHELPER_H__

#include "_Elastos_Security_Cert_CCertStoreHelper.h"

using Elastos::Security::IProvider;

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CCertStoreHelper)
    , public Singleton
    , public ICertStoreHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [in] */ ICertStoreParameters *params,
        /* [out] */ ICertStore **certStore);

    CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [in] */ ICertStoreParameters *params,
        /* [in] */ const String& provider,
        /* [out] */ ICertStore **certStore);

    CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [in] */ ICertStoreParameters *params,
        /* [in] */ IProvider *provider,
        /* [out] */ ICertStore **certStore);

    CARAPI GetDefaultType(
        /* [out] */ String *type);

private:
    // TODO: Add your private member variables here.
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CCERTSTOREHELPER_H__
