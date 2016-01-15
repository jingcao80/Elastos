
#ifndef __ELASTOS_DROID_WIDGET_CSEEKBAR_H__
#define __ELASTOS_DROID_WIDGET_CSEEKBAR_H__

#include "_Elastos_Droid_Widget_CSeekBar.h"
#include "elastos/droid/widget/SeekBar.h"

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * A SeekBar is an extension of ProgressBar that adds a draggable thumb. The user can touch
 * the thumb and drag left or right to set the current progress level or use the arrow keys.
 * Placing focusable widgets to the left or right of a SeekBar is discouraged.
 * <p>
 * Clients of the SeekBar can attach a {@link SeekBar.OnSeekBarChangeListener} to
 * be notified of the user's actions.
 *
 * @attr ref android.R.styleable#SeekBar_thumb
 */
CarClass(CSeekBar)
    , public SeekBar
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CSEEKBAR_H__
