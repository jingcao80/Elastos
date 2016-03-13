
#include "elastos/droid/launcher2/PagedViewIcon.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

PagedViewIcon::MyRunnable::MyRunnable(
    /* [in] */ PagedViewIcon* host)
    : mHost(host)
{
}

ECode PagedViewIcon::MyRunnable::Run()
{
    return mHost->RefreshDrawableState();
}

const String PagedViewIcon::TAG("PagedViewIcon");
const Float PagedViewIcon::PRESS_ALPHA = 0.4f;

CAR_INTERFACE_IMPL(PagedViewIcon, TextView, IPagedViewIcon);

PagedViewIcon::PagedViewIcon()
    : mLockDrawableState(FALSE)
{
}

ECode PagedViewIcon::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode PagedViewIcon::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode PagedViewIcon::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return TextView::constructor(context, attrs, defStyle);
}

ECode PagedViewIcon::ApplyFromApplicationInfo(
    /* [in] */ IApplicationInfo* info,
    /* [in] */ Boolean scaleUp,
    /* [in] */ IPagedViewIconPressedCallback* cb)
{
    ApplicationInfo* _info = (ApplicationInfo*)info;
    mIcon = _info->mIconBitmap;
    mPressedCallback = cb;
    AutoPtr<IFastBitmapDrawable> drawable;
    CFastBitmapDrawable::New(mIcon, (IFastBitmapDrawable**)&drawable);
    SetCompoundDrawablesWithIntrinsicBounds(NULL, drawable, NULL, NULL);
    SetText(_info->mTitle);
    if (_info->mContentDescription != NULL) {
        setContentDescription(_info->mContentDescription);
    }
    return SetTag(info);
}

ECode PagedViewIcon::LockDrawableState()
{
    mLockDrawableState = TRUE;
    return NOERROR;
}

ECode PagedViewIcon::ResetDrawableState()
{
    mLockDrawableState = FALSE;
    AutoPtr<MyRunnable> run = new MyRunnable(this);
    Boolean res;
    return Post(run, &res);
}

ECode PagedViewIcon::DrawableStateChanged()
{
    TextView::DrawableStateChanged();

    // We keep in the pressed state until resetDrawableState() is called to reset the press
    // feedback
    Boolean res;
    IsPressed(&res);
    if (res) {
        SetAlpha(PRESS_ALPHA);
        if (mPressedCallback != NULL) {
            mPressedCallback->IconPressed(this);
        }
    }
    else if (!mLockDrawableState) {
        SetAlpha(1f);
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos