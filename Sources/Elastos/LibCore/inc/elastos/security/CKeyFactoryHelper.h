
#ifndef __ELASTOS_SECURITY_CKEYFACTORYHELPER_H__
#define __ELASTOS_SECURITY_CKEYFACTORYHELPER_H__

#include "_Elastos_Security_CKeyFactoryHelper.h"
#include "Singleton.h"

namespace Elastos {
namespace Security {

CarClass(CKeyFactoryHelper)
    , public Singleton
    , public IKeyFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IKeyFactory** instance);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IKeyFactory** instance);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ IKeyFactory** instance);
};

}
}

#endif // __ELASTOS_SECURITY_CKEYFACTORYHELPER_H__
