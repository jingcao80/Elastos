
#ifndef __ELASTOS_SECURITY_CKEYSTOREBUILDERHELPER_H__
#define __ELASTOS_SECURITY_CKEYSTOREBUILDERHELPER_H__

#include "_Elastos_Security_CKeyStoreBuilderHelper.h"

using Elastos::IO::IFile;

namespace Elastos {
namespace Security {

CarClass(CKeyStoreBuilderHelper)
    , public Singleton
    , public IKeyStoreBuilderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewInstance(
        /* [in] */ IKeyStore* keyStore,
        /* [in] */ IKeyStoreProtectionParameter* protectionParameter,
        /* [out] */ IKeyStoreBuilder** instance);

    CARAPI NewInstance(
        /* [in] */ const String& type,
        /* [in] */ IProvider* provider,
        /* [in] */ IFile* file,
        /* [in] */ IKeyStoreProtectionParameter* protectionParameter,
        /* [out] */ IKeyStoreBuilder** instance);

    CARAPI NewInstance(
        /* [in] */ const String& type,
        /* [in] */ IProvider* provider,
        /* [in] */ IKeyStoreProtectionParameter* protectionParameter,
        /* [out] */ IKeyStoreBuilder** instance);
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_CKEYSTOREBUILDERHELPER_H__
