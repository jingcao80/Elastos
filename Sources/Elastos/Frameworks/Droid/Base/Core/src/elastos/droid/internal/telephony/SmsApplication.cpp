
#include "elastos/droid/Manifest.h"
#include "elastos/droid/os/Process.h"
#include <elastos/utility/logging/Logger.h>


using Elastos::Droid::Manifest;
using Elastos::Droid::App::IAppGlobals;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IDebug;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Internal::IR;
using Elastos::Droid::Internal::Content::IPackageMonitor;

using Elastos::Utility::IArrays;
using Elastos::Utility::ICollection;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

SmsApplication::SmsPackageMonitor::SmsPackageMonitor(
    /* [in] */ IContext* context)
    : mContext(context)
{
}

//@Override
ECode SmsApplication::SmsPackageMonitor::OnPackageDisappeared(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 reason)
{
    return OnPackageChanged(packageName);
}

//@Override
ECode SmsApplication::SmsPackageMonitor::OnPackageAppeared(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 reason)
{
    return OnPackageChanged(packageName);
}

//@Override
ECode SmsApplication::SmsPackageMonitor::OnPackageModified(
    /* [in] */ const String& packageName)
{
    return OnPackageChanged(packageName);
}

void SmsApplication::SmsPackageMonitor::OnPackageChanged(
    /* [in] */ const String& packageName)
{
    AutoPtr<IPackageManager> packageManager;
    mContext->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IContext> userContext;
    Int32 userId = GetSendingUserId();
    if (userId != UserHandle::USER_OWNER) {
        String pn;
        mContext->GetPackageName(&pn);
        AutoPtr<IUserHandle> uh;
        CUserHandle::New(userId, ((IUserHandle**)&uh);

        ECode ec = mContext->CreatePackageContextAsUser(pn, 0, uh, (IContext**)&userContext);
        if (FAILED(ec)) {//Catch (NameNotFoundException nnfe) {
            if (DEBUG_MULTIUSER) {
                Logger::W(LOG_TAG, "Unable to create package context for user %d", userId);
            }
        }
    }
    else {
        userContext = mContext;
    }
    // Ensure this component is still configured as the preferred activity
    AutoPtr<IComponentName> componentName = GetDefaultSendToApplication(userContext, TRUE);
    if (componentName != NULL) {
        ConfigurePreferredActivity(packageManager, componentName, userId);
    }
}

/**
 * Class for managing the primary application that we will deliver SMS/MMS messages to
 *
 * {@hide}
 */
CAR_INTERFACE_IMPL(SmsApplication, Singleton, ISmsApplication)

const String LOG_TAG = "SmsApplication";
const String PHONE_PACKAGE_NAME = "com.android.phone";
const String BLUETOOTH_PACKAGE_NAME = "com.android.bluetooth";
const String MMS_SERVICE_PACKAGE_NAME = "com.android.mms.service";
const String SCHEME_SMS = "sms";
const String SCHEME_SMSTO = "smsto";
const String SCHEME_MMS = "mms";
const String SCHEME_MMSTO = "mmsto";
const Boolean DEBUG_MULTIUSER = FALSE;

CAR_INTERFACE_IMPL(SmsApplication::SmsApplicationData, Object, ISmsApplicationData)

ECode SmsApplication::SmsApplicationData::IsComplete(
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mSmsReceiverClass != NULL && mMmsReceiverClass != NULL
                    && mRespondViaMessageClass != NULL && mSendToClass != NULL);
    return NOERROR;
}

ECode SmsApplication::SmsApplicationData::constructor(
    /* [in] */ const String& applicationName,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    mApplicationName = applicationName;
    mPackageName = packageName;
    mUid = uid;
    return NOERROR;
}

Int32 SmsApplication::GetIncomingUserId(
    /* [in] */ IContext* context)
{
    Int32 contextUserId;
    context->GetUserId(&contextUserId);
    Int32 callingUid = Binder::GetCallingUid();
    //TODO
//    if (DEBUG_MULTIUSER) {
//        Logger::I(LOG_TAG, "getIncomingUserHandle caller=" + callingUid + ", myuid="
//                + Process::MyUid() + "\n\t" + Debug->GetCallers(4));
//    }
    if (UserHandle::GetAppId(callingUid) < IProcess::FIRST_APPLICATION_UID) {
        return contextUserId;
    } else {
        return UserHandle::GetUserId(callingUid);
    }
}

