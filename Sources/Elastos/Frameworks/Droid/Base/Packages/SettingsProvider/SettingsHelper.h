#ifndef _SETTINGSHELPER_H__
#define _SETTINGSHELPER_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioManager;
namespace Elastos {
namespace Droid {
namespace Packages {
namespace SettingsProvider {

class SettingsHelper
    : public ElRefBase
{
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
        /* [in] */ String& name,
        /* [in] */ String& value);

private:

    CARAPI_(Boolean) IsAlreadyConfiguredCriticalAccessibilitySetting(
        /* [in] */ String& name);

    CARAPI_(void) SetAutoRestore(
        /* [in] */ Boolean enabled);

    CARAPI_(void) SetGpsLocation(
        /* [in] */ String& value);

    CARAPI_(void) SetSoundEffects(
        /* [in] */ Boolean enable);

    CARAPI_(void) SetBrightness(
        /* [in] */ Int32 brightness);

    CARAPI_(AutoPtr<ArrayOf<Byte> >) GetLocaleData();

    /**
    * Sets the locale specified. Input data is the equivalent of "ll_cc".getBytes(), where
    * "ll" is the language code and "cc" is the country code.
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

    friend class CSettingsBackupAgent;
private:
    AutoPtr<IContext> mContext;
    AutoPtr<IAudioManager> mAudioManager;
};
} // namespace SettingsProvider
} // namespace Packages
} // namespace Droid
} // namespace Elastos
#endif
