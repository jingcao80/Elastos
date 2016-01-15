#ifndef __ELASTOS_DROID_PROVIDER_CUSERDICTIONARY_H__
#define __ELASTOS_DROID_PROVIDER_CUSERDICTIONARY_H__

#include "_Elastos_Droid_Provider_CUserDictionary.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CUserDictionary)
    , public Singleton
    , public IUserDictionary
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CUSERDICTIONARY_H__