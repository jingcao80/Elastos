#ifndef __ELASTOS_DROID_SERVER_WM_CSESSION_H__
#define __ELASTOS_DROID_SERVER_WM_CSESSION_H__

#include "_Elastos_Droid_Server_Wm_CSession.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IWindowSession;
using Elastos::Droid::View::IIWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceSession;
using Elastos::Droid::View::IIWindowId;
using Elastos::Droid::View::IIWindowSessionCallback;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Internal::View::IIInputContext;
using Elastos::Droid::Internal::View::IInputMethodClient;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

CarClass(CSession)
    , public Object
    , public IWindowSession
    , public IProxyDeathRecipient
{
public:
    CSession();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IIWindowManager* wmService,
        /* [in] */ IIWindowSessionCallback* callback,
        /* [in] */ IInputMethodClient* client,
        /* [in] */ IIInputContext* inputContext);

    CARAPI Add(
        /* [in] */ IIWindow* window,
        /* [in] */ Int32 seq,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ Int32 viewVisibility,
        /* [in] */ IRect* inContentInsets,
        /* [in] */ IInputChannel* inInputChannel,
        /* [out] */ IRect** outContentInsets,
        /* [out] */ IInputChannel** outInputChannel,
        /* [out] */ Int32* result);

    CARAPI AddToDisplay(
        /* [in] */ IIWindow* window,
        /* [in] */ Int32 seq,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ Int32 viewVisibility,
        /* [in] */ Int32 displayId,
        /* [in] */ IRect* inContentInsets,
        /* [in] */ IInputChannel* inInputChannel,
        /* [out] */ IRect** outContentInsets,
        /* [out] */ IInputChannel** outInputChannel,
        /* [out] */ Int32* result);

    CARAPI AddWithoutInputChannel(
        /* [in] */ IIWindow* window,
        /* [in] */ Int32 seq,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ Int32 viewVisibility,
        /* [in] */ IRect* inContentInsets,
        /* [out] */ IRect** outContentInsets,
        /* [out] */ Int32* result);

    CARAPI AddToDisplayWithoutInputChannel(
        /* [in] */ IIWindow* window,
        /* [in] */ Int32 seq,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ Int32 viewVisibility,
        /* [in] */ Int32 layerStackId,
        /* [in] */ IRect* inContentInsets,
        /* [out] */ IRect** outContentInsets,
        /* [out] */ Int32* result);

    CARAPI Remove(
        /* [in] */ IIWindow* window);

    /**
     * Change the parameters of a window.  You supply the
     * new parameters, it returns the new frame of the window on screen (the
     * position should be ignored) and surface of the window.  The surface
     * will be invalid if the window is currently hidden, else you can use it
     * to draw the window's contents.
     *
     * @param window The window being modified.
     * @param attrs If non-null, new attributes to apply to the window.
     * @param requestedWidth The width the window wants to be.
     * @param requestedHeight The height the window wants to be.
     * @param viewVisibility Window root view's visibility.
     * @param insetsPending Set to true if the client will be later giving
     * internal insets; as a result, the window will not impact other window
     * layouts until the insets are given.
     * @param outFrame Rect in which is placed the new position/size on
     * screen.
     * @param outContentInsets Rect in which is placed the offsets from
     * <var>outFrame</var> in which the content of the window should be
     * placed.  This can be used to modify the window layout to ensure its
     * contents are visible to the user, taking into account system windows
     * like the status bar or a soft keyboard.
     * @param outVisibleInsets Rect in which is placed the offsets from
     * <var>outFrame</var> in which the window is actually completely visible
     * to the user.  This can be used to temporarily scroll the window's
     * contents to make sure the user can see it.  This is different than
     * <var>outContentInsets</var> in that these insets change transiently,
     * so complex relayout of the window should not happen based on them.
     * @param outConfiguration New configuration of window, if it is now
     * becoming visible and the global configuration has changed since it
     * was last displayed.
     * @param outSurface Object in which is placed the new display surface.
     *
     * @return int Result flags: {@link WindowManagerImpl#RELAYOUT_SHOW_FOCUS},
     * {@link WindowManagerImpl#RELAYOUT_FIRST_TIME}.
     */
    //todo: In RPC, ISurface variable should be put in the end
    //      because of the implementation of the ReadFromParcel method of it.
    CARAPI Relayout(
        /* [in] */ IIWindow* window,
        /* [in] */ Int32 seq,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ Int32 requestedWidth,
        /* [in] */ Int32 requestedHeight,
        /* [in] */ Int32 viewFlags,
        /* [in] */ Int32 flags,
        /* [in] */ IRect* inFrame,
        /* [in] */ IRect* inOverscanInsets,
        /* [in] */ IRect* inContentInsets,
        /* [in] */ IRect* inVisibleInsets,
        /* [in] */ IRect* inStableInsets,
        /* [in] */ IConfiguration* inConfig,
        /* [in] */ ISurface* inSurface,
        /* [out] */ IRect** outFrame,
        /* [out] */ IRect** outOverscanInsets,
        /* [out] */ IRect** outContentInsets,
        /* [out] */ IRect** outVisibleInsets,
        /* [out] */ IRect** outStableInsets,
        /* [out] */ IConfiguration** outConfig,
        /* [out] */ ISurface** outSurface,
        /* [out] */ Int32* result);

    /**
     * If a call to relayout() asked to have the surface destroy deferred,
     * it must call this once it is okay to destroy that surface.
     */
    CARAPI PerformDeferredDestroy(
        /* [in] */ IIWindow* window);

    /**
     * Called by a client to report that it ran out of graphics memory.
     */
    CARAPI OutOfMemory(
        /* [in] */ IIWindow* window,
        /* [out] */ Boolean* result);

    /**
     * Give the window manager a hint of the part of the window that is
     * completely transparent, allowing it to work with the surface flinger
     * to optimize compositing of this part of the window.
     */
    CARAPI SetTransparentRegion(
        /* [in] */ IIWindow* window,
        /* [in] */ IRegion* region);

    /**
     * Tell the window manager about the content and visible insets of the
     * given window, which can be used to adjust the <var>outContentInsets</var>
     * and <var>outVisibleInsets</var> values returned by
     * {@link #relayout relayout()} for windows behind this one.
     *
     * @param touchableInsets Controls which part of the window inside of its
     * frame can receive pointer events, as defined by
     * {@link android.view.ViewTreeObserver.InternalInsetsInfo}.
     */
    CARAPI SetInsets(
        /* [in] */ IIWindow* window,
        /* [in] */ Int32 touchableInsets,
        /* [in] */ IRect* contentInsets,
        /* [in] */ IRect* visibleInsets,
        /* [in] */ IRegion* touchableRegion);

    /**
     * Return the current display size in which the window is being laid out,
     * accounting for screen decorations around it.
     */
    CARAPI GetDisplayFrame(
        /* [in] */ IIWindow* window,
        /* [out] */ IRect** outDisplayFrame);

    CARAPI FinishDrawing(
        /* [in] */ IIWindow* window);

    CARAPI SetInTouchMode(
        /* [in] */ Boolean mode);

    CARAPI GetInTouchMode(
        /* [out] */ Boolean* result);

    CARAPI PerformHapticFeedback(
        /* [in] */ IIWindow* window,
        /* [in] */ Int32 effectId,
        /* [in] */ Boolean always,
        /* [out] */ Boolean* result);

    /**
     * Allocate the drag's thumbnail surface.  Also assigns a token that identifies
     * the drag to the OS and passes that as the return value.  A return value of
     * null indicates failure.
     */
    CARAPI PrepareDrag(
        /* [in] */ IIWindow* window,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 thumbnailWidth,
        /* [in] */ Int32 thumbnailHeight,
        /* [out] */ ISurface** outSurface,
        /* [out] */ IBinder** binder);

    /**
     * Initiate the drag operation itself
     */
    CARAPI PerformDrag(
        /* [in] */ IIWindow* window,
        /* [in] */ IBinder* dragToken,
        /* [in] */ Float touchX,
        /* [in] */ Float touchY,
        /* [in] */ Float thumbCenterX,
        /* [in] */ Float thumbCenterY,
        /* [in] */ IClipData* data,
        /* [out] */ Boolean* result);

    /**
     * Report the result of a drop action targeted to the given window.
     * consumed is 'true' when the drop was accepted by a valid recipient,
     * 'false' otherwise.
     */
    CARAPI ReportDropResult(
        /* [in] */ IIWindow* window,
        /* [in] */ Boolean consumed);

    /**
     * Tell the OS that we've just dragged into a View that is willing to accept the drop
     */
    CARAPI DragRecipientEntered(
        /* [in] */ IIWindow* window);

    /**
     * Tell the OS that we've just dragged *off* of a View that was willing to accept the drop
     */
    CARAPI DragRecipientExited(
        /* [in] */ IIWindow* window);

    /**
     * For windows with the wallpaper behind them, and the wallpaper is
     * larger than the screen, set the offset within the screen.
     * For multi screen launcher type applications, xstep and ystep indicate
     * how big the increment is from one screen to another.
     */
    CARAPI SetWallpaperPosition(
        /* [in] */ IBinder* windowToken,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float xstep,
        /* [in] */ Float ystep);

    CARAPI WallpaperOffsetsComplete(
        /* [in] */ IBinder* window);

    CARAPI SetWallpaperDisplayOffset(
        /* [in] */ IBinder* windowToken,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI SendWallpaperCommand(
        /* [in] */ IBinder* window,
        /* [in] */ const String& action,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ IBundle* extras,
        /* [in] */ Boolean sync,
        /* [out] */ IBundle** result);

    CARAPI WallpaperCommandComplete(
        /* [in] */ IBinder* window,
        /* [in] */ IBundle* result);

    CARAPI SetUniverseTransform(
        /* [in] */ IBinder* window,
        /* [in] */ Float alpha,
        /* [in] */ Float offx,
        /* [in] */ Float offy,
        /* [in] */ Float dsdx,
        /* [in] */ Float dtdx,
        /* [in] */ Float dsdy,
        /* [in] */ Float dtdy);

    /**
     * Notifies that a rectangle on the screen has been requested.
     */
    CARAPI OnRectangleOnScreenRequested(
        /* [in] */ IBinder* token,
        /* [in] */ IRect* rectangle);

    CARAPI GetWindowId(
        /* [in] */ IBinder* window,
        /* [out] */ IIWindowId** winId);

    CARAPI ProxyDied();

    CARAPI_(void) WindowAddedLocked();

    CARAPI_(void) WindowRemovedLocked();

    CARAPI_(void) KillSessionLocked();

    /**
    * Get the current x offset for the wallpaper
    */
    CARAPI GetLastWallpaperX(
        /* [out] */ Int32* x);

    /**
    * Get the current y offset for the wallpaper
    */
    CARAPI GetLastWallpaperY(
        /* [out] */ Int32* y);

public:
    AutoPtr<CWindowManagerService> mService;
    AutoPtr<IIWindowSessionCallback> mCallback;
    AutoPtr<IInputMethodClient> mClient;
    AutoPtr<IIInputContext> mInputContext;
    Int32 mUid;
    Int32 mPid;
    String mStringName;
    AutoPtr<ISurfaceSession> mSurfaceSession;
    Int32 mNumWindow;
    Boolean mClientDead;
    Float mLastReportedAnimatorScale;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_CSESSION_H__
