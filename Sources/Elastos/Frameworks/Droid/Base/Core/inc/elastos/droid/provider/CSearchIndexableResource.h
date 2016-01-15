#ifndef __ELASTOS_DROID_PROVIDER_CSEARCHINDEXABLERESOURCE_H_
#define __ELASTOS_DROID_PROVIDER_CSEARCHINDEXABLERESOURCE_H_

#include "_Elastos_Droid_Provider_CSearchIndexableResource.h"
#include "elastos/droid/provider/SearchIndexableResource.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CSearchIndexableResource)
    , public SearchIndexableResource
{
public:
    CAR_OBJECT_DECL()
};


} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CSEARCHINDEXABLERESOURE_H_