#include "Elastos.Droid.Os.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentView.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/JellyBeanContentView.h"
#include "elastos/droid/webkit/webview/chromium/native/base/TraceEvent.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Webkit::Webview::Chromium::Base::TraceEvent;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//===============================================================
//           ContentView::InnerSmartClipDataListener
//===============================================================

ContentView::InnerSmartClipDataListener::InnerSmartClipDataListener(
    /* [in] */ ContentView* owner,
    /* [in] */ IHandler* resultHandler)
    : mOwner(owner)
    , mResultHandler(resultHandler)
{
}

void ContentView::InnerSmartClipDataListener::OnSmartClipDataExtracted(
    /* [in] */ const String& text,
    /* [in] */ const String& html,
    /* [in] */ IRect* clipRect)
{
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutString(String("url"), mOwner->mContentViewCore->GetWebContents()->GetVisibleUrl());
    bundle->PutString(String("title"), mOwner->mContentViewCore->GetWebContents()->GetTitle());
    bundle->PutParcelable(String("rect"), IParcelable::Probe(clipRect));
    bundle->PutString(String("text"), text);
    bundle->PutString(String("html"), html);
    // try {
        AutoPtr<IMessageHelper> helper;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
        AutoPtr<IMessage> msg;
        helper->Obtain(mResultHandler, 0, (IMessage**)&msg);
        msg->SetData(bundle);
        msg->SendToTarget();
    // } catch (Exception e) {
    //     Log.e(TAG, "Error calling handler for smart clip data: ", e);
    // }
}

//===============================================================
//                       ContentView
//===============================================================

const String ContentView::TAG("ContentView");

ContentView::ContentView(
    /* [in] */ IContext* context,
    /* [in] */ ContentViewCore* cvc)
{
    FrameLayout::constructor(context, NULL, R::attr::webViewStyle);

    Int32 barStyle;
    if ((GetScrollBarStyle(&barStyle), barStyle) == IView::SCROLLBARS_INSIDE_OVERLAY) {
        SetHorizontalScrollBarEnabled(FALSE);
        SetVerticalScrollBarEnabled(FALSE);
    }

    SetFocusable(TRUE);
    SetFocusableInTouchMode(TRUE);

    mContentViewCore = cvc;
}

CAR_INTERFACE_IMPL(ContentView, FrameLayout, IObject)
/**
 * Creates an instance of a ContentView.
 * @param context The Context the view is running in, through which it can
 *                access the current theme, resources, etc.
 * @param cvc A pointer to the content view core managing this content view.
 * @return A ContentView instance.
 */
AutoPtr<ContentView> ContentView::NewInstance(
    /* [in] */ IContext* context,
    /* [in] */ ContentViewCore* cvc)
{
    if (Build::VERSION::SDK_INT < Build::VERSION_CODES::JELLY_BEAN) {
        return new ContentView(context, cvc);
    }
    else {
        return new JellyBeanContentView(context, cvc);
    }
    return NULL;
}

// Needed by ContentViewCore.InternalAccessDelegate
//@Override
Boolean ContentView::DrawChild(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IView* child,
    /* [in] */ Int64 drawingTime)
{
    return FrameLayout::DrawChild(canvas, child, drawingTime);
}

// Needed by ContentViewCore.InternalAccessDelegate
//@Override
ECode ContentView::OnScrollChanged(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 oldl,
    /* [in] */ Int32 oldt)
{
    return FrameLayout::OnScrollChanged(l, t, oldl, oldt);
}

//@Override
ECode ContentView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 ow,
    /* [in] */ Int32 oh)
{
    TraceEvent::Begin();
    FrameLayout::OnSizeChanged(w, h, ow, oh);
    mContentViewCore->OnSizeChanged(w, h, ow, oh);
    TraceEvent::End();
    return NOERROR;
}

//@Override
AutoPtr<IInputConnection> ContentView::OnCreateInputConnection(
    /* [in] */ IEditorInfo* outAttrs)
{
    return mContentViewCore->OnCreateInputConnection(outAttrs);
}

//@Override
Boolean ContentView::OnCheckIsTextEditor()
{
    return mContentViewCore->OnCheckIsTextEditor();
}

//@Override
void ContentView::OnFocusChanged(
    /* [in] */ Boolean gainFocus,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    TraceEvent::Begin();
    FrameLayout::OnFocusChanged(gainFocus, direction, previouslyFocusedRect);
    mContentViewCore->OnFocusChanged(gainFocus);
    TraceEvent::End();
}

//@Override
ECode ContentView::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    FrameLayout::OnWindowFocusChanged(hasWindowFocus);
    mContentViewCore->OnWindowFocusChanged(hasWindowFocus);
    return NOERROR;
}

//@Override
Boolean ContentView::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    return mContentViewCore->OnKeyUp(keyCode, event);
}

//@Override
Boolean ContentView::DispatchKeyEventPreIme(
    /* [in] */ IKeyEvent* event)
{
    return mContentViewCore->DispatchKeyEventPreIme(event);
}

//@Override
Boolean ContentView::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    Boolean isFocused;
    if (IsFocused(&isFocused), isFocused) {
        return mContentViewCore->DispatchKeyEvent(event);
    }
    else {
        Boolean res;
        FrameLayout::DispatchKeyEvent(event, &res);
        return res;
    }
    return FALSE;
}

//@Override
Boolean ContentView::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    return mContentViewCore->OnTouchEvent(event);
}

/**
 * Mouse move events are sent on hover enter, hover move and hover exit.
 * They are sent on hover exit because sometimes it acts as both a hover
 * move and hover exit.
 */
