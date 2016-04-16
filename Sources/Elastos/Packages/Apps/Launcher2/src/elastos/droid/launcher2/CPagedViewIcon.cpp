
#include "elastos/droid/launcher2/CPagedViewIcon.h"
#include "elastos/droid/launcher2/ApplicationInfo.h"
#include "elastos/droid/launcher2/FastBitmapDrawable.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

using Elastos::Droid::App::CFastBitmapDrawable;
using Elastos::Droid::App::IFastBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CPagedViewIcon::MyRunnable::MyRunnable(
    /* [in] */ CPagedViewIcon* host)
    : mHost(host)
{
}

ECode CPagedViewIcon::MyRunnable::Run()
{
    return mHost->RefreshDrawableState();
}

const String CPagedViewIcon::TAG("PagedViewIcon");
const Float CPagedViewIcon::PRESS_ALPHA = 0.4f;

CAR_INTERFACE_IMPL(CPagedViewIcon, TextView, IPagedViewIcon);

CAR_OBJECT_IMPL(CPagedViewIcon);

CPagedViewIcon::CPagedViewIcon()
    : mLockDrawableState(FALSE)
{
}

ECode CPagedViewIcon::constructor()
{
    return NOERROR;
}

ECode CPagedViewIcon::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CPagedViewIcon::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CPagedViewIcon::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return TextView::constructor(context, attrs, defStyle);
}

ECode CPagedViewIcon::ApplyFromApplicationInfo(
    /* [in] */ IApplicationInfo* info,
    /* [in] */ Boolean scaleUp,
    /* [in] */ IPagedViewIconPressedCallback* cb)
{
    ApplicationInfo* _info = (ApplicationInfo*)info;
    mIcon = _info->mIconBitmap;
    mPressedCallback = cb;
    AutoPtr<FastBitmapDrawable> drawable = new FastBitmapDrawable();
    drawable->constructor(mIcon);
    SetCompoundDrawablesWithIntrinsicBounds(NULL, IDrawable::Probe(drawable), NULL, NULL);
    SetText(_info->mTitle);
    if (_info->mContentDescription != NULL) {
        SetContentDescription(_info->mContentDescription);
    }
    return SetTag(info);
}

ECode CPagedViewIcon::LockDrawableState()
{
    mLockDrawableState = TRUE;
    return NOERROR;
}

ECode CPagedViewIcon::ResetDrawableState()
{
    mLockDrawableState = FALSE;
    AutoPtr<MyRunnable> run = new MyRunnable(this);
    Boolean res;
    return Post(run, &res);
}

ECode CPagedViewIcon::DrawableStateChanged()
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
        SetAlpha(1.0f);
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos