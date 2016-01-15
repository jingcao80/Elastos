
#include "CRecognitionManagerService.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/os/UserHandle.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Speech::IRecognitionService;
using Elastos::Core::CObjectContainer;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String CRecognitionManagerService::TAG("CRecognitionManagerService");
const Boolean CRecognitionManagerService::DEBUG = FALSE;

CRecognitionManagerService::MyPackageMonitor::MyPackageMonitor(
    /* [in] */ CRecognitionManagerService* owner)
    :mOwner(owner)
{}

ECode CRecognitionManagerService::MyPackageMonitor::OnSomePackagesChanged()
{
    Int32 userHandle;
    GetChangingUserId(&userHandle);
    if (CRecognitionManagerService::DEBUG) {
        Slogger::I(TAG, "onSomePackagesChanged user=%d", userHandle);
    }
    AutoPtr<IComponentName> componentName;
    FAIL_RETURN(mOwner->GetCurRecognizer(userHandle, (IComponentName**)&componentName));
    if (NULL == componentName) {
        Boolean result;
        AnyPackagesAppearing(&result);
        if (result) {
            FAIL_RETURN(mOwner->FindAvailRecognizer(String(NULL), userHandle, (IComponentName**)&componentName));
            if (NULL != componentName) {
                FAIL_RETURN(mOwner->SetCurRecognizer(componentName, userHandle));
            }
        }
    }
    return NOERROR;
}

CRecognitionManagerService::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CRecognitionManagerService* owner)
    :mOwner(owner)
{}

ECode CRecognitionManagerService::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    assert(intent != NULL);
    String action;
    intent->GetAction(&action);
    if (CRecognitionManagerService::DEBUG) {
        Slogger::I(TAG, "received %s", action.string());
    }
    if (IIntent::ACTION_BOOT_COMPLETED.Equals(action)) {
        Int32 userHandle;
        FAIL_RETURN(intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -1, &userHandle));
        if (userHandle > 0) {
            return mOwner->InitForUser(userHandle);
        }
    }
    return NOERROR;
}

ECode CRecognitionManagerService::SystemReady()
{
    return (InitForUser(IUserHandle::USER_OWNER));
}

ECode CRecognitionManagerService::FindAvailRecognizer(
    /* [in] */ const String& prefPackage,
    /* [in] */ Int32 userHandle,
    /* [out] */ IComponentName** componentName)
{
    VALIDATE_NOT_NULL(componentName);
    *componentName = NULL;

    AutoPtr<IPackageManager> packageManager;
    FAIL_RETURN(mContext->GetPackageManager((IPackageManager**)&packageManager));
    assert(packageManager != NULL);
    AutoPtr<IIntent> intent;
    FAIL_RETURN(CIntent::New(IRecognitionService::SERVICE_INTERFACE, (IIntent**)&intent));

    AutoPtr<IObjectContainer> available;
    FAIL_RETURN(packageManager->QueryIntentServicesAsUser(intent, 0, userHandle, (IObjectContainer**)&available));
    Int32 numAvailable;
    assert(available != NULL);
    available->GetObjectCount(&numAvailable);

    if (0 == numAvailable) {
        Slogger::W(TAG, "no available voice recognition services found for user %d ", userHandle);
        return E_NULL_POINTER_EXCEPTION;
    }
    else {
        Boolean hasNext = FALSE;
        AutoPtr<IObjectEnumerator> enumerator;
        AutoPtr<IResolveInfo> resolveInfo;
        AutoPtr<IServiceInfo> serviceInfo;
        String packageName, name;
        if (NULL != prefPackage)  {
            FAIL_RETURN(available->GetObjectEnumerator((IObjectEnumerator**)&enumerator));
            while(enumerator->MoveNext(&hasNext), hasNext) {
                resolveInfo = NULL;
                enumerator->Current((IInterface**)&resolveInfo);
                serviceInfo = NULL;
                resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
                serviceInfo->GetPackageName(&packageName);
                serviceInfo->GetName(&name);
                if (prefPackage.Equals(packageName)) {
                    return CComponentName::New(packageName, name, componentName);
                }
            }
        }
        if (numAvailable > 1) {
            Slogger::W(TAG, "more than one voice recognition service found, picking first");
        }

        enumerator = NULL;
        FAIL_RETURN(available->GetObjectEnumerator((IObjectEnumerator**)&enumerator));
        enumerator->MoveNext(&hasNext);
        resolveInfo = NULL;
        enumerator->Current((IInterface**)&resolveInfo);
        serviceInfo = NULL;
        resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
        serviceInfo->GetPackageName(&packageName);
        serviceInfo->GetName(&name);
        return CComponentName::New(packageName, name, componentName);
    }
}

