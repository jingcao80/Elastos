
#ifndef __ELASTOS_DROID_SETTINGS_DASHBOARD_NOHOMEDIALOGFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_DASHBOARD_NOHOMEDIALOGFRAGMENT_H__

#include "elastos/droid/app/DialogFragment.h"

using Elastos::Droid::App::DialogFragment;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

class NoHomeDialogFragment
    : public DialogFragment
{
public:
    NoHomeDialogFragment();

    ~NoHomeDialogFragment();

    static CARAPI_(void) Show(
        /* [in] */ IActivity* parent);

    //@Override
    CARAPI OnCreateDialog(
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IDialog** dialog);
};

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DASHBOARD_NOHOMEDIALOGFRAGMENT_H__
