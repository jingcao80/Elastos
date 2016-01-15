
#ifndef __ELASTOS_DROID_WEBKIT_BASE_APICOMPATIBILITYUTILS_H__
#define __ELASTOS_DROID_WEBKIT_BASE_APICOMPATIBILITYUTILS_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
// import android.view.ViewTreeObserver;
using Elastos::Droid::View::IOnGlobalLayoutListener;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Base {

/**
 * Utility class to use new APIs that were added after ICS (API level 14).
 */
class ApiCompatibilityUtils
{
public:
    /**
     * Returns true if view's layout direction is right-to-left.
     *
     * @param view the View whose layout is being considered
     */
    static CARAPI_(Boolean) IsLayoutRtl(
        /* [in] */ IView* view);

    /**
     * @see Configuration#getLayoutDirection()
     */
    static CARAPI_(Int32) GetLayoutDirection(
        /* [in] */ IConfiguration* configuration);

    /**
     * @return True if the running version of the Android supports printing.
     */
    static CARAPI_(Boolean) IsPrintingSupported();

    /**
     * @return True if the running version of the Android supports HTML clipboard.
     */
    static CARAPI_(Boolean) IsHTMLClipboardSupported();

    /**
     * @see android.view.View#setLayoutDirection(int)
     */
    static CARAPI_(void) SetLayoutDirection(
        /* [in] */ IView* view,
        /* [in] */ Int32 layoutDirection);

    /**
     * @see android.view.View#setTextDirection(int)
     */
    static CARAPI_(void) SetTextAlignment(
        /* [in] */ IView* view,
        /* [in] */ Int32 textAlignment);

    /**
     * @see android.view.ViewGroup.MarginLayoutParams#setMarginEnd(int)
     */
    static CARAPI_(void) SetMarginEnd(
        /* [in] */ IViewGroupMarginLayoutParams* layoutParams,
        /* [in] */ Int32 end);

    /**
     * @see android.view.ViewGroup.MarginLayoutParams#getMarginEnd()
     */
    static CARAPI_(Int32) GetMarginEnd(
        /* [in] */ IViewGroupMarginLayoutParams* layoutParams);

    /**
     * @see android.view.ViewGroup.MarginLayoutParams#setMarginStart(int)
     */
    static CARAPI_(void) SetMarginStart(
        /* [in] */ IViewGroupMarginLayoutParams* layoutParams,
        /* [in] */ Int32 start);

    /**
     * @see android.view.ViewGroup.MarginLayoutParams#getMarginStart()
     */
    static CARAPI_(Int32) GetMarginStart(
        /* [in] */ IViewGroupMarginLayoutParams* layoutParams);

    /**
     * @see android.view.View#setPaddingRelative(int, int, int, int)
     */
    static CARAPI_(void) SetPaddingRelative(
        /* [in] */ IView* view,
        /* [in] */ Int32 start,
        /* [in] */ Int32 top,
        /* [in] */ Int32 end,
        /* [in] */ Int32 bottom);

    /**
     * @see android.view.View#getPaddingStart()
     */
    static CARAPI_(Int32) GetPaddingStart(
        /* [in] */ IView* view);

    /**
     * @see android.view.View#getPaddingEnd()
     */
    static CARAPI_(Int32) GetPaddingEnd(
        /* [in] */ IView* view);

    /**
     * @see android.widget.TextView#setCompoundDrawablesRelative(Drawable, Drawable, Drawable,
     *      Drawable)
     */
    static CARAPI_(void) SetCompoundDrawablesRelative(
        /* [in] */ ITextView* textView,
        /* [in] */ IDrawable* start,
        /* [in] */ IDrawable* top,
        /* [in] */ IDrawable* end,
        /* [in] */ IDrawable* bottom);

    /**
     * @see android.widget.TextView#setCompoundDrawablesRelativeWithIntrinsicBounds(Drawable,
     *      Drawable, Drawable, Drawable)
     */
    static CARAPI_(void) SetCompoundDrawablesRelativeWithIntrinsicBounds(
        /* [in] */ ITextView* textView,
        /* [in] */ IDrawable* start,
        /* [in] */ IDrawable* top,
        /* [in] */ IDrawable* end,
        /* [in] */ IDrawable* bottom);

    /**
     * @see android.widget.TextView#setCompoundDrawablesRelativeWithIntrinsicBounds(int, int, int,
     *      int)
     */
    static CARAPI_(void) SetCompoundDrawablesRelativeWithIntrinsicBounds(
        /* [in] */ ITextView* textView,
        /* [in] */ Int32 start,
        /* [in] */ Int32 top,
        /* [in] */ Int32 end,
        /* [in] */ Int32 bottom);

    /**
     * @see android.view.View#postInvalidateOnAnimation()
     */
    static CARAPI_(void) PostInvalidateOnAnimation(
        /* [in] */ IView* view);

    /**
     * @see android.widget.RemoteViews#setContentDescription(int, CharSequence)
     */
    static CARAPI_(void) SetContentDescriptionForRemoteView(
        /* [in] */ IRemoteViews* remoteViews,
        /* [in] */ Int32 viewId,
        /* [in] */ ICharSequence* contentDescription);

    // These methods have a new name, and the old name is deprecated.

    /**
     * @see android.view.View#setBackground(Drawable)
     */
    static CARAPI_(void) SetBackgroundForView(
        /* [in] */ IView* view,
        /* [in] */ IDrawable* drawable);

    /**
     * @see android.view.ViewTreeObserver#removeOnGlobalLayoutListener()
     */
    static CARAPI_(void) RemoveOnGlobalLayoutListener(
        /* [in] */ IView* view,
        /* [in] */ IOnGlobalLayoutListener* listener);

    /**
     * @see android.widget.ImageView#setImageAlpha(int)
     */
    static CARAPI_(void) SetImageAlpha(
        /* [in] */ IImageView* iv,
        /* [in] */ Int32 alpha);

    /**
     * @see android.app.PendingIntent#getCreatorPackage()
     */
    static CARAPI_(String) GetCreatorPackage(
        /* [in] */ IPendingIntent* intent);

    static CARAPI_(Boolean) DatePickerRequiresAccept();

private:
    ApiCompatibilityUtils();

    static const String TAG;
};

} // namespace Base
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_BASE_APICOMPATIBILITYUTILS_H__
