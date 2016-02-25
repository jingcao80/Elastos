#include "elastos/droid/systemui/statusbar/tablet/ShirtPocket.h"
#include "elastos/droid/view/CDragShadowBuilder.h"

using Elastos::Droid::View::IView;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::IDragShadowBuilder;
using Elastos::Droid::View::CDragShadowBuilder;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

const String ShirtPocket::TAG("StatusBar/ShirtPocket");

ShirtPocket::ShirtPocket()
{}

ShirtPocket::ShirtPocket(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : ImageView(context, attrs)
{}

ECode ShirtPocket::OnAttachedToWindow()
{
    return NOERROR;
}

Boolean ShirtPocket::Holding()
{
    return (mClipping != NULL);
}

ECode ShirtPocket::Stash(
    /* [in] */ IClipData* clipping)
{
    mClipping = clipping;
    if (mClipping != NULL) {
        SetVisibility(IView::VISIBLE);
        AutoPtr<IBitmap> icon;
        mClipping->GetIcon((IBitmap**)&icon);
//            mDescription.setText(mClipping.getDescription().getLabel());
        if (icon != NULL) {
            SetImageBitmap(icon);
        }
        else {
            Int32 count;
            mClipping->GetItemCount(&count);
            if (count > 0) {
                // TODO: figure out how to visualize every kind of ClipData!
                //mAltText.setText(mClipping.getItemAt(0).coerceToText(getContext()));
            }
        }
    }
    else {
        SetVisibility(IView::GONE);
    }
    return NOERROR;
}

Boolean ShirtPocket::OnTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    Int32 action;
    ev->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        AutoPtr<IClipData> clip = mClipping;
        if (clip != NULL) {
            AutoPtr<IBitmap> icon;
            clip->GetIcon((IBitmap**)&icon);
            AutoPtr<IDragShadowBuilder> shadow;
            if (icon != NULL) {
                // shadow = new DragShadowBuilder(this) {
                //     public void onProvideShadowMetrics(Point shadowSize, Point shadowTouchPoint) {
                //         shadowSize.set(icon.getWidth(), icon.getHeight());
                //         shadowTouchPoint.set(shadowSize.x / 2, shadowSize.y / 2);
                //     }
                //     public void onDrawShadow(Canvas canvas) {
                //         canvas.drawBitmap(icon, 0, 0, new Paint());
                //     }
                // };
                //TODO
                CDragShadowBuilder::New(THIS_PROBE(IView), (IDragShadowBuilder**)&shadow);
            } else {
                // uhhh, what now?
                CDragShadowBuilder::New(THIS_PROBE(IView), (IDragShadowBuilder**)&shadow);
            }

            StartDrag(clip, shadow, NULL, 0);

            // TODO: only discard the clipping if it was accepted
            Stash(NULL);

            return TRUE;
        }
    }
    return FALSE;
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