ECode SmsApplication::GetApplicationCollection(
    /* [in] */ IContext* context,
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 userId = GetIncomingUserId(context);
    Int64 token = Binder::ClearCallingIdentity();

    AutoPtr<ICollection> col = GetApplicationCollectionInternal(context, userId);
    *result = col;
    REFCOUNT_ADD(*result)

    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

AutoPtr<ICollection> SmsApplication::GetApplicationCollectionInternal(
    /* [in] */ IIContext* context,
    /* [in] */ Int32 userId)
{
    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);

    // Get the list of apps registered for SMS
    AutoPtr<IIntent> intent
    CIntent::New(IIntents::SMS_DELIVER_ACTION, (IIntent**)&intent);
    AutoPtr<IList> smsReceivers
    packageManager->QueryBroadcastReceivers(intent, 0, userId, (IList**)&smsReceivers);

    AutoPtr<IHashMap> receivers;
    CHashMap::New((IHashMap**)&receivers);

    // Add one entry to the map for every sms Receiver (ignoring duplicate sms receivers)
    FOR_EACH(iter, smsReceivers) {
        AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(Ptr(iter)->Func(iter->GetNext));
        AutoPtr<IActivityInfo> activityInfo;
        resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        if (activityInfo == NULL) {
            continue;
        }
        String permission;
        activityInfo->GetPermission(&permission);
        if (!Manifest::permission::BROADCAST_SMS->Equals(permission)) {
            continue;
        }
        String packageName;
        activityInfo->GetPackageName(&packageName);
        if (!receivers->ContainsKey(packageName)) {
            AutoPtr<ICharSequence> label;
            resolveInfo->LoadLabel(packageManager, (ICharSequence**)&label);
            String applicationName = Object::ToString(label);

            AutoPtr<IApplicationInfo> appInfo;
            activityInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 uid;
            appInfo->GetUid(&uid);

            String name;
            activityInfo->GetName(&name);

            AutoPtr<ISmsApplicationData> smsApplicationData = new SmsApplicationData(
                    applicationName, packageName, uid);
            smsApplicationData.mSmsReceiverClass = name;
            receivers->Put(packageName, smsApplicationData);
        }
    }

    // Update any existing entries with mms receiver class
    intent = NULL;
    CIntent::New(IIntents::WAP_PUSH_DELIVER_ACTION, (IIntent**)&intent);
    intent->SetDataAndType(NULL, String("application/vnd.wap.mms-message"));
    AutoPtr<IList> mmsReceivers;
    packageManager->QueryBroadcastReceivers(intent, 0, userId, (IList**)&mmsReceivers);
    FOR_EACH(iter, mmsReceivers) {
        AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(Ptr(iter)->Func(iter->GetNext));
        AutoPtr<IActivityInfo> activityInfo;
        resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        if (activityInfo == NULL) {
            continue;
        }

        String permission;
        activityInfo->GetPermission(&permission);
        if (!Manifest::permission::BROADCAST_WAP_PUSH->Equals(permission)) {
            continue;
        }
        String packageName;
        activityInfo->GetPackageName(&packageName);
        AutoPtr<IInterface> obj;
        receivers->Get(packageName, (IInterface**)&obj);
        AutoPtr<ISmsApplicationData> smsApplicationData = ISmsApplicationData::Probe(obj);
        if (smsApplicationData != NULL) {
            String name;
            activityInfo->GetName(&name);
            smsApplicationData.mMmsReceiverClass = name;
        }
    }

    // Update any existing entries with respond via message intent class.
    AutoPtr<IUri> uri;
    Uri::FromParts(SCHEME_SMSTO, String(""), String(NULL), (IUri**)&uri);
    intent = NULL;
    CIntent::New(ITelephonyManager::ACTION_RESPOND_VIA_MESSAGE, uri, (IIntent**)&intent);

    AutoPtr<IList> respondServices;
    packageManager->QueryIntentServicesAsUser(intent, 0, userId, (IList**)&respondServices);
    FOR_EACH(iter, respondServices) {
        AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(Ptr(iter)->Func(iter->GetNext));
        AutoPtr<IServiceInfo> serviceInfo;
        resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
        if (serviceInfo == NULL) {
            continue;
        }
        String permission;
        serviceInfo->GetPermission(&permission);
        if (!Manifest::permission::SEND_RESPOND_VIA_MESSAGE->Equals(permission)) {
            continue;
        }
        String packageName;
        serviceInfo->GetPackageName(&packageName);
        AutoPtr<IInterface> obj;
        receivers->Get(packageName, (IInterface**)&obj);
        AutoPtr<ISmsApplicationData> smsApplicationData = ISmsApplicationData::Probe(obj);
        if (smsApplicationData != NULL) {
            String name;
            serviceInfo->GetName(&name);
            smsApplicationData.mRespondViaMessageClass = name;
        }
    }

    // Update any existing entries with supports send to.
    intent = NULL;
    CIntent::New(IIntent::ACTION_SENDTO, uri, (IIntent**)&intent);
    AutoPtr<IList> sendToActivities;
    packageManager->QueryIntentActivitiesAsUser(intent, 0, userId, (IList**)&sendToActivities);
    FOR_EACH(iter, sendToActivities) {
        AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(Ptr(iter)->Func(iter->GetNext));
        AutoPtr<IActivityInfo> activityInfo;
        resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        if (activityInfo == NULL) {
            continue;
        }
        String packageName;
        activityInfo->GetPackageName(&packageName);
        AutoPtr<IInterface> obj;
        receivers->Get(packageName, (IInterface**)&obj);
        AutoPtr<ISmsApplicationData> smsApplicationData = ISmsApplicationData::Probe(obj);
        if (smsApplicationData != NULL) {
            String name;
            serviceInfo->GetName(&name);
            smsApplicationData.mSendToClass = name;
        }
    }

    // Remove any entries for which we did not find all required intents.
    FOR_EACH(iter, smsReceivers) {
        AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(Ptr(iter)->Func(iter->GetNext));
        AutoPtr<IActivityInfo> activityInfo;
        resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        if (activityInfo == NULL) {
            continue;
        }
        String packageName;
        activityInfo->GetPackageName(&packageName);
        AutoPtr<IInterface> obj;
        receivers->Get(packageName, (IInterface**)&obj);
        AutoPtr<ISmsApplicationData> smsApplicationData = ISmsApplicationData::Probe(obj);
        if (smsApplicationData != NULL) {
            Boolean bComplete
            smsApplicationData->IsComplete(&bComplete);
            if (!bComplete) {
                receivers->Remove(packageName);
            }
        }
    }

    AutoPtr<ICollection> collection;
    receivers->GetValues((ICollection**)&collection);
    return collection;
}

