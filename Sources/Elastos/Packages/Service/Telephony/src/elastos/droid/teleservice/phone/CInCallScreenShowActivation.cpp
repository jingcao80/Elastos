//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/teleservice/phone/CInCallScreenShowActivation.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/teleservice/phone/PhoneUtils.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::ITelephonyCapabilities;
using Elastos::Droid::Internal::Telephony::CTelephonyCapabilities;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Core::StringBuilder;

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

const String CInCallScreenShowActivation::TAG("InCallScreenShowActivation");

static Boolean initDBG()
{
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    Int32 tmp;
    helper->GetInt32(String("ro.debuggable"), 0, &tmp);

    return (IPhoneGlobals::DBG_LEVEL >= 1) && (tmp == 1);
}

const Boolean CInCallScreenShowActivation::DBG = initDBG();

CAR_OBJECT_IMPL(CInCallScreenShowActivation)

ECode CInCallScreenShowActivation::constructor()
{
    return Activity::constructor();
}

ECode CInCallScreenShowActivation::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);

    StringBuilder sb;
    sb += "onCreate: intent = ";
    sb += TO_CSTR(intent);
    if (DBG) Logger::D(TAG, sb.ToString());

    AutoPtr<IBundle> extras;
    intent->GetExtras((IBundle**)&extras);
    if (DBG && (extras != NULL)) {
        StringBuilder sb;
        sb += "      - has extras: size = ";
        Int32 size;
        extras->GetSize(&size);
        sb += size;
        Logger::D(TAG, sb.ToString()); // forces an unparcel()

        StringBuilder sb2;
        sb2 += "      - extras = ";
        sb2 += TO_CSTR(extras);
        Logger::D(TAG, sb2.ToString());
    }

    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);
    AutoPtr<IPhone> phone = app->GetPhone();

    AutoPtr<ITelephonyCapabilities> helper;
    CTelephonyCapabilities::AcquireSingleton((ITelephonyCapabilities**)&helper);
    Boolean res;
    helper->SupportsOtasp(phone, &res);
    if (!res) {
        Logger::W(TAG, "CDMA Provisioning not supported on this device");
        SetResult(RESULT_CANCELED);
        return Finish();
    }

    String action;
    intent->GetAction(&action);
    if (action.Equals(IOtaUtils::ACTION_PERFORM_CDMA_PROVISIONING)) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        Boolean usesHfa;
        resources->GetBoolean(Elastos::Droid::TeleService::R::bool_::config_use_hfa_for_provisioning,
                &usesHfa);
        if (usesHfa) {
            Logger::I(TAG, "Starting Hfa from ACTION_PERFORM_CDMA_PROVISIONING");
            StartHfa();
            return Finish();
        }

        Boolean usesOtasp;
        resources->GetBoolean(Elastos::Droid::TeleService::R::bool_::config_use_otasp_for_provisioning,
                &usesOtasp);
        if (usesOtasp) {
            // On voice-capable devices, we perform CDMA provisioning in
            // "interactive" mode by directly launching the InCallScreen.
            // boolean interactiveMode = PhoneGlobals.sVoiceCapable;
            // TODO: Renable interactive mode for device provisioning.
            Boolean interactiveMode = FALSE;
            StringBuilder sb;
            sb += "ACTION_PERFORM_CDMA_PROVISIONING (interactiveMode = ";
            sb += interactiveMode;
            sb += ")...";
            Logger::I(TAG, sb.ToString());

            // Testing: this intent extra allows test apps manually
            // enable/disable "interactive mode", regardless of whether
            // the current device is voice-capable.  This is allowed only
            // in userdebug or eng builds.
            Boolean result;
            if (intent->HasExtra(IOtaUtils::EXTRA_OVERRIDE_INTERACTIVE_MODE, &result), result) {
                AutoPtr<ISystemProperties> helper;
                CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
                Int32 tmp;
                helper->GetInt32(String("ro.debuggable"), 0, &tmp);
                if (tmp == 1) {
                    intent->GetBooleanExtra(IOtaUtils::EXTRA_OVERRIDE_INTERACTIVE_MODE, FALSE, &interactiveMode);
                    StringBuilder sb;
                    sb += "==> MANUALLY OVERRIDING interactiveMode to ";
                    sb += interactiveMode;
                    Logger::D(TAG, sb.ToString());
                }
            }

            // We allow the caller to pass a PendingIntent (as the
            // EXTRA_NONINTERACTIVE_OTASP_RESULT_PENDING_INTENT extra)
            // which we'll later use to notify them when the OTASP call
            // fails or succeeds.
            //
            // Stash that away here, and we'll fire it off later in
            // OtaUtils.sendOtaspResult().
            AutoPtr<IParcelable> parcelable;
            intent->GetParcelableExtra(IOtaUtils::EXTRA_OTASP_RESULT_CODE_PENDING_INTENT,
                    (IParcelable**)&parcelable);
            app->mCdmaOtaScreenState->mOtaspResultCodePendingIntent =
                        IPendingIntent::Probe(parcelable);

            if (interactiveMode) {
                // On voice-capable devices, launch an OTASP call and arrange
                // for the in-call UI to come up.  (The InCallScreen will
                // notice that an OTASP call is active, and display the
                // special OTASP UI instead of the usual in-call controls.)

                if (DBG) Logger::D(TAG, "==> Starting interactive CDMA provisioning...");
                OtaUtils::StartInteractiveOtasp(this);

                // The result we set here is actually irrelevant, since the
                // InCallScreen's "interactive" OTASP sequence never actually
                // finish()es; it ends by directly launching the Home
                // activity.  So our caller won't actually ever get an
                // onActivityResult() call in this case.
                SetResult(IOtaUtils::RESULT_INTERACTIVE_OTASP_STARTED);
            }
            else {
                // On data-only devices, manually launch the OTASP call
                // *without* displaying any UI.  (Our caller, presumably
                // SetupWizardActivity, is responsible for displaying some
                // sort of progress UI.)

                if (DBG) Logger::D(TAG, "==> Starting non-interactive CDMA provisioning...");
                Int32 callStatus;
                OtaUtils::StartNonInteractiveOtasp(this, &callStatus);

                if (callStatus == PhoneUtils::CALL_STATUS_DIALED) {
                    if (DBG) {
                        StringBuilder sb;
                        sb += "  ==> successful result from startNonInteractiveOtasp(): ";
                        sb += callStatus;
                        Logger::D(TAG, sb.ToString());
                    }
                    SetResult(IOtaUtils::RESULT_NONINTERACTIVE_OTASP_STARTED);
                }
                else {
                    StringBuilder sb;
                    sb += "Failure code from startNonInteractiveOtasp(): ";
                    sb += callStatus;
                    Logger::W(TAG, sb.ToString());
                    SetResult(IOtaUtils::RESULT_NONINTERACTIVE_OTASP_FAILED);
                }
            }
        }
        else {
            Logger::I(TAG, "Skipping activation.");
        }
    }
    else {
        StringBuilder sb;
        sb += "Unexpected intent action: ";
        sb += TO_CSTR(intent);
        Logger::E(TAG, sb.ToString());
        SetResult(RESULT_CANCELED);
    }

    return Finish();
}

