
#ifndef __ELASTOS_DROID_MEDIA_CROUTECATEGORY_H__
#define __ELASTOS_DROID_MEDIA_CROUTECATEGORY_H__

#include "_Elastos_Droid_Media_CRouteCategory.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/CMediaRouter.h"

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Definition of a category of routes. All routes belong to a category.
 */
CarClass(CRouteCategory)
    , public CMediaRouter::RouteCategory
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CROUTECATEGORY_H__
