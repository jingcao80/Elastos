#include "elastos/droid/systemui/statusbar/policy/Prefs.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {


const String Prefs::SHARED_PREFS_NAME = String("status_bar");

// a boolean
const String Prefs::DO_NOT_DISTURB_PREF = String("do_not_disturb");
const Boolean Prefs::DO_NOT_DISTURB_DEFAULT = FALSE;

const String Prefs::SHOWN_COMPAT_MODE_HELP = String("shown_compat_mode_help");
const String Prefs::SHOWN_QUICK_SETTINGS_HELP = String("shown_quick_settings_help");

AutoPtr<ISharedPreferences> Prefs::Read(
    /* [in] */ IContext* context)
{
    AutoPtr<ISharedPreferences> sp;
    context->GetSharedPreferences(
        Prefs::SHARED_PREFS_NAME,
        IContext::MODE_PRIVATE,
        (ISharedPreferences**)&sp);
    return sp;
}

AutoPtr<ISharedPreferencesEditor> Prefs::Edit(
    /* [in] */ IContext* context)
{
    AutoPtr<ISharedPreferencesEditor> editor;
    AutoPtr<ISharedPreferences> sp;
    context->GetSharedPreferences(
        Prefs::SHARED_PREFS_NAME,
        IContext::MODE_PRIVATE,
        (ISharedPreferences**)&sp);
    if (sp)
        sp->Edit((ISharedPreferencesEditor**)&editor);
    return editor;
}


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
