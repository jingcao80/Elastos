#include "elastos/droid/systemui/statusbar/DoNotDisturb.h"
#include "elastos/droid/systemui/statusbar/policy/Prefs.h"

using Elastos::Droid::SystemUI::StatusBar::Policy::Prefs;
using Elastos::Droid::Content::EIID_ISharedPreferencesOnSharedPreferenceChangeListener;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_INTERFACE_IMPL(DoNotDisturb, ISharedPreferencesOnSharedPreferenceChangeListener)

DoNotDisturb::DoNotDisturb(
    /* [in] */ IContext* context)
    : mDoNotDisturb(FALSE)
{
    assert(context != NULL);
    mContext = context;
    AutoPtr<IInterface> tmpObj;
    mContext->GetSystemService(IContext::STATUS_BAR_SERVICE, (IInterface**)&tmpObj);
    mStatusBar = IStatusBarManager::Probe(tmpObj.Get());
    assert(mStatusBar != NULL);
    mPrefs = Prefs::Read(mContext);
    if (mPrefs != NULL) {
        mPrefs->RegisterOnSharedPreferenceChangeListener(this);
        mPrefs->GetBoolean(Prefs::DO_NOT_DISTURB_PREF, Prefs::DO_NOT_DISTURB_DEFAULT, &mDoNotDisturb);
    }

    UpdateDisableRecord();
}

ECode DoNotDisturb::OnSharedPreferenceChanged(
    /* [in] */ ISharedPreferences* prefs,
    /* [in] */ const String& key)
{
    assert(prefs != NULL);
    Boolean val;
    prefs->GetBoolean(Prefs::DO_NOT_DISTURB_PREF, Prefs::DO_NOT_DISTURB_DEFAULT, &val);
    if (val != mDoNotDisturb) {
        mDoNotDisturb = val;
        UpdateDisableRecord();
    }
    return NOERROR;
}

void DoNotDisturb::UpdateDisableRecord()
{
    Int32 disabled = IStatusBarManager::DISABLE_NOTIFICATION_ICONS
        | IStatusBarManager::DISABLE_NOTIFICATION_ALERTS
        | IStatusBarManager::DISABLE_NOTIFICATION_TICKER;
    if (mStatusBar != NULL) {
        mStatusBar->Disable(mDoNotDisturb ? disabled : 0);
    }
}


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
