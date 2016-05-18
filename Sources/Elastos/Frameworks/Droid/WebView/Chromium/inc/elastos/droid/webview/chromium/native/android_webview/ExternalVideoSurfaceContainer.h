#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_EXTERNALVIDEOSURFACECONTAINER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_EXTERNALVIDEOSURFACECONTAINER_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/RenderCoordinates.h"
#include "elastos/droid/view/SurfaceView.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::View::ISurfaceHolderCallback;
using Elastos::Droid::View::ISurfaceView;
using Elastos::Droid::View::SurfaceView;
using Elastos::Droid::View::IViewGroup;


using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ContentViewCore;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::RenderCoordinates;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * This is a container for external video surfaces.
 * The object is owned by the native peer and it is owned by WebContents.
 *
 * The expected behavior of the media player on the video hole punching is as follows.
 * 1) If it requests the surface, it will call requestExternalVideoSurface().
 *    When the resolution of the video is changed, it'll call requestExternalVideoSurface().
 * 2) Whenever the size or the position of the video element is changed, it'll notify through
 *    onExternalVideoSurfacePositionChanged().
 * 3) Whenever the page that contains the video element is scrolled or zoomed,
 *    onFrameInfoUpdated() will be called.
 * 4) Usually steps 1) ~ 3) are repeated during the playback.
 * 5) If the player no longer needs the surface any more, it'll call
 *    releaseExternalVideoSurface().
 *
 * Please contact ycheo@chromium.org or wonsik@chromium.org if you have any
 * questions or issues for this class.
 */
