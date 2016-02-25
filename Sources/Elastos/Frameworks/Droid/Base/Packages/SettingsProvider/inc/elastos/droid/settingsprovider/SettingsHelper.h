#ifndef __ELASTOS_DROID_SETTINGSPROVIDER_SETTINGSHELPER_H__
#define __ELASTOS_DROID_SETTINGSPROVIDER_SETTINGSHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioManager;

namespace Elastos {
namespace Droid {
namespace SettingsProvider {

class SettingsHelper
    : public Object
{
    friend class SettingsBackupAgent;
public:
    SettingsHelper(
        /* [in] */ IContext* context);

    /**
    * Sets the property via a call to the appropriate API, if any, and returns
    * whether or not the setting should be saved to the database as well.
    * @param name the name of the setting
    * @param value the string value of the setting
    * @return whether to continue with writing the value to the database. In
    * some cases the data will be written by the call to the appropriate API,
    * and in some cases the property value needs to be modified before setting.
    */
    CARAPI_(Boolean) RestoreValue(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI_(String) OnBackupValue(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

protected:
    CARAPI_(AutoPtr<ArrayOf<Byte> >) GetLocaleData();

    /**
     * Sets the locale specified. Input data is the byte representation of a
     * BCP-47 language tag. For backwards compatibility, strings of the form
     * {@code ll_CC} are also accepted, where {@code ll} is a two letter language
     * code and {@code CC} is a two letter country code.
     *
     * @param data the locale string in bytes.
     */
    CARAPI_(void) SetLocaleData(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 size);

    /**
    * Informs the audio service of changes to the settings so that
    * they can be re-read and applied.
    */
    CARAPI_(void) ApplyAudioSettings();

private:
    /**
     * Sets the ringtone of type specified by the name.
     *
     * @param name should be Settings.System.RINGTONE or Settings.System.NOTIFICATION_SOUND.
     * @param value can be a canonicalized uri or "_silent" to indicate a silent (null) ringtone.
     */
    CARAPI_(void) SetRingtone(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI_(String) GetCanonicalRingtoneValue(
        /* [in] */ const String& value);

    CARAPI_(Boolean) IsAlreadyConfiguredCriticalAccessibilitySetting(
        /* [in] */ const String& name);

    CARAPI_(void) SetAutoRestore(
        /* [in] */ Boolean enabled);

    CARAPI_(void) SetGpsLocation(
        /* [in] */ const String& value);

    CARAPI_(void) SetSoundEffects(
        /* [in] */ Boolean enable);

    CARAPI_(void) SetBrightness(
        /* [in] */ Int32 brightness);

private:
    static const String SILENT_RINGTONE;
    AutoPtr<IContext> mContext;
    AutoPtr<IAudioManager> mAudioManager;
};

} // namespace SettingsProvider
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGSPROVIDER_SETTINGSHELPER_H__
