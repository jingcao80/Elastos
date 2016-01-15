
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/webkit/native/base/ApiCompatibilityUtils.h"
#include "elastos/droid/os/Build.h"

using Elastos::Droid::Os::Build;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::IViewTreeObserver;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Base {

const String ApiCompatibilityUtils::TAG("ApiCompatibilityUtils");

ApiCompatibilityUtils::ApiCompatibilityUtils()
{
}

/**
 * Returns true if view's layout direction is right-to-left.
 *
 * @param view the View whose layout is being considered
 */
Boolean ApiCompatibilityUtils::IsLayoutRtl(
    /* [in] */ IView* view)
{
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        Int32 direction;
        view->GetLayoutDirection(&direction);
        return direction == IView::LAYOUT_DIRECTION_RTL;
    }
    else {
        // All layouts are LTR before JB MR1.
        return FALSE;
    }
}

/**
 * @see Configuration#getLayoutDirection()
 */
Int32 ApiCompatibilityUtils::GetLayoutDirection(
    /* [in] */ IConfiguration* configuration)
{
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        Int32 dir;
        configuration->GetLayoutDirection(&dir);
        return dir;
    }
    else {
        // All layouts are LTR before JB MR1.
        return IView::LAYOUT_DIRECTION_LTR;
    }
}

/**
 * @return True if the running version of the Android supports printing.
 */
Boolean ApiCompatibilityUtils::IsPrintingSupported()
{
    return Build::VERSION::SDK_INT >= Build::VERSION_CODES::KITKAT;
}

/**
 * @return True if the running version of the Android supports HTML clipboard.
 */
Boolean ApiCompatibilityUtils::IsHTMLClipboardSupported()
{
    return Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN;
}

/**
 * @see android.view.View#setLayoutDirection(int)
 */
void ApiCompatibilityUtils::SetLayoutDirection(
    /* [in] */ IView* view,
    /* [in] */ Int32 layoutDirection)
{
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        view->SetLayoutDirection(layoutDirection);
    }
    else {
        // Do nothing. RTL layouts aren't supported before JB MR1.
    }
}

/**
 * @see android.view.View#setTextDirection(int)
 */
void ApiCompatibilityUtils::SetTextAlignment(
    /* [in] */ IView* view,
    /* [in] */ Int32 textAlignment)
{
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        view->SetTextAlignment(textAlignment);
    }
    else {
        // Do nothing. RTL text isn't supported before JB MR1.
    }
}

/**
 * @see android.view.ViewGroup.MarginLayoutParams#setMarginEnd(int)
 */
void ApiCompatibilityUtils::SetMarginEnd(
    /* [in] */ IViewGroupMarginLayoutParams* layoutParams,
    /* [in] */ Int32 end)
{
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        layoutParams->SetMarginEnd(end);
    }
    else {
        layoutParams->SetRightMargin(end);
    }
}

/**
 * @see android.view.ViewGroup.MarginLayoutParams#getMarginEnd()
 */
Int32 ApiCompatibilityUtils::GetMarginEnd(
    /* [in] */ IViewGroupMarginLayoutParams* layoutParams)
{
    Int32 margin;
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        layoutParams->GetMarginEnd(&margin);
        return margin;
    }
    else {
        layoutParams->GetRightMargin(&margin);
        return margin;
    }
}

/**
 * @see android.view.ViewGroup.MarginLayoutParams#setMarginStart(int)
 */
void ApiCompatibilityUtils::SetMarginStart(
    /* [in] */ IViewGroupMarginLayoutParams* layoutParams,
    /* [in] */ Int32 start)
{
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        layoutParams->SetMarginStart(start);
    }
    else {
        layoutParams->SetLeftMargin(start);
    }
}

/**
 * @see android.view.ViewGroup.MarginLayoutParams#getMarginStart()
 */
Int32 ApiCompatibilityUtils::GetMarginStart(
    /* [in] */ IViewGroupMarginLayoutParams* layoutParams)
{
    Int32 margin;
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        layoutParams->GetMarginStart(&margin);
        return margin;
    }
    else {
        layoutParams->GetLeftMargin(&margin);
        return margin;
    }
}