//@JNINamespace("android_webview")
class ExternalVideoSurfaceContainer
    : public Object
    , public ISurfaceHolderCallback
{
public:
    CAR_INTERFACE_DECL()
private:
    // Because WebView does hole-punching by itself, instead, the hole-punching logic
    // in SurfaceView can clear out some web elements like media control or subtitle.
    // So we need to disable its hole-punching logic.
    class NoPunchingSurfaceView
        : public SurfaceView
    {
    public:
        NoPunchingSurfaceView(
            /* [in] */ IContext* context);

    protected:
        // SurfaceView.dispatchDraw implementation punches a hole in the view hierarchy.
        // Disable this by making this a no-op.
        //@Override
        CARAPI DispatchDraw(
            /* [in] */ ICanvas* canvas);
    };

public:
    /**
     * Factory class to facilitate dependency injection.
     */
    class Factory
        : public Object
    {
        public:
            static AutoPtr<ExternalVideoSurfaceContainer> Create(
                /* [in] */ Handle64 nativeExternalVideoSurfaceContainer,
                /* [in] */ ContentViewCore* contentViewCore);
    };

public:
    //@VisibleForTesting
    static CARAPI_(void) SetFactory(
        /* [in] */ Factory* factory);

    // SurfaceHolder.Callback methods.
    //@Override
    CARAPI SurfaceChanged(
        /* [in] */ ISurfaceHolder* holder,
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    //@Override
    // surfaceCreated() callback can be called regardless of requestExternalVideoSurface,
    // if the activity comes back from the background and becomes visible.
    CARAPI SurfaceCreated(
        /* [in] */ ISurfaceHolder* holder);

    // surfaceDestroyed() callback can be called regardless of releaseExternalVideoSurface,
    // if the activity moves to the backgound and becomes invisible.
    //@Override
    CARAPI SurfaceDestroyed(
        /* [in] */ ISurfaceHolder* holder);

protected:
    static const Int32 INVALID_PLAYER_ID = -1;

    ExternalVideoSurfaceContainer(
        /* [in] */ Handle64 nativeExternalVideoSurfaceContainer,
        /* [in] */ ContentViewCore* contentViewCore);

    /**
     * Called when a media player wants to request an external video surface.
     * @param playerId The ID of the media player.
     */
    //@CalledByNative
    CARAPI_(void) RequestExternalVideoSurface(
        /* [in] */ Int32 playerId);

    /**
     * Called when a media player wants to release an external video surface.
     * @param playerId The ID of the media player.
     */
    //@CalledByNative
    CARAPI_(void) ReleaseExternalVideoSurface(
        /* [in] */ Int32 playerId);

    //@CalledByNative
    CARAPI_(void) Destroy();

    /**
     * Called when the position of the video element which uses the external
     * video surface is changed.
     * @param playerId The ID of the media player.
     * @param left The absolute CSS X coordinate of the left side of the video element.
     * @param top The absolute CSS Y coordinate of the top side of the video element.
     * @param right The absolute CSS X coordinate of the right side of the video element.
     * @param bottom The absolute CSS Y coordinate of the bottom side of the video element.
     */
    //@CalledByNative
    CARAPI_(void) OnExternalVideoSurfacePositionChanged(
        /* [in] */ Int32 playerId,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom);

    /**
     * Called when the page that contains the video element is scrolled or zoomed.
     */
    //@CalledByNative
    CARAPI_(void) OnFrameInfoUpdated();

private:
    CARAPI_(void) InitializeCurrentPositionOfSurfaceView();

    static CARAPI_(void) SetActiveContainer(
        /* [in] */ ExternalVideoSurfaceContainer* container);

    static CARAPI_(void) ReleaseIfActiveContainer(
        /* [in] */ ExternalVideoSurfaceContainer* container);

    CARAPI_(void) CreateSurfaceView();

    CARAPI_(void) RemoveSurfaceView();

    CARAPI_(void) LayOutSurfaceView();

    CARAPI_(void) NativeSurfaceCreated(
        /* [in] */ Handle64 nativeExternalVideoSurfaceContainerImpl,
        /* [in] */ Int32 playerId,
        /* [in] */ ISurface* surface);

    CARAPI_(void) NativeSurfaceDestroyed(
        /* [in] */ Handle64 nativeExternalVideoSurfaceContainerImpl,
        /* [in] */ Int32 playerId);

    //@CalledByNative return ExternalVideoSurfaceContainer
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ Handle64 nativeExternalVideoSurfaceContainer,
        /* [in] */ ContentViewCore* contentViewCore);

//callback function declaration
public:
    static CARAPI_(void*) ElaExternalVideoSurfaceContainerCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ Handle64 nativeExternalVideoSurfaceContainer,
        /* [in] */ IInterface* contentViewCore);

    static CARAPI_(void) RequestExternalVideoSurface(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 playerId);

    static CARAPI_(void) ReleaseExternalVideoSurface(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 playerId);

    static CARAPI_(void) Destroy(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnExternalVideoSurfacePositionChanged(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 playerId,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom);

    static CARAPI_(void) OnFrameInfoUpdated(
        /* [in] */ IInterface* obj);

private:
    // There can be at most 1 external video surface for now.
    // If there are the multiple requests for the surface, then the second video will
    // kick the first one off.
    // To support the mulitple video surfaces seems impractical, because z-order between
    // the multiple SurfaceViews is non-deterministic.
    //static WeakReference<ExternalVideoSurfaceContainer> sActiveContainer = new WeakReference<ExternalVideoSurfaceContainer>(null);
    static AutoPtr<IWeakReference> sActiveContainer;

    const Handle64 mNativeExternalVideoSurfaceContainer;
    const AutoPtr<ContentViewCore> mContentViewCore;
    Int32 mPlayerId ;
    AutoPtr<ISurfaceView> mSurfaceView;

    // The absolute CSS coordinates of the video element.
    Float mLeft;
    Float mTop;
    Float mRight;
    Float mBottom;

    // The physical location/size of the external video surface in pixels.
    Int32 mX;
    Int32 mY;
    Int32 mWidth;
    Int32 mHeight;

    static AutoPtr<Factory> sFactory;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_EXTERNALVIDEOSURFACECONTAINER_H__