Boolean CInCallScreenShowActivation::IsWizardRunning(
    /* [in] */ IContext* context)
{
    AutoPtr<IIntent> intent;
    CIntent::New(String("android.intent.action.DEVICE_INITIALIZATION_WIZARD"), (IIntent**)&intent);

    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IResolveInfo> resolveInfo;
    packageManager->ResolveActivity(intent, IPackageManager::MATCH_DEFAULT_ONLY, (IResolveInfo**)&resolveInfo);


    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);

    AutoPtr<ISettingsGlobal> helper;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
    Int32 tmp;
    helper->GetInt32(contentResolver, ISettingsGlobal::DEVICE_PROVISIONED,
            0, &tmp);
    Boolean provisioned = (tmp != 0);

    AutoPtr<ISystemProperties> helper2;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper2);
    String mode;
    helper2->Get(String("ro.setupwizard.mode"), String("REQUIRED"), &mode);
    Boolean runningSetupWizard = mode.Equals("REQUIRED") || mode.Equals("OPTIONAL");
    if (DBG) {
        StringBuilder sb;
        sb += "resolvInfo = ";
        sb += TO_CSTR(resolveInfo);
        sb += ", provisioned = ";
        sb += provisioned;
        sb += ", runningSetupWizard = ";
        sb += runningSetupWizard;
        Logger::V(TAG, sb.ToString());
    }
    return resolveInfo != NULL && !provisioned && runningSetupWizard;
}

/**
 * Starts the HFA provisioning process by bringing up the HFA Activity.
 */
void CInCallScreenShowActivation::StartHfa()
{
    Boolean isWizardRunning = IsWizardRunning(this);
    // We always run our HFA logic if we're in setup wizard, but if we're outside of setup
    // wizard then we have to check a config to see if we should still run HFA.
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    Boolean res;
    resources->GetBoolean(Elastos::Droid::TeleService::R::bool_::config_allow_hfa_outside_of_setup_wizard,
            &res);
    if (isWizardRunning || res) {

        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);

        AutoPtr<IIntent> tmp;
        CIntent::New((IIntent**)&tmp);
        AutoPtr<IParcelable> parcelable;
        tmp->GetParcelableExtra(IOtaUtils::EXTRA_OTASP_RESULT_CODE_PENDING_INTENT,
                (IParcelable**)&parcelable);

        AutoPtr<IPendingIntent> otaResponseIntent = IPendingIntent::Probe(parcelable);

        Boolean showUi = !isWizardRunning;
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

        if (otaResponseIntent != NULL) {
            intent->PutExtra(IOtaUtils::EXTRA_OTASP_RESULT_CODE_PENDING_INTENT, IParcelable::Probe(otaResponseIntent));
        }

        Logger::V(TAG, "Starting hfa activation activity");
        if (showUi) {
            intent->SetClassName(this, String("CHfaActivity") /*HfaActivity.class.getName()*/);
            StartActivity(intent);
        }
        else {
            intent->SetClassName(this, String("CHfaService") /*HfaService.class.getName()*/);
            AutoPtr<IComponentName> name;
            StartService(intent, (IComponentName**)&name);
        }

    }
    SetResult(RESULT_OK);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos