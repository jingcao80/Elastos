
#ifndef __ELASTOS_DROID_VIEW_CWINDOWMANAGERLAYOUTPARAMS_H__
#define __ELASTOS_DROID_VIEW_CWINDOWMANAGERLAYOUTPARAMS_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_View_CWindowManagerLayoutParams.h"
#include "elastos/droid/view/ViewGroup.h"

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CWindowManagerLayoutParams)
    , public ViewGroup::LayoutParams
    , public IWindowManagerLayoutParams
    , public IParcelable
{
public:
    /**
    * Given a particular set of window manager flags, determine whether
    * such a window may be a target for an input method when it has
    * focus.  In particular, this checks the
    * {@link #FLAG_NOT_FOCUSABLE} and {@link #FLAG_ALT_FOCUSABLE_IM}
    * flags and returns true if the combination of the two corresponds
    * to a window that needs to be behind the input method so that the
    * user can type into it.
    *
    * @param flags The current window manager flags.
    *
    * @return Returns true if such a window should be behind/interact
    * with an input method, false if not.
    */
    static Boolean MayUseInputMethod(
        /* [in] */ Int32 flags);

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWindowManagerLayoutParams();

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    CARAPI GetTitle(
        /* [out] */ ICharSequence** title);

    CARAPI CopyFrom(
        /* [in] */ IWindowManagerLayoutParams* source,
        /* [out] */ Int32* changes);

    CARAPI Scale(
        /* [in] */ Float scale);

    CARAPI Backup();

    CARAPI Restore();

    CARAPI GetX(
        /* [out] */ Int32* x);

    CARAPI SetX(
        /* [in] */ Int32 x);

    CARAPI GetY(
        /* [out] */ Int32* y);

    CARAPI SetY(
        /* [in] */ Int32 y);

    CARAPI GetType(
        /* [out] */ Int32* type);

    CARAPI SetType(
        /* [in] */ Int32 type);

    CARAPI GetMemoryType(
        /* [out] */ Int32* type);

    CARAPI SetMemoryType(
        /* [in] */ Int32 type);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    CARAPI GetPrivateFlags(
        /* [out] */ Int32* privateFlags);

    CARAPI SetPrivateFlags(
        /* [in] */ Int32 privateFlags);

    CARAPI SetSoftInputMode(
        /* [in] */ Int32 mode);

    CARAPI GetSoftInputMode(
        /* [out] */ Int32* mode);

    CARAPI GetGravity(
        /* [out] */ Int32* gravity);

    CARAPI SetGravity(
        /* [in] */ Int32 gravity);

    CARAPI GetHorizontalMargin(
        /* [out] */ Float* horizontalMargin);

    CARAPI SetHorizontalMargin(
        /* [in] */ Float horizontalMargin);

    CARAPI GetVerticalMargin(
        /* [out] */ Float* verticalMargin);

    CARAPI SetVerticalMargin(
        /* [in] */ Float verticalMargin);

    CARAPI GetSurfaceInsets(
        /* [out] */ IRect** surfaceInsets);

    CARAPI SetSurfaceInsets(
        /* [in] */ IRect* surfaceInsets);

    CARAPI GetFormat(
        /* [out] */ Int32* format);

    CARAPI SetFormat(
        /* [in] */ Int32 format);

    CARAPI GetWindowAnimations(
        /* [out] */ Int32* animations);

    CARAPI SetWindowAnimations(
        /* [in] */ Int32 animations);

    CARAPI GetAlpha(
        /* [out] */ Float* alpha);

    CARAPI SetAlpha(
        /* [in] */ Float alpha);

    CARAPI GetDimAmount(
        /* [out] */ Float* dimAmount);

    CARAPI SetDimAmount(
        /* [in] */ Float dimAmount);

    CARAPI GetScreenBrightness(
        /* [out] */ Float* brightness);

    CARAPI SetScreenBrightness(
        /* [in] */ Float brightness);

    CARAPI GetButtonBrightness(
        /* [out] */ Float* brightness);

    CARAPI SetButtonBrightness(
        /* [in] */ Float brightness);

    CARAPI GetToken(
        /* [out] */ IBinder** token);

    CARAPI SetToken(
        /* [in] */ IBinder* token);

    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI SetPackageName(
        /* [in] */ const String& packageName);

    CARAPI GetScreenOrientation(
        /* [out] */ Int32* orientation);

    CARAPI SetScreenOrientation(
        /* [in] */ Int32 orientation);

    CARAPI GetPreferredRefreshRate(
        /* [out] */ Float* rate);

    CARAPI SetPreferredRefreshRate(
        /* [in] */ Float rate);

    CARAPI GetSystemUiVisibility(
        /* [out] */ Int32* systemUiVisibility);

    CARAPI SetSystemUiVisibility(
        /* [in] */ Int32 systemUiVisibility);

    CARAPI GetSubtreeSystemUiVisibility(
        /* [out] */ Int32* subtreeSystemUiVisibility);

    CARAPI SetSubtreeSystemUiVisibility(
        /* [in] */ Int32 subtreeSystemUiVisibility);

    CARAPI GetHasSystemUiListeners(
        /* [out] */ Boolean* hasSystemUiListeners);

    CARAPI SetHasSystemUiListeners(
        /* [in] */ Boolean hasSystemUiListeners);

    CARAPI GetInputFeatures(
        /* [out] */ Int32* inputFeatures);

    CARAPI SetInputFeatures(
        /* [in] */ Int32 inputFeatures);

    CARAPI GetUserActivityTimeout(
        /* [out] */ Int32* userActivityTimeout);

    CARAPI SetUserActivityTimeout(
        /* [in] */ Int32 userActivityTimeout);

    CARAPI GetBlurMaskAlphaThreshold(
    /* [out] */ Float* blurMaskAlphaThreshold);

    CARAPI SetBlurMaskAlphaThreshold(
    /* [in] */ Float blurMaskAlphaThreshold);

    CARAPI GetRotationAnimation(
        /* [out] */ Int32* rotationAnimation);

    CARAPI SetRotationAnimation(
        /* [in] */ Int32 rotationAnimation);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel *source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel *dest);

    CARAPI ToString(
        /* [out] */ String* description);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 type);

    CARAPI constructor(
        /* [in] */ Int32 type,
        /* [in] */ Int32 flags);

    CARAPI constructor(
        /* [in] */ Int32 type,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 format);

    CARAPI constructor(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 type,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 format);

    CARAPI constructor(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 xpos,
        /* [in] */ Int32 ypos,
        /* [in] */ Int32 type,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 format);

    // CARAPI constructor(
    //     /* [in] */ IParcel *in);

