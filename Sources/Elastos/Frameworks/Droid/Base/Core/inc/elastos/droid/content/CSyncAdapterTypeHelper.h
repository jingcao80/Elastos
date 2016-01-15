
#ifndef __ELASTOS_DROID_CONTENT_CSYNCADAPTERTYPEHELPER_H__
#define __ELASTOS_DROID_CONTENT_CSYNCADAPTERTYPEHELPER_H__

#include "_Elastos_Droid_Content_CSyncAdapterTypeHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CSyncAdapterTypeHelper)
    , public Singleton
    , public ISyncAdapterTypeHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewKey(
        /* [in] */ const String& authority,
        /* [in] */ const String& accountType,
        /* [out] */ ISyncAdapterType** syncAdapterType);

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CSYNCADAPTERTYPEHELPER_H__
