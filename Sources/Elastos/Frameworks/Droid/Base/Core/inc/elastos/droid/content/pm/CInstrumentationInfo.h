
#ifndef __ELASTOS_DROID_CONTENT_PM_CINSTRUMENTATIONINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CINSTRUMENTATIONINFO_H__

#include "_Elastos_Droid_Content_Pm_CInstrumentationInfo.h"
#include "elastos/droid/content/pm/InstrumentationInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CInstrumentationInfo)
    , public InstrumentationInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CINSTRUMENTATIONINFO_H__
