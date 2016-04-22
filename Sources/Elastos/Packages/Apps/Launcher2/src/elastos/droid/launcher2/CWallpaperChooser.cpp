
#include "elastos/droid/launcher2/CWallpaperChooser.h"
#include "elastos/droid/launcher2/WallpaperChooserDialogFragment.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

using Elastos::Droid::Launcher2::IWallpaperChooserDialogFragment;
using Elastos::Droid::Launcher2::WallpaperChooserDialogFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IDialogFragment;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const String CWallpaperChooser::TAG("Launcher.WallpaperChooser");

CAR_OBJECT_IMPL(CWallpaperChooser);

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
        AutoPtr<IFragmentManager> _fmanager;
        GetFragmentManager((IFragmentManager**)&_fmanager);
        fragment->Show(_fmanager, String("dialog"));
    }

    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos
