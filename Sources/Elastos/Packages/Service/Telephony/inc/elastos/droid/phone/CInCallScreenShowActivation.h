#ifndef  __ELASTOS_DROID_PHONE_CINCALLSCREENSHOWACTIVATION_H__
#define  __ELASTOS_DROID_PHONE_CINCALLSCREENSHOWACTIVATION_H__

#include "_Elastos_Droid_Phone_CInCallScreenShowActivation.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Invisible activity that handles the com.android.phone.PERFORM_CDMA_PROVISIONING intent.
 * This activity is protected by the android.permission.PERFORM_CDMA_PROVISIONING permission.
 *
 * We handle the PERFORM_CDMA_PROVISIONING action by launching an OTASP
 * call via one of the OtaUtils helper methods: startInteractiveOtasp() on
 * regular phones, or startNonInteractiveOtasp() on data-only devices.
 *
 * TODO: The class name InCallScreenShowActivation is misleading, since
 * this activity is totally unrelated to the InCallScreen (which
 * implements the in-call UI.)  Let's eventually rename this to something
 * like CdmaProvisioningLauncher or CdmaProvisioningHandler...
 */
CarClass(CInCallScreenShowActivation)
    , public Activity
    , public IInCallScreenShowActivation
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CARAPI constructor();

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

private:
    /**
     * On devices that provide a phone initialization wizard (such as Google Setup Wizard),
     * the wizard displays it's own activation UI. The Hfa activation started by this class
     * will show a UI or not depending on the status of the setup wizard. If the setup wizard
     * is running, do not show a UI, otherwise show our own UI since setup wizard will not.
     *
     * The method checks two properties:
     * 1. Does the device require a setup wizard (ro.setupwizard.mode == (REQUIRED|OPTIONAL))
     * 2. Is device_provisioned set to non-zero--a property that setup wizard sets at completion.
     * @return true if wizard is running, false otherwise.
     */
    CARAPI_(Boolean) IsWizardRunning(
        /* [in] */ IContext* context);

    /**
     * Starts the HFA provisioning process by bringing up the HFA Activity.
     */
    CARAPI_(void) StartHfa();

private:
    static const String LOG_TAG = "InCallScreenShowActivation";
    static const Boolean DBG = (PhoneGlobals.DBG_LEVEL >= 1) && (SystemProperties.getInt("ro.debuggable", 0) == 1);
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CINCALLSCREENSHOWACTIVATION_H__