AutoPtr<ISmsApplicationData> SmsApplication::GetApplicationForPackage(
    /* [in] */ ICollection* applications,
    /* [in] */ const String& packageName)
{
    if (packageName == NULL) {
        return NULL;
    }
    // Is there an entry in the application list for the specified package?
    FOR_EACH(iter, applications) {
        AutoPtr<ISmsApplicationData> application = ISmsApplicationData::Probe(Ptr(iter)->Func(iter->GetNext());
        String pName;
        application->GetPackageName(pName);
        if (pName->ContentEquals(packageName)) {
            return application;
        }
    }
    return NULL;
}

AutoPtr<ISmsApplicationData> SmsApplication::GetApplication(
    /* [in] */ IContext* context,
    /* [in] */ Boolean updateIfNeeded,
    /* [in] */ Int32 userId)
{
    ECode ec;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelephonyManager> tm = ITelephonyManager::Probe(obj);
    if (!tm->IsSmsCapable()) {
        // No phone, no SMS
        return NULL;
    }

    AutoPtr<ICollection> applications = GetApplicationCollectionInternal(context, userId);
    if (DEBUG_MULTIUSER) {
        Logger::I(LOG_TAG, "getApplication userId=%d", userId);
    }
    // Determine which application receives the broadcast
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);

    String defaultApplication;
    Settings::Secure::GetStringForUser(cr,
            ISettingsSecure::SMS_DEFAULT_APPLICATION, userId, &defaultApplication);
    if (DEBUG_MULTIUSER) {
        Logger::I(LOG_TAG, "getApplication defaultApp=%d", defaultApplication);
    }

    AutoPtr<ISmsApplicationData> applicationData;
    if (defaultApplication != NULL) {
        applicationData = GetApplicationForPackage(applications, defaultApplication);
    }
    if (DEBUG_MULTIUSER) {
        Logger::I(LOG_TAG, "getApplication appData=%d", applicationData);
    }
    // Picking a new SMS app requires AppOps and Settings.Secure permissions, so we only do
    // this if the caller asked us to.
    if (updateIfNeeded && applicationData == NULL) {
        // Try to find the default SMS package for this device
        AutoPtr<IResources> r;
        context->GetResources((IResources**)&r);
        String defaultPackage;
        r->GetString(R::string::default_sms_application, &defaultPackage);
        applicationData = GetApplicationForPackage(applications, defaultPackage);

        if (applicationData == NULL) {
            // Are there any applications?
            Int32 size;
            applications->GetSize(&size);
            if (size != 0) {
                AutoPtr<ArrayOf<IInterface*> > array;
                applications->ToArray((ArrayOf<IInterface*>**)&array);
                applicationData = IISmsApplicationData::Probe((*array)[0]);
            }
        }

        // if we found a new default app, update the setting
        if (applicationData != NULL) {
            SetDefaultApplicationInternal(applicationData.mPackageName, context, userId);
        }
    }

    // if we found a package, make sure AppOps permissions are set up correctly
    if (applicationData != NULL) {
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
        AutoPtr<IAppOpsManager> appOps = IAppOpsManager::Probe(obj);

        // We can only call checkOp if we are Privileged (updateIfNeeded) or if the app we
        // are checking is for our current uid. Doing this check from the unprivileged current
        // SMS app allows us to tell the current SMS app that it is not in a good state and
        // needs to ask to be the current SMS app again to work properly.
        if (updateIfNeeded || applicationData.mUid == Process::MyUid()) {
            // Verify that the SMS app has permissions
            Int32 mode = appOps->CheckOp(IAppOpsManager::OP_WRITE_SMS, applicationData.mUid,
                    applicationData.mPackageName);
            if (mode != IAppOpsManager::MODE_ALLOWED) {
                Rlog->E(LOG_TAG, applicationData.mPackageName + " lost OP_WRITE_SMS: " +
                        (updateIfNeeded ? " (fixing)" : " (no permission to fix)"));
                if (updateIfNeeded) {
                    appOps->SetMode(IAppOpsManager::OP_WRITE_SMS, applicationData.mUid,
                            applicationData.mPackageName, IAppOpsManager::MODE_ALLOWED);
                } else {
                    // We can not return a package if permissions are not set up correctly
                    applicationData = NULL;
                }
            }
        }

        // We can only verify the phone and BT app's permissions from a privileged caller
        if (updateIfNeeded) {
            // Ensure this component is still configured as the preferred activity. Usually the
            // current SMS app will already be the preferred activity - but checking whether or
            // not this is TRUE is just as expensive as reconfiguring the preferred activity so
            // we just reconfigure every time.
            AutoPtr<IPackageManager> packageManager;
            context->GetPackageManager((IPackageManager**)&packageManager);

            AutoPtr<IComponentName> cn;
            CComponentName::New(applicationData.mPackageName, applicationData.mSendToClass, (IComponentName**)&cn);
            ConfigurePreferredActivity(packageManager, cn, userId);
            // Verify that the phone, BT app and MmsService have permissions

            AutoPtr<IPackageInfo> info;
            ec = packageManager->GetPackageInfo(PHONE_PACKAGE_NAME, 0, (IPackageInfo**)&info);
            if (SUCCEEDED(ec)) {
                AutoPtr<IApplicationInfo> appInfo;
                info->GetApplicationInfo((IApplicationInfo**)&appInfo);
                Int32 uid;
                appInfo->GetUid(&uid);
                Int32 mode = appOps->CheckOp(IAppOpsManager::OP_WRITE_SMS, uid, PHONE_PACKAGE_NAME);
                if (mode != IAppOpsManager::MODE_ALLOWED) {
                    Rlog->E(LOG_TAG, PHONE_PACKAGE_NAME + " lost OP_WRITE_SMS:  (fixing)");
                    appOps->SetMode(IAppOpsManager::OP_WRITE_SMS, uid,
                            PHONE_PACKAGE_NAME, IAppOpsManager::MODE_ALLOWED);
                }
            }
            else {//} Catch (NameNotFoundException e) {
                // No phone app on this Device (unexpected, even for non-phone devices)
                Rlog->E(LOG_TAG, "Phone package not found: " + PHONE_PACKAGE_NAME);
                applicationData = NULL;
            }

            info = NULL;
            ec = packageManager->GetPackageInfo(BLUETOOTH_PACKAGE_NAME, 0, (IPackageInfo**)&info);
            if (SUCCEEDED(ec)) {
                AutoPtr<IApplicationInfo> appInfo;
                info->GetApplicationInfo((IApplicationInfo**)&appInfo);
                Int32 uid;
                appInfo->GetUid(&uid);
                Int32 mode = appOps->CheckOp(IAppOpsManager::OP_WRITE_SMS, uid, BLUETOOTH_PACKAGE_NAME);
                if (mode != IAppOpsManager::MODE_ALLOWED) {
                    Rlog->E(LOG_TAG, BLUETOOTH_PACKAGE_NAME + " lost OP_WRITE_SMS:  (fixing)");
                    appOps->SetMode(IAppOpsManager::OP_WRITE_SMS, uid,
                            BLUETOOTH_PACKAGE_NAME, IAppOpsManager::MODE_ALLOWED);
                }
            } else {// Catch (NameNotFoundException e) {
                // No BT app on this device
                Rlog->E(LOG_TAG, "Bluetooth package not found: " + BLUETOOTH_PACKAGE_NAME);
            }

            info = NULL;
            ec = packageManager->GetPackageInfo(MMS_SERVICE_PACKAGE_NAME, 0, (IPackageInfo**)&info);
            if (SUCCEEDED(ec)) {
                AutoPtr<IApplicationInfo> appInfo;
                info->GetApplicationInfo((IApplicationInfo**)&appInfo);
                Int32 uid;
                appInfo->GetUid(&uid);
                Int32 mode = appOps->CheckOp(IAppOpsManager::OP_WRITE_SMS, uid, MMS_SERVICE_PACKAGE_NAME);
                if (mode != IAppOpsManager::MODE_ALLOWED) {
                    Rlog->E(LOG_TAG, MMS_SERVICE_PACKAGE_NAME + " lost OP_WRITE_SMS:  (fixing)");
                    appOps->SetMode(IAppOpsManager::OP_WRITE_SMS, uid,
                            MMS_SERVICE_PACKAGE_NAME, IAppOpsManager::MODE_ALLOWED);
                }
            } else {//Catch (NameNotFoundException e) {
                // No phone app on this Device (unexpected, even for non-phone devices)
                Rlog->E(LOG_TAG, "MmsService package not found: " + MMS_SERVICE_PACKAGE_NAME);
                applicationData = NULL;
            }

        }
    }
    if (DEBUG_MULTIUSER) {
        Logger::I(LOG_TAG, "getApplication returning appData=" + applicationData);
    }
    return applicationData;
}

ECode SmsApplication::SetDefaultApplication(
    /* [in] */ const String& packageName,
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelephonyManager> tm = ITelephonyManager::Probe(obj);
    if (!tm->IsSmsCapable()) {
        // No phone, no SMS
        return;
    }

    Int32 userId = GetIncomingUserId(context);
    Int64 token = Binder::ClearCallingIdentity();

    SetDefaultApplicationInternal(packageName, context, userId);

    Binder::RestoreCallingIdentity(token);

    return NOERROR;
}

void SmsApplication::SetDefaultApplicationInternal(
    /* [in] */ const String& packageName,
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId)
{
    ECode ec;
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    // Get old package name
    String oldPackageName;
    Settings::Secure::GetStringForUser(cr,
            ISettingsSecure::SMS_DEFAULT_APPLICATION, userId, &oldPackageName);

    if (packageName != NULL && oldPackageName != NULL && packageName->Equals(oldPackageName)) {
        // No change
        return;
    }

    // We only make the change if the new package is valid
    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<ICollection> applications = GetApplicationCollection(context);
    AutoPtr<ISmsApplicationData> applicationData = GetApplicationForPackage(applications, packageName);
    if (applicationData != NULL) {
        // Ignore OP_WRITE_SMS for the previously configured default SMS app.
        AutoPtr<IPackageInfo> info;
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
        AutoPtr<IAppOpsManager> appOps = IAppOpsManager::Probe(obj);
        if (oldPackageName != NULL) {
            ec = packageManager->GetPackageInfo(oldPackageName, IPackageManager::GET_UNINSTALLED_PACKAGES, (IPackageInfo**)&info);
            if (SUCCEEDED(ec)) {
                AutoPtr<IApplicationInfo> appInfo;
                info->GetApplicationInfo((IApplicationInfo**)&appInfo);
                Int32 uid;
                appInfo->GetUid(&uid);
                appOps->SetMode(IAppOpsManager::OP_WRITE_SMS, uid, oldPackageName, IAppOpsManager::MODE_IGNORED);
            } else {// Catch (NameNotFoundException e) {
                Rlog->W(LOG_TAG, "Old SMS package not found: " + oldPackageName);
            }
        }

        // Update the secure setting.
        Settings::Secure::PutStringForUser(context->GetContentResolver(),
                ISettingsSecure::SMS_DEFAULT_APPLICATION, applicationData.mPackageName,
                userId);

        // Configure this as the preferred activity for SENDTO sms/mms intents
        AutoPtr<IComponentName> cn;
        CComponentName::New(applicationData.mPackageName, applicationData.mSendToClass, (IComponentName**)&cn);
        ConfigurePreferredActivity(packageManager, cn, userId);

        // Allow OP_WRITE_SMS for the newly configured default SMS app.
        appOps->SetMode(IAppOpsManager::OP_WRITE_SMS, applicationData.mUid,
                applicationData.mPackageName, IAppOpsManager::MODE_ALLOWED);

        // Phone needs to always have this permission to write to the sms database
        info = NULL;
        ec = packageManager->GetPackageInfo(PHONE_PACKAGE_NAME, 0, (IPackageInfo**)&info);
        if (SUCCEEDED(ec)) {
            AutoPtr<IApplicationInfo> appInfo;
            info->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 uid;
            appInfo->GetUid(&uid);
            appOps->SetMode(IAppOpsManager::OP_WRITE_SMS, uid, PHONE_PACKAGE_NAME, IAppOpsManager::MODE_ALLOWED);
        } else {//Catch (NameNotFoundException e) {
            // No phone app on this Device (unexpected, even for non-phone devices)
            Rlog->E(LOG_TAG, "Phone package not found: " + PHONE_PACKAGE_NAME);
        }

        // BT needs to always have this permission to write to the sms database
        info = NULL;
        ec = packageManager->GetPackageInfo(BLUETOOTH_PACKAGE_NAME, 0, (IPackageInfo**)&info);
        if (SUCCEEDED(ec)) {
            AutoPtr<IApplicationInfo> appInfo;
            info->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 uid;
            appInfo->GetUid(&uid);
            appOps->SetMode(IAppOpsManager::OP_WRITE_SMS, uid, BLUETOOTH_PACKAGE_NAME, IAppOpsManager::MODE_ALLOWED);
        } else {//Catch (NameNotFoundException e) {
            // No BT app on this device
            Rlog->E(LOG_TAG, "Bluetooth package not found: " + BLUETOOTH_PACKAGE_NAME);
        }

        // MmsService needs to always have this permission to write to the sms database
        info = NULL;
        ec = packageManager->GetPackageInfo(MMS_SERVICE_PACKAGE_NAME, 0, (IPackageInfo**)&info);
        if (SUCCEEDED(ec)) {
            AutoPtr<IApplicationInfo> appInfo;
            info->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 uid;
            appInfo->GetUid(&uid);
            appOps->SetMode(IAppOpsManager::OP_WRITE_SMS, uid, MMS_SERVICE_PACKAGE_NAME, IAppOpsManager::MODE_ALLOWED);
        } else {//Catch (NameNotFoundException e) {
            // No phone app on this Device (unexpected, even for non-phone devices)
            Rlog->E(LOG_TAG, "MmsService package not found: " + MMS_SERVICE_PACKAGE_NAME);
        }
    }
}

