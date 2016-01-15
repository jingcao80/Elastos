
#include "elastos/droid/preference/CSeekBarDialogPreferenceHelper.h"
#include "elastos/droid/preference/SeekBarDialogPreference.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(CSeekBarDialogPreferenceHelper, Singleton, ISeekBarDialogPreferenceHelper)

CAR_SINGLETON_IMPL(CSeekBarDialogPreferenceHelper)

ECode CSeekBarDialogPreferenceHelper::GetSeekBar(
        /* [in] */ IView* dialogView,
        /* [out] */ ISeekBar** bar)
{
    return SeekBarDialogPreference::GetSeekBar(dialogView, bar);
}

} // namespace Preference
} // namespace Droid
} // namespace Elastos
