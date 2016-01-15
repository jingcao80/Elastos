#include "elastos/droid/service/wallpaper/WallpaperSettingsActivity.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Wallpaper {

CAR_INTERFACE_IMPL(WallpaperSettingsActivity, PreferenceActivity, IWallpaperSettingsActivity)

WallpaperSettingsActivity::WallpaperSettingsActivity()
{
}

ECode WallpaperSettingsActivity::constructor()
{
    return NOERROR;
}

ECode WallpaperSettingsActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    if (FALSE) {
        AutoPtr<IResourcesTheme> theme;
        GetTheme((IResourcesTheme**)&theme);//IContext
        AutoPtr<IIntent> intent;
        GetIntent((IIntent**)&intent);//IActivity
        Boolean value;
        intent->GetBooleanExtra(EXTRA_PREVIEW_MODE, FALSE, &value);
        if (value) {
            theme->ApplyStyle(R::style::PreviewWallpaperSettings, TRUE);
        } else {
            theme->ApplyStyle(R::style::ActiveWallpaperSettings, TRUE);
        }
    }
    PreferenceActivity::OnCreate(icicle);
    return NOERROR;
}

} // namespace Wallpaper
} // namespace Service
} // namepsace Droid
} // namespace Elastos