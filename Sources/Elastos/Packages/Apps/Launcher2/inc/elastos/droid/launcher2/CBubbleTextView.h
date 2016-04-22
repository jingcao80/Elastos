#ifndef  __ELASTOS_DROID_LAUNCHER2_CBUBBLETEXTVIEW_H__
#define  __ELASTOS_DROID_LAUNCHER2_CBUBBLETEXTVIEW_H__

#include "_Elastos_Droid_Launcher2_CBubbleTextView.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/BubbleTextView.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * TextView that draws a bubble behind the text. We cannot use a LineBackgroundSpan
 * because we want to make the bubble taller than the text and TextView's clip is
 * too aggressive.
 */
 CarClass(CBubbleTextView)
    , public BubbleTextView
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CBUBBLETEXTVIEW_H__