//@Override
Boolean ContentView::OnHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    Boolean consumed = mContentViewCore->OnHoverEvent(event);
    if (!mContentViewCore->IsTouchExplorationEnabled()) {
        Boolean res;
        FrameLayout::OnHoverEvent(event, &res);
    }
    return consumed;
}

//@Override
Boolean ContentView::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    return mContentViewCore->OnGenericMotionEvent(event);
}

//@Override
Boolean ContentView::PerformLongClick()
{
    return FALSE;
}

//@Override
ECode ContentView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    mContentViewCore->OnConfigurationChanged(newConfig);
    return NOERROR;
}

/**
 * Currently the ContentView scrolling happens in the native side. In
 * the Java view system, it is always pinned at (0, 0). scrollBy() and scrollTo()
 * are overridden, so that View's mScrollX and mScrollY will be unchanged at
 * (0, 0). This is critical for drawing ContentView correctly.
 */
//@Override
ECode ContentView::ScrollBy(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    mContentViewCore->ScrollBy(x, y);
    return NOERROR;
}

//@Override
ECode ContentView::ScrollTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    mContentViewCore->ScrollTo(x, y);
    return NOERROR;
}

//@Override
Int32 ContentView::ComputeHorizontalScrollExtent()
{
    // TODO(dtrainor): Need to expose scroll events properly to public. Either make getScroll*
    // work or expose computeHorizontalScrollOffset()/computeVerticalScrollOffset as public.
    return mContentViewCore->ComputeHorizontalScrollExtent();
}

//@Override
Int32 ContentView::ComputeHorizontalScrollOffset()
{
    return mContentViewCore->ComputeHorizontalScrollOffset();
}

//@Override
Int32 ContentView::ComputeHorizontalScrollRange()
{
    return mContentViewCore->ComputeHorizontalScrollRange();
}

//@Override
Int32 ContentView::ComputeVerticalScrollExtent()
{
    return mContentViewCore->ComputeVerticalScrollExtent();
}

//@Override
Int32 ContentView::ComputeVerticalScrollOffset()
{
    return mContentViewCore->ComputeVerticalScrollOffset();
}

//@Override
Int32 ContentView::ComputeVerticalScrollRange()
{
    return mContentViewCore->ComputeVerticalScrollRange();
}

// End FrameLayout overrides.

//@Override
Boolean ContentView::AwakenScrollBars(
    /* [in] */ Int32 startDelay,
    /* [in] */ Boolean invalidate)
{
    return mContentViewCore->AwakenScrollBars(startDelay, invalidate);
}

//@Override
Boolean ContentView::AwakenScrollBars()
{
    return FrameLayout::AwakenScrollBars();
}

//@Override
ECode ContentView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    FrameLayout::OnInitializeAccessibilityNodeInfo(info);
    mContentViewCore->OnInitializeAccessibilityNodeInfo(info);
    return NOERROR;
}

/**
 * Fills in scrolling values for AccessibilityEvents.
 * @param event Event being fired.
 */
//@Override
ECode ContentView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    FrameLayout::OnInitializeAccessibilityEvent(event);
    mContentViewCore->OnInitializeAccessibilityEvent(event);
    return NOERROR;
}

//@Override
ECode ContentView::OnAttachedToWindow()
{
    FrameLayout::OnAttachedToWindow();
    mContentViewCore->OnAttachedToWindow();
    return NOERROR;
}

//@Override
ECode ContentView::OnDetachedFromWindow()
{
    FrameLayout::OnDetachedFromWindow();
    mContentViewCore->OnDetachedFromWindow();
    return NOERROR;
}

//@Override
ECode ContentView::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    FrameLayout::OnVisibilityChanged(changedView, visibility);
    mContentViewCore->OnVisibilityChanged(changedView, visibility);
    return NOERROR;
}

// Implements SmartClipProvider
//@Override
ECode ContentView::ExtractSmartClipData(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mContentViewCore->ExtractSmartClipData(x, y, width, height);
    return NOERROR;
}

// Implements SmartClipProvider
//@Override
ECode ContentView::SetSmartClipResultHandler(
    /* [in] */ IHandler* resultHandler)
{
    if (resultHandler == NULL) {
        mContentViewCore->SetSmartClipDataListener(NULL);
        return NOERROR;
    }

    AutoPtr<ContentViewCore::SmartClipDataListener> listener = new InnerSmartClipDataListener(this, resultHandler);
    mContentViewCore->SetSmartClipDataListener(listener);

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//              Start Implementation of ContentViewCore.InternalAccessDelegate               //
///////////////////////////////////////////////////////////////////////////////////////////////

//@Override
Boolean ContentView::Super_onKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    Boolean res;
    FrameLayout::OnKeyUp(keyCode, event, &res);
    return res;
}

//@Override
Boolean ContentView::Super_dispatchKeyEventPreIme(
    /* [in] */ IKeyEvent* event)
{
    Boolean res;
    FrameLayout::DispatchKeyEventPreIme(event, &res);
    return res;
}

//@Override
Boolean ContentView::Super_dispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    Boolean res;
    FrameLayout::DispatchKeyEvent(event, &res);
    return res;
}

//@Override
Boolean ContentView::Super_onGenericMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    Boolean res;
    FrameLayout::OnGenericMotionEvent(event, &res);
    return res;
}

//@Override
void ContentView::Super_onConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    FrameLayout::OnConfigurationChanged(newConfig);
}

//@Override
Boolean ContentView::Super_awakenScrollBars(
    /* [in] */ Int32 startDelay,
    /* [in] */ Boolean invalidate)
{
    return FrameLayout::AwakenScrollBars(startDelay, invalidate);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//                End Implementation of ContentViewCore.InternalAccessDelegate               //
///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
