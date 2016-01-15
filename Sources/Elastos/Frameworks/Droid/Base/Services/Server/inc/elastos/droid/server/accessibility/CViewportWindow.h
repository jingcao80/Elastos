

#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_CVIEWPORTWINDOW_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_CVIEWPORTWINDOW_H__

#include "_Elastos_Droid_Server_Accessibility_CViewportWindow.h"
#include "elastos/droid/ext/frameworkext.h"
// #include "elastos/droid/view/View.h"
//

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindowManagerLayoutParams;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

class DisplayProvider;

CarClass(CViewportWindow)
{
private:
    // class _ContentView
    //     : public View
    // {
    // public:
    //     _ContentView(
    //         /* [in] */ IContext* context);

    //     virtual CARAPI_(void) OnDraw(
    //         /* [in] */ ICanvas* canvas);

    // private:
    //     AutoPtr<IDrawable> mHighlightFrame;
    // };

    // class ContentView
    //     : public ElRefBase
    //     , public _ContentView
    //     , public IView
    //     , public IDrawableCallback
    //     , public IKeyEventCallback
    //     , public IAccessibilityEventSource
    //     , public IWeakReferenceSource
    // {
    // public:
    //     IVIEW_METHODS_DECL()
    //     IDRAWABLECALLBACK_METHODS_DECL()
    //     IKEYEVENTCALLBACK_METHODS_DECL()
    //     IACCESSIBILITYEVENTSOURCE_METHODS_DECL()

    //     CARAPI_(PInterface) Probe(
    //         /* [in] */ REIID riid);

        // CARAPI GetWeakReference(
        //     /* [out] */ IWeakReference** weakReference);

    //     ContentView(
    //         /* [in] */ IContext* context);
    // };

public:
    CViewportWindow();

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IWindowManager* service,
        /* [in] */ Handle32 displayProvider);

    CARAPI IsShown(
        /* [out] */ Boolean* result);

    CARAPI Show();

    CARAPI Hide();

    CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    CARAPI GetBounds(
        /* [out] */ IRect** bounds);

    CARAPI RotationChanged();

    CARAPI SetBounds(
        /* [in] */ IRect* bounds);

private:
    static const String WINDOW_TITLE;

    AutoPtr<IWindowManager> mWindowManager;
    DisplayProvider* mDisplayProvider;

    AutoPtr<IView> mWindowContent;
    AutoPtr<IWindowManagerLayoutParams> mWindowParams;

    AutoPtr<IRect> mBounds;
    Boolean mShown;
    Int32 mAlpha;
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_CVIEWPORTWINDOW_H__
