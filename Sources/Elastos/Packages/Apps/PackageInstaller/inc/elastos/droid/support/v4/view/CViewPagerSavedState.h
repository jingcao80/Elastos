#ifndef __ELASTOS_DROID_SUPPORT_V4_VIEW_CVIEWPAGERSAVEDSTATE_H__
#define __ELASTOS_DROID_SUPPORT_V4_VIEW_CVIEWPAGERSAVEDSTATE_H__

#include "_Elastos_Droid_Support_V4_View_CViewPagerSavedState.h"
#include "elastos/droid/support/v4/view/ViewPager.h"

namespace Elastos {
namespace Droid {
namespace Support {
namespace V4 {
namespace View {

CarClass(CViewPagerSavedState)
    , public ViewPager::SavedState
{
public:
    CAR_OBJECT_DECL()
};

} // namespace View
} // namespace V4
} // namespace Support
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SUPPORT_V4_VIEW_CVIEWPAGERSAVEDSTATE_H__