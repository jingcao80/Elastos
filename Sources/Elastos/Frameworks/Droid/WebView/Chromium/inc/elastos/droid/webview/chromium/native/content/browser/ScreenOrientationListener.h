// wuweizuo automatic build .h file from .java file.
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_SCREENORIENTATIONLISTENER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_SCREENORIENTATIONLISTENER_H_

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ObserverList.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Content::IComponentCallbacks;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Hardware::Display::IDisplayListener;
using Elastos::Droid::Os::Build;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ObserverList;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * ScreenOrientationListener is a class that informs its observers when the
  * screen orientation changes.
  */
// @VisibleForTesting
class ScreenOrientationListener : public Object
{
public:
    /**
      * Observes changes in screen orientation.
      */
    class ScreenOrientationObserver : public Object
    {
    public:
        /**
          * Called whenever the screen orientation changes.
          *
          * @param orientation The orientation angle of the screen.
          */
        virtual CARAPI OnScreenOrientationChanged(
            /* [in] */ Int32 orientation) = 0;
    };

    class InnerRunnable
        : public Runnable
    {
    public:
        InnerRunnable(
            /* [in] */ ScreenOrientationListener* owner,
            /* [in] */ ScreenOrientationObserver* obs);

        // @Override
        CARAPI Run();

    private:
        ScreenOrientationListener* mOwner;
        ScreenOrientationObserver* mObs;
    };

private:
    /**
      * ScreenOrientationListenerBackend is an interface that abstract the
      * mechanism used for the actual screen orientation listening. The reason
      * being that from Android API Level 17 DisplayListener will be used. Before
      * that, an unreliable solution based on onConfigurationChanged has to be
      * used.
      */
    class ScreenOrientationListenerBackend : public Object
    {
    public:
        /**
          * Starts to listen for screen orientation changes. This will be called
          * when the first observer is added.
          */
        virtual CARAPI StartListening() = 0;

        /**
          * Stops to listen for screen orientation changes. This will be called
          * when the last observer is removed.
          */
        virtual CARAPI StopListening() = 0;
    };

    /**
      * ScreenOrientationConfigurationListener implements ScreenOrientationListenerBackend
      * to use ComponentCallbacks in order to listen for screen orientation
      * changes.
      *
      * This method is known to not correctly detect 180 degrees changes but it
      * is the only method that will work before API Level 17 (excluding polling).
      */
    class ScreenOrientationConfigurationListener
        : public ScreenOrientationListenerBackend
        , public IComponentCallbacks
    {
    public:
        ScreenOrientationConfigurationListener(
          /* [in] */ ScreenOrientationListener* owner);

        CAR_INTERFACE_DECL()

        // ScreenOrientationListenerBackend implementation:
        // @Override
        CARAPI StartListening();

        // @Override
        CARAPI StopListening();

        // ComponentCallbacks implementation:
        // @Override
        CARAPI OnConfigurationChanged(
            /* [in] */ IConfiguration* newConfig);

        // @Override
        CARAPI OnLowMemory();

    private:
      ScreenOrientationListener* mOwner;
    };

    /**
      * ScreenOrientationDisplayListener implements ScreenOrientationListenerBackend
      * to use DisplayListener in order to listen for screen orientation changes.
      *
      * This method is reliable but DisplayListener is only available for API Level 17+.
      */
    // @SuppressLint("NewApi")
    class ScreenOrientationDisplayListener
        : public ScreenOrientationListenerBackend
        , public IDisplayListener
    {
    public:
        ScreenOrientationDisplayListener(
          /* [in] */ ScreenOrientationListener* owner);

        CAR_INTERFACE_DECL();

        // ScreenOrientationListenerBackend implementation:
        // @Override
        CARAPI StartListening();

        // @Override
        CARAPI StopListening();

        // DisplayListener implementation:
        // @Override
        CARAPI OnDisplayAdded(
            /* [in] */ Int32 displayId);

        // @Override
        CARAPI OnDisplayRemoved(
            /* [in] */ Int32 displayId);

        // @Override
        CARAPI OnDisplayChanged(
            /* [in] */ Int32 displayId);

    private:
        ScreenOrientationListener* mOwner;
    };

public:
    /**
      * Returns a ScreenOrientationListener implementation based on the device's
      * supported API level.
      */
    static CARAPI_(AutoPtr<ScreenOrientationListener>) GetInstance();

    /**
      * Creates a ScreenOrientationConfigurationListener backend regardless of
      * the current SDK.
      */
    // @VisibleForTesting
    virtual CARAPI InjectConfigurationListenerBackendForTest();

    /**
      * Add |observer| in the ScreenOrientationListener observer list and
      * immediately call |onScreenOrientationChanged| on it with the current
      * orientation value.
      *
      * @param observer The observer that will get notified.
      * @param context The context associated with this observer.
      */
    virtual CARAPI AddObserver(
        /* [in] */ ScreenOrientationObserver* observer,
        /* [in] */ IContext* context);

    /**
      * Remove the |observer| from the ScreenOrientationListener observer list.
      *
      * @param observer The observer that will no longer receive notification.
      */
    virtual CARAPI RemoveObserver(
        /* [in] */ ScreenOrientationObserver* observer);

private:
    ScreenOrientationListener();

    /**
      * This should be called by classes extending ScreenOrientationListener when
      * it is possible that there is a screen orientation change. If there is an
      * actual change, the observers will get notified.
      */
    CARAPI NotifyObservers();

    /**
      * Updates |mOrientation| based on the default display rotation.
      */
    CARAPI UpdateOrientation();

private:
    static const String TAG;
    // List of observers to notify when the screen orientation changes.
    ///*const*/ AutoPtr< ObserverList<ScreenOrientationObserver> > mObservers;
    ObserverList mObservers;
    // mOrientation will be updated every time the orientation changes. When not
    // listening for changes, the value will be invalid and will be updated when
    // starting to listen again.
    Int32 mOrientation;
    // Current application context derived from the first context being received.
    AutoPtr<IContext> mAppContext;
    AutoPtr<ScreenOrientationListenerBackend> mBackend;
    static AutoPtr<ScreenOrientationListener> sInstance;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_SCREENORIENTATIONLISTENER_H_
