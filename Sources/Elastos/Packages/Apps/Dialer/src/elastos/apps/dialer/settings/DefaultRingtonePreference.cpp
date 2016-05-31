
#include "DefaultRingtonePreference.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Settings {

CAR_INTERFACE_IMPL(DefaultRingtonePreference, RingtonePreference, IDefaultRingtonePreference)

ECode DefaultRingtonePreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ AttributeSet attrs)
{
    return DefaultRingtonePreference::constructor(context, attrs);
}

ECode DefaultRingtonePreference::OnPrepareRingtonePickerIntent(
    /* [in] */ IIntent* ringtonePickerIntent)
{
    DefaultRingtonePreference::OnPrepareRingtonePickerIntent(ringtonePickerIntent);

    /*
     * Since this preference is for choosing the default ringtone, it
     * doesn't make sense to show a 'Default' item.
     */
    ringtonePickerIntent->PutExtra(IRingtoneManager::EXTRA_RINGTONE_SHOW_DEFAULT, FALSE);
    return NOERROR;
}

ECode DefaultRingtonePreference::OnSaveRingtone(
    /* [in] */ IUri* ringtoneUri)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Int32 type;
    GetRingtoneType(&type);
    AutoPtr<IRingtoneManagerHelper> helper;
    CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&helper);
    helper->SetActualDefaultRingtoneUri(context, type, ringtoneUri);
    return NOERROR;
}

ECode DefaultRingtonePreference::OnRestoreRingtone(
    /* [out] */ IUri** uri)
{
    VALUE_NOT_NULL(uri);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Int32 type;
    GetRingtoneType(&type);
    AutoPtr<IRingtoneManagerHelper> helper;
    CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&helper);
    return helper->GetActualDefaultRingtoneUri(context, type, uri);
}

} // Settings
} // Dialer
} // Apps
} // Elastos
