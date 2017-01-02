//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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