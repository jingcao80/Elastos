#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CCONDITIONPROVIDERSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CCONDITIONPROVIDERSETTINGS_H__

#include "_Elastos_Droid_Settings_Notification_CConditionProviderSettings.h"
#include "elastos/droid/settings/notification/ManagedServiceSettings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CarClass(CConditionProviderSettings)
    , public ManagedServiceSettings
{
public:
    CAR_OBJECT_DECL()

    TO_STRING_IMPL("CConditionProviderSettings")

    static CARAPI_(Int32) GetProviderCount(
        /* [in] */ IPackageManager* pm);

    static CARAPI_(Int32) GetEnabledProviderCount(
        /* [in] */ IContext* context);

protected:
    //@Override
    CARAPI_(AutoPtr<ManagedServiceSettings::Config>) GetConfig();

private:
    static CARAPI_(AutoPtr<ManagedServiceSettings::Config>) GetConditionProviderConfig();

private:
    static const String TAG;
    static const  AutoPtr<ManagedServiceSettings::Config> CONFIG;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CCONDITIONPROVIDERSETTINGS_H__
