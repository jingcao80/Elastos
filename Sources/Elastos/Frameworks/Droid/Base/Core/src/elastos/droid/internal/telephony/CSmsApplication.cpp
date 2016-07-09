
#include "elastos/droid/Manifest.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/internal/telephony/CSmsApplication.h"
#include "elastos/droid/provider/CSettingsSecure.h"
#include "elastos/droid/telephony/CSmsManagerHelper.h"
#include "elastos/droid/R.h"

#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::R;
using Elastos::Droid::Manifest;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ITelephonySmsIntents;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::ISmsManagerHelper;
using Elastos::Droid::Telephony::CSmsManagerHelper;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Core::CoreUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

AutoPtr<IHashMap> gReceivers;

CSmsApplication::SmsPackageMonitor::SmsPackageMonitor(
    /* [in] */ IContext* context,
    /* [in] */ CSmsApplication* host)
    : mContext(context)
    , mHost(host)
{
}

//@Override
ECode CSmsApplication::SmsPackageMonitor::OnPackageDisappeared(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 reason)
{
    OnPackageChanged(packageName);
    return NOERROR;
}

ECode CSmsApplication::SmsPackageMonitor::OnPackageAppeared(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 reason)
{
    OnPackageChanged(packageName);
    return NOERROR;
}

ECode CSmsApplication::SmsPackageMonitor::OnPackageModified(
    /* [in] */ const String& packageName)
{
    OnPackageChanged(packageName);
    return NOERROR;
}

void CSmsApplication::SmsPackageMonitor::OnPackageChanged(
    /* [in] */ const String& packageName)
{
    AutoPtr<IPackageManager> packageManager;
    mContext->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IContext> userContext;
    Int32 userId = 0;
    GetSendingUserId(&userId);
    if (userId != IUserHandle::USER_OWNER) {
        String pn;
        mContext->GetPackageName(&pn);
        AutoPtr<IUserHandle> uh;
        CUserHandle::New(userId, (IUserHandle**)&uh);

        ECode ec = mContext->CreatePackageContextAsUser(pn, 0, uh, (IContext**)&userContext);
        if (FAILED(ec)) {//Catch (NameNotFoundException nnfe) {
            if (DEBUG_MULTIUSER) {
                Logger::W(LOGTAG, "Unable to create package context for user %d", userId);
            }
        }
    }
    else {
        userContext = mContext;
    }
    // Ensure this component is still configured as the preferred activity
    AutoPtr<IComponentName> componentName;
    mHost->GetDefaultSendToApplication(userContext, TRUE, (IComponentName**)&componentName);
    if (componentName != NULL) {
        ConfigurePreferredActivity(packageManager, componentName, userId);
    }
}

CAR_INTERFACE_IMPL(CSmsApplication::SmsApplicationData, Object, ISmsApplicationData)

ECode CSmsApplication::SmsApplicationData::IsComplete(
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mSmsReceiverClass != NULL && mMmsReceiverClass != NULL
                    && mRespondViaMessageClass != NULL && mSendToClass != NULL);
    return NOERROR;
}

ECode CSmsApplication::SmsApplicationData::constructor(
    /* [in] */ const String& applicationName,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    mApplicationName = applicationName;
    mPackageName = packageName;
    mUid = uid;
    return NOERROR;
}

CAR_SINGLETON_IMPL(CSmsApplication)

CAR_INTERFACE_IMPL(CSmsApplication, Singleton, ISmsApplication)

const String CSmsApplication::LOGTAG("CSmsApplication");
const String CSmsApplication::PHONE_PACKAGE_NAME("com.android.phone");
const String CSmsApplication::BLUETOOTH_PACKAGE_NAME("com.android.bluetooth");
const String CSmsApplication::MMS_SERVICE_PACKAGE_NAME("com.android.mms.service");
const String CSmsApplication::SCHEME_SMS("sms");
const String CSmsApplication::SCHEME_SMSTO("smsto");
const String CSmsApplication::SCHEME_MMS("mms");
const String CSmsApplication::SCHEME_MMSTO("mmsto");
const Boolean CSmsApplication::DEBUG_MULTIUSER = FALSE;

AutoPtr<CSmsApplication::SmsPackageMonitor> CSmsApplication::sSmsPackageMonitor;

