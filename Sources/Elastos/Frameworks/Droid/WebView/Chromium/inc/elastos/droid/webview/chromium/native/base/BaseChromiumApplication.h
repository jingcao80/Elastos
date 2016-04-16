
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_BASECHROMIUMAPPLICATION_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_BASECHROMIUMAPPLICATION_H__

#include "Elastos.Droid.App.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ObserverList.h"
#include "elastos/droid/webkit/webview/chromium/native/base/WindowCallbackWrapper.h"
#include "elastos/droid/app/Application.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::Application;
using Elastos::Droid::App::IApplication;
using Elastos::Droid::App::IActivityLifecycleCallbacks;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * Basic application functionality that should be shared among all browser applications.
 */
class BaseChromiumApplication
    :public Application
{
public:
    /**
     * Interface to be implemented by listeners for window focus events.
     */
    class WindowFocusChangedListener : public Object
    {
    public:
        /**
         * Called when the window focus changes for {@code activity}.
         * @param activity The {@link Activity} that has a window focus changed event.
         * @param hasFocus Whether or not {@code activity} gained or lost focus.
         */
        virtual CARAPI_(void) OnWindowFocusChanged(
            /* [in] */ IActivity* activity,
            /* [in] */ Boolean hasFocus);
    };

private:
    class InnerActivityLifecycleCallbacks
        : public Object
        , public IActivityLifecycleCallbacks
    {
    private:
        class InnerWindowCallbackWrapper : public WindowCallbackWrapper
        {
        public:
            InnerWindowCallbackWrapper(
                /* [in] */ InnerActivityLifecycleCallbacks* owner,
                /* [in] */ IWindowCallback* callback);

            //@Override
            CARAPI OnWindowFocusChanged(
                /* [in] */ IActivity* activity,
                /* [in] */ Boolean hasFocus);

        private:
            InnerActivityLifecycleCallbacks* mOwner;
        };

    public:
        CAR_INTERFACE_DECL()

        InnerActivityLifecycleCallbacks(
            /* [in] */ BaseChromiumApplication* owner);

        //@Override
        CARAPI OnActivityCreated(
            /* [in] */ IActivity* activity,
            /* [in] */ IBundle* savedInstanceState);

        //@Override
        CARAPI OnActivityDestroyed(
            /* [in] */ IActivity* activity);

        //@Override
        CARAPI OnActivityPaused(
            /* [in] */ IActivity* activity);

        //@Override
        CARAPI OnActivityResumed(
            /* [in] */ IActivity* activity);

        //@Override
        CARAPI OnActivitySaveInstanceState(
            /* [in] */ IActivity* activity,
            /* [in] */ IBundle* outState);

        //@Override
        CARAPI OnActivityStarted(
            /* [in] */ IActivity* activity);

        //@Override
        CARAPI OnActivityStopped(
            /* [in] */ IActivity* activity);

    private:
        BaseChromiumApplication* mOwner;
    };

public:
    CAR_INTERFACE_DECL();

    //@Override
    CARAPI OnCreate();

    /**
     * Registers a listener to receive window focus updates on activities in this application.
     * @param listener Listener to receive window focus events.
     */
    virtual CARAPI_(void) RegisterWindowFocusChangedListener(
        /* [in] */ WindowFocusChangedListener* listener);

    /**
     * Unregisters a listener from receiving window focus updates on activities in this application.
     * @param listener Listener that doesn't want to receive window focus events.
     */
    virtual CARAPI_(void) UnregisterWindowFocusChangedListener(
        /* [in] */ WindowFocusChangedListener* listener);

private:
    // ObserverList<WindowFocusChangedListener> mWindowFocusListeners =
    //         new ObserverList<WindowFocusChangedListener>();
    ObserverList mWindowFocusListeners;
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_BASECHROMIUMAPPLICATION_H__
