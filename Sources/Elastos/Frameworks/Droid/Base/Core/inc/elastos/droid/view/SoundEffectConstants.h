#ifndef __ELASTOS_DROID_VIEW_SOUNDEFFECTCONSTANT_H__
#define __ELASTOS_DROID_VIEW_SOUNDEFFECTCONSTANT_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace View {

class SoundEffectConstants
    : public Object
{
public:
    static const Int32 CLICK = 0;
    static const Int32 NAVIGATION_LEFT = 1;
    static const Int32 NAVIGATION_UP = 2;
    static const Int32 NAVIGATION_RIGHT = 3;
    static const Int32 NAVIGATION_DOWN = 4;

    /**
     * Get the sonification constant for the focus directions.
     * @param direction One of {@link View#FOCUS_UP}, {@link View#FOCUS_DOWN},
     *     {@link View#FOCUS_LEFT}, {@link View#FOCUS_RIGHT}, {@link View#FOCUS_FORWARD}
     *     or {@link View#FOCUS_BACKWARD}

     * @return The appropriate sonification constant.
     */
    static Int32 GetContantForFocusDirection(
        /* [in] */ Int32 direction);

private:
    SoundEffectConstants();
    SoundEffectConstants(const SoundEffectConstants&);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_SOUNDEFFECTCONSTANT_H__