ECode CRecognitionManagerService::GetCurRecognizer(
    /* [in] */ Int32 userHandle,
    /* [out] */ IComponentName** componentName)
{
    VALIDATE_NOT_NULL(componentName);
    *componentName = NULL;

    String curRecognizer;
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetStringForUser(contentResolver, ISettingsSecure::VOICE_RECOGNITION_SERVICE, userHandle, &curRecognizer);
    if (curRecognizer.IsNullOrEmpty()) {
        *componentName = NULL;
        return NOERROR;
    }
    if (DEBUG) {
        Slogger::I(TAG, "getCurRecognizer curRecognizer=%s user=%d", curRecognizer.string(), userHandle);
    }
    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    return helper->UnflattenFromString(curRecognizer, componentName);
}

ECode CRecognitionManagerService::SetCurRecognizer(
    /* [in] */ IComponentName* comp,
    /* [in] */ Int32 userHandle)
{
    AutoPtr<IContentResolver> contentResolver;
    FAIL_RETURN(mContext->GetContentResolver((IContentResolver**)&contentResolver));
    String str;
    if (NULL != comp) {
        comp->FlattenToShortString(&str);
    }
    else {
        str = "";
    }
    Boolean result;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->PutStringForUser(contentResolver, ISettingsSecure::VOICE_RECOGNITION_SERVICE, str, userHandle, &result);
    if (DEBUG) {
      Slogger::I(TAG, "setCurRecognizer comp=%s user=%d", str.string(), userHandle);
    }
    return NOERROR;
}

ECode CRecognitionManagerService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("CRecognitionManagerService");
    return NOERROR;
}

ECode CRecognitionManagerService::constructor(
    /* [in] */ IContext * context)
{
    mBroadcastReceiver = new MyBroadcastReceiver(this);
    mContext = context;
    mMonitor = new MyPackageMonitor(this);
    mMonitor->Register(mContext, NULL, UserHandle::ALL, TRUE);
    mIPm = AppGlobals::GetPackageManager();
    AutoPtr<IIntentFilter> intentFilter;
    FAIL_RETURN(CIntentFilter::New(IIntent::ACTION_BOOT_COMPLETED, (IIntentFilter**)&intentFilter));
    AutoPtr<IIntent> tmpIntent;
    return(mContext->RegisterReceiverAsUser(mBroadcastReceiver, UserHandle::ALL, intentFilter, String(NULL), (IHandler*)NULL, (IIntent**)&tmpIntent));
}

ECode CRecognitionManagerService::InitForUser(
        /* [in] */ Int32 userHandle)
{
    if (DEBUG) {
        Slogger::I(TAG, "initForUser user=%d", userHandle);
    }

    AutoPtr<IServiceInfo> serviceInfo;
    AutoPtr<IComponentName> comp;
    PFL_EX("CRecognitionManagerService::InitForUser start");
    GetCurRecognizer(userHandle, (IComponentName**)&comp);
    if (NULL != comp) {
        // See if the current recognizer is still available.
        // try {
            mIPm->GetServiceInfo(comp, 0, userHandle, (IServiceInfo**)&serviceInfo);
        // } catch (RemoteException e) {
        // }
    }

    if (serviceInfo == NULL) {
        comp = NULL;
        FindAvailRecognizer(String(NULL), userHandle, (IComponentName**)&comp);
        if (NULL != comp) {
            SetCurRecognizer(comp, userHandle);
        }
    }
    return NOERROR;
}

}   // Server
}   // Droid
}   // Elastos