Int32 CSmsApplication::GetIncomingUserId(
    /* [in] */ IContext* context)
{
    Int32 contextUserId;
    context->GetUserId(&contextUserId);
    Int32 callingUid = Binder::GetCallingUid();
    //TODO
//    if (DEBUG_MULTIUSER) {
//        Logger::I(LOGTAG, "getIncomingUserHandle caller=" + callingUid + ", myuid="
//                + Process::MyUid() + "\n\t" + Debug->GetCallers(4));
//    }
    AutoPtr<IUserHandleHelper> hlp;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&hlp);
    Int32 appId = 0, userId = 0;
    hlp->GetAppId(callingUid, &appId);
    if (appId < IProcess::FIRST_APPLICATION_UID) {
        return contextUserId;
    }
    else {
        hlp->GetUserId(callingUid, &userId);
        return userId;
    }
}

ECode CSmsApplication::GetApplicationCollection(
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

AutoPtr<ICollection> CSmsApplication::GetApplicationCollectionInternal(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId)
{
    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);

    // Get the list of apps registered for SMS
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonySmsIntents::SMS_DELIVER_ACTION, (IIntent**)&intent);
    AutoPtr<IList> smsReceivers;
    packageManager->QueryBroadcastReceivers(intent, 0, userId, (IList**)&smsReceivers);

    AutoPtr<IHashMap> receivers;
    CHashMap::New((IHashMap**)&receivers);
    gReceivers = receivers;

    // Add one entry to the map for every sms Receiver (ignoring duplicate sms receivers)
    AutoPtr<IIterator> it;
    smsReceivers->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(p);
        AutoPtr<IActivityInfo> activityInfo;
        resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        if (activityInfo == NULL) {
            continue;
        }
        String permission;
        activityInfo->GetPermission(&permission);
        if (!Manifest::permission::BROADCAST_SMS.Equals(permission)) {
            continue;
        }
        String packageName;
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&packageName);
        Boolean bCK = FALSE;
        receivers->ContainsKey(CoreUtils::Convert(packageName), &bCK);
        if (!bCK) {
            AutoPtr<ICharSequence> label;
            resolveInfo->LoadLabel(packageManager, (ICharSequence**)&label);
            String applicationName = Object::ToString(label);

            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(activityInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 uid;
            appInfo->GetUid(&uid);

            String name;
            IPackageItemInfo::Probe(activityInfo)->GetName(&name);

            AutoPtr<SmsApplicationData> smsApplicationData = new SmsApplicationData();
            smsApplicationData->constructor(applicationName, packageName, uid);
            smsApplicationData->mSmsReceiverClass = name;
            receivers->Put(CoreUtils::Convert(packageName), (IObject*)smsApplicationData.Get());
        }
    }

    // Update any existing entries with mms receiver class
    intent = NULL;
    CIntent::New(ITelephonySmsIntents::WAP_PUSH_DELIVER_ACTION, (IIntent**)&intent);
    intent->SetDataAndType(NULL, String("application/vnd.wap.mms-message"));
    AutoPtr<IList> mmsReceivers;
    packageManager->QueryBroadcastReceivers(intent, 0, userId, (IList**)&mmsReceivers);
    AutoPtr<IIterator> itMms;
    mmsReceivers->GetIterator((IIterator**)&itMms);
    bHasNext = FALSE;
    while ((itMms->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        itMms->GetNext((IInterface**)&p);
        AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(p);
        AutoPtr<IActivityInfo> activityInfo;
        resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        if (activityInfo == NULL) {
            continue;
        }

        String permission;
        activityInfo->GetPermission(&permission);
        if (!Manifest::permission::BROADCAST_WAP_PUSH.Equals(permission)) {
            continue;
        }
        String packageName;
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&packageName);
        AutoPtr<IInterface> obj;
        receivers->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
        AutoPtr<SmsApplicationData> smsApplicationData = (SmsApplicationData*)ISmsApplicationData::Probe(obj);
        if (smsApplicationData != NULL) {
            String name;
            IPackageItemInfo::Probe(activityInfo)->GetName(&name);
            smsApplicationData->mMmsReceiverClass = name;
        }
    }

    // Update any existing entries with respond via message intent class.
    AutoPtr<IUriHelper> urihlp;
    CUriHelper::AcquireSingleton((IUriHelper**)&urihlp);
    AutoPtr<IUri> uri;
    urihlp->FromParts(SCHEME_SMSTO, String(""), String(NULL), (IUri**)&uri);
    intent = NULL;
    CIntent::New(ITelephonyManager::ACTION_RESPOND_VIA_MESSAGE, uri, (IIntent**)&intent);

    AutoPtr<IList> respondServices;
    packageManager->QueryIntentServicesAsUser(intent, 0, userId, (IList**)&respondServices);
    AutoPtr<IIterator> itRespond;
    respondServices->GetIterator((IIterator**)&itRespond);
    bHasNext = FALSE;
    while ((itRespond->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        itRespond->GetNext((IInterface**)&p);
        AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(p);
        AutoPtr<IServiceInfo> serviceInfo;
        resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
        if (serviceInfo == NULL) {
            continue;
        }
        String permission;
        serviceInfo->GetPermission(&permission);
        if (!Manifest::permission::SEND_RESPOND_VIA_MESSAGE.Equals(permission)) {
            continue;
        }
        String packageName;
        IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&packageName);
        AutoPtr<IInterface> obj;
        receivers->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
        AutoPtr<SmsApplicationData> smsApplicationData = (SmsApplicationData*)ISmsApplicationData::Probe(obj);
        if (smsApplicationData != NULL) {
            String name;
            IPackageItemInfo::Probe(serviceInfo)->GetName(&name);
            smsApplicationData->mRespondViaMessageClass = name;
        }
    }

    // Update any existing entries with supports send to.
    intent = NULL;
    CIntent::New(IIntent::ACTION_SENDTO, uri, (IIntent**)&intent);
    AutoPtr<IList> sendToActivities;
    packageManager->QueryIntentActivitiesAsUser(intent, 0, userId, (IList**)&sendToActivities);
    AutoPtr<IIterator> itSend;
    sendToActivities->GetIterator((IIterator**)&itSend);
    bHasNext = FALSE;
    while ((itSend->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        itSend->GetNext((IInterface**)&p);
        AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(p);
        AutoPtr<IActivityInfo> activityInfo;
        resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        if (activityInfo == NULL) {
            continue;
        }
        String packageName;
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&packageName);
        AutoPtr<IInterface> obj;
        receivers->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
        AutoPtr<SmsApplicationData> smsApplicationData = (SmsApplicationData*)ISmsApplicationData::Probe(obj);
        if (smsApplicationData != NULL) {
            String name;
            IPackageItemInfo::Probe(activityInfo)->GetName(&name);
            smsApplicationData->mSendToClass = name;
        }
    }

    // Remove any entries for which we did not find all required intents.
    AutoPtr<IIterator> itSms;
    smsReceivers->GetIterator((IIterator**)&itSms);
    bHasNext = FALSE;
    while ((itSms->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        itSms->GetNext((IInterface**)&p);
        AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(p);
        AutoPtr<IActivityInfo> activityInfo;
        resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        if (activityInfo == NULL) {
            continue;
        }
        String packageName;
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&packageName);
        AutoPtr<IInterface> obj;
        receivers->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
        AutoPtr<ISmsApplicationData> smsApplicationData = ISmsApplicationData::Probe(obj);
        if (smsApplicationData != NULL) {
            Boolean bComplete;
            smsApplicationData->IsComplete(&bComplete);
            if (!bComplete) {
                receivers->Remove(CoreUtils::Convert(packageName));
            }
        }
    }

    AutoPtr<ICollection> collection;
    receivers->GetValues((ICollection**)&collection);
    return collection;
}

