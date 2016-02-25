
#include "elastos/droid/systemui/statusbar/policy/Prefs.h"
#include "Elastos.Droid.Content.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String Prefs::SHARED_PREFS_NAME = String("status_bar");
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
