#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_DONOTDISTURB_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_DONOTDISTURB_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesOnSharedPreferenceChangeListener;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

class DoNotDisturb
    : public ElRefBase
    , public ISharedPreferencesOnSharedPreferenceChangeListener
{
public:
    CAR_INTERFACE_DECL()

    DoNotDisturb(
        /* [in] */ IContext* context);

    CARAPI OnSharedPreferenceChanged(
        /* [in] */ ISharedPreferences* sharedPreferences,
        /* [in] */ const String& key);

private:
    void UpdateDisableRecord();

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IStatusBarManager> mStatusBar;
    AutoPtr<ISharedPreferences> mPrefs;
    Boolean mDoNotDisturb;
};


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_DONOTDISTURB_H__
