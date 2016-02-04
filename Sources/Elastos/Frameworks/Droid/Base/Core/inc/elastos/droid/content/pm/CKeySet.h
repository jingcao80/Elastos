
#ifndef __ELASTOS_DROID_CONTENT_PM_CKEYSET_H__
#define __ELASTOS_DROID_CONTENT_PM_CKEYSET_H__

#include "_Elastos_Droid_Content_Pm_CKeySet.h"
#include "elastos/droid/content/pm/KeySet.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CKeySet)
    , public KeySet
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CKEYSET_H__
