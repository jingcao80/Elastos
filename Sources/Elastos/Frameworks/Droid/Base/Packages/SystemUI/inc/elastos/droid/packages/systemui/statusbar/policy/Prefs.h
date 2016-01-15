
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_PREFS_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_PREFS_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class Prefs
{
public:
    static AutoPtr<ISharedPreferences> Read(
        /* [in] */ IContext* context);

    static AutoPtr<ISharedPreferencesEditor> Edit(
        /* [in] */ IContext* context);

public:
    static const String SHARED_PREFS_NAME;// = "status_bar";

    // a boolean
    static const String DO_NOT_DISTURB_PREF;// = "do_not_disturb";
    static const Boolean DO_NOT_DISTURB_DEFAULT;// = false;

    static const String SHOWN_COMPAT_MODE_HELP;// = "shown_compat_mode_help";
    static const String SHOWN_QUICK_SETTINGS_HELP;// = "shown_quick_settings_help";

private:
    Prefs();
    ~Prefs();
};


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_PREFS_H__