ECode SmsApplication::InitSmsPackageMonitor(
    /* [in] */ IContext* context)
{
    sSmsPackageMonitor = new SmsPackageMonitor(context);
    AutoPtr<ILooper> looper;
    context->GetMainLooper((ILooper**)&looper);
    return sSmsPackageMonitor->Register(context, looper, UserHandle::ALL, FALSE);
}

ECode SmsApplication::ConfigurePreferredActivity(
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 userId)
{
    // Add the four activity preferences we want to direct to this app.
    ReplacePreferredActivity(packageManager, componentName, userId, SCHEME_SMS);
    ReplacePreferredActivity(packageManager, componentName, userId, SCHEME_SMSTO);
    ReplacePreferredActivity(packageManager, componentName, userId, SCHEME_MMS);
    ReplacePreferredActivity(packageManager, componentName, userId, SCHEME_MMSTO);
    return NOERROR;
}

ECode SmsApplication::ReplacePreferredActivity(
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 userId,
    /* [in] */ const String& scheme)
{
    // Build the set of existing activities that handle this scheme
    AutoPtr<IUri> uri;
    Uri::FromParts(scheme, String(""), String(NULL), (IUri**)&uri);

    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_SENDTO, uri, (IIntent**)&intent);

    AutoPtr<IList> resolveInfoList;
    FAIL_RETURN(packageManager->QueryIntentActivitiesAsUser(
            intent, IPackageManager::MATCH_DEFAULT_ONLY | IPackageManager::GET_RESOLVED_FILTER,
            userId, (IList**)&resolveInfoList));

    // Build the set of ComponentNames for these activities
    Int32 n = resolveInfoList->Size();
    Autoptr<ArrayOf<IComponentName*> > set = ArrayOf<IComponentName*>::Alloc(n);
    for (Int32 i = 0; i < n; i++) {
        Autoptr<IInterface> item;
        resolveInfoList->Get(i, (IInterface**)&item);
        Autoptr<IResolveInfo> info = IResolveInfo::Probe(item);
        AutoPtr<IActivityInfo> aInfo;
        info->GetActivityInfo((IActivityInfo**)&aInfo);
        String pkgName, name;
        IPackageItemInfo * pii = IPackageItemInfo::Probe(aInfo);
        pii->GetPackageName(&pkgName);
        pii->GetName(&name);
        CComponentName::New(pkgName, name, (IComponentName**)&(*set)[i]);
    }

    // Update the preferred SENDTO activity for the specified scheme
    AutoPtr<IIntentFilter> intentFilter
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_SENDTO);
    intentFilter->AddCategory(Intent.CATEGORY_DEFAULT);
    intentFilter->AddDataScheme(scheme);
    return packageManager->ReplacePreferredActivityAsUser(intentFilter,
            IIntentFilter::MATCH_CATEGORY_SCHEME + IIntentFilter::MATCH_ADJUSTMENT_NORMAL,
            set, componentName, userId);
}

