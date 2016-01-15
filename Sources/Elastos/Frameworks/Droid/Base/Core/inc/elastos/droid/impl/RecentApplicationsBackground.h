
#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_RECENTAPPLICATIONSBACKGROUND_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_RECENTAPPLICATIONSBACKGROUND_H__

#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

/**
 * A vertical linear layout.  However, instead of drawing the background
 * behnd the items, it draws the background outside the items based on the
 * padding.  If there isn't enough room to draw both, it clips the background
 * instead of the contents.
 */
class RecentApplicationsBackground : public LinearLayout
{
public:
    RecentApplicationsBackground();

    // @Override
    CARAPI JumpDrawablesToCurrentState();

    // @Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

protected:
    // @Override
    CARAPI_(Boolean) SetFrame(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    // @Override
    CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* who);

    // @Override
    CARAPI DrawableStateChanged();

    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL);

    CARAPI_(void) GetChildBounds(
        /* [in] */ IRect* r);

private:
    static const String TAG;

private:
    Boolean mBackgroundSizeChanged;
    AutoPtr<IDrawable> mBackground;
    AutoPtr<IRect> mTmp0;
    AutoPtr<IRect> mTmp1;
};

} // namespace Elastos
} // namespace Droid
} // namespace Internal
} // namespace Policy
} // namespace Impl

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_RECENTAPPLICATIONSBACKGROUND_H__
