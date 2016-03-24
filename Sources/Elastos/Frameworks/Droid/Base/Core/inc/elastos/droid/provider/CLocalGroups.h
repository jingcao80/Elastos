#ifndef __ELASTOS_DROID_PROVIDER_CLOCALGROUPS_H__
#define __ELASTOS_DROID_PROVIDER_CLOCALGROUPS_H__

#include "_Elastos_Droid_Provider_CLocalGroups.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CLocalGroups)
    , public Singleton
    , public ILocalGroups
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetAUTHORITY_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CLOCALGROUPS_H__
