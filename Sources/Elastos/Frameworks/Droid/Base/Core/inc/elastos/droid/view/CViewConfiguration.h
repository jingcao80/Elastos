
#ifndef __ELASTOS_DROID_VIEW_CVIEWCONFIGURATION_H__
#define __ELASTOS_DROID_VIEW_CVIEWCONFIGURATION_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_View_CViewConfiguration.h"
#include "elastos/droid/view/ViewConfiguration.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CViewConfiguration)
    , public ViewConfiguration
{
public:
    CAR_OBJECT_DECL();
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CVIEWCONFIGURATION_H__
