
#include "accessibility/CViewportWindow.h"
#include "accessibility/ScreenMagnifier.h"

using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::IGravity;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

//=====================================================================
// CViewportWindow::ContentView
//=====================================================================
// CViewportWindow::_ContentView::_ContentView(
//     /* [in] */ IContext* context)
//     : View(context)
// {
//     AutoPtr<IResources> r;
//     ASSERT_SUCCEEDED(context->GetREsources((IResources**)&r));
//     r->GetDrawable(R::drawable::magnified_region_frame, (IDrawable**)&mHighlightFrame);
// }

// void CViewportWindow::_ContentView::OnDraw(
//     /* [in] */ ICanvas* canvas)
// {
//     canvas->DrawColor(IColor::TRANSPARENT, PorterDuffMode_CLEAR);
//     mHighlightFrame->SetBounds(mBounds);
//     mHighlightFrame->SetAlpha(mAlpha);
//     mHighlightFrame->Draw(canvas);
// }

// IVIEW_METHODS_IMPL(CViewportWindow::ContentView, CViewportWindow::_ContentView)
// IDRAWABLECALLBACK_METHODS_IMPL(CViewportWindow::ContentView, CViewportWindow::_ContentView)
// IKEYEVENTCALLBACK_METHODS_IMPL(CViewportWindow::ContentView, CViewportWindow::_ContentView)
// IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CViewportWindow::ContentView, CViewportWindow::_ContentView)

// PInterface CViewportWindow::ContentView::Probe(
//     /* [in] */ REIID riid)
// {
//     if (riid == Elastos::Droid::View::EIID_View) {
//         return reinterpret_cast<PInterface>((View*)this);
//     }
//     else if (riid == Elastos::Droid::View::EIID_IInterface) {
//         return (PInterface)(IView*)this;
//     }
//     else if (riid == Elastos::Droid::View::EIID_IView) {
//         return (PInterface)(IView*)this;
//     }
//     else if (riid == Elastos::Droid::View::EIID_IDrawableCallback) {
//         return (PInterface)(IDrawableCallback*)this;
//     }
//     else if (riid == Elastos::Droid::View::EIID_IKeyEventCallback) {
//         return (PInterface)(IKeyEventCallback*)this;
//     }
//     else if (riid == Elastos::Droid::View::EIID_IAccessibilityEventSource) {
//         return (PInterface)(IAccessibilityEventSource*)this;
//     }
//     return _CView::Probe(riid);
// }

// CViewportWindow::ContentView::ContentView(
//     /* [in] */ IContext* context)
//     : _ContentView(context)
// {
// }

//=====================================================================
// CViewportWindow::ContentView
//=====================================================================
const String CViewportWindow::WINDOW_TITLE("Magnification Overlay");

CViewportWindow::CViewportWindow()
    : mShown(FALSE)
    , mAlpha(0)
{
}

ECode CViewportWindow::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IWindowManager* windowManager,
    /* [in] */ Handle32 displayProvider)
{
    CRect::New((IRect**)&mBounds);

    mWindowManager = windowManager;
    mDisplayProvider = (DisplayProvider*)displayProvider;

    // AutoPtr<IViewGroupLayoutParams> contentParams;
    // CViewGroupLayoutParams::New(
    //     IViewGroupLayoutParams::MATCH_PARENT, IViewGroupLayoutParams::MATCH_PARENT,
    //     (IViewGroupLayoutParams**)&contentParams);
    // mWindowContent = new ContentView(context);
    // mWindowContent->SetLayoutParams(contentParams);
    // mWindowContent->SetBackgroundColor(R::color::transparent);

    CWindowManagerLayoutParams::New(
        IWindowManagerLayoutParams::TYPE_MAGNIFICATION_OVERLAY,
        (IWindowManagerLayoutParams**)&mWindowParams);
    Int32 flags;
    mWindowParams->GetFlags(&flags);
    flags |= IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
        | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
        | IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE;
    mWindowParams->SetFlags(flags);

    AutoPtr<ICharSequence> seq;
    CString::New(WINDOW_TITLE, (ICharSequence**)&seq);
    mWindowParams->SetTitle(seq);
    mWindowParams->SetGravity(IGravity::CENTER);
    AutoPtr<IDisplayInfo> info = mDisplayProvider->GetDisplayInfo();
    Int32 w, h;
    info->GetLogicalWidth(&w);
    info->GetLogicalHeight(&h);
    mWindowParams->SetWidth(w);
    mWindowParams->SetHeight(h);
    mWindowParams->SetFormat(IPixelFormat::TRANSLUCENT);
    return NOERROR;
}

ECode CViewportWindow::IsShown(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mShown;
    return NOERROR;
}

ECode CViewportWindow::Show()
{
    if (mShown) {
        return NOERROR;
    }
    mShown = TRUE;
    mWindowManager->AddView(mWindowContent, mWindowParams);
    // if (DEBUG_VIEWPORT_WINDOW) {
    //     Slog.i(LOG_TAG, "ViewportWindow shown.");
    // }
    return NOERROR;
}

ECode CViewportWindow::Hide()
{
    if (!mShown) {
        return NOERROR;
    }
    mShown = FALSE;
    mWindowManager->RemoveView(mWindowContent);
    // if (DEBUG_VIEWPORT_WINDOW) {
    //     Slog.i(LOG_TAG, "ViewportWindow hidden.");
    // }
    return NOERROR;
}

ECode CViewportWindow::GetAlpha(
    /* [out] */ Int32* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = mAlpha;
    return NOERROR;
}

ECode CViewportWindow::SetAlpha(
    /* [in] */ Int32 alpha)
{
    if (mAlpha == alpha) {
        return NOERROR;
    }
    mAlpha = alpha;
    if (mShown) {
        mWindowContent->Invalidate();
    }
    // if (DEBUG_VIEWPORT_WINDOW) {
    //     Slog.i(LOG_TAG, "ViewportFrame set alpha: " + alpha);
    // }
    return NOERROR;
}

ECode CViewportWindow::GetBounds(
    /* [out] */ IRect** bounds)
{
    VALIDATE_NOT_NULL(bounds);
    *bounds = mBounds;
    REFCOUNT_ADD(*bounds);
    return NOERROR;
}

ECode CViewportWindow::RotationChanged()
{
    AutoPtr<IDisplayInfo> info = mDisplayProvider->GetDisplayInfo();
    Int32 w, h;
    info->GetLogicalWidth(&w);
    info->GetLogicalHeight(&h);
    mWindowParams->SetWidth(w);
    mWindowParams->SetHeight(h);
    if (mShown) {
        mWindowManager->UpdateViewLayout(mWindowContent, mWindowParams);
    }
    return NOERROR;
}

ECode CViewportWindow::SetBounds(
    /* [in] */ IRect* bounds)
{
    Boolean bval;
    if (mBounds->Equals(bounds, &bval), bval) {
        return NOERROR;
    }
    mBounds->Set(bounds);
    if (mShown) {
        mWindowContent->Invalidate();
    }
    // if (DEBUG_VIEWPORT_WINDOW) {
    //     Slog.i(LOG_TAG, "ViewportFrame set bounds: " + bounds);
    // }
    return NOERROR;
}


} // Accessibility
} // Server
} // Droid
} // Elastos
