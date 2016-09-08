#ifndef __ELASTOS_DROID_SETTINGS_CDEFAULTRINGTONEPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_CDEFAULTRINGTONEPREFERENCE_H__

#include "_Elastos_Droid_Settings_CDefaultRingtonePreference.h"
#include "elastos/droid/preference/RingtonePreference.h"

using Elastos::Droid::Preference::RingtonePreference;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CDefaultRingtonePreference)
    , public RingtonePreference
{
public:
    CAR_OBJECT_DECL()

    TO_STRING_IMPL("CDefaultRingtonePreference")

    CDefaultRingtonePreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    //@Override
    CARAPI OnPrepareRingtonePickerIntent(
        /* [in] */ IIntent* ringtonePickerIntent);

    //@Override
    CARAPI OnSaveRingtone(
        /* [in] */ IUri* ringtoneUri);

    //@Override
    CARAPI OnRestoreRingtone(
        /* [out] */ IUri** uri);

private:
    static const String TAG;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CDEFAULTRINGTONEPREFERENCE_H__
