
#ifndef __ELASTOS_SECURITY_CSECURERANDOMHELPER_H__
#define __ELASTOS_SECURITY_CSECURERANDOMHELPER_H__

#include "_Elastos_Security_CSecureRandomHelper.h"
#include "Singleton.h"

namespace Elastos {
namespace Security {

CarClass(CSecureRandomHelper)
    , public Singleton
    , public ISecureRandomHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ ISecureRandom** result) /*throws NoSuchAlgorithmException*/;

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ ISecureRandom** result) /*throws NoSuchAlgorithmException, NoSuchProviderException*/;

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ ISecureRandom** result) /*throws NoSuchAlgorithmException*/;

    CARAPI GetSeed(
        /* [in] */ Int32 numBytes,
        /* [out] */ ArrayOf<Byte>** seed);
};

}
}

#endif // __ELASTOS_SECURITY_CSECURERANDOMHELPER_H__
