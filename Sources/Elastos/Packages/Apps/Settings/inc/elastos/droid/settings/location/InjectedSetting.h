#ifndef __ELASTOS_DROID_SETTINGS_LOCATION_INJECTEDSETTING_H__
#define __ELASTOS_DROID_SETTINGS_LOCATION_INJECTEDSETTING_H__

#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

/**
 * Specifies a setting that is being injected into Settings &gt; Location &gt; Location services.
 *
 * @see android.location.SettingInjectorService
 */
// @Immutable
class InjectedSetting
    : public Object
    , public IInjectedSetting
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns a new instance, or NULL.
     */
    static CARAPI_(AutoPtr<InjectedSetting>) NewInstance(
        /* [in] */ const String& packageName,
        /* [in] */ const String& className,
        /* [in] */ const String& title,
        /* [in] */ Int32 iconId,
        /* [in] */ const String& settingsActivity);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Returns the intent to start the {@link #className} service.
     */
    virtual CARAPI_(AutoPtr<IIntent>) GetServiceIntent();

    //@Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetHashCode(
        /* [out] */ Int32* code);

private:
    InjectedSetting(
        /* [in] */ const String& packageName,
        /* [in] */ const String& className,
        /* [in] */ const String& title,
        /* [in] */ Int32 iconId,
        /* [in] */ const String& settingsActivity);

public:
    /**
     * Package for the subclass of {@link android.location.SettingInjectorService} and for the
     * settings activity.
     */
    String mPackageName;

    /**
     * Class name for the subclass of {@link android.location.SettingInjectorService} that
     * specifies dynamic values for the location setting.
     */
    String mClassName;

    /**
     * The {@link android.preference.Preference#GetTitle()} value.
     */
    String mTitle;

    /**
     * The {@link android.preference.Preference#GetIcon()} value.
     */
    Int32 mIconId;

    /**
     * The activity to launch to allow the user to modify the settings value. Assumed to be in the
     * {@link #packageName} package.
     */
    String mSettingsActivity;
};

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_LOCATION_INJECTEDSETTING_H__