#ifndef __ELASTOS_DROID_SERVER_WM_WINDOWSTATE_H__
#define __ELASTOS_DROID_SERVER_WM_WINDOWSTATE_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/server/wm/CSession.h"
#include "elastos/droid/server/wm/WindowToken.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::View::IWindowState;
using Elastos::Droid::View::IIWindowId;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IApplicationToken;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::IIWindow;
using Elastos::Droid::View::IIWindowFocusObserver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Server::Input::IInputWindowHandle;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;

typedef IArrayList WindowList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class DisplayContent;
class TaskStack;
class WindowStateAnimator;

class WindowState
    : public Object
    , public IWindowState
{
private:
    class DeathRecipient
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        DeathRecipient(
            /* [in] */ WindowState* owner);

        //@Override
        CARAPI ProxyDied();

    private:
        AutoPtr<IWeakReference> mWeakOwner;
    };

    class ResizeRunnable : public Runnable
    {
    public:
        ResizeRunnable(
            /* [in] */ WindowState* host,
            /* [in] */ IRect* frame,
            /* [in] */ IRect* overscanInsets,
            /* [in] */ IRect* contentInsets,
            /* [in] */ IRect* visibleInsets,
            /* [in] */ IRect* stableInsets,
            /* [in] */ Boolean reportDraw,
            /* [in] */ IConfiguration* newConfig);

        CARAPI Run();

    private:
        WindowState* mHost;
        AutoPtr<IRect> mFrame;
        AutoPtr<IRect> mOverscanInsets;
        AutoPtr<IRect> mContentInsets;
        AutoPtr<IRect> mVisibleInsets;
        AutoPtr<IRect> mStableInsets;
        Boolean mReportDraw;
        AutoPtr<IConfiguration> mNewConfig;
    };