AutoPtr<CSmsApplication::SmsApplicationData> CSmsApplication::GetApplicationForPackage(
    /* [in] */ ICollection* applications,
    /* [in] */ const String& packageName)
{
    if (packageName == NULL) {
        return NULL;
    }
    // Is there an entry in the application list for the specified package?
    AutoPtr<IIterator> it;
    applications->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<SmsApplicationData> application = (SmsApplicationData*)ISmsApplicationData::Probe(p);
        if (application->mPackageName.Equals(packageName)) {
            return application;
        }
    }
    return NULL;
}

AutoPtr<CSmsApplication::SmsApplicationData> CSmsApplication::GetApplication(
    /* [in] */ IContext* context,
    /* [in] */ Boolean updateIfNeeded,
    /* [in] */ Int32 userId)
{
    ECode ec;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelephonyManager> tm = ITelephonyManager::Probe(obj);
    Boolean bCap = FALSE;
    tm->IsSmsCapable(&bCap);
    if (!bCap) {
        // No phone, no SMS
        return NULL;
    }

    AutoPtr<ICollection> applications = GetApplicationCollectionInternal(context, userId);
    if (DEBUG_MULTIUSER) {
        Logger::I(LOGTAG, "getApplication userId=%d", userId);
    }
    // Determine which application receives the broadcast
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<ISettingsSecure> ss;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
    String defaultApplication;
    ss->GetStringForUser(cr,
            ISettingsSecure::SMS_DEFAULT_APPLICATION, userId, &defaultApplication);
    if (DEBUG_MULTIUSER) {
        Logger::I(LOGTAG, "getApplication defaultApp=%s", defaultApplication.string());
    }

    AutoPtr<SmsApplicationData> applicationData;
    if (defaultApplication != NULL) {
        applicationData = GetApplicationForPackage(applications, defaultApplication);
    }
    if (DEBUG_MULTIUSER) {
        Logger::I(LOGTAG, "getApplication appData=%s", TO_CSTR(applicationData));
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
                applicationData = (SmsApplicationData*)ISmsApplicationData::Probe((*array)[0]);
            }
        }

        // if we found a new default app, update the setting
        if (applicationData != NULL) {
            SetDefaultApplicationInternal(applicationData->mPackageName, context, userId);
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
        if (updateIfNeeded || applicationData->mUid == Process::MyUid()) {
            // Verify that the SMS app has permissions
            Int32 mode = 0;
            appOps->CheckOp(IAppOpsManager::OP_WRITE_SMS, applicationData->mUid,
                    applicationData->mPackageName, &mode);
            if (mode != IAppOpsManager::MODE_ALLOWED) {
                Logger::E(LOGTAG, "%s lost OP_WRITE_SMS: ", applicationData->mPackageName.string(),
                        (updateIfNeeded ? " (fixing)" : " (no permission to fix)"));
                if (updateIfNeeded) {
                    appOps->SetMode(IAppOpsManager::OP_WRITE_SMS, applicationData->mUid,
                            applicationData->mPackageName, IAppOpsManager::MODE_ALLOWED);
                }
                else {
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
            CComponentName::New(applicationData->mPackageName, applicationData->mSendToClass, (IComponentName**)&cn);
            ConfigurePreferredActivity(packageManager, cn, userId);
            // Verify that the phone, BT app and MmsService have permissions

            AutoPtr<IPackageInfo> info;
            ec = packageManager->GetPackageInfo(PHONE_PACKAGE_NAME, 0, (IPackageInfo**)&info);
            if (SUCCEEDED(ec)) {
                AutoPtr<IApplicationInfo> appInfo;
                info->GetApplicationInfo((IApplicationInfo**)&appInfo);
                Int32 uid;
                appInfo->GetUid(&uid);
                Int32 mode = 0;
                appOps->CheckOp(IAppOpsManager::OP_WRITE_SMS, uid, PHONE_PACKAGE_NAME, &mode);
                if (mode != IAppOpsManager::MODE_ALLOWED) {
                    Logger::E(LOGTAG, "%s lost OP_WRITE_SMS:  (fixing)", PHONE_PACKAGE_NAME.string());
                    appOps->SetMode(IAppOpsManager::OP_WRITE_SMS, uid,
                            PHONE_PACKAGE_NAME, IAppOpsManager::MODE_ALLOWED);
                }
            }
            else {//} Catch (NameNotFoundException e) {
                // No phone app on this Device (unexpected, even for non-phone devices)
                Logger::E(LOGTAG, "Phone package not found: %s", PHONE_PACKAGE_NAME.string());
                applicationData = NULL;
            }

            info = NULL;
            ec = packageManager->GetPackageInfo(BLUETOOTH_PACKAGE_NAME, 0, (IPackageInfo**)&info);
            if (SUCCEEDED(ec)) {
                AutoPtr<IApplicationInfo> appInfo;
                info->GetApplicationInfo((IApplicationInfo**)&appInfo);
                Int32 uid;
                appInfo->GetUid(&uid);
                Int32 mode = 0;
                appOps->CheckOp(IAppOpsManager::OP_WRITE_SMS, uid, BLUETOOTH_PACKAGE_NAME, &mode);
                if (mode != IAppOpsManager::MODE_ALLOWED) {
                    Logger::E(LOGTAG, "%s lost OP_WRITE_SMS:  (fixing)", BLUETOOTH_PACKAGE_NAME.string());
                    appOps->SetMode(IAppOpsManager::OP_WRITE_SMS, uid,
                            BLUETOOTH_PACKAGE_NAME, IAppOpsManager::MODE_ALLOWED);
                }
            }
            else {// Catch (NameNotFoundException e) {
                // No BT app on this device
                Logger::E(LOGTAG, "Bluetooth package not found: %s", BLUETOOTH_PACKAGE_NAME.string());
            }

            info = NULL;
            ec = packageManager->GetPackageInfo(MMS_SERVICE_PACKAGE_NAME, 0, (IPackageInfo**)&info);
            if (SUCCEEDED(ec)) {
                AutoPtr<IApplicationInfo> appInfo;
                info->GetApplicationInfo((IApplicationInfo**)&appInfo);
                Int32 uid;
                appInfo->GetUid(&uid);
                Int32 mode = 0;
                appOps->CheckOp(IAppOpsManager::OP_WRITE_SMS, uid, MMS_SERVICE_PACKAGE_NAME, &mode);
                if (mode != IAppOpsManager::MODE_ALLOWED) {
                    Logger::E(LOGTAG, "%s lost OP_WRITE_SMS:  (fixing)", MMS_SERVICE_PACKAGE_NAME.string());
                    appOps->SetMode(IAppOpsManager::OP_WRITE_SMS, uid,
                            MMS_SERVICE_PACKAGE_NAME, IAppOpsManager::MODE_ALLOWED);
                }
            }
            else {//Catch (NameNotFoundException e) {
                // No phone app on this Device (unexpected, even for non-phone devices)
                Logger::E(LOGTAG, "MmsService package not found: %s", MMS_SERVICE_PACKAGE_NAME.string());
                applicationData = NULL;
            }

        }
    }
    if (DEBUG_MULTIUSER) {
        Logger::I(LOGTAG, "getApplication returning appData=%p", applicationData.Get());
    }
    return applicationData;
}

ECode CSmsApplication::SetDefaultApplication(
    /* [in] */ const String& packageName,
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelephonyManager> tm = ITelephonyManager::Probe(obj);
    Boolean bCap = FALSE;
    tm->IsSmsCapable(&bCap);
    if (!bCap) {
        // No phone, no SMS
        return NOERROR;
    }

    Int32 userId = GetIncomingUserId(context);
    Int64 token = Binder::ClearCallingIdentity();

    SetDefaultApplicationInternal(packageName, context, userId);

    Binder::RestoreCallingIdentity(token);

    return NOERROR;
}

void CSmsApplication::SetDefaultApplicationInternal(
    /* [in] */ const String& packageName,
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId)
{
    ECode ec;
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    // Get old package name
    AutoPtr<ISettingsSecure> ss;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
    String oldPackageName;
    ss->GetStringForUser(cr,
            ISettingsSecure::SMS_DEFAULT_APPLICATION, userId, &oldPackageName);

    if (packageName != NULL && oldPackageName != NULL && packageName.Equals(oldPackageName)) {
        // No change
        return;
    }

    // We only make the change if the new package is valid
    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<ICollection> applications;
    assert(0 && "TODO");
    // GetApplicationCollection(context, (ICollection**)&applications);
    AutoPtr<SmsApplicationData> applicationData = GetApplicationForPackage(applications, packageName);
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
            }
            else {// Catch (NameNotFoundException e) {
                assert(0 && "TODO");
                // Rlog->W(LOGTAG, "Old SMS package not found: " + oldPackageName);
            }
        }

        // Update the secure setting.
        AutoPtr<ISettingsSecure> ss;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);
        Boolean bForUser = FALSE;
        ss->PutStringForUser(cr,
                ISettingsSecure::SMS_DEFAULT_APPLICATION, applicationData->mPackageName,
                userId, &bForUser);

        // Configure this as the preferred activity for SENDTO sms/mms intents
        AutoPtr<IComponentName> cn;
        CComponentName::New(applicationData->mPackageName, applicationData->mSendToClass, (IComponentName**)&cn);
        ConfigurePreferredActivity(packageManager, cn, userId);

        // Allow OP_WRITE_SMS for the newly configured default SMS app.
        appOps->SetMode(IAppOpsManager::OP_WRITE_SMS, applicationData->mUid,
                applicationData->mPackageName, IAppOpsManager::MODE_ALLOWED);

        // Phone needs to always have this permission to write to the sms database
        info = NULL;
        ec = packageManager->GetPackageInfo(PHONE_PACKAGE_NAME, 0, (IPackageInfo**)&info);
        if (SUCCEEDED(ec)) {
            AutoPtr<IApplicationInfo> appInfo;
            info->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 uid;
            appInfo->GetUid(&uid);
            appOps->SetMode(IAppOpsManager::OP_WRITE_SMS, uid, PHONE_PACKAGE_NAME, IAppOpsManager::MODE_ALLOWED);
        }
        else {//Catch (NameNotFoundException e) {
            // No phone app on this Device (unexpected, even for non-phone devices)
            assert(0 && "TODO");
            // Rlog::E(LOGTAG, "Phone package not found: " + PHONE_PACKAGE_NAME);
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
        }
        else {//Catch (NameNotFoundException e) {
            // No BT app on this device
            assert(0 && "TODO");
            // Rlog->E(LOGTAG, "Bluetooth package not found: " + BLUETOOTH_PACKAGE_NAME);
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
        }
        else {//Catch (NameNotFoundException e) {
            // No phone app on this Device (unexpected, even for non-phone devices)
            assert(0 && "TODO");
            // Rlog->E(LOGTAG, "MmsService package not found: " + MMS_SERVICE_PACKAGE_NAME);
        }
    }
}