/**
 * @see android.view.View#setPaddingRelative(int, int, int, int)
 */
void ApiCompatibilityUtils::SetPaddingRelative(
    /* [in] */ IView* view,
    /* [in] */ Int32 start,
    /* [in] */ Int32 top,
    /* [in] */ Int32 end,
    /* [in] */ Int32 bottom)
{
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        view->SetPaddingRelative(start, top, end, bottom);
    }
    else {
        // Before JB MR1, all layouts are left-to-right, so start == left, etc.
        view->SetPadding(start, top, end, bottom);
    }
}

/**
 * @see android.view.View#getPaddingStart()
 */
Int32 ApiCompatibilityUtils::GetPaddingStart(
    /* [in] */ IView* view)
{
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        Int32 start;
        view->GetPaddingStart(&start);
        return start;
    }
    else {
        // Before JB MR1, all layouts are left-to-right, so start == left.
        Int32 left;
        view->GetPaddingLeft(&left);
        return left;
    }
}

/**
 * @see android.view.View#getPaddingEnd()
 */
Int32 ApiCompatibilityUtils::GetPaddingEnd(
    /* [in] */ IView* view)
{
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        Int32 end;
        view->GetPaddingEnd(&end);
        return end;
    }
    else {
        // Before JB MR1, all layouts are left-to-right, so end == right.
        Int32 right;
        view->GetPaddingRight(&right);
        return right;
    }
}

/**
 * @see android.widget.TextView#setCompoundDrawablesRelative(Drawable, Drawable, Drawable,
 *      Drawable)
 */
void ApiCompatibilityUtils::SetCompoundDrawablesRelative(
    /* [in] */ ITextView* textView,
    /* [in] */ IDrawable* start,
    /* [in] */ IDrawable* top,
    /* [in] */ IDrawable* end,
    /* [in] */ IDrawable* bottom)
{
    if (Build::VERSION::SDK_INT == Build::VERSION_CODES::JELLY_BEAN_MR1) {
        // On JB MR1, due to a platform bug, setCompoundDrawablesRelative() is a no-op if the
        // view has ever been measured. As a workaround, use setCompoundDrawables() directly.
        // See: http://crbug.com/368196 and http://crbug.com/361709
        AutoPtr<IView> view = IView::Probe(textView);
        Boolean isRtl = IsLayoutRtl(view);
        textView->SetCompoundDrawables(isRtl ? end : start, top, isRtl ? start : end, bottom);
    }
    else if (Build::VERSION::SDK_INT > Build::VERSION_CODES::JELLY_BEAN_MR1) {
        textView->SetCompoundDrawablesRelative(start, top, end, bottom);
    }
    else {
        textView->SetCompoundDrawables(start, top, end, bottom);
    }
}

/**
 * @see android.widget.TextView#setCompoundDrawablesRelativeWithIntrinsicBounds(Drawable,
 *      Drawable, Drawable, Drawable)
 */
void ApiCompatibilityUtils::SetCompoundDrawablesRelativeWithIntrinsicBounds(
    /* [in] */ ITextView* textView,
    /* [in] */ IDrawable* start,
    /* [in] */ IDrawable* top,
    /* [in] */ IDrawable* end,
    /* [in] */ IDrawable* bottom)
{
    if (Build::VERSION::SDK_INT == Build::VERSION_CODES::JELLY_BEAN_MR1) {
        // Work around the platform bug described in setCompoundDrawablesRelative() above.
        AutoPtr<IView> view = IView::Probe(textView);
        Boolean isRtl = IsLayoutRtl(view);
        textView->SetCompoundDrawablesWithIntrinsicBounds(isRtl ? end : start, top,
                isRtl ? start : end, bottom);
    }
    else if (Build::VERSION::SDK_INT > Build::VERSION_CODES::JELLY_BEAN_MR1) {
        textView->SetCompoundDrawablesRelativeWithIntrinsicBounds(start, top, end, bottom);
    }
    else {
        textView->SetCompoundDrawablesWithIntrinsicBounds(start, top, end, bottom);
    }
}

/**
 * @see android.widget.TextView#setCompoundDrawablesRelativeWithIntrinsicBounds(int, int, int,
 *      int)
 */