ECode SmsApplication::GetSmsApplicationData(
    /* [in] */ const String& packageName,
    /* [in] */ IContext* context,
    /* [out] */ ISmsApplicationData** result)
{
    AutoPtr<ICollection> applications = GetApplicationCollection(context);
    return GetApplicationForPackage(applications, packageName);
}

ECode SmsApplication::GetDefaultSmsApplication(
    /* [in] */ IContext* context,
    /* [in] */ Boolean updateIfNeeded,
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 userId = GetIncomingUserId(context);
    Int64 token = Binder::ClearCallingIdentity();

    AutoPtr<ISmsApplicationData> smsApplicationData = GetApplication(context, updateIfNeeded, userId);
    if (smsApplicationData != NULL) {
        CComponentName::New(smsApplicationData.mPackageName,
                smsApplicationData.mSmsReceiverClass, result);
    }

    return Binder->RestoreCallingIdentity(token);
}

ECode SmsApplication::GetDefaultMmsApplication(
    /* [in] */ IContext* context,
    /* [in] */ Boolean updateIfNeeded,
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 userId = GetIncomingUserId(context);
    Int64 token = Binder::ClearCallingIdentity();

    AutoPtr<ISmsApplicationData> smsApplicationData = GetApplication(context, updateIfNeeded, userId);
    if (smsApplicationData != NULL) {
        CComponentName::New(smsApplicationData.mPackageName,
                smsApplicationData.mMmsReceiverClass, result);
    }

    return Binder->RestoreCallingIdentity(token);
}

