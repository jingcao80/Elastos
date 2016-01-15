
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CONTENTVIEW_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CONTENTVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/webkit/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/native/content/browser/SmartClipProvider.h"
//#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
// import android.util.Log;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::InputMethod::IInputConnection;
//using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IFrameLayout;

// import org.chromium.base.TraceEvent;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

/**
 * The containing view for {@link ContentViewCore} that exists in the Android UI hierarchy and
 * exposes the various {@link View} functionality to it.
 */
class ContentView
    //: public FrameLayout
    : public ContentViewCore::InternalAccessDelegate
    , public SmartClipProvider
{
private:
    class InnerSmartClipDataListener
        : public ContentViewCore::SmartClipDataListener
    {
    public:
        InnerSmartClipDataListener(
            /* [in] */ ContentView* owner,
            /* [in] */ const IHandler* resultHandler);

        CARAPI_(void) OnSmartClipDataExtracted(
            /* [in] */ const String& text,
            /* [in] */ const String& html,
            /* [in] */ IRect* clipRect);

    private:
        ContentView* mOwner;
        const IHandler* mResultHandler;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Creates an instance of a ContentView.
     * @param context The Context the view is running in, through which it can
     *                access the current theme, resources, etc.
     * @param cvc A pointer to the content view core managing this content view.
     * @return A ContentView instance.
     */
    static CARAPI_(AutoPtr<ContentView>) NewInstance(
        /* [in] */ IContext* context,
        /* [in] */ ContentViewCore* cvc);

    // Needed by ContentViewCore.InternalAccessDelegate
    //@Override
    CARAPI_(Boolean) DrawChild(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IView* child,
        /* [in] */ Int64 drawingTime);

    // Needed by ContentViewCore.InternalAccessDelegate
    //@Override
    CARAPI_(void) OnScrollChanged(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 oldl,
        /* [in] */ Int32 oldt);

    //@Override
    CARAPI_(AutoPtr<IInputConnection>) OnCreateInputConnection(
        /* [in] */ IEditorInfo* outAttrs);

    //@Override
    CARAPI_(Boolean) OnCheckIsTextEditor();

    //@Override
    CARAPI_(void) OnWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus);

    //@Override
    CARAPI_(Boolean) OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event);

    //@Override
    CARAPI_(Boolean) DispatchKeyEventPreIme(
        /* [in] */ IKeyEvent* event);

    //@Override
    CARAPI_(Boolean) DispatchKeyEvent(
        /* [in] */ IKeyEvent* event);

    //@Override
    CARAPI_(Boolean) OnTouchEvent(
        /* [in] */ IMotionEvent* event);

    /**
     * Mouse move events are sent on hover enter, hover move and hover exit.
     * They are sent on hover exit because sometimes it acts as both a hover
     * move and hover exit.
     */
    //@Override
    CARAPI_(Boolean) OnHoverEvent(
        /* [in] */ IMotionEvent* event);

    //@Override
    CARAPI_(Boolean) OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event);

    //@Override
    CARAPI_(Boolean) PerformLongClick();

    /**
     * Currently the ContentView scrolling happens in the native side. In
     * the Java view system, it is always pinned at (0, 0). scrollBy() and scrollTo()
     * are overridden, so that View's mScrollX and mScrollY will be unchanged at
     * (0, 0). This is critical for drawing ContentView correctly.
     */
    //@Override
    CARAPI_(void) ScrollBy(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    //@Override
    CARAPI_(void) ScrollTo(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    //@Override
    CARAPI_(Boolean) AwakenScrollBars(
        /* [in] */ Int32 startDelay,
        /* [in] */ Boolean invalidate);

    //@Override
    CARAPI_(Boolean) AwakenScrollBars();

    //@Override
    CARAPI_(void) OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    /**
     * Fills in scrolling values for AccessibilityEvents.
     * @param event Event being fired.
     */
    //@Override
    CARAPI_(void) OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // Implements SmartClipProvider
    //@Override
    CARAPI ExtractSmartClipData(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    // Implements SmartClipProvider
    //@Override
    CARAPI SetSmartClipResultHandler(
        /* [in] */ const IHandler* resultHandler);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //              Start Implementation of ContentViewCore.InternalAccessDelegate               //
    ///////////////////////////////////////////////////////////////////////////////////////////////

    //@Override
    CARAPI_(Boolean) Super_onKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event);

    //@Override
    CARAPI_(Boolean) Super_dispatchKeyEventPreIme(
        /* [in] */ IKeyEvent* event);

    //@Override
    CARAPI_(Boolean) Super_dispatchKeyEvent(
        /* [in] */ IKeyEvent* event);

    //@Override
    CARAPI_(Boolean) Super_onGenericMotionEvent(
        /* [in] */ IMotionEvent* event);

    //@Override
    CARAPI_(void) Super_onConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    //@Override
    CARAPI_(Boolean) Super_awakenScrollBars(
        /* [in] */ Int32 startDelay,
        /* [in] */ Boolean invalidate);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //                End Implementation of ContentViewCore.InternalAccessDelegate               //
    ///////////////////////////////////////////////////////////////////////////////////////////////

protected:
    ContentView(
        /* [in] */ IContext* context,
        /* [in] */ ContentViewCore* cvc);

    //@Override
    CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 ow,
        /* [in] */ Int32 oh);

    //@Override
    CARAPI_(void) OnFocusChanged(
        /* [in] */ Boolean gainFocus,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    //@Override
    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    //@Override
    CARAPI_(Int32) ComputeHorizontalScrollExtent();

    //@Override
    CARAPI_(Int32) ComputeHorizontalScrollOffset();

    //@Override
    CARAPI_(Int32) ComputeHorizontalScrollRange();

    //@Override
    CARAPI_(Int32) ComputeVerticalScrollExtent();

    //@Override
    CARAPI_(Int32) ComputeVerticalScrollOffset();

    //@Override
    CARAPI_(Int32) ComputeVerticalScrollRange();

    // End FrameLayout overrides.

    //@Override
    CARAPI_(void) OnAttachedToWindow();

    //@Override
    CARAPI_(void) OnDetachedFromWindow();

    //@Override
    CARAPI_(void) OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

protected:
    const AutoPtr<ContentViewCore> mContentViewCore;

private:
    static const String TAG;
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CONTENTVIEW_H__