ECode CSmsApplication::InitSmsPackageMonitor(
    /* [in] */ IContext* context)
{
    sSmsPackageMonitor = new SmsPackageMonitor(context, this);
    AutoPtr<ILooper> looper;
    context->GetMainLooper((ILooper**)&looper);
    AutoPtr<IUserHandleHelper> hlp;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&hlp);
    AutoPtr<IUserHandle> all;
    hlp->GetALL((IUserHandle**)&all);
    return sSmsPackageMonitor->Register(context, looper, all, FALSE);
}

void CSmsApplication::ConfigurePreferredActivity(
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 userId)
{
    // Add the four activity preferences we want to direct to this app.
    ReplacePreferredActivity(packageManager, componentName, userId, SCHEME_SMS);
    ReplacePreferredActivity(packageManager, componentName, userId, SCHEME_SMSTO);
    ReplacePreferredActivity(packageManager, componentName, userId, SCHEME_MMS);
    ReplacePreferredActivity(packageManager, componentName, userId, SCHEME_MMSTO);
}

void CSmsApplication::ReplacePreferredActivity(
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 userId,
    /* [in] */ const String& scheme)
{
    // Build the set of existing activities that handle this scheme
    AutoPtr<IUriHelper> urihlp;
    CUriHelper::AcquireSingleton((IUriHelper**)&urihlp);
    AutoPtr<IUri> uri;
    urihlp->FromParts(scheme, String(""), String(NULL), (IUri**)&uri);

    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_SENDTO, uri, (IIntent**)&intent);

    AutoPtr<IList> resolveInfoList;
    packageManager->QueryIntentActivitiesAsUser(
            intent, IPackageManager::MATCH_DEFAULT_ONLY | IPackageManager::GET_RESOLVED_FILTER,
            userId, (IList**)&resolveInfoList);

    // Build the set of ComponentNames for these activities
    Int32 n = 0;
    resolveInfoList->GetSize(&n);
    AutoPtr<ArrayOf<IComponentName*> > set = ArrayOf<IComponentName*>::Alloc(n);
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IInterface> item;
        resolveInfoList->Get(i, (IInterface**)&item);
        AutoPtr<IResolveInfo> info = IResolveInfo::Probe(item);
        AutoPtr<IActivityInfo> aInfo;
        info->GetActivityInfo((IActivityInfo**)&aInfo);
        String pkgName, name;
        AutoPtr<IPackageItemInfo> pii = IPackageItemInfo::Probe(aInfo);
        pii->GetPackageName(&pkgName);
        pii->GetName(&name);
        CComponentName::New(pkgName, name, (IComponentName**)&(*set)[i]);
    }

    // Update the preferred SENDTO activity for the specified scheme
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_SENDTO);
    intentFilter->AddCategory(IIntent::CATEGORY_DEFAULT);
    intentFilter->AddDataScheme(scheme);
    assert(0 && "TODO add method to car");
    // packageManager->ReplacePreferredActivityAsUser(intentFilter,
    //         IIntentFilter::MATCH_CATEGORY_SCHEME + IIntentFilter::MATCH_ADJUSTMENT_NORMAL,
    //         set, componentName, userId);
}