ECode SmsApplication::GetDefaultRespondViaMessageApplication(
    /* [in] */ IContext* context,
    /* [in] */ Boolean updateIfNeeded,
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 userId = GetIncomingUserId(context);
    Int64 token = Binder::ClearCallingIdentity();

    AutoPtr<ISmsApplicationData> smsApplicationData = GetApplication(context, updateIfNeeded, userId);
    if (smsApplicationData != NULL) {
        CComponentName::New(smsApplicationData.mPackageName,
                smsApplicationData.mRespondViaMessageClass, result);
    }
    return Binder->RestoreCallingIdentity(token);
}

ECode SmsApplication::GetDefaultSendToApplication(
    /* [in] */ IContext* context,
    /* [in] */ Boolean updateIfNeeded,
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 userId = GetIncomingUserId(context);
    Int64 token = Binder::ClearCallingIdentity();

    AutoPtr<ISmsApplicationData> smsApplicationData = GetApplication(context, updateIfNeeded, userId);
    if (smsApplicationData != NULL) {
        CComponentName::New(smsApplicationData.mPackageName,
                smsApplicationData.mSendToClass, result);
    }

    return Binder->RestoreCallingIdentity(token);
}

ECode SmsApplication::ShouldWriteMessageForPackage(
    /* [in] */ const String& packageName,
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;

    if (packageName == NULL) return NOERROR;

    AutoPtr<ISmsManagerHelper> smsMHelper;
    AutoPtr<ISmsManager> smsM;
    CSmsManager::AcquireSingleton((ISmsManager**)&smsMHelper);
    smsMHelper->GetDefault((ISmsManager**)&smsM);

    Boolean bAutoPersisting;
    smsM->GetAutoPersisting(&bAutoPersisting);
    if (bAutoPersisting) {
        return NOERROR;
    }

    String defaultSmsPackage;
    AutoPtr<IComponentName> component = GetDefaultSmsApplication(context, FALSE);
    if (component != NULL) {
        component->GetPackageName(&defaultSmsPackage);
    }

    AutoPtr< ArrayOf<String> > ignoredPack;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetStringArray(R::array::config_ignored_sms_packages, (ArrayOf<String>**)&ignoredPack);
    AutoPtr<IList> ignorePackages;
    Arrays::AsList(ignoredPack, (IList**)&ignorePackages);

    if (ignorePackages->Contains(packageName)) {
        *result = FALSE;
        return NOERROR;
    }

    if ((defaultSmsPackage == NULL || !defaultSmsPackage->Equals(packageName)) &&
            !packageName->Equals(BLUETOOTH_PACKAGE_NAME)) {
        // To write the message for someone other than the default SMS and BT app
        return TRUE;
    }

    *result = FALSE;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