void ApiCompatibilityUtils::SetCompoundDrawablesRelativeWithIntrinsicBounds(
    /* [in] */ ITextView* textView,
    /* [in] */ Int32 start,
    /* [in] */ Int32 top,
    /* [in] */ Int32 end,
    /* [in] */ Int32 bottom)
{
    if (Build::VERSION::SDK_INT == Build::VERSION_CODES::JELLY_BEAN_MR1) {
        // Work around the platform bug described in setCompoundDrawablesRelative() above.
        AutoPtr<IView> view = IView::Probe(textView);
        Boolean isRtl = IsLayoutRtl(view);
        textView->SetCompoundDrawablesWithIntrinsicBounds(isRtl ? end : start, top,
                isRtl ? start : end, bottom);
    }
    else if (Build::VERSION::SDK_INT > Build::VERSION_CODES::JELLY_BEAN_MR1) {
        textView->SetCompoundDrawablesRelativeWithIntrinsicBounds(start, top, end, bottom);
    }
    else {
        textView->SetCompoundDrawablesWithIntrinsicBounds(start, top, end, bottom);
    }
}

/**
 * @see android.view.View#postInvalidateOnAnimation()
 */
void ApiCompatibilityUtils::PostInvalidateOnAnimation(
    /* [in] */ IView* view)
{
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN) {
        view->PostInvalidateOnAnimation();
    }
    else {
        view->PostInvalidate();
    }
}

/**
 * @see android.widget.RemoteViews#setContentDescription(int, CharSequence)
 */
void ApiCompatibilityUtils::SetContentDescriptionForRemoteView(
    /* [in] */ IRemoteViews* remoteViews,
    /* [in] */ Int32 viewId,
    /* [in] */ ICharSequence* contentDescription)
{
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::ICE_CREAM_SANDWICH_MR1) {
        remoteViews->SetContentDescription(viewId, contentDescription);
    }
    else {
        // setContentDescription() is unavailable in earlier versions.
    }
}

// These methods have a new name, and the old name is deprecated.

/**
 * @see android.view.View#setBackground(Drawable)
 */
void ApiCompatibilityUtils::SetBackgroundForView(
    /* [in] */ IView* view,
    /* [in] */ IDrawable* drawable)
{
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN) {
        view->SetBackground(drawable);
    }
    else {
        view->SetBackgroundDrawable(drawable);
    }
}

/**
 * @see android.view.ViewTreeObserver#removeOnGlobalLayoutListener()
 */
void ApiCompatibilityUtils::RemoveOnGlobalLayoutListener(
    /* [in] */ IView* view,
    /* [in] */ IOnGlobalLayoutListener* listener)
{
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN) {
        AutoPtr<IViewTreeObserver> obsever;
        view->GetViewTreeObserver((IViewTreeObserver**)&obsever);
        obsever->RemoveOnGlobalLayoutListener(listener);
    }
    else {
        AutoPtr<IViewTreeObserver> obsever;
        view->GetViewTreeObserver((IViewTreeObserver**)&obsever);
        obsever->RemoveGlobalOnLayoutListener(listener);
    }
}

/**
 * @see android.widget.ImageView#setImageAlpha(int)
 */
void ApiCompatibilityUtils::SetImageAlpha(
    /* [in] */ IImageView* iv,
    /* [in] */ Int32 alpha)
{
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN) {
        iv->SetImageAlpha(alpha);
    }
    else {
        iv->SetAlpha(alpha);
    }
}

/**
 * @see android.app.PendingIntent#getCreatorPackage()
 */
String ApiCompatibilityUtils::GetCreatorPackage(
    /* [in] */ IPendingIntent* intent)
{
    String pkgName;
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        intent->GetCreatorPackage(&pkgName);
        return pkgName;
    }
    else {
        intent->GetTargetPackage(&pkgName);
        return pkgName;
    }
}

Boolean ApiCompatibilityUtils::DatePickerRequiresAccept()
{
    // TODO(miguelg) use the final code for the L
    // https://crbug.com/399198
    return Build::VERSION::SDK_INT <  20; /* CUR_DEVELOPMENT */
}

} // namespace Base
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