ECode CSmsApplication::GetSmsApplicationData(
    /* [in] */ const String& packageName,
    /* [in] */ IContext* context,
    /* [out] */ ISmsApplicationData** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICollection> applications;
    GetApplicationCollection(context, (ICollection**)&applications);
    AutoPtr<ISmsApplicationData> res = GetApplicationForPackage(applications, packageName);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSmsApplication::GetDefaultSmsApplication(
    /* [in] */ IContext* context,
    /* [in] */ Boolean updateIfNeeded,
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 userId = GetIncomingUserId(context);
    Int64 token = Binder::ClearCallingIdentity();

    AutoPtr<SmsApplicationData> smsApplicationData = GetApplication(context, updateIfNeeded, userId);
    if (smsApplicationData != NULL) {
        CComponentName::New(smsApplicationData->mPackageName,
                smsApplicationData->mSmsReceiverClass, result);
    }

    return Binder::RestoreCallingIdentity(token);
}

ECode CSmsApplication::GetDefaultMmsApplication(
    /* [in] */ IContext* context,
    /* [in] */ Boolean updateIfNeeded,
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 userId = GetIncomingUserId(context);
    Int64 token = Binder::ClearCallingIdentity();

    AutoPtr<SmsApplicationData> smsApplicationData = GetApplication(context, updateIfNeeded, userId);
    if (smsApplicationData != NULL) {
        CComponentName::New(smsApplicationData->mPackageName,
                smsApplicationData->mMmsReceiverClass, result);
    }

    return Binder::RestoreCallingIdentity(token);
}

ECode CSmsApplication::GetDefaultRespondViaMessageApplication(
    /* [in] */ IContext* context,
    /* [in] */ Boolean updateIfNeeded,
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 userId = GetIncomingUserId(context);
    Int64 token = Binder::ClearCallingIdentity();

    AutoPtr<SmsApplicationData> smsApplicationData = GetApplication(context, updateIfNeeded, userId);
    if (smsApplicationData != NULL) {
        CComponentName::New(smsApplicationData->mPackageName,
                smsApplicationData->mRespondViaMessageClass, result);
    }
    return Binder::RestoreCallingIdentity(token);
}

ECode CSmsApplication::GetDefaultSendToApplication(
    /* [in] */ IContext* context,
    /* [in] */ Boolean updateIfNeeded,
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 userId = GetIncomingUserId(context);
    Int64 token = Binder::ClearCallingIdentity();

    AutoPtr<SmsApplicationData> smsApplicationData = GetApplication(context, updateIfNeeded, userId);
    if (smsApplicationData != NULL) {
        CComponentName::New(smsApplicationData->mPackageName,
                smsApplicationData->mSendToClass, result);
    }

    return Binder::RestoreCallingIdentity(token);
}

ECode CSmsApplication::ShouldWriteMessageForPackage(
    /* [in] */ const String& packageName,
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;

    if (packageName == NULL) return NOERROR;

    AutoPtr<ISmsManagerHelper> smsMHelper;
    CSmsManagerHelper::AcquireSingleton((ISmsManagerHelper**)&smsMHelper);
    AutoPtr<ISmsManager> smsM;
    smsMHelper->GetDefault((ISmsManager**)&smsM);

    Boolean bAutoPersisting = FALSE;
    smsM->GetAutoPersisting(&bAutoPersisting);
    if (bAutoPersisting) {
        return NOERROR;
    }

    String defaultSmsPackage;
    AutoPtr<IComponentName> component;
    GetDefaultSmsApplication(context, FALSE, (IComponentName**)&component);
    if (component != NULL) {
        component->GetPackageName(&defaultSmsPackage);
    }

    AutoPtr< ArrayOf<String> > ignoredPack;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetStringArray(R::array::config_ignored_sms_packages, (ArrayOf<String>**)&ignoredPack);
    AutoPtr<IList> ignorePackages;
    Arrays::AsList(ignoredPack, (IList**)&ignorePackages);

    Boolean bContain = FALSE;
    ignorePackages->Contains(CoreUtils::Convert(packageName), &bContain);
    if (bContain) {
        *result = FALSE;
        return NOERROR;
    }

    if ((defaultSmsPackage.IsNull() || !defaultSmsPackage.Equals(packageName)) &&
            !packageName.Equals(BLUETOOTH_PACKAGE_NAME)) {
        // To write the message for someone other than the default SMS and BT app
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

