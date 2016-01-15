
#include "elastos/droid/webkit/native/content/browser/ContentView.h"
#include "elastos/droid/webkit/native/content/browser/JellyBeanContentView.h"
#include "elastos/droid/os/CBundle.h"

using Elastos::Droid::Os::CBundle;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

//===============================================================
//           ContentView::InnerSmartClipDataListener
//===============================================================

ContentView::InnerSmartClipDataListener::InnerSmartClipDataListener(
    /* [in] */ ContentView* owner,
    /* [in] */ const IHandler* resultHandler)
    : mOwner(owner)
    , mResultHandler(resultHandler)
{
}

void ContentView::InnerSmartClipDataListener::OnSmartClipDataExtracted(
    /* [in] */ const String& text,
    /* [in] */ const String& html,
    /* [in] */ IRect* clipRect)
{
    assert(0);
#if 0
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutString(String("url"), mContentViewCore->GetWebContents()->GetVisibleUrl());
    bundle->PutString(String("title"), mContentViewCore->GetWebContents()->GetTitle());
    bundle->PutParcelable(String("rect"), clipRect);
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
#endif
}

//===============================================================
//                       ContentView
//===============================================================

const String ContentView::TAG("ContentView");

ContentView::ContentView(
    /* [in] */ IContext* context,
    /* [in] */ ContentViewCore* cvc)
{
    assert(0);
#if 0
    super(context, NULL, android::R::attr::webViewStyle);

    if (GetScrollBarStyle() == IView::SCROLLBARS_INSIDE_OVERLAY) {
        SetHorizontalScrollBarEnabled(FALSE);
        SetVerticalScrollBarEnabled(FALSE);
    }

    SetFocusable(TRUE);
    SetFocusableInTouchMode(TRUE);

    mContentViewCore = cvc;
#endif
}

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
}

// Needed by ContentViewCore.InternalAccessDelegate
//@Override
Boolean ContentView::DrawChild(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IView* child,
    /* [in] */ Int64 drawingTime)
{
    assert(0);
//    return super.drawChild(canvas, child, drawingTime);
    return FALSE;
}

// Needed by ContentViewCore.InternalAccessDelegate
//@Override
void ContentView::OnScrollChanged(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 oldl,
    /* [in] */ Int32 oldt)
{
    assert(0);
//    super.onScrollChanged(l, t, oldl, oldt);
}

//@Override
void ContentView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 ow,
    /* [in] */ Int32 oh)
{
    assert(0);
#if 0
    TraceEvent::Begin();
    super.onSizeChanged(w, h, ow, oh);
    mContentViewCore->OnSizeChanged(w, h, ow, oh);
    TraceEvent::End();
#endif
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
    assert(0);
#if 0
    TraceEvent::Begin();
    super.onFocusChanged(gainFocus, direction, previouslyFocusedRect);
    mContentViewCore->OnFocusChanged(gainFocus);
    TraceEvent::End();
#endif
}

//@Override
void ContentView::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    assert(0);
#if 0
    super.onWindowFocusChanged(hasWindowFocus);
    mContentViewCore->OnWindowFocusChanged(hasWindowFocus);
#endif
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
    assert(0);
#if 0
    if (IsFocused()) {
        return mContentViewCore->DispatchKeyEvent(event);
    }
    else {
        return super.dispatchKeyEvent(event);
    }
#endif

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
        assert(0);
//        super.onHoverEvent(event);
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
void ContentView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    mContentViewCore->OnConfigurationChanged(newConfig);
}

/**
 * Currently the ContentView scrolling happens in the native side. In
 * the Java view system, it is always pinned at (0, 0). scrollBy() and scrollTo()
 * are overridden, so that View's mScrollX and mScrollY will be unchanged at
 * (0, 0). This is critical for drawing ContentView correctly.
 */
//@Override
void ContentView::ScrollBy(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    mContentViewCore->ScrollBy(x, y);
}

//@Override
void ContentView::ScrollTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    mContentViewCore->ScrollTo(x, y);
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
    assert(0);
//    return super.awakenScrollBars();
    return FALSE;
}

//@Override
void ContentView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    assert(0);
//    super.onInitializeAccessibilityNodeInfo(info);
//    mContentViewCore->OnInitializeAccessibilityNodeInfo(info);
}

/**
 * Fills in scrolling values for AccessibilityEvents.
 * @param event Event being fired.
 */
//@Override
void ContentView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    assert(0);
//    super.onInitializeAccessibilityEvent(event);
//    mContentViewCore->OnInitializeAccessibilityEvent(event);
}

//@Override
void ContentView::OnAttachedToWindow()
{
    assert(0);
//    super.onAttachedToWindow();
//    mContentViewCore->OnAttachedToWindow();
}

//@Override
void ContentView::OnDetachedFromWindow()
{
    assert(0);
//    super.onDetachedFromWindow();
//    mContentViewCore->OnDetachedFromWindow();
}

//@Override
void ContentView::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    assert(0);
//    super.onVisibilityChanged(changedView, visibility);
//    mContentViewCore->OnVisibilityChanged(changedView, visibility);
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
    /* [in] */ const IHandler* resultHandler)
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
    assert(0);
//    return super.onKeyUp(keyCode, event);
    return FALSE;
}

//@Override
Boolean ContentView::Super_dispatchKeyEventPreIme(
    /* [in] */ IKeyEvent* event)
{
    assert(0);
//    return super.dispatchKeyEventPreIme(event);
    return FALSE;
}

//@Override
Boolean ContentView::Super_dispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    assert(0);
//    return super.dispatchKeyEvent(event);
    return FALSE;
}

//@Override
Boolean ContentView::Super_onGenericMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    assert(0);
//    return super.onGenericMotionEvent(event);
    return FALSE;
}

//@Override
void ContentView::Super_onConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    assert(0);
//    super.onConfigurationChanged(newConfig);
}

//@Override
Boolean ContentView::Super_awakenScrollBars(
    /* [in] */ Int32 startDelay,
    /* [in] */ Boolean invalidate)
{
    assert(0);
//    return super.awakenScrollBars(startDelay, invalidate);
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//                End Implementation of ContentViewCore.InternalAccessDelegate               //
///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
