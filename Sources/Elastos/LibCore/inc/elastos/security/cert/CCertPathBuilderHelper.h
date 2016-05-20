
#ifndef __ELASTOS_SECURITY_CERT_CCERTPATHBUILDERHELPER_H__
#define __ELASTOS_SECURITY_CERT_CCERTPATHBUILDERHELPER_H__

#include "_Elastos_Security_Cert_CCertPathBuilderHelper.h"

using Elastos::Security::IProvider;

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CCertPathBuilderHelper)
    , public Singleton
    , public ICertPathBuilderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ ICertPathBuilder **builder);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ ICertPathBuilder **builder);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider *provider,
        /* [out] */ ICertPathBuilder **builder);

    CARAPI GetDefaultType(
        /* [out] */ String *type);
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CCERTPATHBUILDERHELPER_H__
