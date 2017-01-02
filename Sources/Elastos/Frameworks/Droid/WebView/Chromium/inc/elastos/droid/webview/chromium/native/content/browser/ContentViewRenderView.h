//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTVIEWRENDERVIEW_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTVIEWRENDERVIEW_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/FrameLayout.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentReadbackHandler.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Core::Runnable;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::View::ISurfaceHolderCallback;
using Elastos::Droid::View::ISurfaceView;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IFrameLayout;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Base {

class WindowElastos;

} // namespace Base
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

using Elastos::Droid::Webkit::Webview::Chromium::Ui::Base::WindowElastos;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import org.chromium.ui.base.WindowAndroid;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/***
 * This view is used by a ContentView to render its content.
 * Call {@link #setCurrentContentViewCore(ContentViewCore)} with the contentViewCore that should be
 * managing the view.
 * Note that only one ContentViewCore can be shown at a time.
 */
//@JNINamespace("content")
class ContentViewRenderView
    : public FrameLayout
{
private:
    class InnerSurfaceHolderCallback
        : public Object
        , public ISurfaceHolderCallback
    {
    public:
        InnerSurfaceHolderCallback(
            /* [in] */ ContentViewRenderView* owner);

        CAR_INTERFACE_DECL()

        //@Override
        CARAPI SurfaceChanged(
            /* [in] */ ISurfaceHolder* holder,
            /* [in] */ Int32 format,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        //@Override
        CARAPI SurfaceCreated(
            /* [in] */ ISurfaceHolder* holder);

        //@Override
        CARAPI SurfaceDestroyed(
            /* [in] */ ISurfaceHolder* holder);

    private:
        ContentViewRenderView* mOwner;
    };

    class InnerContentReadbackHandler
        : public ContentReadbackHandler
    {
    public:
        InnerContentReadbackHandler(
            /* [in] */ ContentViewRenderView* owner);

    protected:
        //@Override
        CARAPI_(Boolean) ReadyForReadback();

    private:
        ContentViewRenderView* mOwner;
    };

    class InnerRunnable
        : public Runnable
    {
    public:
        InnerRunnable(
            /* [in] */ ContentViewRenderView* owner);

        CARAPI Run();

    private:
        ContentViewRenderView* mOwner;
    };

public:
    /**
     * Constructs a new ContentViewRenderView.
     * This should be called and the {@link ContentViewRenderView} should be added to the view
     * hierarchy before the first draw to avoid a black flash that is seen every time a
     * {@link SurfaceView} is added.
     * @param context The context used to create this.
     */
    ContentViewRenderView(
        /* [in] */ IContext* context);

    /**
     * Initialization that requires native libraries should be done here.
     * Native code should add/remove the layers to be rendered through the ContentViewLayerRenderer.
     * @param rootWindow The {@link WindowAndroid} this render view should be linked to.
     */
    CARAPI_(void) OnNativeLibraryLoaded(
        /* [in] */ WindowElastos* rootWindow);

    /**
     * @return The content readback handler.
     */
    CARAPI_(AutoPtr<ContentReadbackHandler>) GetContentReadbackHandler();

    /**
     * Sets the background color of the surface view.  This method is necessary because the
     * background color of ContentViewRenderView itself is covered by the background of
     * SurfaceView.
     * @param color The color of the background.
     */
    CARAPI_(void) SetSurfaceViewBackgroundColor(
        /* [in] */ Int32 color);

    /**
     * Should be called when the ContentViewRenderView is not needed anymore so its associated
     * native resource can be freed.
     */
    CARAPI_(void) Destroy();

    CARAPI_(void) SetCurrentContentViewCore(
        /* [in] */ ContentViewCore* contentViewCore);

    /**
     * @return whether the surface view is initialized and ready to render.
     */
    CARAPI_(Boolean) IsInitialized();

    /**
     * Enter or leave overlay video mode.
     * @param enabled Whether overlay mode is enabled.
     */
    CARAPI_(void) SetOverlayVideoMode(
        /* [in] */ Boolean enabled);

    /**
     * Set the native layer tree helper for this {@link ContentViewRenderView}.
     * @param layerTreeBuildHelperNativePtr Native pointer to the layer tree build helper.
     */
    CARAPI_(void) SetLayerTreeBuildHelper(
        /* [in] */ Handle64 layerTreeBuildHelperNativePtr);

    static CARAPI_(void*) ElaContentViewRenderViewCallback_Init();

protected:
    /**
     * This method should be subclassed to provide actions to be performed once the view is ready to
     * render.
     */
    CARAPI_(void) OnReadyToRender();

    /**
     * This method could be subclassed optionally to provide a custom SurfaceView object to
     * this ContentViewRenderView.
     * @param context The context used to create the SurfaceView object.
     * @return The created SurfaceView object.
     */
    CARAPI_(AutoPtr<ISurfaceView>) CreateSurfaceView(
        /* [in] */ IContext* context);

    //@CalledByNative
    CARAPI_(void) OnCompositorLayout();

private:
    static CARAPI_(void) OnCompositorLayout(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnSwapBuffersCompleted(
        /* [in] */ IInterface* obj);

    //@CalledByNative
    CARAPI_(void) OnSwapBuffersCompleted();

    CARAPI_(Handle64) NativeInit(
        /* [in] */ Handle64 rootWindowNativePointer);

    CARAPI_(void) NativeDestroy(
        /* [in] */ Handle64 nativeContentViewRenderView);

    CARAPI_(void) NativeSetCurrentContentViewCore(
        /* [in] */ Handle64 nativeContentViewRenderView,
        /* [in] */ Handle64 nativeContentViewCore);

    CARAPI_(void) NativeSetLayerTreeBuildHelper(
        /* [in] */ Handle64 nativeContentViewRenderView,
        /* [in] */ Handle64 buildHelperNativePtr);

    CARAPI_(void) NativeSurfaceCreated(
        /* [in] */ Handle64 nativeContentViewRenderView);

    CARAPI_(void) NativeSurfaceDestroyed(
        /* [in] */ Handle64 nativeContentViewRenderView);

    CARAPI_(void) NativeSurfaceChanged(
        /* [in] */ Handle64 nativeContentViewRenderView,
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ ISurface* surface);

    CARAPI_(void) NativeSetOverlayVideoMode(
        /* [in] */ Handle64 nativeContentViewRenderView,
        /* [in] */ Boolean enabled);

protected:
    AutoPtr<ContentViewCore> mContentViewCore;

private:
    // The native side of this object.
    Handle64 mNativeContentViewRenderView;
    AutoPtr<ISurfaceHolderCallback> mSurfaceCallback;

    AutoPtr<ISurfaceView> mSurfaceView;

    AutoPtr<ContentReadbackHandler> mContentReadbackHandler;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTVIEWRENDERVIEW_H__
