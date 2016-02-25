#include "elastos/droid/systemui/statusbar/tablet/DropZone.h"
#include "elastos/droid/animation/CObjectAnimator.h"

using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::CObjectAnimator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

DropZone::MyListener::MyListener(
    /* [in] */ DropZone* host)
{
    mHost = host;
}

ECode DropZone::MyListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->SetTranslationY(mHost->GetHeight() + 2);
    mHost->SetAlpha(0.f);
    return NOERROR;
}


DropZone::DropZone()
{}

DropZone::DropZone(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : View(context, attrs)
{
}

ECode DropZone::SetPocket(
    /* [in] */ IShirtPocket* p)
{
    mPocket = p;
    return NOERROR;
}

ECode DropZone::OnAttachedToWindow()
{
    View::OnAttachedToWindow();
    Boolean holding;
    mPocket->Holding(&holding);
    if (holding) {
        Show(FALSE);
    }
    else {
        Hide(FALSE);
    }
    return NOERROR;
}


// Drag API notes: we must be visible to receive drag events
void DropZone::Show(
    /* [in] */ Boolean animate)
{
    SetTranslationY(0.f);
    if (animate) {
        SetAlpha(0.0f);
        AutoPtr<ArrayOf<Float> > params = ArrayOf<Float>::Alloc(2);
        (*params)[0] = 0.f;
        (*params)[1] = 1.f;

        CObjectAnimator::OfFloat(THIS_PROBE(IInterface), String("alpha"), params)->Start();
    } else {
        SetAlpha(1.0f);
    }
}

void DropZone::Hide(
    /* [in] */ Boolean animate)
{
    SetTranslationY(GetHeight() + 2);
    SetAlpha(0.0f);
    AutoPtr<AnimatorListenerAdapter> onEnd = new MyListener(this);

    if (animate) {
        AutoPtr<ArrayOf<Float> > params = ArrayOf<Float>::Alloc(2);
        (*params)[0] = GetAlpha();
        (*params)[1] = 0.f;
        AutoPtr<IAnimator> a = CObjectAnimator::OfFloat(THIS_PROBE(IInterface), String("alpha"), params);
        a->AddListener(onEnd);
        a->Start();
    } else {
        onEnd->OnAnimationEnd(NULL);
    }
}

Boolean DropZone::OnDragEvent(
    /* [in] */ IDragEvent* event)
{
    // if (DEBUG) Slog.d(TAG, "onDragEvent: " + event);
    Int32 action;
    event->GetAction(&action);
    switch (action) {
        // We want to appear whenever a potential drag takes off from anywhere in the UI.
        case IDragEvent::ACTION_DRAG_STARTED:
            Show(TRUE);
            break;
        case IDragEvent::ACTION_DRAG_ENTERED:
            // if (DEBUG) Slog.d(TAG, "entered!");
            // XXX: TODO
            break;
        case IDragEvent::ACTION_DRAG_EXITED:
            // if (DEBUG) Slog.d(TAG, "exited!");
            break;
        case IDragEvent::ACTION_DROP: {
                // if (DEBUG) Slog.d(TAG, "dropped!");
                AutoPtr<IClipData> clipData;
                event->GetClipData((IClipData**)&clipData);
                mPocket->Stash(clipData);
            }
            break;
        case IDragEvent::ACTION_DRAG_ENDED:
            Hide(TRUE);
            break;
    }
    return TRUE; // we want everything, thank you
}

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