public:
    CAR_INTERFACE_DECL()

    WindowState();

    ~WindowState();

    CARAPI constructor(
        /* [in] */ CWindowManagerService* service,
        /* [in] */ CSession* s,
        /* [in] */ IIWindow* c,
        /* [in] */ WindowToken* token,
        /* [in] */ WindowState* attachedWindow,
        /* [in] */ Int32 appOp,
        /* [in] */ Int32 seq,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ Int32 viewVisibility,
        /* [in] */ DisplayContent* displayContent);


    CARAPI_(void) Attach();

    CARAPI GetOwningUid(
        /* [out] */ Int32* id);

    CARAPI GetOwningPackage(
        /* [out] */ String* package);

    CARAPI ComputeFrameLw(
        /* [in] */ IRect* pf,
        /* [in] */ IRect* df,
        /* [in] */ IRect* of,
        /* [in] */ IRect* cf,
        /* [in] */ IRect* vf,
        /* [in] */ IRect* dcf,
        /* [in] */ IRect* sf);

    CARAPI GetFrameLw(
        /* [out] */ IRect** frame);

    CARAPI GetShownFrameLw(
        /* [out] */ IRectF** shownFrame);

    CARAPI GetDisplayFrameLw(
        /* [out] */ IRect** displayFrame);

    CARAPI GetOverscanFrameLw(
        /* [out] */ IRect** displayFrame);

    CARAPI GetContentFrameLw(
        /* [out] */ IRect** contentFrame);

    CARAPI GetVisibleFrameLw(
        /* [out] */ IRect** visibleFrame);

    CARAPI GetGivenInsetsPendingLw(
        /* [out] */ Boolean* result);

    CARAPI GetGivenContentInsetsLw(
        /* [out] */ IRect** insetsRect);

    CARAPI GetGivenVisibleInsetsLw(
        /* [out] */ IRect** visibleArea);

    CARAPI GetAttrs(
        /* [out] */ IWindowManagerLayoutParams** attrs);

    CARAPI GetNeedsMenuLw(
        /* [in] */ IWindowState* bottom,
        /* [out] */ Boolean* result);

    CARAPI GetSystemUiVisibility(
        /* [out] */ Int32* flag);

    CARAPI GetSurfaceLayer(
        /* [out] */ Int32* surfaceLayer);

    CARAPI GetAppToken(
        /* [out] */ IApplicationToken** token);

    CARAPI IsVoiceInteraction(
        /* [out] */ Boolean* result);

    CARAPI_(Boolean) SetInsetsChanged();

    CARAPI_(AutoPtr<DisplayContent>) GetDisplayContent();

    CARAPI_(Int32) GetDisplayId();

    CARAPI_(AutoPtr<TaskStack>) GetStack();

    CARAPI_(void) GetStackBounds(
        /* [in] */ IRect* bounds);

    CARAPI_(Int64) GetInputDispatchingTimeoutNanos();

    /**
     * Return true if, at any point, the application token associated with
     * this window has actually displayed any windows.  This is most useful
     * with the "starting up" window to determine if any windows were
     * displayed when it is closed.
     *
     * @return Returns true if one or more windows have been displayed,
     *         else false.
     */
    CARAPI HasAppShownWindows(
        /* [out] */ Boolean* result);

    CARAPI_(Boolean) IsIdentityMatrix(
        /* [in] */ Float dsdx,
        /* [in] */ Float dtdx,
        /* [in] */ Float dsdy,
        /* [in] */ Float dtdy);

    CARAPI_(void) Prelayout();

    /**
     * Is this window visible?  It is not visible if there is no
     * surface, or we are in the process of running an exit animation
     * that will remove the surface.
     */
    CARAPI IsVisibleLw(
        /* [out] */ Boolean* isVisible);

    /**
     * Like {@link #isVisibleLw}, but also counts a window that is currently
     * "hidden" behind the keyguard as visible.  This allows us to apply
     * things like window flags that impact the keyguard.
     */
    CARAPI IsVisibleOrBehindKeyguardLw(
        /* [out] */ Boolean* result);

    /**
     * Is this window visible, ignoring its app token?  It is not visible
     * if there is no surface, or we are in the process of running an exit animation
     * that will remove the surface.
     */
    CARAPI_(Boolean) IsWinVisibleLw();

    /**
     * The same as isVisible(), but follows the current hidden state of
     * the associated app token, not the pending requested hidden state.
     */
    CARAPI_(Boolean) IsVisibleNow();

    /**
     * Can this window possibly be a drag/drop target?  The test here is
     * a combination of the above "visible now" with the check that the
     * Input Manager uses when discarding windows from input consideration.
     */
    CARAPI_(Boolean) IsPotentialDragTarget();

    /**
     * Same as isVisible(), but we also count it as visible between the
     * call to IWindowSession.add() and the first relayout().
     */
    CARAPI_(Boolean) IsVisibleOrAdding();

    /**
     * Is this window currently on-screen?  It is on-screen either if it
     * is visible or it is currently running an animation before no longer
     * being visible.
     */
    CARAPI_(Boolean) IsOnScreen();

    /**
     * Like isOnScreen(), but we don't return true if the window is part
     * of a transition that has not yet been started.
     */
    CARAPI_(Boolean) IsReadyForDisplay();

    /**
     * Like isReadyForDisplay(), but ignores any force hiding of the window due
     * to the keyguard.
     */
    CARAPI_(Boolean) IsReadyForDisplayIgnoringKeyguard();

    /**
     * Return true if this window or its app token is currently animating.
     */
    CARAPI IsDisplayedLw(
        /* [out] */ Boolean* isDisplayed);

    /**
     * Return true if this window (or a window it is attached to, but not
     * considering its app token) is currently animating.
     */
    CARAPI IsAnimatingLw(
        /* [out] */ Boolean* isAnimating);

    /**
     * Is this window considered to be gone for purposes of layout?
     */
    CARAPI IsGoneForLayoutLw(
        /* [out] */ Boolean* isGone);

    /**
     * Returns true if the window has a surface that it has drawn a
     * complete UI in to.
     */
    CARAPI_(Boolean) IsDrawFinishedLw();

    /**
     * Returns true if the window has a surface that it has drawn a
     * complete UI in to.
     */
    CARAPI_(Boolean) IsDrawnLw();

    /**
     * Return true if the window is opaque and fully drawn.  This indicates
     * it may obscure windows behind it.
     */
    CARAPI_(Boolean) IsOpaqueDrawn();

    /**
     * Return whether this window is wanting to have a translation
     * animation applied to it for an in-progress move.  (Only makes
     * sense to call from performLayoutAndPlaceSurfacesLockedInner().)
     */
    CARAPI_(Boolean) ShouldAnimateMove();

    CARAPI_(Boolean) IsFullscreen(
        /* [in] */ Int32 screenWidth,
        /* [in] */ Int32 screenHeight);

    CARAPI_(Boolean) IsConfigChanged();

    CARAPI_(void) RemoveLocked();

    CARAPI_(void) SetInputChannel(
        /* [in] */ IInputChannel* inputChannel);

    CARAPI_(void) DisposeInputChannel();

    /**
     * @return true if this window desires key events.
     */
    CARAPI_(Boolean) CanReceiveKeys();

    /**
     * Returns true if this window has been shown on screen at some time in
     * the past.  Must be called with the window manager lock held.
     */
    CARAPI HasDrawnLw(
        /* [out] */ Boolean* drawn);

    /**
     * Can be called to undo the effect of {@link #hideLw}, allowing a
     * window to be shown as long as the window manager and client would
     * also like it to be shown.  Must be called with the window manager
     * lock held.
     * Returns true if {@link #hideLw} was last called for the window.
     */
    CARAPI ShowLw(
        /* [in] */ Boolean doAnimation,
        /* [out] */ Boolean* canShown);

    CARAPI_(Boolean) ShowLw(
        /* [in] */ Boolean doAnimation,
        /* [in] */ Boolean requestAnim);

    /**
     * Can be called by the policy to force a window to be hidden,
     * regardless of whether the client or window manager would like
     * it shown.  Must be called with the window manager lock held.
     * Returns true if {@link #showLw} was last called for the window.
     */
    CARAPI HideLw(
        /* [in] */ Boolean doAnimation,
        /* [out] */ Boolean* canHidden);

    CARAPI_(Boolean) HideLw(
        /* [in] */ Boolean doAnimation,
        /* [in] */ Boolean requestAnim);

    CARAPI_(void) SetAppOpVisibilityLw(
        /* [in] */ Boolean state);

    /**
     * Check whether the process hosting this window is currently alive.
     */
    CARAPI IsAlive(
        /* [out] */ Boolean* isAlive);

    CARAPI_(Boolean) IsClosing();

    /**
     * Check if window is on {@link Display#DEFAULT_DISPLAY}.
     * @return true if window is on default display.
     */
    CARAPI IsDefaultDisplay(
        /* [out] */ Boolean* result);

    CARAPI_(void) SetShowToOwnerOnlyLocked(
        /* [in] */ Boolean showToOwnerOnly);

    CARAPI_(Boolean) IsHiddenFromUserLocked();

    CARAPI_(void) GetTouchableRegion(
        /* [in] */ IRegion* outRegion);

    CARAPI_(AutoPtr<WindowList>) GetWindowList();

    /**
     * Report a focus change.  Must be called with no locks held, and consistently
     * from the same serialized thread (such as dispatched from a handler).
     */
    CARAPI_(void) ReportFocusChangedSerialized(
        /* [in] */ Boolean focused,
        /* [in] */ Boolean inTouchMode);

    CARAPI_(void) ReportResized();

    CARAPI_(void) RegisterFocusObserver(
        /* [in] */ IIWindowFocusObserver* observer);

    CARAPI_(void) UnregisterFocusObserver(
        /* [in] */ IIWindowFocusObserver* observer);

    CARAPI_(Boolean) IsFocused();

    // void dump(PrintWriter pw, String prefix, boolean dumpAll);

    CARAPI_(String) MakeInputChannelName();

    CARAPI SetConfiguration(
        /* [in] */ IConfiguration* config);

    CARAPI ToString(
        /* [out]*/ String* str);

