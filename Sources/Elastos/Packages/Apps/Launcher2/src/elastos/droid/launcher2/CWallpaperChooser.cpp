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

#include "elastos/droid/launcher2/CWallpaperChooser.h"
#include "elastos/droid/launcher2/WallpaperChooserDialogFragment.h"
#include "Elastos.Droid.Service.h"
#include <elastos/utility/logging/Slogger.h>
#include "R.h"

using Elastos::Droid::Launcher2::IWallpaperChooserDialogFragment;
using Elastos::Droid::Launcher2::WallpaperChooserDialogFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const String CWallpaperChooser::TAG("Launcher.WallpaperChooser");

CAR_OBJECT_IMPL(CWallpaperChooser);

CWallpaperChooser::CWallpaperChooser()
{
}

ECode CWallpaperChooser::constructor()
{
    return Activity::constructor();
}

ECode CWallpaperChooser::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);
    SetContentView(Elastos::Droid::Launcher2::R::layout::wallpaper_chooser_base);

    AutoPtr<IFragmentManager> fmanager;
    GetFragmentManager((IFragmentManager**)&fmanager);
    AutoPtr<IFragment> fragmentView;
    fmanager->FindFragmentById(Elastos::Droid::Launcher2::R::id::wallpaper_chooser_fragment,
            (IFragment**)&fragmentView);

    // TODO: The following code is currently not exercised. Leaving it here in case it
    // needs to be revived again.
    if (fragmentView == NULL) {
        /* When the screen is XLarge, the fragment is not included in the layout, so show it
         * as a dialog
         */
        AutoPtr<IWallpaperChooserDialogFragment> wfragment;
        WallpaperChooserDialogFragment::NewInstance((IWallpaperChooserDialogFragment**)&wfragment);
        AutoPtr<IDialogFragment> fragment = IDialogFragment::Probe(wfragment);
        fragment->Show(fmanager, String("dialog"));
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos
