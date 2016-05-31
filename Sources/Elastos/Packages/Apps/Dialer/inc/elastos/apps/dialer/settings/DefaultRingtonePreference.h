#ifndef __ELASTOS_APPS_DIALER_SETTINGS_DEFAULTRINGTONEPREFERENCE_H__
#define __ELASTOS_APPS_DIALER_SETTINGS_DEFAULTRINGTONEPREFERENCE_H__

namespace Elastos{
namespace Apps{
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
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ AttributeSet attrs);

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
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_SETTINGS_DEFAULTRINGTONEPREFERENCE_H__
