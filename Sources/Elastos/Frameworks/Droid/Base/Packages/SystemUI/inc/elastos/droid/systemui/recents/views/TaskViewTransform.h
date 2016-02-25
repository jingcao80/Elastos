
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEWTRANSFORM_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEWTRANSFORM_H__

#include "_SystemUI.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::Animation::IInterpolator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/* The transform state for a task view */
class TaskViewTransform : public Object
{
public:
    TaskViewTransform();

    TaskViewTransform(
        /* [in] */ TaskViewTransform* o);

    /** Resets the current transform */
    CARAPI_(void) Reset();

    /** Convenience functions to compare against current property values */
    CARAPI_(Boolean) HasAlphaChangedFrom(
        /* [in] */ Float v);

    CARAPI_(Boolean) HasScaleChangedFrom(
        /* [in] */ Float v);

    CARAPI_(Boolean) HasTranslationYChangedFrom(
        /* [in] */ Float v);

    CARAPI_(Boolean) HasTranslationZChangedFrom(
        /* [in] */ Float v);

    /** Applies this transform to a view. */
    CARAPI_(void) ApplyToTaskView(
        /* [in] */ IView* v,
        /* [in] */ Int32 duration,
        /* [in] */ IInterpolator* interp,
        /* [in] */ Boolean allowLayers,
        /* [in] */ Boolean allowShadows,
        /* [in] */ IAnimatorUpdateListener* updateCallback);

    /** Reset the transform on a view. */
    static CARAPI_(void) Reset(
        /* [in] */ IView* v);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

public:
    Int32 mStartDelay;
    Int32 mTranslationY;
    Float mTranslationZ;
    Float mScale;
    Float mAlpha;
    Boolean mVisible;
    AutoPtr<IRect> mRect;
    Float mP;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEWTRANSFORM_H__