private:
    CARAPI_(void) GetStackBounds(
        /* [in] */ TaskStack* stack,
        /* [in] */ IRect* bounds);

    static CARAPI_(void) ApplyInsets(
        /* [in] */ IRegion* outRegion,
        /* [in] */ IRect* frame,
        /* [in] */ IRect* inset);

public:
    static const String TAG;

public:
    AutoPtr<CWindowManagerService> mService;
    AutoPtr<IWindowManagerPolicy> mPolicy;
    AutoPtr<IContext> mContext;
    AutoPtr<CSession> mSession;
    AutoPtr<IIWindow> mClient;
    Int32 mAppOp;
    // UserId and appId of the owner. Don't display windows of non-current user.
    Int32 mOwnerUid;
    AutoPtr<IIWindowId> mWindowId;
    AutoPtr<WindowToken> mToken;
    AutoPtr<WindowToken> mRootToken;
    AppWindowToken* mAppToken;
    AutoPtr<AppWindowToken> mTargetAppToken;

    // mAttrs.flags is tested in animation without being locked. If the bits tested are ever
    // modified they will need to be locked.
    AutoPtr<IWindowManagerLayoutParams> mAttrs;
    AutoPtr<DeathRecipient> mDeathRecipient;
    AutoPtr<WindowState> mAttachedWindow;
    AutoPtr<WindowList> mChildWindows;
    Int32 mBaseLayer;
    Int32 mSubLayer;
    Boolean mLayoutAttached;
    Boolean mIsImWindow;
    Boolean mIsWallpaper;
    Boolean mIsFloatingLayer;
    Int32 mSeq;
    Boolean mEnforceSizeCompat;
    Int32 mViewVisibility;
    Int32 mSystemUiVisibility;
    Boolean mPolicyVisibility;
    Boolean mPolicyVisibilityAfterAnim;
    Boolean mAppOpVisibility;
    Boolean mAppFreezing;
    Boolean mAttachedHidden;    // is our parent window hidden?
    Boolean mWallpaperVisible;  // for wallpaper, what was last vis report?
    AutoPtr<IRemoteCallbackList> mFocusCallbacks;

    /**
     * The window size that was requested by the application.  These are in
     * the application's coordinate space (without compatibility scale applied).
     */
    Int32 mRequestedWidth;
    Int32 mRequestedHeight;
    Int32 mLastRequestedWidth;
    Int32 mLastRequestedHeight;

    Int32 mLayer;
    Boolean mHaveFrame;
    Boolean mObscured;
    Boolean mTurnOnScreen;

    Int32 mLayoutSeq;

    AutoPtr<IConfiguration> mConfiguration;
    // Sticky answer to isConfigChanged(), remains true until new Configuration is assigned.
    // Used only on {@link #TYPE_KEYGUARD}.
    Boolean mConfigHasChanged;

    /**
     * Actual frame shown on-screen (may be modified by animation).  These
     * are in the screen's coordinate space (WITH the compatibility scale
     * applied).
     */
    AutoPtr<IRectF> mShownFrame;

    /**
     * Insets that determine the actually visible area.  These are in the application's
     * coordinate space (without compatibility scale applied).
     */
    AutoPtr<IRect> mVisibleInsets;
    AutoPtr<IRect> mLastVisibleInsets;
    Boolean mVisibleInsetsChanged;

    /**
     * Insets that are covered by system windows (such as the status bar) and
     * transient docking windows (such as the IME).  These are in the application's
     * coordinate space (without compatibility scale applied).
     */
    AutoPtr<IRect> mContentInsets;
    AutoPtr<IRect> mLastContentInsets;
    Boolean mContentInsetsChanged;

    /**
     * Insets that determine the area covered by the display overscan region.  These are in the
     * application's coordinate space (without compatibility scale applied).
     */
    AutoPtr<IRect> mOverscanInsets;
    AutoPtr<IRect> mLastOverscanInsets;
    Boolean mOverscanInsetsChanged;

    /**
     * Insets that determine the area covered by the stable system windows.  These are in the
     * application's coordinate space (without compatibility scale applied).
     */
    AutoPtr<IRect> mStableInsets;
    AutoPtr<IRect> mLastStableInsets;
    Boolean mStableInsetsChanged;

    /**
     * Set to true if we are waiting for this window to receive its
     * given internal insets before laying out other windows based on it.
     */
    Boolean mGivenInsetsPending;

    /**
     * These are the content insets that were given during layout for
     * this window, to be applied to windows behind it.
     */
    AutoPtr<IRect> mGivenContentInsets;

    /**
     * These are the visible insets that were given during layout for
     * this window, to be applied to windows behind it.
     */
    AutoPtr<IRect> mGivenVisibleInsets;

    /**
     * This is the given touchable area relative to the window frame, or null if none.
     */
    AutoPtr<IRegion> mGivenTouchableRegion;

    /**
     * Flag indicating whether the touchable region should be adjusted by
     * the visible insets; if false the area outside the visible insets is
     * NOT touchable, so we must use those to adjust the frame during hit
     * tests.
     */
    Int32 mTouchableInsets;

    /**
     * This is rectangle of the window's surface that is not covered by
     * system decorations.
     */
    AutoPtr<IRect> mSystemDecorRect;
    AutoPtr<IRect> mLastSystemDecorRect;

    // Current transformation being applied.
    Float mGlobalScale;
    Float mInvGlobalScale;
    Float mHScale;
    Float mVScale;
    Float mLastHScale;
    Float mLastVScale;
    AutoPtr<IMatrix> mTmpMatrix;

    // "Real" frame that the application sees, in display coordinate space.
    AutoPtr<IRect> mFrame;
    AutoPtr<IRect> mLastFrame;
    // Frame that is scaled to the application's coordinate space when in
    // screen size compatibility mode.
    AutoPtr<IRect> mCompatFrame;

    AutoPtr<IRect> mContainingFrame;
    AutoPtr<IRect> mDisplayFrame;
    AutoPtr<IRect> mOverscanFrame;
    AutoPtr<IRect> mContentFrame;
    AutoPtr<IRect> mParentFrame;
    AutoPtr<IRect> mVisibleFrame;
    AutoPtr<IRect> mDecorFrame;
    AutoPtr<IRect> mStableFrame;

    Boolean mContentChanged;

    // If a window showing a wallpaper: the requested offset for the
    // wallpaper; if a wallpaper window: the currently applied offset.
    Float mWallpaperX;
    Float mWallpaperY;

    // If a window showing a wallpaper: what fraction of the offset
    // range corresponds to a full virtual screen.
    Float mWallpaperXStep;
    Float mWallpaperYStep;

    // If a window showing a wallpaper: a raw pixel offset to forcibly apply
    // to its window; if a wallpaper window: not used.
    Int32 mWallpaperDisplayOffsetX;
    Int32 mWallpaperDisplayOffsetY;

    // Wallpaper windows: pixels offset based on above variables.
    Int32 mXOffset;
    Int32 mYOffset;

    /**
     * This is set after IWindowSession.relayout() has been called at
     * least once for the window.  It allows us to detect the situation
     * where we don't yet have a surface, but should have one soon, so
     * we can give the window focus before waiting for the relayout.
     */
    Boolean mRelayoutCalled;

    /**
     * If the application has called relayout() with changes that can
     * impact its window's size, we need to perform a layout pass on it
     * even if it is not currently visible for layout.  This is set
     * when in that case until the layout is done.
     */
    Boolean mLayoutNeeded;

    /** Currently running an exit animation? */
    Boolean mExiting;

    /** Currently on the mDestroySurface list? */
    Boolean mDestroying;

    /** Completely remove from window manager after exit animation? */
    Boolean mRemoveOnExit;

    /**
     * Set when the orientation is changing and this window has not yet
     * been updated for the new orientation.
     */
    Boolean mOrientationChanging;

    /**
     * How long we last kept the screen frozen.
     */
    Int32 mLastFreezeDuration;

    // Is this window now (or just being) removed?
    Boolean mRemoved;

    // Temp for keeping track of windows that have been removed when
    // rebuilding window list.
    Boolean mRebuilding;

    // InpAutoPtr<ut channel and input window handle used by the input dispatcher.
    AutoPtr<IInputWindowHandle> mInputWindowHandle;
    AutoPtr<IInputChannel> mInputChannel;

    // Used to improve performance of toString()
    String mStringNameCache;
    AutoPtr<ICharSequence> mLastTitle;
    Boolean mWasExiting;

    AutoPtr<WindowStateAnimator> mWinAnimator;

    Boolean mHasSurface;

    Boolean mNotOnAppsDisplay;

    AutoPtr<DisplayContent>  mDisplayContent;

    /** When true this window is at the top of the screen and should be layed out to extend under
     * the status bar */
    Boolean mUnderStatusBar;

private:
    /** When true this window can be displayed on screens owther than mOwnerUid's */
    Boolean mShowToOwnerOnly;
};

} // Wm
} // Server
} // Droid
} // Elastos

#define To_WindowState(obj) \
    ((WindowState*)IWindowState::Probe(obj))

template <>
struct Conversion<Elastos::Droid::Server::Wm::WindowState*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

#endif //__ELASTOS_DROID_SERVER_WM_WINDOWSTATE_H__
