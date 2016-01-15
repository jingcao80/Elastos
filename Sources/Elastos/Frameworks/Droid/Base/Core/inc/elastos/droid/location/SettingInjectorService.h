
#ifndef __ELASTOS_DROID_LOCATION_SETTINGINJECTORSERVICE_H__
#define __ELASTOS_DROID_LOCATION_SETTINGINJECTORSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include "elastos/droid/app/Service.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Location {

class SettingInjectorService
    : public Elastos::Droid::App::Service
    , public ISettingInjectorService
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructor.
     *
     * @param name used to identify your subclass in log messages
     */
    SettingInjectorService(
        /* [in] */ const String& name);

    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

    // @Override
    CARAPI OnStart(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 startId);

    // @Override
    CARAPI OnStartCommand(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 startId,
        /* [out] */ Int32* result);

private:
    CARAPI OnHandleIntent(
        /* [in] */ IIntent* intent);

    /**
     * Send the enabled values back to the caller via the messenger encoded in the
     * intent.
     */
    CARAPI SendStatus(
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean enabled);

protected:
    /**
     * This method is no longer called, because status values are no longer shown for any injected
     * setting.
     *
     * @return ignored
     *
     * @deprecated not called any more
     */
    // @Deprecated
    virtual CARAPI OnGetSummary(
        /* [out] */ String* result) = 0;

    /**
     * Returns the {@link android.preference.Preference#isEnabled()} value. Should not perform
     * unpredictably-long operations such as network access--see the running-time comments in the
     * class-level javadoc.
     * <p/>
     * Note that to prevent churn in the settings list, there is no support for dynamically choosing
     * to hide a setting. Instead you should have this method return false, which will disable the
     * setting and its link to your setting activity. One reason why you might choose to do this is
     * if {@link android.provider.Settings.Secure#LOCATION_MODE} is {@link
     * android.provider.Settings.Secure#LOCATION_MODE_OFF}.
     * <p/>
     * It is possible that the user may click on the setting before this method returns, so your
     * settings activity must handle the case where it is invoked even though the setting is
     * disabled. The simplest approach may be to simply call {@link android.app.Activity#finish()}
     * when disabled.
     *
     * @return the {@link android.preference.Preference#isEnabled()} value
     */
    virtual CARAPI OnGetEnabled(
        /* [out] */ Boolean* result) = 0;

private:
    const static String TAG;

    const String mName;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_SETTINGINJECTORSERVICE_H__
