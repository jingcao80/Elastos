
#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/utility/UserIcons.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_IN;
using Elastos::Droid::Os::IUserHandle;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

const Int32 UserIcons::USER_ICON_COLORS[8] = {
    // R::color::user_icon_1,
    // R::color::user_icon_2,
    // R::color::user_icon_3,
    // R::color::user_icon_4,
    // R::color::user_icon_5,
    // R::color::user_icon_6,
    // R::color::user_icon_7,
    // R::color::user_icon_8
};

ECode UserIcons::ConvertToBitmap(
    /* [in] */ IDrawable* icon,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap)
    if (icon == NULL) {
        *bitmap = NULL;
        return NOERROR;
    }
    Int32 width, height;
    icon->GetIntrinsicWidth(&width);
    icon->GetIntrinsicHeight(&height);
    FAIL_RETURN(CBitmap::CreateBitmap(width, height, BitmapConfig_ARGB_8888, bitmap))
    AutoPtr<ICanvas> canvas;
    CCanvas::New(*bitmap, (ICanvas**)&canvas);
    return icon->Draw(canvas);
}

ECode UserIcons::GetDefaultUserIcon(
    /* [in] */ Int32 userId,
    /* [in] */ Boolean light,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    Int32 colorResId = light ? R::color::user_icon_default_white : R::color::user_icon_default_gray;
    if (userId != IUserHandle::USER_NULL) {
        // Return colored icon instead
        colorResId = USER_ICON_COLORS[userId % 8/*USER_ICON_COLORS.length*/];
    }

    AutoPtr<IDrawable> d;
    CResources::GetSystem()->GetDrawable(R::drawable::ic_account_circle, (IDrawable**)&d);
    AutoPtr<IDrawable> icon;
    d->Mutate((IDrawable**)&icon);
    Int32 color;
    CResources::GetSystem()->GetColor(colorResId, &color);
    icon->SetColorFilter(color, PorterDuffMode_SRC_IN);
    Int32 width, height;
    icon->GetIntrinsicWidth(&width);
    icon->GetIntrinsicHeight(&height);
    icon->SetBounds(0, 0, width, height);
    *drawable = icon;
    REFCOUNT_ADD(*drawable)
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
