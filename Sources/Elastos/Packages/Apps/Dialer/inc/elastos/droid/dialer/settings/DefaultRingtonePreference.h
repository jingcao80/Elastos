#ifndef __ELASTOS_DROID_DIALER_SETTINGS_DEFAULTRINGTONEPREFERENCE_H__
#define __ELASTOS_DROID_DIALER_SETTINGS_DEFAULTRINGTONEPREFERENCE_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/prefernece.RingtonePreference.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Preference::RingtonePreference;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Settings {

/**
 * RingtonePreference which doesn't show default ringtone setting.
 */
class DefaultRingtonePreference
    : public RingtonePreference
    , public IDefaultRingtonePreference
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    // @Override
    CARAPI OnPrepareRingtonePickerIntent(
        /* [in] */ IIntent* ringtonePickerIntent);

    // @Override
    CARAPI OnSaveRingtone(
        /* [in] */ IUri* ringtoneUri);

    // @Override
    CARAPI OnRestoreRingtone(
        /* [out] */ IUri** uri);
};

} // Settings
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_SETTINGS_DEFAULTRINGTONEPREFERENCE_H__
