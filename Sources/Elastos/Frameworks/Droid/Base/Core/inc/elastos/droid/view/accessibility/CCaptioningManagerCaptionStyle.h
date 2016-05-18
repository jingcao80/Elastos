#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CAPTIONINGMANAGERCAPTIONSTYLE_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CAPTIONINGMANAGERCAPTIONSTYLE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_View_Accessibility_CCaptioningManagerCaptionStyle.h"
#include "elastos/droid/view/accessibility/CaptioningManager.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CCaptioningManagerCaptionStyle)
    , public CaptioningManager::CaptionStyle
{
public:
    CAR_OBJECT_DECL()
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CAPTIONINGMANAGERCAPTIONSTYLE_H__