public:
    /**
     * X position for this window.  With the default gravity it is ignored.
     * When using {@link Gravity#LEFT} or {@link Gravity#RIGHT} it provides
     * an offset from the given edge.
     */
    Int32 mX;

    /**
     * Y position for this window.  With the default gravity it is ignored.
     * When using {@link Gravity#TOP} or {@link Gravity#BOTTOM} it provides
     * an offset from the given edge.
     */
    Int32 mY;

    /**
     * Indicates how much of the extra space will be allocated horizontally
     * to the view associated with these LayoutParams. Specify 0 if the view
     * should not be stretched. Otherwise the extra pixels will be pro-rated
     * among all views whose weight is greater than 0.
     */
    Float mHorizontalWeight;

    /**
     * Indicates how much of the extra space will be allocated vertically
     * to the view associated with these LayoutParams. Specify 0 if the view
     * should not be stretched. Otherwise the extra pixels will be pro-rated
     * among all views whose weight is greater than 0.
     */
    Float mVerticalWeight;

    /**
     * The general type of window.  There are three main classes of
     * window types:
     * <ul>
     * <li> <strong>Application windows</strong> (ranging from
     * {@link #FIRST_APPLICATION_WINDOW} to
     * {@link #LAST_APPLICATION_WINDOW}) are normal top-level application
     * windows.  For these types of windows, the {@link #token} must be
     * set to the token of the activity they are a part of (this will
     * normally be done for you if {@link #token} is null).
     * <li> <strong>Sub-windows</strong> (ranging from
     * {@link #FIRST_SUB_WINDOW} to
     * {@link #LAST_SUB_WINDOW}) are associated with another top-level
     * window.  For these types of windows, the {@link #token} must be
     * the token of the window it is attached to.
     * <li> <strong>System windows</strong> (ranging from
     * {@link #FIRST_SYSTEM_WINDOW} to
     * {@link #LAST_SYSTEM_WINDOW}) are special types of windows for
     * use by the system for specific purposes.  They should not normally
     * be used by applications, and a special permission is required
     * to use them.
     */
    Int32 mType;

    /**
     * @deprecated this is ignored
     */
    //@Deprecated
    Int32 mMemoryType;

    Int32 mFlags;

    /**
     * Control flags that are private to the platform.
     * @hide
     */
    Int32 mPrivateFlags;

    /**
     * Desired operating mode for any soft input area.
     */
    Int32 mSoftInputMode;

    /**
     * Placement of window within the screen as per {@link Gravity}
     */
    Int32 mGravity;

    /**
     * The horizontal margin, as a percentage of the container's width,
     * between the container and the widget.
     */
    Float mHorizontalMargin;

    /**
     * The vertical margin, as a percentage of the container's height,
     * between the container and the widget.
     */
    Float mVerticalMargin;

    /**
     * Positive insets between the drawing surface and window content.
     *
     * @hide
     */
    AutoPtr<IRect> mSurfaceInsets;

    /**
     * The desired bitmap format.  May be one of the constants in
     * {@link android.graphics.PixelFormat}.  Default is OPAQUE.
     */
    Int32 mFormat;

    /**
     * A style resource defining the animations to use for this window.
     * This must be a system resource; it can not be an application resource
     * because the window manager does not have access to applications.
     */
    Int32 mWindowAnimations;

    /**
     * An alpha value to apply to this entire window.
     * An alpha of 1.0 means fully opaque and 0.0 means fully transparent
     */
    Float mAlpha;

    /**
     * When {@link #FLAG_DIM_BEHIND} is set, this is the amount of dimming
     * to apply.  Range is from 1.0 for completely opaque to 0.0 for no
     * dim.
     */
    Float mDimAmount;

    /**
     * This can be used to override the user's preferred brightness of
     * the screen.  A value of less than 0, the default, means to use the
     * preferred screen brightness.  0 to 1 adjusts the brightness from
     * dark to full bright.
     * It's real type is float.
     */
    Float mScreenBrightness;

    /**
     * This can be used to override the standard behavior of the button and
     * keyboard backlights.  A value of less than 0, the default, means to
     * use the standard backlight behavior.  0 to 1 adjusts the brightness
     * from dark to full bright.
     * It's real type is float.
     */
    Float mButtonBrightness;

    /**
     * Define the exit and entry animations used on this window when the device is rotated.
     * This only has an affect if the incoming and outgoing topmost
     * opaque windows have the #FLAG_FULLSCREEN bit set and are not covered
     * by other windows. All other situations default to the
     * {@link #ROTATION_ANIMATION_ROTATE} behavior.
     *
     * @see #ROTATION_ANIMATION_ROTATE
     * @see #ROTATION_ANIMATION_CROSSFADE
     * @see #ROTATION_ANIMATION_JUMPCUT
     */
    Int32 mRotationAnimation;

    /**
     * Identifier for this window.  This will usually be filled in for
     * you.
     */
    AutoPtr<IBinder> mToken;

    /**
     * Name of the package owning this window.
     */
    String mPackageName;

    /**
     * Specific orientation value for a window.
     * May be any of the same values allowed
     * for {@link android.content.pm.ActivityInfo#screenOrientation}.
     * If not set, a default value of
     * {@link android.content.pm.ActivityInfo#SCREEN_ORIENTATION_UNSPECIFIED}
     * will be used.
     */
    Int32 mScreenOrientation;

    /**
     * The preferred refresh rate for the window.
     *
     * This must be one of the supported refresh rates obtained for the display(s) the window
     * is on.
     *
     * @see Display#getSupportedRefreshRates()
     */
    Float mPreferredRefreshRate;

    /**
     * Control the visibility of the status bar.
     *
     * @see View#STATUS_BAR_VISIBLE
     * @see View#STATUS_BAR_HIDDEN
     */
    Int32 mSystemUiVisibility;

    /**
     * @hide
     * The ui visibility as requested by the views in this hierarchy.
     * the combined value should be systemUiVisibility | subtreeSystemUiVisibility.
     */
    Int32 mSubtreeSystemUiVisibility;

    /**
     * Get callbacks about the system ui visibility changing.
     *
     * TODO: Maybe there should be a bitfield of optional callbacks that we need.
     *
     * @hide
     */
    Boolean mHasSystemUiListeners;

    /**
     * Control special features of the input subsystem.
     *
     * @see #INPUT_FEATURE_DISABLE_TOUCH_PAD_GESTURES
     * @see #INPUT_FEATURE_NO_INPUT_CHANNEL
     * @see #INPUT_FEATURE_DISABLE_USER_ACTIVITY
     * @hide
     */
    Int32 mInputFeatures;

    /**
     * Sets the number of milliseconds before the user activity timeout occurs
     * when this window has focus.  A value of -1 uses the standard timeout.
     * A value of 0 uses the minimum support display timeout.
     * <p>
     * This property can only be used to reduce the user specified display timeout;
     * it can never make the timeout longer than it normally would be.
     * </p><p>
     * Should only be used by the keyguard and phone app.
     * </p>
     *
     * @hide
     */
    Int64 mUserActivityTimeout;

    /**
     * Threshold value that blur masking layer uses to determine whether
     * to use or discard the blurred color.
     * Value should be between 0.0 and 1.0
     * @hide
     */
    Float mBlurMaskAlphaThreshold;

private:
    // internal buffer to backup/restore parameters under compatibility mode.
    AutoPtr<ArrayOf<Int32> > mCompatibilityParamsBackup;

    AutoPtr<ICharSequence> mTitle;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CWINDOWMANAGERLAYOUTPARAMS_H__
