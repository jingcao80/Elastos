
#ifndef __ELASTOS_SECURITY_CKEYSTOREHELPER_H__
#define __ELASTOS_SECURITY_CKEYSTOREHELPER_H__

#include <elastos/core/Singleton.h>
#include "_Elastos_Security_CKeyStoreHelper.h"

using Elastos::Security::IProvider;

namespace Elastos {
namespace Security {

CarClass(CKeyStoreHelper)
    , public Singleton
    , public IKeyStoreHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [out] */ IKeyStore **instance);

    CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [in] */ const String& provider,
        /* [out] */ IKeyStore **instance);

    CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [in] */ IProvider *provider,
        /* [out] */ IKeyStore **instance);

    CARAPI GetDefaultType(
        /* [out] */ String *defaultType);
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_CKEYSTOREHELPER_H__
