
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_PREFS_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_PREFS_H__

#include <elastos/droid/ext/frameworkext.h>

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

private:
    Prefs(){};
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_PREFS_H__
