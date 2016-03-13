
#include "elastos/droid/systemui/qs/GlobalSetting.h"
#include <elastos/droid/provider/Settings.h>

using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IListenable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

CAR_INTERFACE_IMPL_2(GlobalSetting, ContentObserver, IGlobalSetting, IListenable);
GlobalSetting::GlobalSetting(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ const String& settingName)
{
    ContentObserver::constructor(handler);
    mContext = context;
    mSettingName = settingName;
}

ECode GlobalSetting::GetValue(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    return Elastos::Droid::Provider::Settings::Global::GetInt32(cr, mSettingName, 0, result);
}

ECode GlobalSetting::SetValue(
    /* [in] */ Int32 value)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Boolean tmp = FALSE;
    Elastos::Droid::Provider::Settings::Global::PutInt32(cr, mSettingName, value, &tmp);
    return NOERROR;
}

ECode GlobalSetting::SetListening(
    /* [in] */ Boolean listening)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    if (listening) {
        AutoPtr<IUri> uri;
        Elastos::Droid::Provider::Settings::Global::GetUriFor(mSettingName, (IUri**)&uri);
        cr->RegisterContentObserver(uri, FALSE, this);
    }
    else {
        cr->UnregisterContentObserver(this);
    }
    return NOERROR;
}

ECode GlobalSetting::OnChange(
    /* [in] */ Boolean selfChange)
{
    Int32 v = 0;
    GetValue(&v);
    HandleValueChanged(v);
    return NOERROR;
}

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
