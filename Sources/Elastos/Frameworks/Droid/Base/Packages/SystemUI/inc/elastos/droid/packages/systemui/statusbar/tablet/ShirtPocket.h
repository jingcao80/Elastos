#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_SHIRTPOCKET_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_SHIRTPOCKET_H__

#include "elastos/droid/widget/ImageView.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::ImageView;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

class ShirtPocket : public ImageView
{
public:
    ShirtPocket();

    ShirtPocket(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    Boolean Holding();

    virtual CARAPI_(Boolean) OnTouchEvent(
        /* [in] */ IMotionEvent* event);

    CARAPI Stash(
        /* [in] */ IClipData* clipping);

protected:

    virtual CARAPI OnAttachedToWindow();

protected:

    // TODO: "pin area" panel, dragging things out
    AutoPtr<IObjectAnimator> mAnimHide;
    AutoPtr<IObjectAnimator> mAnimShow;

private:
    static const String TAG;

    AutoPtr<IClipData> mClipping;

    AutoPtr<IImageView> mPreviewIcon;

/*
private boolean isInViewContentArea(View v, int x, int y) {
    final int l = v.getPaddingLeft();
    final int r = v.getWidth() - v.getPaddingRight();
    final int t = v.getPaddingTop();
    final int b = v.getHeight() - v.getPaddingBottom();
    return x >= l && x < r && y >= t && y < b;
}

View.OnTouchListener mWindowTouchListener = new View.OnTouchListener() {
    public boolean onTouch(View v, MotionEvent ev) {
        final int action = ev.getAction();
        if (action == MotionEvent.ACTION_OUTSIDE
                || (action == MotionEvent.ACTION_DOWN
                    && !isInViewContentArea(mWindow, (int)ev.getX(), (int)ev.getY()))) {
            hideWindow();
            return true;
        } else if (action == MotionEvent.ACTION_DOWN) {
            final ClipData clip = mClipping;
            if (clip != null) {
                final Bitmap icon = clip.getIcon();
                DragShadowBuilder shadow;
                if (icon != null) {
                    shadow = new DragShadowBuilder(v) {
                        public void onProvideShadowMetrics(Point shadowSize, Point shadowTouchPoint) {
                            shadowSize.set(icon.getWidth(), icon.getHeight());
                            shadowTouchPoint.set(shadowSize.x / 2, shadowSize.y / 2);
                        }
                        public void onDrawShadow(Canvas canvas) {
                            canvas.drawBitmap(icon, 0, 0, new Paint());
                        }
                    };
                } else {
                    // uhhh, what now?
                    shadow = new DragShadowBuilder(mWindow.findViewById(R.id.preview));
                }

                v.startDrag(clip, shadow, null, 0);

                // TODO: only discard the clipping if it was accepted
                stash(null);

                return true;
            }
        }
        return false;
    }
};
*/
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_SHIRTPOCKET_H__
