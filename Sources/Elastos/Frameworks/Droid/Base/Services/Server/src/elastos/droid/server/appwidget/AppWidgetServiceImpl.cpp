
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/server/appwidget/AppWidgetServiceImpl.h"
#include "elastos/droid/server/appwidget/CAppWidgetServiceImplBroadcastReceiver.h"
#include "elastos/droid/server/LocalServices.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/Xml.h"
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.View.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include <elastos/core/AutoLock.h>
#include "elastos/core/CoreUtils.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Slogger.h"
#include "elastos/droid/R.h"

// using Elastos::Droid::App::Admin::CDevicePolicyManager;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::AppWidget::CAppWidgetProviderInfo;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CIntentFilterComparison;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IIntentFilterComparison;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Internal::AppWidget::EIID_IIAppWidgetService;
using Elastos::Droid::Internal::Os::CBackgroundThreadHelper;
using Elastos::Droid::Internal::Os::CSomeArgsHelper;
using Elastos::Droid::Internal::Os::IBackgroundThreadHelper;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::ISomeArgsHelper;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Internal::Widget::IIRemoteViewsFactory;
using Elastos::Droid::Manifest;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CBundleHelper;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::IBaseBundle;
using Elastos::Droid::Os::IBundleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Server::EIID_IWidgetBackupProvider;
using Elastos::Droid::Server::LocalServices;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::CPairHelper;
using Elastos::Droid::Utility::CSparseInt32Array;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::IPairHelper;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::CParcelableList;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IWindowManager;
using Elastos::Core::CoreUtils;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICloneable;
using Elastos::Core::IComparable;
using Elastos::Core::IInteger32;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::Logging::Slogger;
using Org::Xmlpull::V1::IXmlSerializer;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace AppWidget {

//=====================================================================
//                AppWidgetServiceImpl::CallbackHandler
//=====================================================================
const Int32 AppWidgetServiceImpl::CallbackHandler::MSG_NOTIFY_UPDATE_APP_WIDGET;
const Int32 AppWidgetServiceImpl::CallbackHandler::MSG_NOTIFY_PROVIDER_CHANGED;
const Int32 AppWidgetServiceImpl::CallbackHandler::MSG_NOTIFY_PROVIDERS_CHANGED;
const Int32 AppWidgetServiceImpl::CallbackHandler::MSG_NOTIFY_VIEW_DATA_CHANGED;

AppWidgetServiceImpl::CallbackHandler::CallbackHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ AppWidgetServiceImpl* owner)
    : Handler(looper, NULL, FALSE)
    , mOwner(owner)
{
    assert(NULL != mOwner);
}

ECode AppWidgetServiceImpl::CallbackHandler::HandleMessage(
    /* [in] */ IMessage* message)
{
    VALIDATE_NOT_NULL(message);

    Int32 what = 0;
    message->GetWhat(&what);
    switch (what) {
        case MSG_NOTIFY_UPDATE_APP_WIDGET:
            {
                AutoPtr<IInterface> interfaceTmp;
                message->GetObj((IInterface**)&interfaceTmp);
                AutoPtr<ISomeArgs> args = ISomeArgs::Probe(interfaceTmp);

                interfaceTmp = NULL;
                args->GetObjectArg(1, (IInterface**)&interfaceTmp);
                AutoPtr<IObject> objTmp = IObject::Probe(interfaceTmp);
                AutoPtr<AppWidgetServiceImpl::Host> host = (AppWidgetServiceImpl::Host*)objTmp.Get();

                interfaceTmp = NULL;
                args->GetObjectArg(2, (IInterface**)&interfaceTmp);
                AutoPtr<IIAppWidgetHost> callbacks = IIAppWidgetHost::Probe(interfaceTmp);

                interfaceTmp = NULL;
                args->GetObjectArg(3, (IInterface**)&interfaceTmp);
                AutoPtr<IRemoteViews> views = IRemoteViews::Probe(interfaceTmp);

                Int32 appWidgetId = 0;
                args->GetInt32Arg(1, &appWidgetId);
                args->Recycle();
                mOwner->HandleNotifyUpdateAppWidget(host, callbacks, appWidgetId, views);
            }
            break;
        case MSG_NOTIFY_PROVIDER_CHANGED:
            {
                AutoPtr<IInterface> interfaceTmp;
                message->GetObj((IInterface**)&interfaceTmp);
                AutoPtr<ISomeArgs> args = ISomeArgs::Probe(interfaceTmp);

                interfaceTmp = NULL;
                args->GetObjectArg(1, (IInterface**)&interfaceTmp);
                AutoPtr<IObject> objTmp = IObject::Probe(interfaceTmp);
                AutoPtr<AppWidgetServiceImpl::Host> host = (AppWidgetServiceImpl::Host*)objTmp.Get();

                interfaceTmp = NULL;
                args->GetObjectArg(2, (IInterface**)&interfaceTmp);
                AutoPtr<IIAppWidgetHost> callbacks = IIAppWidgetHost::Probe(interfaceTmp);

                interfaceTmp = NULL;
                args->GetObjectArg(3, (IInterface**)&interfaceTmp);
                AutoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(interfaceTmp);

                Int32 appWidgetId = 0;
                args->GetInt32Arg(1, &appWidgetId);
                args->Recycle();
                mOwner->HandleNotifyProviderChanged(host, callbacks, appWidgetId, info);
            }
            break;

        case MSG_NOTIFY_PROVIDERS_CHANGED:
            {
                AutoPtr<IInterface> interfaceTmp;
                message->GetObj((IInterface**)&interfaceTmp);
                AutoPtr<ISomeArgs> args = ISomeArgs::Probe(interfaceTmp);

                interfaceTmp = NULL;
                args->GetObjectArg(1, (IInterface**)&interfaceTmp);
                AutoPtr<IObject> objTmp = IObject::Probe(interfaceTmp);
                AutoPtr<AppWidgetServiceImpl::Host> host = (AppWidgetServiceImpl::Host*)objTmp.Get();

                interfaceTmp = NULL;
                args->GetObjectArg(2, (IInterface**)&interfaceTmp);
                AutoPtr<IIAppWidgetHost> callbacks = IIAppWidgetHost::Probe(interfaceTmp);
                args->Recycle();
                mOwner->HandleNotifyProvidersChanged(host, callbacks);
            }
            break;

        case MSG_NOTIFY_VIEW_DATA_CHANGED:
            {
                AutoPtr<IInterface> interfaceTmp;
                message->GetObj((IInterface**)&interfaceTmp);
                AutoPtr<ISomeArgs> args = ISomeArgs::Probe(interfaceTmp);

                interfaceTmp = NULL;
                args->GetObjectArg(1, (IInterface**)&interfaceTmp);
                AutoPtr<IObject> objTmp = IObject::Probe(interfaceTmp);
                AutoPtr<AppWidgetServiceImpl::Host> host = (AppWidgetServiceImpl::Host*)objTmp.Get();

                interfaceTmp = NULL;
                args->GetObjectArg(2, (IInterface**)&interfaceTmp);
                AutoPtr<IIAppWidgetHost> callbacks = IIAppWidgetHost::Probe(interfaceTmp);

                Int32 appWidgetId = 0, viewId = 0;
                args->GetInt32Arg(1, &appWidgetId);
                args->GetInt32Arg(2, &viewId);
                args->Recycle();
                mOwner->HandleNotifyAppWidgetViewDataChanged(host, callbacks, appWidgetId, viewId);
            }
            break;
    }
    return NOERROR;
}

//=====================================================================
//                 AppWidgetServiceImpl::SecurityPolicy
//=====================================================================
AppWidgetServiceImpl::SecurityPolicy::SecurityPolicy(
    /* [in] */ AppWidgetServiceImpl* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

Boolean AppWidgetServiceImpl::SecurityPolicy::IsEnabledGroupProfile(
    /* [in] */ Int32 profileId)
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 parentId = 0;
    helper->GetCallingUserId(&parentId);
    return IsParentOrProfile(parentId, profileId) && IsProfileEnabled(profileId);
}

ECode AppWidgetServiceImpl::SecurityPolicy::GetEnabledGroupProfileIds(
    /* [in] */ Int32 userId,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 parentId = 0;
    GetGroupParent(userId, &parentId);

    AutoPtr<IList> profiles;
    Int64 identity = Binder::ClearCallingIdentity();
    //try {
        mOwner->mUserManager->GetProfiles(parentId, (IList**)&profiles);
    //} finally {
        Binder::RestoreCallingIdentity(identity);
    //}

    Int32 enabledProfileCount = 0;
    Int32 profileCount = 0;
    profiles->GetSize(&profileCount);

    AutoPtr<IInterface> interfaceTmp;
    IUserInfo* infoTmp = NULL;
    Boolean resTmp;
    for (Int32 i = 0; i < profileCount; ++i) {
        interfaceTmp = NULL;
        profiles->Get(i, (IInterface**)&interfaceTmp);
        infoTmp = IUserInfo::Probe(interfaceTmp);
        infoTmp->IsEnabled(&resTmp);
        if (resTmp) {
            ++enabledProfileCount;
        }
    }

    Int32 enabledProfileIndex = 0;
    *result = ArrayOf<Int32>::Alloc(enabledProfileCount);
    IUserInfo* profile = NULL;
    for (Int32 i = 0; i < profileCount; ++i) {
        interfaceTmp = NULL;
        profiles->Get(i, (IInterface**)&interfaceTmp);
        profile = IUserInfo::Probe(interfaceTmp);
        profile->IsEnabled(&resTmp);
        if (resTmp) {
            AutoPtr<IUserHandle> userHandleTmp;
            profile->GetUserHandle((IUserHandle**)&userHandleTmp);
            Int32 identify = 0;
            userHandleTmp->GetIdentifier(&identify);
            (*result)->Set(enabledProfileIndex, identify);
            ++enabledProfileIndex;
        }
    }
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AppWidgetServiceImpl::SecurityPolicy::EnforceServiceExistsAndRequiresBindRemoteViewsPermission(
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 userId)
{
    VALIDATE_NOT_NULL(componentName);

    Int64 identity = Binder::ClearCallingIdentity();
    //try {
        AutoPtr<IServiceInfo> serviceInfo;
        mOwner->mPackageManager->GetServiceInfo(componentName,
                IPackageManager::GET_PERMISSIONS, userId, (IServiceInfo**)&serviceInfo);
        if (serviceInfo == NULL) {
            //throw new SecurityException("Service " + componentName
            //        + " not installed for user " + userId);
            return E_SECURITY_EXCEPTION;
        }

        String permission;
        serviceInfo->GetPermission(&permission);
        if (!Manifest::permission::BIND_REMOTEVIEWS.Equals(permission)) {
            //throw new SecurityException("Service " + componentName
            //        + " in user " + userId + "does not require "
            //        + android.Manifest.permission.BIND_REMOTEVIEWS);
            return E_SECURITY_EXCEPTION;
        }
    //} catch (RemoteException re) {
        // Local call - shouldn't happen.
    //} finally {
        Binder::RestoreCallingIdentity(identity);
    //}
    return NOERROR;
}

ECode AppWidgetServiceImpl::SecurityPolicy::EnforceModifyAppWidgetBindPermissions(
    /* [in] */ const String& packageName)
{
    mOwner->mContext->EnforceCallingPermission(
            Manifest::permission::MODIFY_APPWIDGET_BIND_PERMISSIONS,
            String("hasBindAppWidgetPermission packageName=") + packageName);
    return NOERROR;
}

ECode AppWidgetServiceImpl::SecurityPolicy::EnforceCallFromPackage(
    /* [in] */ const String& packageName)
{
    mOwner->mAppOpsManager->CheckPackage(Binder::GetCallingUid(), packageName);
    return NOERROR;
}

ECode AppWidgetServiceImpl::SecurityPolicy::HasCallerBindPermissionOrBindWhiteListedLocked(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    //try {
        mOwner->mContext->EnforceCallingOrSelfPermission(
                Manifest::permission::BIND_APPWIDGET, String(NULL));
    //} catch (SecurityException se) {
        //if (!isCallerBindAppWidgetWhiteListedLocked(packageName)) {
        //    return false;
        //}
    //}
    *result = TRUE;
    return NOERROR;
}

ECode AppWidgetServiceImpl::SecurityPolicy::CanAccessAppWidget(
    /* [in] */ Widget* widget,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean resTmp = FALSE;
    IsHostInPackageForUid(widget->mHost, uid, packageName, &resTmp);
    if (resTmp) {
        // Apps hosting the AppWidget have access to it.
        *result = TRUE;
        return NOERROR;
    }

    IsProviderInPackageForUid(widget->mProvider, uid, packageName, &resTmp);
    if (resTmp) {
        // Apps providing the AppWidget have access to it.
        *result = TRUE;
        return NOERROR;
    }

    IsHostAccessingProvider(widget->mHost, widget->mProvider, uid, packageName, &resTmp);
    if (resTmp) {
        // Apps hosting the AppWidget get to bind to a remote view service in the provider.
        *result = TRUE;
        return NOERROR;
    }

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetUserId(uid, &userId);

    Int32 userIdTmp = 0;
    widget->mHost->GetUserId(&userIdTmp);
    Int32 userIdTmp1 = 0;
    widget->mProvider->GetUserId(&userIdTmp1);
    Int32 permissionTmp = 0;
    mOwner->mContext->CheckCallingPermission(Manifest::permission::BIND_APPWIDGET, &permissionTmp);
    if ((userIdTmp == userId || (widget->mProvider != NULL && userIdTmp1 == userId))
        && permissionTmp == IPackageManager::PERMISSION_GRANTED) {
        // Apps that run in the same user as either the host or the provider and
        // have the bind widget permission have access to the widget.
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode AppWidgetServiceImpl::SecurityPolicy::IsProviderInCallerOrInProfileAndWhitelListed(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 profileId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 callerId = 0;
    helper->GetCallingUserId(&callerId);
    if (profileId == callerId) {
        *result = TRUE;
        return NOERROR;
    }

    Int32 parentId = 0;
    GetProfileParent(profileId, &parentId);
    if (parentId != callerId) {
        *result = FALSE;
        return NOERROR;
    }
    return IsProviderWhitelListed(packageName, profileId, result);
}

ECode AppWidgetServiceImpl::SecurityPolicy::IsProviderWhitelListed(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 profileId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    assert(0);
    AutoPtr<IInterface> interfaceTmp;// = LocalServices::GetService(EIID_IDevicePolicyManagerInternal);
    //IDevicePolicyManagerInternal* devicePolicyManager = IDevicePolicyManagerInternal::Probe(interfaceTmp);

    // If the policy manager is not available on the device we deny it all.
    if (/*devicePolicyManager == */NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IDevicePolicyManager> policyManager;
    //CDevicePolicyManager::New((IDevicePolicyManager**)&policyManager);
    AutoPtr<IList> crossProfilePackages;
    //devicePolicyManager->GetCrossProfileWidgetProviders(profileId, (IList**)&crossProfilePackages);

    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(packageName, (ICharSequence**)&charSequenceTmp);
    return crossProfilePackages->Contains(TO_IINTERFACE(charSequenceTmp), result);
}

ECode AppWidgetServiceImpl::SecurityPolicy::GetProfileParent(
    /* [in] */ Int32 profileId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int64 identity = Binder::ClearCallingIdentity();
    //try {
        AutoPtr<IUserInfo> parent;
        mOwner->mUserManager->GetProfileParent(profileId, (IUserInfo**)&parent);
        if (parent != NULL) {
            AutoPtr<IUserHandle> userHandle;
            parent->GetUserHandle((IUserHandle**)&userHandle);
            return userHandle->GetIdentifier(result);
        }
    //} finally {
        Binder::RestoreCallingIdentity(identity);
    //}
    *result = UNKNOWN_USER_ID;
    return NOERROR;
}

ECode AppWidgetServiceImpl::SecurityPolicy::GetGroupParent(
    /* [in] */ Int32 profileId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 parentId = 0;
    mOwner->mSecurityPolicy->GetProfileParent(profileId, &parentId);
    *result = (parentId != UNKNOWN_USER_ID) ? parentId : profileId;
    return NOERROR;
}

ECode AppWidgetServiceImpl::SecurityPolicy::IsHostInPackageForUid(
    /* [in] */ Host* host,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = host->mId->mUid == uid && host->mId->mPackageName.Equals(packageName);
    return NOERROR;
}

ECode AppWidgetServiceImpl::SecurityPolicy::IsProviderInPackageForUid(
    /* [in] */ Provider* provider,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // Packages providing the AppWidget have access to it.
    String packageNameTmp;
    provider->mId->mComponentName->GetPackageName(&packageNameTmp);
    *result = provider != NULL && provider->mId->mUid == uid
            && packageNameTmp.Equals(packageName);
    return NOERROR;
}

ECode AppWidgetServiceImpl::SecurityPolicy::IsHostAccessingProvider(
    /* [in] */ Host* host,
    /* [in] */ Provider* provider,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // The host creates a package context to bind to remote views service in the provider.
    String packageNameTmp;
    provider->mId->mComponentName->GetPackageName(&packageNameTmp);
    *result = host->mId->mUid == uid && provider != NULL
            && packageNameTmp.Equals(packageName);
    return NOERROR;
}

ECode AppWidgetServiceImpl::SecurityPolicy::IsCallerBindAppWidgetWhiteListedLocked(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    Int32 packageUid = mOwner->GetUidForPackage(packageName, userId);
    if (packageUid < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    {
        AutoLock syncLock(mOwner->mLock);
        mOwner->EnsureGroupStateLoadedLocked(userId);
        AutoPtr<IPairHelper> pairHelper;
        CPairHelper::AcquireSingleton((IPairHelper**)&pairHelper);

        AutoPtr<IInteger32> userIdTmp = CoreUtils::Convert(userId);
        AutoPtr<ICharSequence> charSequenceTmp;
        CString::New(packageName, (ICharSequence**)&charSequenceTmp);
        AutoPtr<IPair> packageId;
        pairHelper->Create(TO_IINTERFACE(userIdTmp), TO_IINTERFACE(charSequenceTmp), (IPair**)&packageId);

        Boolean resTmp = FALSE;
        ICollection::Probe(mOwner->mPackagesWithBindWidgetPermission)->Contains(packageId, &resTmp);
        if (resTmp) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

Boolean AppWidgetServiceImpl::SecurityPolicy::IsParentOrProfile(
    /* [in] */ Int32 parentId,
    /* [in] */ Int32 profileId)
{
    if (parentId == profileId) {
        return TRUE;
    }

    Int32 intTmp = 0;
    GetProfileParent(profileId, &intTmp);
    return intTmp == parentId;
}

Boolean AppWidgetServiceImpl::SecurityPolicy::IsProfileEnabled(
    /* [in] */ Int32 profileId)
{
    Int64 identity = Binder::ClearCallingIdentity();
    //try {
        AutoPtr<IUserInfo> userInfo;
        mOwner->mUserManager->GetUserInfo(profileId, (IUserInfo**)&userInfo);
        Boolean resTmp = FALSE;
        userInfo->IsEnabled(&resTmp);
        if (userInfo == NULL || !resTmp) {
            return FALSE;
        }
    //} finally {
        Binder::RestoreCallingIdentity(identity);
    //}
    return TRUE;
}

//=====================================================================
//                   AppWidgetServiceImpl::ProviderId
//=====================================================================
ECode AppWidgetServiceImpl::ProviderId::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (TO_IINTERFACE(this) == TO_IINTERFACE(obj)) {
        *result = TRUE;
        return NOERROR;
    }
    if (obj == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    //if (GetClass() != obj.getClass()) {
    //    return false;
    //}

    ProviderId* other = (ProviderId*)IObject::Probe(obj);
    if (this->mUid != other->mUid)  {
        *result = FALSE;
        return NOERROR;
    }

    Boolean equals = 0;
    if (mComponentName == NULL) {
        if (other->mComponentName != NULL) {
            *result = FALSE;
            return NOERROR;
        }
    }
    else if (IObject::Probe(mComponentName)->Equals(other->mComponentName, &equals), !equals) {
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode AppWidgetServiceImpl::ProviderId::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 resultTmp = mUid;
    IObject* objTmp = IObject::Probe(mComponentName);
    Int32 hashCode = 0;
    objTmp->GetHashCode(&hashCode);
    resultTmp = 31 * resultTmp + ((mComponentName != NULL) ? hashCode : 0);
    *result = resultTmp;
    return NOERROR;
}

ECode AppWidgetServiceImpl::ProviderId::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetUserId(mUid, &userId);
    Int32 appId = 0;
    helper->GetAppId(mUid, &appId);
    String strComponentName;
    mComponentName->ToString(&strComponentName);
    StringBuilder sb;
    sb += "ProviderId{user:";
    sb += userId;
    sb += ", app:";
    sb += appId;
    sb += ", cmp:";
    sb += strComponentName;
    sb += "}";
    sb.ToString(result);
    return NOERROR;
}

AppWidgetServiceImpl::ProviderId::ProviderId(
    /* [in] */ Int32 uid,
    /* [in] */ IComponentName* componentName)
    : mUid(uid)
    , mComponentName(componentName)
{
}

//=====================================================================
//                    AppWidgetServiceImpl::Provider
//=====================================================================

AppWidgetServiceImpl::Provider::Provider()
    : mZombie(FALSE)
    , mTag(0)
{
    CArrayList::New((IList**)&mWidgets);
}

ECode AppWidgetServiceImpl::Provider::GetUserId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    return helper->GetUserId(mId->mUid, result);
}

ECode AppWidgetServiceImpl::Provider::IsInPackageForUser(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 userIdTmp = 0;
    GetUserId(&userIdTmp);
    String packageNameTmp;
    mId->mComponentName->GetPackageName(&packageNameTmp);
    *result = userIdTmp == userId && packageNameTmp.Equals(packageName);
    return NOERROR;
}

ECode AppWidgetServiceImpl::Provider::HostedByPackageForUser(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 N = 0;
    mWidgets->GetSize(&N);
    AutoPtr<IInterface> interfaceTmp;
    Int32 userIdTmp = 0;
    IObject* objTmp = NULL;
    Widget* widget = NULL;
    for (Int32 i = 0; i < N; ++i) {
        interfaceTmp = NULL;
        mWidgets->Get(i, (IInterface**)&interfaceTmp);
        objTmp = IObject::Probe(interfaceTmp);
        widget = (Widget*)objTmp;
        widget->mHost->GetUserId(&userIdTmp);
        if (packageName.Equals(widget->mHost->mId->mPackageName)
                && userIdTmp == userId) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode AppWidgetServiceImpl::Provider::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    String strIdTmp;
    mId->ToString(&strIdTmp);
    *result = String("Provider{") + strIdTmp + (mZombie ? String(" Z") : String("")) + String("}");
    return NOERROR;
}

//=====================================================================
//                      AppWidgetServiceImpl::Host
//=====================================================================

AppWidgetServiceImpl::Host::Host()
    : mZombie(FALSE)
{
    CArrayList::New((IList**)&mWidgets);
    mTag = TAG_UNDEFINED; // for use while saving state (the index)
}

ECode AppWidgetServiceImpl::Host::GetUserId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    return helper->GetUserId(mId->mUid, result);
}

ECode AppWidgetServiceImpl::Host::IsInPackageForUser(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 userIdTmp = 0;
    GetUserId(&userIdTmp);
    *result = userIdTmp == userId && mId->mPackageName.Equals(packageName);
    return NOERROR;
}

ECode AppWidgetServiceImpl::Host::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    String strIdTmp;
    mId->ToString(&strIdTmp);
    *result = String("Host{") + strIdTmp + (mZombie ? String(" Z") : String("")) + String("}");
    return NOERROR;
}

ECode AppWidgetServiceImpl::Host::HostsPackageForUser(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 N = 0;
    mWidgets->GetSize(&N);
    AutoPtr<IInterface> interfaceTmp;
    IObject* objTmp = NULL;
    Provider* provider = NULL;
    Int32 userIdTmp = 0;
    provider->GetUserId(&userIdTmp);
    String packageNameTmp;
    for (Int32 i = 0; i < N; ++i) {
        interfaceTmp = NULL;
        mWidgets->Get(i, (IInterface**)&interfaceTmp);
        objTmp = IObject::Probe(interfaceTmp);
        provider = (Provider*)objTmp;

        AutoPtr<IComponentName> providerTmp;
        provider->mInfo->GetProvider((IComponentName**)&providerTmp);
        providerTmp->GetPackageName(&packageNameTmp);
        if (provider != NULL && userIdTmp == userId && provider->mInfo != NULL
                && pkg.Equals(packageNameTmp)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

//=====================================================================
//                     AppWidgetServiceImpl::HostId
//=====================================================================
AppWidgetServiceImpl::HostId::HostId(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 hostId,
    /* [in] */ const String& packageName)
    : mUid(uid)
    , mHostId(hostId)
    , mPackageName(packageName)
{
}

ECode AppWidgetServiceImpl::HostId::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(obj);
    VALIDATE_NOT_NULL(result);

    if (TO_IINTERFACE(this)== TO_IINTERFACE(obj)) {
        *result = TRUE;
        return NOERROR;
    }
    if (obj == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    //if (getClass() != obj.getClass()) {
    //    return false;
    //}

    IObject* objTmp = IObject::Probe(obj);
    HostId* other = (HostId*)objTmp;
    if (mUid != other->mUid)  {
        *result = FALSE;
        return NOERROR;
    }
    if (mHostId != other->mHostId) {
        *result = FALSE;
        return NOERROR;
    }
    if (mPackageName == NULL) {
        if (other->mPackageName != NULL) {
            *result = FALSE;
            return NOERROR;
        }
    }
    else if (!mPackageName.Equals(other->mPackageName)) {
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode AppWidgetServiceImpl::HostId::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 resultTmp = mUid;
    resultTmp = 31 * resultTmp + mHostId;
    resultTmp = 31 * resultTmp + ((mPackageName != NULL)
            ? mPackageName.GetHashCode() : 0);
    *result = resultTmp;
    return NOERROR;
}

ECode AppWidgetServiceImpl::HostId::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userIdTmp = 0;
    helper->GetUserId(mUid, &userIdTmp);
    Int32 appIdTmp = 0;
    helper->GetAppId(mUid, &appIdTmp);
    StringBuilder sb;
    sb += "HostId{user:";
    sb += userIdTmp;
    sb += ", app:";
    sb += appIdTmp;
    sb += ", hostId:";
    sb += mHostId;
    sb += ", pkg:";
    sb += mPackageName;
    sb += "}";
    sb.ToString(result);
    return NOERROR;
}

//=====================================================================
//                     AppWidgetServiceImpl::Widget
//=====================================================================
ECode AppWidgetServiceImpl::Widget::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder sb;
    sb += "AppWidgetId{";
    sb += mAppWidgetId;
    sb += ":";
    sb += mHost;
    sb += ":";
    sb += mProvider;
    sb += "}";
    sb.ToString(result);
    return NOERROR;
}

//=====================================================================
//             AppWidgetServiceImpl::ServiceConnectionProxy
//=====================================================================
CAR_INTERFACE_IMPL(AppWidgetServiceImpl::ServiceConnectionProxy, Object, IServiceConnection)

AppWidgetServiceImpl::ServiceConnectionProxy::ServiceConnectionProxy(
    /* [in] */ IBinder* connectionCb)
{
    mConnectionCb = IIRemoteViewsAdapterConnection::Probe(connectionCb);
}

ECode AppWidgetServiceImpl::ServiceConnectionProxy::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    //try {
        mConnectionCb->OnServiceConnected(service);
    //} catch (RemoteException re) {
        //Slog.e(TAG, "Error passing service interface", re);
    //}
    return NOERROR;
}

ECode AppWidgetServiceImpl::ServiceConnectionProxy::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    Disconnect();
    return NOERROR;
}

ECode AppWidgetServiceImpl::ServiceConnectionProxy::Disconnect()
{
    //try {
        mConnectionCb->OnServiceDisconnected();
    //} catch (RemoteException re) {
        //Slog.e(TAG, "Error clearing service interface", re);
    //}
    return NOERROR;
}

//=====================================================================
//               AppWidgetServiceImpl::SaveStateRunnable
//=====================================================================
CAR_INTERFACE_IMPL(AppWidgetServiceImpl::SaveStateRunnable, Object, IRunnable)

AppWidgetServiceImpl::SaveStateRunnable::SaveStateRunnable(
    /* [in] */ Int32 userId,
    /* [in] */ AppWidgetServiceImpl* owner)
    : mUserId(userId)
    , mOwner(owner)
{
    assert(NULL != mOwner);
}

ECode AppWidgetServiceImpl::SaveStateRunnable::Run()
{
    AutoLock syncLock(mOwner->mLock);
    mOwner->EnsureGroupStateLoadedLocked(mUserId);
    mOwner->SaveStateLocked(mUserId);
    return NOERROR;
}

//=====================================================================
//  AppWidgetServiceImpl::BackupRestoreController::RestoreUpdateRecord
//=====================================================================
AppWidgetServiceImpl::BackupRestoreController::RestoreUpdateRecord::RestoreUpdateRecord(
    /* [in] */ Int32 theOldId,
    /* [in] */ Int32 theNewId)
    : mOldId(theOldId)
    , mNewId(theNewId)
    , mNotified(FALSE)
{
}

//=====================================================================
//            AppWidgetServiceImpl::BackupRestoreController
//=====================================================================
const String AppWidgetServiceImpl::BackupRestoreController::TAG("BackupRestoreController");
const Boolean AppWidgetServiceImpl::BackupRestoreController::DEBUG = TRUE;
const Int32 AppWidgetServiceImpl::BackupRestoreController::WIDGET_STATE_VERSION;

AppWidgetServiceImpl::BackupRestoreController::BackupRestoreController(
    /* [in] */ AppWidgetServiceImpl* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

ECode AppWidgetServiceImpl::BackupRestoreController::GetWidgetParticipants(
    /* [in] */ Int32 userId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG) {
        Slogger::I(TAG, "Getting widget participants for user: %d", userId);
    }

    CArrayList::New(result);
    AutoLock syncLock(mOwner->mLock);
    Int32 N = 0;
    mOwner->mWidgets->GetSize(&N);

    AutoPtr<IInterface> interfaceTmp;
    IObject* objTmp = NULL;
    Widget* widget = NULL;
    AutoPtr<ICharSequence> charSequenceTmp;
    Provider* provider = NULL;
    String strTmp;

    for (Int32 i = 0; i < N; ++i) {
        interfaceTmp = NULL;
        mOwner->mWidgets->Get(i, (IInterface**)&interfaceTmp);
        objTmp = IObject::Probe(interfaceTmp);
        widget = (Widget*)objTmp;

        // Skip cross-user widgets.
        if (!IsProviderAndHostInUser(widget, userId)) {
            continue;
        }

        charSequenceTmp = NULL;
        CString::New(widget->mHost->mId->mPackageName, (ICharSequence**)&charSequenceTmp);
        (*result)->Add(TO_IINTERFACE(charSequenceTmp));
        provider = widget->mProvider;
        if (provider != NULL) {
            provider->mId->mComponentName->GetPackageName(&strTmp);
            charSequenceTmp = NULL;
            CString::New(strTmp, (ICharSequence**)&charSequenceTmp);
            (*result)->Add(TO_IINTERFACE(charSequenceTmp));
        }
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::BackupRestoreController::GetWidgetState(
    /* [in] */ const String& backedupPackage,
    /* [in] */ Int32 userId,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG) {
        Slogger::I(TAG, "Getting widget state for user: %d", userId);
    }

    AutoPtr<IByteArrayOutputStream> stream;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&stream);

    {
        AutoLock syncLock(mOwner->mLock);
        if (!PackageNeedsWidgetBackupLocked(backedupPackage, userId)) {
            *result = NULL;
            return NOERROR;
        }

        //try {
            AutoPtr<IXmlSerializer> out;
            CFastXmlSerializer::New((IXmlSerializer**)&out);
            out->SetOutput(IOutputStream::Probe(stream), String("utf-8"));
            out->StartDocument(String(NULL), TRUE);
            out->WriteStartTag(String(NULL), String("ws"));
            out->WriteAttribute(String(NULL), String("version"), StringUtils::ToString(WIDGET_STATE_VERSION));
            out->WriteAttribute(String(NULL), String("pkg"), backedupPackage);

            Int32 index = 0;
            Int32 N = 0;
            mOwner->mProviders->GetSize(&N);

            AutoPtr<IInterface> interfaceTmp;
            IObject* objTmp = NULL;
            Provider* provider = NULL;
            Boolean boolTmp = FALSE;
            Boolean boolTmp1 = FALSE;
            Boolean boolTmp2 = FALSE;

            for (Int32 i = 0; i < N; ++i) {
                interfaceTmp = NULL;
                mOwner->mProviders->Get(i, (IInterface**)&interfaceTmp);
                objTmp = IObject::Probe(interfaceTmp);
                provider = (Provider*)objTmp;
                provider->mWidgets->IsEmpty(&boolTmp);
                provider->IsInPackageForUser(backedupPackage, userId, &boolTmp1);
                provider->HostedByPackageForUser(backedupPackage, userId, &boolTmp2);
                if (!boolTmp && (boolTmp1 || boolTmp2)) {
                    provider->mTag = index;
                    SerializeProvider(out, provider);
                    ++index;
                }
            }

            Host* host = NULL;
            mOwner->mHosts->GetSize(&N);
            index = 0;
            for (Int32 i = 0; i < N; ++i) {
                interfaceTmp = NULL;
                mOwner->mHosts->Get(i, (IInterface**)&interfaceTmp);
                objTmp = IObject::Probe(interfaceTmp);
                host = (Host*)objTmp;
                host->mWidgets->IsEmpty(&boolTmp);
                host->IsInPackageForUser(backedupPackage, userId, &boolTmp1);
                host->HostsPackageForUser(backedupPackage, userId, &boolTmp2);
                if (!boolTmp && (boolTmp1 || boolTmp2)) {
                    host->mTag = index;
                    SerializeHost(out, host);
                    ++index;
                }
            }

            Widget* widget = NULL;
            mOwner->mWidgets->GetSize(&N);
            for (Int32 i = 0; i < N; ++i) {
                interfaceTmp = NULL;
                mOwner->mWidgets->Get(i, (IInterface**)&interfaceTmp);
                objTmp = IObject::Probe(interfaceTmp);
                widget = (Widget*)objTmp;
                provider = widget->mProvider;
                widget->mHost->IsInPackageForUser(backedupPackage, userId, &boolTmp);
                provider->IsInPackageForUser(backedupPackage, userId, &boolTmp1);
                if (boolTmp || (provider != NULL &&  boolTmp1)) {
                    SerializeAppWidget(out, widget);
                }
            }

            out->WriteEndTag(String(NULL), String("ws"));
            out->EndDocument();
        //} catch (IOException e) {
            //Slog.w(TAG, "Unable to save widget state for " + backedupPackage);
            //return null;
        //}
    }

    stream->ToByteArray(result);
    return NOERROR;
}

ECode AppWidgetServiceImpl::BackupRestoreController::RestoreStarting(
    /* [in] */ Int32 userId)
{
    if (DEBUG) {
        Slogger::I(TAG, "Restore starting for user: %d", userId);
    }

    {
        AutoLock syncLock(mOwner->mLock);
        // We're starting a new "system" restore operation, so any widget restore
        // state that we see from here on is intended to replace the current
        // widget configuration of any/all of the affected apps.
        mPrunedApps->Clear();
        mUpdatesByProvider->Clear();
        mUpdatesByHost->Clear();
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::BackupRestoreController::RestoreWidgetState(
    /* [in] */ const String& packageName,
    /* [in] */ ArrayOf<Byte>* restoredState,
    /* [in] */ Int32 userId)
{
    VALIDATE_NOT_NULL(restoredState);

    if (DEBUG) {
        Slogger::I(TAG, "Restoring widget state for user:%d package: %s", userId, packageName.string());
    }

    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(restoredState, (IByteArrayInputStream**)&stream);
    //try {
        // Providers mentioned in the widget dataset by ordinal
        AutoPtr<IList> restoredProviders;
        CArrayList::New((IList**)&restoredProviders);

        // Hosts mentioned in the widget dataset by ordinal
        AutoPtr<IList> restoredHosts;
        CArrayList::New((IList**)&restoredHosts);

        AutoPtr<IXmlPullParser> parser;
        Xml::NewPullParser((IXmlPullParser**)&parser);
        parser->SetInput(IInputStream::Probe(stream), String(NULL));

        {
            AutoLock syncLock(mOwner->mLock);
            Int32 type = 0;
            do {
                parser->Next(&type);
                if (type == IXmlPullParser::START_TAG) {
                    String tag;
                    parser->GetName(&tag);
                    if (String("ws").Equals(tag)) {
                        String version;
                        parser->GetAttributeValue(String(NULL), String("version"), &version);
                        Int32 versionNumber = StringUtils::ParseInt32(version);
                        if (versionNumber > WIDGET_STATE_VERSION) {
                            Slogger::W(TAG, "Unable to process state version %s", version.string());
                            return NOERROR;
                        }

                        String pkg;
                        parser->GetAttributeValue(String(NULL), String("pkg"), &pkg);
                        if (!packageName.Equals(pkg)) {
                            Slogger::W(TAG, "Package mismatch in ws");
                            return NOERROR;
                        }
                    }
                    else if (String("p").Equals(tag)) {
                        String pkg;
                        parser->GetAttributeValue(String(NULL), String("pkg"), &pkg);
                        String cl;
                        parser->GetAttributeValue(String(NULL), String("cl"), &cl);

                        AutoPtr<IComponentName> componentName;
                        CComponentName::New(pkg, cl, (IComponentName**)&componentName);

                        AutoPtr<Provider> p = FindProviderLocked(componentName, userId);
                        if (p == NULL) {
                            p = new Provider();
                            p->mId = new ProviderId(UNKNOWN_UID, componentName);
                            CAppWidgetProviderInfo::New((IAppWidgetProviderInfo**)&(p->mInfo));
                            p->mInfo->SetProvider(componentName);
                            p->mZombie = TRUE;
                            mOwner->mProviders->Add(TO_IINTERFACE(p));
                        }
                        if (DEBUG) {
                            Slogger::I(TAG, "   provider %s", TO_CSTR(p->mId));
                        }
                        restoredProviders->Add(TO_IINTERFACE(p));
                    }
                    else if (String("h").Equals(tag)) {
                        String pkg;
                        parser->GetAttributeValue(String(NULL), String("pkg"), &pkg);
                        Int32 uid = mOwner->GetUidForPackage(pkg, userId);

                        String strTmp;
                        parser->GetAttributeValue(String(NULL), String("id"), &strTmp);
                        Int32 hostId = StringUtils::ParseInt32(strTmp, 16);

                        AutoPtr<HostId> id = new HostId(uid, hostId, pkg);
                        AutoPtr<Host> h = mOwner->LookupOrAddHostLocked(id);
                        restoredHosts->Add(TO_IINTERFACE(h));

                        if (DEBUG) {
                            Int32 size = 0;
                            restoredHosts->GetSize(&size);
                            h->mId->ToString(&strTmp);
                            Slogger::I(TAG, "   host[%d]: {%s}", size, TO_CSTR(h->mId));
                        }
                    }
                    else if (String("g").Equals(tag)) {
                        String strTmp;
                        parser->GetAttributeValue(String(NULL), String("id"), &strTmp);
                        Int32 restoredId = StringUtils::ParseInt32(strTmp, 16);

                        parser->GetAttributeValue(String(NULL), String("h"), &strTmp);
                        Int32 hostIndex = StringUtils::ParseInt32(strTmp, 16);

                        AutoPtr<IInterface> interfaceTmp;
                        restoredHosts->Get(hostIndex, (IInterface**)&interfaceTmp);
                        IObject* objTmp = IObject::Probe(interfaceTmp);
                        Host* host = (Host*)objTmp;
                        Provider* p = NULL;
                        String prov;
                        parser->GetAttributeValue(String(NULL), String("p"), &prov);
                        if (!prov.IsEmpty()) {
                            Int32 which = StringUtils::ParseInt32(prov, 16);
                            interfaceTmp = NULL;
                            restoredProviders->Get(which, (IInterface**)&interfaceTmp);
                            objTmp = IObject::Probe(interfaceTmp);
                            p = (Provider*)objTmp;
                        }

                        PruneWidgetStateLocked(host->mId->mPackageName, userId);
                        if (p != NULL) {
                            String packageName;
                            p->mId->mComponentName->GetPackageName(&packageName);
                            PruneWidgetStateLocked(packageName, userId);
                        }

                        AutoPtr<Widget> id = FindRestoredWidgetLocked(restoredId, host, p);
                        if (id == NULL) {
                            id = new Widget();
                            id->mAppWidgetId = mOwner->IncrementAndGetAppWidgetIdLocked(userId);
                            id->mRestoredId = restoredId;
                            id->mOptions = ParseWidgetIdOptions(parser);
                            id->mHost = host;
                            id->mHost->mWidgets->Add(TO_IINTERFACE(id));
                            id->mProvider = p;
                            if (id->mProvider != NULL) {
                                id->mProvider->mWidgets->Add(TO_IINTERFACE(id));
                            }
                            if (DEBUG) {
                                Slogger::I(TAG, "New restored id %d now %s", restoredId, TO_CSTR(id));
                            }
                            mOwner->mWidgets->Add(TO_IINTERFACE(id));
                        }
                        if (id->mProvider->mInfo != NULL) {
                            StashProviderRestoreUpdateLocked(id->mProvider, restoredId, id->mAppWidgetId);
                        }
                        else {
                            Slogger::W(TAG, "Missing provider for restored widget %s", TO_CSTR(id));
                        }
                        StashHostRestoreUpdateLocked(id->mHost, restoredId, id->mAppWidgetId);

                        if (DEBUG) {
                            Slogger::I(TAG, "   instance: %d -> %d :: p=%s", restoredId,
                                id->mAppWidgetId, TO_CSTR(id->mProvider));
                        }
                    }
                }
            } while (type != IXmlPullParser::END_DOCUMENT);
        }
    //} catch (XmlPullParserException | IOException e) {
        //Slog.w(TAG, "Unable to restore widget state for " + packageName);
    //} finally {
        mOwner->SaveGroupStateAsync(userId);
    //}
    return NOERROR;
}

ECode AppWidgetServiceImpl::BackupRestoreController::RestoreFinished(
    /* [in] */ Int32 userId)
{
    if (DEBUG) {
        Slogger::I(TAG, "restoreFinished for %d", userId);
    }

    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(userId, (IUserHandle**)&userHandle);
    {
        AutoLock syncLock(mOwner->mLock);
        AutoPtr<ISet> providerEntries;
        mUpdatesByProvider->GetEntrySet((ISet**)&providerEntries);
        AutoPtr<IIterator> itor;
        providerEntries->GetIterator((IIterator**)&itor);
        Boolean hasNext = FALSE;
        while (itor->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> interfaceTmp;
            itor->GetNext((IInterface**)&interfaceTmp);

            AutoPtr<IPair> pair = IPair::Probe(interfaceTmp);
            AutoPtr<IInterface> keyInterfaceTmp;
            pair->GetFirst((IInterface**)&keyInterfaceTmp);
            AutoPtr<IInterface> valueInterfaceTmp;
            pair->GetSecond((IInterface**)&valueInterfaceTmp);

            IObject* keyObjTmp = IObject::Probe(keyInterfaceTmp);
            Provider* provider = (Provider*)keyObjTmp;
            IList* updates = IList::Probe(valueInterfaceTmp);
            Int32 pending = CountPendingUpdates(updates);
            if (DEBUG) {
                Slogger::I(TAG, "Provider %s pending: %d", TO_CSTR(provider), pending);
            }
            if (pending > 0) {
                AutoPtr< ArrayOf<Int32> > oldIds = ArrayOf<Int32>::Alloc(pending);
                AutoPtr< ArrayOf<Int32> > newIds = ArrayOf<Int32>::Alloc(pending);
                Int32 N = 0;
                updates->GetSize(&N);
                Int32 nextPending = 0;
                for (Int32 i = 0; i < N; ++i) {
                    AutoPtr<IInterface> interfaceItemTmp;
                    updates->Get(i, (IInterface**)&interfaceItemTmp);
                    IObject* objItemTmp = IObject::Probe(interfaceItemTmp);
                    RestoreUpdateRecord* r = (RestoreUpdateRecord*)objItemTmp;
                    if (!r->mNotified) {
                        r->mNotified = TRUE;
                        oldIds->Set(nextPending, r->mOldId);
                        newIds->Set(nextPending, r->mNewId);
                        ++nextPending;
                        if (DEBUG) {
                            Slogger::I(TAG, "   %d => %d", r->mOldId, r->mNewId);
                        }
                    }
                }
                SendWidgetRestoreBroadcastLocked(
                        IAppWidgetManager::ACTION_APPWIDGET_RESTORED,
                        provider, NULL, oldIds, newIds, userHandle);
            }
        }

        AutoPtr<ISet> hostEntries;
        mUpdatesByHost->GetEntrySet((ISet**)&hostEntries);
        itor = NULL;
        hostEntries->GetIterator((IIterator**)&itor);
        while (itor->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> interfaceTmp;
            itor->GetNext((IInterface**)&interfaceTmp);

            AutoPtr<IPair> pair = IPair::Probe(interfaceTmp);
            AutoPtr<IInterface> keyInterfaceTmp;
            pair->GetFirst((IInterface**)&keyInterfaceTmp);
            AutoPtr<IInterface> valueInterfaceTmp;
            pair->GetSecond((IInterface**)&valueInterfaceTmp);

            IObject* keyObjTmp = IObject::Probe(keyInterfaceTmp);
            Host* host = (Host*)keyObjTmp;

            if (host->mId->mUid != UNKNOWN_UID) {
                IList* updates = IList::Probe(valueInterfaceTmp);
                Int32 pending = CountPendingUpdates(updates);
                if (DEBUG) {
                    Slogger::I(TAG, "Host %s pending: %d", TO_CSTR(host), pending);
                }
                if (pending > 0) {
                    AutoPtr< ArrayOf<Int32> > oldIds = ArrayOf<Int32>::Alloc(pending);
                    AutoPtr< ArrayOf<Int32> > newIds = ArrayOf<Int32>::Alloc(pending);

                    Int32 N = 0;
                    updates->GetSize(&N);

                    Int32 nextPending = 0;
                    for (int i = 0; i < N; ++i) {
                        AutoPtr<IInterface> interfaceItemTmp;
                        updates->Get(i, (IInterface**)&interfaceItemTmp);
                        IObject* objItemTmp = IObject::Probe(interfaceItemTmp);
                        RestoreUpdateRecord* r = (RestoreUpdateRecord*)objItemTmp;
                        if (!r->mNotified) {
                            r->mNotified = TRUE;
                            oldIds->Set(nextPending, r->mOldId);
                            newIds->Set(nextPending, r->mNewId);
                            ++nextPending;
                            if (DEBUG) {
                                Slogger::I(TAG, "   %d => %d", r->mOldId, r->mNewId);
                            }
                        }
                    }
                    SendWidgetRestoreBroadcastLocked(
                            IAppWidgetManager::ACTION_APPWIDGET_HOST_RESTORED,
                            NULL, host, oldIds, newIds, userHandle);
                }
            }
        }
    }
    return NOERROR;
}

AutoPtr<AppWidgetServiceImpl::Provider> AppWidgetServiceImpl::BackupRestoreController::FindProviderLocked(
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 userId)
{
    Int32 providerCount = 0;
    mOwner->mProviders->GetSize(&providerCount);
    for (Int32 i = 0; i < providerCount; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mOwner->mProviders->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Provider* provider = (Provider*)objTmp;
        Int32 userIdTmp = 0;
        provider->GetUserId(&userIdTmp);

        String str1, str2;
        provider->mId->mComponentName->ToString(&str1);
        componentName->ToString(&str2);
        if (userIdTmp == userId
                && str1.Equals(str2)) {
            return provider;
        }
    }
    return NULL;
}

AutoPtr<AppWidgetServiceImpl::Widget> AppWidgetServiceImpl::BackupRestoreController::FindRestoredWidgetLocked(
    /* [in] */ Int32 restoredId,
    /* [in] */ Host* host,
    /* [in] */ Provider* p)
{
    if (DEBUG) {
        String strTmp, str1Tmp;
        host->ToString(&strTmp);
        p->ToString(&str1Tmp);
        Slogger::I(TAG, "Find restored widget: id=%d, host=%s, provider=%s",
            restoredId, TO_CSTR(host), TO_CSTR(p));
    }

    if (p == NULL || host == NULL) {
        return NULL;
    }

    Int32 N = 0;
    mOwner->mWidgets->GetSize(&N);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mOwner->mWidgets->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Widget* widget = (Widget*)objTmp;
        Boolean resTmp = FALSE, res1Tmp = FALSE;
        widget->mHost->mId->Equals(TO_IINTERFACE(host->mId), &resTmp);
        widget->mProvider->mId->Equals(TO_IINTERFACE(p->mId), &res1Tmp);
        if (widget->mRestoredId == restoredId && resTmp && res1Tmp) {
            if (DEBUG) {
                Slogger::I(TAG, "   Found at %d : %s", i, TO_CSTR(widget));
            }
            return widget;
        }
    }
    return NULL;
}

Boolean AppWidgetServiceImpl::BackupRestoreController::PackageNeedsWidgetBackupLocked(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId)
{
    Int32 N = 0;
    mOwner->mWidgets->GetSize(&N);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mOwner->mWidgets->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Widget* widget = (Widget*)objTmp;

        if (!IsProviderAndHostInUser(widget, userId)) {
            continue;
        }

        Boolean resTmp = FALSE;
        widget->mHost->IsInPackageForUser(packageName, userId, &resTmp);
        if (resTmp) {
            // this package is hosting widgets, so it knows widget IDs.
            return TRUE;
        }

        Provider* provider = widget->mProvider;
        provider->IsInPackageForUser(packageName, userId, &resTmp);
        if (provider != NULL && resTmp) {
            // someone is hosting this app's widgets, so it knows widget IDs.
            return TRUE;
        }
    }
    return FALSE;
}

void AppWidgetServiceImpl::BackupRestoreController::StashProviderRestoreUpdateLocked(
    /* [in] */ Provider* provider,
    /* [in] */ Int32 oldId,
    /* [in] */ Int32 newId)
{
    AutoPtr<IInterface> valueTmp;
    mUpdatesByProvider->Get(TO_IINTERFACE(provider), (IInterface**)&valueTmp);
    AutoPtr<IList> r = IList::Probe(valueTmp);

    if (r == NULL) {
        CArrayList::New((IList**)&r);
        mUpdatesByProvider->Put(TO_IINTERFACE(provider), TO_IINTERFACE(r));
    }
    else {
        // don't duplicate
        if (AlreadyStashed(r, oldId, newId)) {
            if (DEBUG) {
                String strTmp;
                provider->ToString(&strTmp);
                Slogger::I(TAG, "ID remap %d -> %d already stashed for %s", oldId, newId, TO_CSTR(provider));
            }
            return;
        }
    }

    AutoPtr<RestoreUpdateRecord> record = new RestoreUpdateRecord(oldId, newId);
    r->Add(TO_IINTERFACE(record));
}

Boolean AppWidgetServiceImpl::BackupRestoreController::AlreadyStashed(
    /* [in] */ IList* stash,
    /* [in] */ Int32 oldId,
    /* [in] */ Int32 newId)
{
    Int32 N = 0;
    stash->GetSize(&N);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        stash->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        RestoreUpdateRecord* r = (RestoreUpdateRecord*)objTmp;
        if (r->mOldId == oldId && r->mNewId == newId) {
            return TRUE;
        }
    }
    return FALSE;
}

void AppWidgetServiceImpl::BackupRestoreController::StashHostRestoreUpdateLocked(
    /* [in] */ Host* host,
    /* [in] */ Int32 oldId,
    /* [in] */ Int32 newId)
{
    AutoPtr<IInterface> valueTmp;
    mUpdatesByHost->Get(TO_IINTERFACE(host), (IInterface**)&valueTmp);
    AutoPtr<IList> r = IList::Probe(valueTmp);
    if (r == NULL) {
        CArrayList::New((IList**)&r);
        mUpdatesByHost->Put(TO_IINTERFACE(host), TO_IINTERFACE(r));
    }
    else {
        if (AlreadyStashed(r, oldId, newId)) {
            if (DEBUG) {
                String strTmp;
                host->ToString(&strTmp);
                Slogger::I(TAG, "ID remap %d -> %d already stashed for %s", oldId, newId, TO_CSTR(host));
            }
            return;
        }
    }

    AutoPtr<RestoreUpdateRecord> record = new RestoreUpdateRecord(oldId, newId);
    r->Add(TO_IINTERFACE(record));
}

void AppWidgetServiceImpl::BackupRestoreController::SendWidgetRestoreBroadcastLocked(
    /* [in] */ const String& action,
    /* [in] */ Provider* provider,
    /* [in] */ Host* host,
    /* [in] */ ArrayOf<Int32>* oldIds,
    /* [in] */ ArrayOf<Int32>* newIds,
    /* [in] */ IUserHandle* userHandle)
{
    AutoPtr<IIntent> intent;
    CIntent::New(action, (IIntent**)&intent);
    intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_OLD_IDS, oldIds);
    intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_IDS, newIds);
    if (provider != NULL) {
        AutoPtr<IComponentName> component;
        provider->mInfo->GetProvider((IComponentName**)&component);
        intent->SetComponent(component);
        mOwner->SendBroadcastAsUser(intent, userHandle);
    }
    if (host != NULL) {
        intent->SetComponent(NULL);
        intent->SetPackage(host->mId->mPackageName);
        intent->PutExtra(IAppWidgetManager::EXTRA_HOST_ID, host->mId->mHostId);
        mOwner->SendBroadcastAsUser(intent, userHandle);
    }
}

void AppWidgetServiceImpl::BackupRestoreController::PruneWidgetStateLocked(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 userId)
{
    AutoPtr<ICharSequence> pkgTmp;
    CString::New(pkg, (ICharSequence**)&pkgTmp);
    Boolean resTmp = FALSE;
    mPrunedApps->Contains(TO_IINTERFACE(pkgTmp), &resTmp);
    if (!resTmp) {
        if (DEBUG) {
            Slogger::I(TAG, "pruning widget state for restoring package %s", pkg.string());
        }
        Int32 widgetSize = 0;
        mOwner->mWidgets->GetSize(&widgetSize);
        for (Int32 i = widgetSize - 1; i >= 0; --i) {
            AutoPtr<IInterface> interfaceTmp;
            mOwner->mWidgets->Get(i, (IInterface**)&interfaceTmp);
            IObject* objTmp = IObject::Probe(interfaceTmp);
            Widget* widget = (Widget*)objTmp;
            Host* host = widget->mHost;
            Provider* provider = widget->mProvider;

            resTmp = FALSE;
            host->HostsPackageForUser(pkg, userId, &resTmp);
            Boolean res1Tmp = FALSE;
            provider->IsInPackageForUser(pkg, userId, &res1Tmp);
            if (resTmp || (provider != NULL && res1Tmp)) {
                // 'pkg' is either the host or the provider for this instances,
                // so we tear it down in anticipation of it (possibly) being
                // reconstructed due to the restore
                host->mWidgets->Remove(TO_IINTERFACE(widget));
                provider->mWidgets->Remove(TO_IINTERFACE(widget));
                mOwner->UnbindAppWidgetRemoteViewsServicesLocked(widget);
                mOwner->mWidgets->Remove(i);
            }
        }
        mPrunedApps->Add(TO_IINTERFACE(pkgTmp));
    }
    else {
        if (DEBUG) {
            Slogger::I(TAG, "already pruned %s, continuing normally", pkg.string());
        }
    }
}

Boolean AppWidgetServiceImpl::BackupRestoreController::IsProviderAndHostInUser(
    /* [in] */ Widget* widget,
    /* [in] */ Int32 userId)
{
    Int32 userIdTmp = 0;
    widget->mHost->GetUserId(&userIdTmp);
    Int32 userId1Tmp = 0;
    widget->mProvider->GetUserId(&userId1Tmp);
    return userIdTmp == userId && (widget->mProvider == NULL || userId1Tmp == userId);
}

AutoPtr<IBundle> AppWidgetServiceImpl::BackupRestoreController::ParseWidgetIdOptions(
    /* [in] */ IXmlPullParser* parser)
{
    AutoPtr<IBundle> options;
    CBundle::New((IBundle**)&options);
    String minWidthString;
    parser->GetAttributeValue(String(NULL), String("min_width"), &minWidthString);
    if (!minWidthString.IsEmpty()) {
        options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MIN_WIDTH,
                StringUtils::ParseInt32(minWidthString, 16));
    }

    String minHeightString;
    parser->GetAttributeValue(String(NULL), String("min_height"), &minHeightString);
    if (!minHeightString.IsEmpty()) {
        options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MIN_HEIGHT,
                StringUtils::ParseInt32(minHeightString, 16));
    }

    String maxWidthString;
    parser->GetAttributeValue(String(NULL), String("max_width"), &maxWidthString);
    if (!maxWidthString.IsEmpty()) {
        options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_WIDTH,
                StringUtils::ParseInt32(maxWidthString, 16));
    }

    String maxHeightString;
    parser->GetAttributeValue(String(NULL), String("max_height"), &maxHeightString);
    if (!maxHeightString.IsEmpty()) {
        options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_HEIGHT,
                StringUtils::ParseInt32(maxHeightString, 16));
    }

    String categoryString;
    parser->GetAttributeValue(String(NULL), String("host_category"), &categoryString);
    if (!categoryString.IsEmpty()) {
        options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_HOST_CATEGORY,
                StringUtils::ParseInt32(categoryString, 16));
    }
    return options;
}

Int32 AppWidgetServiceImpl::BackupRestoreController::CountPendingUpdates(
    /* [in] */ IList* updates)
{
    Int32 pending = 0;
    Int32 N = 0;
    updates->GetSize(&N);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        updates->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        RestoreUpdateRecord* r = (RestoreUpdateRecord*)objTmp;
        if (!r->mNotified) {
            ++pending;
        }
    }
    return pending;
}

//=====================================================================
//             AppWidgetServiceImpl::InnerBroadcastReceiver
//=====================================================================
AppWidgetServiceImpl::InnerBroadcastReceiver::InnerBroadcastReceiver()
{
}

ECode AppWidgetServiceImpl::InnerBroadcastReceiver::constructor()
{
    return BroadcastReceiver::constructor();
}

AppWidgetServiceImpl::InnerBroadcastReceiver::constructor(
    /* [in] */ IInterface* owner)
{
    mOwner = (AppWidgetServiceImpl*)IObject::Probe(owner);
    return BroadcastReceiver::constructor();
}

ECode AppWidgetServiceImpl::InnerBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (DEBUG) {
        Slogger::I(TAG, "Received broadcast: %s", action.string());
    }

    if (IIntent::ACTION_CONFIGURATION_CHANGED.Equals(action)) {
        mOwner->OnConfigurationChanged();
    }
    else if (IIntent::ACTION_USER_STARTED.Equals(action)) {
        Int32 intTmp = 0;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_NULL, &intTmp);
        mOwner->OnUserStarted(intTmp);
    }
    else if (IIntent::ACTION_USER_STOPPED.Equals(action)) {
        Int32 intTmp = 0;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_NULL, &intTmp);
        mOwner->OnUserStopped(intTmp);
    }
    else {
        Int32 intTmp = 0;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_NULL, &intTmp);
        mOwner->OnPackageBroadcastReceived(intent, intTmp);
    }
    return NOERROR;
}

//=====================================================================
//            AppWidgetServiceImpl::InnerServiceConnection
//=====================================================================
CAR_INTERFACE_IMPL(AppWidgetServiceImpl::InnerServiceConnection, Object, IServiceConnection)

AppWidgetServiceImpl::InnerServiceConnection::InnerServiceConnection(
    /* [in] */ AppWidgetServiceImpl* owner,
    /* [in] */ IIntent* intent)
    : mOwner(owner)
    , mIntent(intent)
{
    assert(NULL != mOwner);
    assert(NULL != mIntent);
}

ECode AppWidgetServiceImpl::InnerServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    IIRemoteViewsFactory* cb = IIRemoteViewsFactory::Probe(service);
    //try {
        cb->OnDestroy(mIntent);
    //} catch (RemoteException re) {
        //Slog.e(TAG, "Error calling remove view factory", re);
    //}
    mOwner->mContext->UnbindService(IServiceConnection::Probe(this));
    return NOERROR;
}

ECode AppWidgetServiceImpl::InnerServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    // Do nothing
    return NOERROR;
}

//=====================================================================
//            AppWidgetServiceImpl::InnerServiceConnection1
//=====================================================================
CAR_INTERFACE_IMPL(AppWidgetServiceImpl::InnerServiceConnection1, Object, IServiceConnection)

AppWidgetServiceImpl::InnerServiceConnection1::InnerServiceConnection1(
    /* [in] */ AppWidgetServiceImpl* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

ECode AppWidgetServiceImpl::InnerServiceConnection1::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{

    IIRemoteViewsFactory* cb = IIRemoteViewsFactory::Probe(service);
    //try {
        cb->OnDataSetChangedAsync();
    //} catch (RemoteException e) {
        //Slog.e(TAG, "Error calling onDataSetChangedAsync()", e);
    //}
    mOwner->mContext->UnbindService(IServiceConnection::Probe(this));
    return NOERROR;
}

ECode AppWidgetServiceImpl::InnerServiceConnection1::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    // Do nothing
    return NOERROR;
}

//=====================================================================
//               AppWidgetServiceImpl::LoadedWidgetState
//=====================================================================
AppWidgetServiceImpl::LoadedWidgetState::LoadedWidgetState(
    /* [in] */ Widget* widget,
    /* [in] */ Int32 hostTag,
    /* [in] */ Int32 providerTag)
    : mWidget(widget)
    , mHostTag(hostTag)
    , mProviderTag(providerTag)
{
}

//=====================================================================
//                         AppWidgetServiceImpl
//=====================================================================
const String AppWidgetServiceImpl::TAG("AppWidgetServiceImpl");
const Boolean AppWidgetServiceImpl::DEBUG = FALSE;
const String AppWidgetServiceImpl::OLD_KEYGUARD_HOST_PACKAGE("android");
const String AppWidgetServiceImpl::NEW_KEYGUARD_HOST_PACKAGE("com.android.keyguard");
const Int32 AppWidgetServiceImpl::KEYGUARD_HOST_ID;
const String AppWidgetServiceImpl::STATE_FILENAME("appwidgets.xml");
const Int32 AppWidgetServiceImpl::MIN_UPDATE_PERIOD = AppWidgetServiceImpl::InitMinUpdatePeriod();
const Int32 AppWidgetServiceImpl::TAG_UNDEFINED;
const Int32 AppWidgetServiceImpl::UNKNOWN_UID;
const Int32 AppWidgetServiceImpl::LOADED_PROFILE_ID;
const Int32 AppWidgetServiceImpl::UNKNOWN_USER_ID;
const Int32 AppWidgetServiceImpl::CURRENT_VERSION;

CAR_INTERFACE_IMPL_3(AppWidgetServiceImpl, Object, IIAppWidgetService, IWidgetBackupProvider, IBinder)

AppWidgetServiceImpl::AppWidgetServiceImpl()
    : mSafeMode(FALSE)
    , mMaxWidgetBitmapMemory(0)
{
    CAppWidgetServiceImplBroadcastReceiver::New((IObject*)this, (IBroadcastReceiver**)&mBroadcastReceiver);

    CHashMap::New((IHashMap**)&mBoundRemoteViewsServices);
    CHashMap::New((IHashMap**)&mRemoteViewsServicesAppWidgets);
    CArrayList::New((IArrayList**)&mWidgets);
    CArrayList::New((IArrayList**)&mHosts);
    CArrayList::New((IArrayList**)&mProviders);

    CArraySet::New((IArraySet**)&mPackagesWithBindWidgetPermission);

    CSparseInt32Array::New((ISparseInt32Array**)&mLoadedUserIds);
    CSparseInt32Array::New((ISparseInt32Array**)&mNextAppWidgetIds);
}

ECode AppWidgetServiceImpl::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    mPackageManager = AppGlobals::GetPackageManager();

    AutoPtr<IInterface> interfaceTmp;
    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&interfaceTmp);
    mAlarmManager = IAlarmManager::Probe(interfaceTmp);

    interfaceTmp = NULL;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&interfaceTmp);
    mUserManager = IUserManager::Probe(interfaceTmp);

    interfaceTmp = NULL;
    mContext->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&interfaceTmp);
    mAppOpsManager = IAppOpsManager::Probe(interfaceTmp);

    AutoPtr<IBackgroundThreadHelper> backgroundThreadHelper;
    CBackgroundThreadHelper::AcquireSingleton((IBackgroundThreadHelper**)&backgroundThreadHelper);
    backgroundThreadHelper->GetHandler((IHandler**)&mSaveStateHandler);

    AutoPtr<ILooper> lopper;
    mContext->GetMainLooper((ILooper**)&lopper);
    mCallbackHandler = new CallbackHandler(lopper, this);

    mBackupRestoreController = new BackupRestoreController(this);
    mSecurityPolicy = new SecurityPolicy(this);
    ComputeMaximumWidgetBitmapMemory();
    RegisterBroadcastReceiver();
    RegisterOnCrossProfileProvidersChangedListener();
    return NOERROR;
}

ECode AppWidgetServiceImpl::SetSafeMode(
    /* [in] */ Boolean safeMode)
{
    mSafeMode = safeMode;
    return NOERROR;
}

ECode AppWidgetServiceImpl::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    StringBuilder sb;
    sb += "Permission Denial: can't dump from from pid=";
    sb += Binder::GetCallingPid();
    sb += ", uid=";
    sb += Binder::GetCallingUid();
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::DUMP, sb.ToString());

    {
        AutoLock syncLock(mLock);
        Int32 N = 0;
        mProviders->GetSize(&N);
        pw->Println(String("Providers:"));
        for (Int32 i = 0; i < N; ++i) {
            AutoPtr<IInterface> interfaceTmp;
            mProviders->Get(i, (IInterface**)&interfaceTmp);
            IObject* objTmp = IObject::Probe(interfaceTmp);
            Provider* provider = (Provider*)objTmp;
            DumpProvider(provider, i, pw);
        }

        mWidgets->GetSize(&N);
        pw->Println(String(" "));
        pw->Println(String("Widgets:"));
        for (Int32 i = 0; i < N; ++i) {
            AutoPtr<IInterface> interfaceTmp;
            mWidgets->Get(i, (IInterface**)&interfaceTmp);
            IObject* objTmp = IObject::Probe(interfaceTmp);
            Widget* widget = (Widget*)objTmp;
            DumpWidget(widget, i, pw);
        }

        mHosts->GetSize(&N);
        pw->Println(String(" "));
        pw->Println(String("Hosts:"));
        for (Int32 i = 0; i < N; ++i) {
            AutoPtr<IInterface> interfaceTmp;
            mWidgets->Get(i, (IInterface**)&interfaceTmp);
            IObject* objTmp = IObject::Probe(interfaceTmp);
            Host* host = (Host*)objTmp;
            DumpHost(host, i, pw);
        }

        ICollection::Probe(mPackagesWithBindWidgetPermission)->GetSize(&N);
        pw->Println(String(" "));
        pw->Println(String("Grants:"));
        for (Int32 i = 0; i < N; ++i) {
            AutoPtr<IInterface> interfaceTmp;
            mPackagesWithBindWidgetPermission->GetValueAt(i, (IInterface**)&interfaceTmp);
            IPair* grant = IPair::Probe(interfaceTmp);
            DumpGrant(grant, i, pw);
        }
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::StartListening(
    /* [in] */ IIAppWidgetHost* callbacks,
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 hostId,
    /* [out] */ IList** updatedViews,
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IList> newUpdatedViews;
    CParcelableList::New((IList**)&newUpdatedViews);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "startListening() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    mSecurityPolicy->EnforceCallFromPackage(callingPackage);

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);
        AutoPtr<HostId> id = new HostId(Binder::GetCallingUid(), hostId, callingPackage);
        AutoPtr<Host> host = LookupOrAddHostLocked(id);
        host->mCallbacks = callbacks;
        newUpdatedViews->Clear();
        AutoPtr<IList> instances = host->mWidgets;
        Int32 N = 0;
        instances->GetSize(&N);
        AutoPtr<ArrayOf<Int32> > updatedIds = ArrayOf<Int32>::Alloc(N);
        for (Int32 i = 0; i < N; ++i) {
            AutoPtr<IInterface> interfaceTmp;
            instances->Get(i, (IInterface**)&interfaceTmp);
            Widget* widget = (Widget*)IObject::Probe(interfaceTmp);
            (*updatedIds)[i] = widget->mAppWidgetId;
            AutoPtr<IRemoteViews> viewTmp = CloneIfLocalBinder(widget->mViews);
            newUpdatedViews->Add(viewTmp);
        }
        *result = updatedIds;
        REFCOUNT_ADD(*result);
    }
    *updatedViews = newUpdatedViews;
    REFCOUNT_ADD(*updatedViews);
    return NOERROR;
}

ECode AppWidgetServiceImpl::StopListening(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 hostId)
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "stopListening() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    mSecurityPolicy->EnforceCallFromPackage(callingPackage);

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);

        // NOTE: The lookup is enforcing security across users by making
        // sure the caller can only access hosts it owns.
        AutoPtr<HostId> id = new HostId(Binder::GetCallingUid(), hostId, callingPackage);
        AutoPtr<Host> host = LookupHostLocked(id);
        if (host != NULL) {
            host->mCallbacks = NULL;
            PruneHostLocked(host);
        }
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::AllocateAppWidgetId(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 hostId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "allocateAppWidgetId() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    mSecurityPolicy->EnforceCallFromPackage(callingPackage);

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);
        Int32 idxOfKey = 0;
        mNextAppWidgetIds->IndexOfKey(userId, &idxOfKey);
        if (idxOfKey < 0) {
            mNextAppWidgetIds->Put(userId, IAppWidgetManager::INVALID_APPWIDGET_ID + 1);
        }

        Int32 appWidgetId = IncrementAndGetAppWidgetIdLocked(userId);
        AutoPtr<HostId> id = new HostId(Binder::GetCallingUid(), hostId, callingPackage);
        AutoPtr<Host> host = LookupOrAddHostLocked(id);

        AutoPtr<Widget> widget = new Widget();
        widget->mAppWidgetId = appWidgetId;
        widget->mHost = host;

        host->mWidgets->Add(TO_IINTERFACE(widget));
        mWidgets->Add(TO_IINTERFACE(widget));
        SaveGroupStateAsync(userId);

        if (DEBUG) {
            Slogger::I(TAG, "Allocated widget id %d for host %s", appWidgetId, TO_CSTR(host->mId));
        }

        *result = appWidgetId;
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::DeleteAppWidgetId(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 appWidgetId)
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "deleteAppWidgetId() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    mSecurityPolicy->EnforceCallFromPackage(callingPackage);

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);

        // NOTE: The lookup is enforcing security across users by making
        // sure the caller can only access widgets it hosts or provides.
        AutoPtr<Widget> widget = LookupWidgetLocked(appWidgetId, Binder::GetCallingUid(), callingPackage);
        if (widget == NULL) {
            return NOERROR;
        }

        DeleteAppWidgetLocked(widget);
        SaveGroupStateAsync(userId);
        if (DEBUG) {
            Slogger::I(TAG, "Deleted widget id %d for host %s", appWidgetId, TO_CSTR(widget->mHost->mId));
        }
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::HasBindAppWidgetPermission(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 grantId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG) {
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        Int32 userId = 0;
        helper->GetCallingUserId(&userId);
        Slogger::I(TAG, "hasBindAppWidgetPermission() %d", userId);
    }

    // A special permission is required for managing white listing.
    mSecurityPolicy->EnforceModifyAppWidgetBindPermissions(packageName);

    {
        AutoLock syncLock(mLock);
        // The grants are stored in user state wich gets the grant.
        EnsureGroupStateLoadedLocked(grantId);

        Int32 packageUid = GetUidForPackage(packageName, grantId);
        if (packageUid < 0) {
            *result = FALSE;
            return NOERROR;
        }

        AutoPtr<IInteger32> firstTmp = CoreUtils::Convert(grantId);
        AutoPtr<ICharSequence> secondTmp;
        CString::New(packageName, (ICharSequence**)&secondTmp);

        AutoPtr<IPairHelper> helper;
        CPairHelper::AcquireSingleton((IPairHelper**)&helper);
        AutoPtr<IPair> packageId;
        helper->Create(TO_IINTERFACE(firstTmp), TO_IINTERFACE(secondTmp), (IPair**)&packageId);
        return ICollection::Probe(mPackagesWithBindWidgetPermission)->Contains(TO_IINTERFACE(packageId), result);
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::SetBindAppWidgetPermission(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 grantId,
    /* [in] */ Boolean grantPermission)
{
    if (DEBUG) {
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        Int32 userId = 0;
        helper->GetCallingUserId(&userId);
        Slogger::I(TAG, "setBindAppWidgetPermission() %d", userId);
    }

    // A special permission is required for managing white listing.
    mSecurityPolicy->EnforceModifyAppWidgetBindPermissions(packageName);

    {
        AutoLock syncLock(mLock);
        // The grants are stored in user state wich gets the grant.
        EnsureGroupStateLoadedLocked(grantId);

        Int32 packageUid = GetUidForPackage(packageName, grantId);
        if (packageUid < 0) {
            return NOERROR;
        }

        AutoPtr<IInteger32> firstTmp = CoreUtils::Convert(grantId);
        AutoPtr<ICharSequence> secondTmp;
        CString::New(packageName, (ICharSequence**)&secondTmp);

        AutoPtr<IPairHelper> helper;
        CPairHelper::AcquireSingleton((IPairHelper**)&helper);
        AutoPtr<IPair> packageId;
        helper->Create(TO_IINTERFACE(firstTmp), TO_IINTERFACE(secondTmp), (IPair**)&packageId);
        if (grantPermission) {
            ICollection::Probe(mPackagesWithBindWidgetPermission)->Add(TO_IINTERFACE(packageId));
        }
        else {
            ICollection::Probe(mPackagesWithBindWidgetPermission)->Remove(TO_IINTERFACE(packageId));
        }

        SaveGroupStateAsync(grantId);
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::CreateAppWidgetConfigIntentSender(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 appWidgetId,
    /* [out] */ IIntentSender** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "createAppWidgetConfigIntentSender() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    mSecurityPolicy->EnforceCallFromPackage(callingPackage);

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);

        // NOTE: The lookup is enforcing security across users by making
        // sure the caller can only access widgets it hosts or provides.
        AutoPtr<Widget> widget = LookupWidgetLocked(appWidgetId,
                Binder::GetCallingUid(), callingPackage);
        if (widget == NULL) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        Provider* provider = widget->mProvider;
        if (provider == NULL) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        AutoPtr<IIntent> intent;
        CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_CONFIGURE, (IIntent**)&intent);
        intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_ID, appWidgetId);

        AutoPtr<IComponentName> configure;
        provider->mInfo->GetConfigure((IComponentName**)&configure);
        intent->SetComponent(configure);

        Int64 identity = Binder::ClearCallingIdentity();
        //try {
            userId = 0;
            provider->GetUserId(&userId);
            AutoPtr<IUserHandle> userHandle;
            CUserHandle::New(userId, (IUserHandle**)&userHandle);

            AutoPtr<IPendingIntentHelper> pendingHelper;
            CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingHelper);
            AutoPtr<IPendingIntent> pending;
            pendingHelper->GetActivityAsUser(mContext, 0, intent
                , IPendingIntent::FLAG_ONE_SHOT | IPendingIntent::FLAG_CANCEL_CURRENT
                , NULL, userHandle, (IPendingIntent**)&pending);

            pending->GetIntentSender(result);
        //} finally {
            Binder::RestoreCallingIdentity(identity);
        //}
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::BindAppWidgetId(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ Int32 providerProfileId,
    /* [in] */ IComponentName* providerComponent,
    /* [in] */ IBundle* options,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "bindAppWidgetId() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    mSecurityPolicy->EnforceCallFromPackage(callingPackage);

    // Check that if a cross-profile binding is attempted, it is allowed.
    if (!mSecurityPolicy->IsEnabledGroupProfile(providerProfileId)) {
        *result = FALSE;
        return NOERROR;
    }

    // If the provider is not under the calling user, make sure this
    // provider is white listed for access from the parent.
    String packageName;
    providerComponent->GetPackageName(&packageName);
    Boolean resTmp = FALSE;
    mSecurityPolicy->IsProviderInCallerOrInProfileAndWhitelListed(packageName, providerProfileId, &resTmp);
    if (!resTmp) {
        *result = FALSE;
        return NOERROR;
    }

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);

        // A special permission or white listing is required to bind widgets.
        resTmp = FALSE;
        mSecurityPolicy->HasCallerBindPermissionOrBindWhiteListedLocked(callingPackage, &resTmp);
        if (!resTmp) {
            *result = FALSE;
            return NOERROR;
        }

        // NOTE: The lookup is enforcing security across users by making
        // sure the caller can only access widgets it hosts or provides.
        AutoPtr<Widget> widget = LookupWidgetLocked(appWidgetId,
                Binder::GetCallingUid(), callingPackage);
        if (widget == NULL) {
            Slogger::E(TAG, "Bad widget id %d", appWidgetId);
            *result = FALSE;
            return NOERROR;
        }

        if (widget->mProvider != NULL) {
            Slogger::E(TAG, "Widget id %d already bound to: %s", appWidgetId, TO_CSTR(widget->mProvider->mId));
            *result = FALSE;
            return NOERROR;
        }

        Int32 providerUid = GetUidForPackage(packageName, providerProfileId);
        if (providerUid < 0) {
            Slogger::E(TAG, "Package %s not installed  for profile %d", packageName.string(), providerProfileId);
            *result = FALSE;
            return NOERROR;
        }

        // NOTE: The lookup is enforcing security across users by making
        // sure the provider is in the already vetted user profile.
        AutoPtr<ProviderId> providerId = new ProviderId(providerUid, providerComponent);
        AutoPtr<Provider> provider = LookupProviderLocked(providerId);
        if (provider == NULL) {
            Slogger::E(TAG, "No widget provider %s for profile %d", TO_CSTR(providerComponent), providerProfileId);
            *result = FALSE;
            return NOERROR;
        }

        if (provider->mZombie) {
            Slogger::E(TAG, "Can't bind to a 3rd party provider in safe mode %s", TO_CSTR(provider));
            *result = FALSE;
            return NOERROR;
        }

        widget->mProvider = provider;
        if (options != NULL) {
            widget->mOptions = CloneIfLocalBinder(options);
        }
        else {
            CBundle::New((IBundle**)&(widget->mOptions));
        }

        // We need to provide a default value for the widget category if it is not specified
        IBaseBundle::Probe(widget->mOptions)->ContainsKey(IAppWidgetManager::OPTION_APPWIDGET_HOST_CATEGORY, &resTmp);
        if (!resTmp) {
            widget->mOptions->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_HOST_CATEGORY,
                    IAppWidgetProviderInfo::WIDGET_CATEGORY_HOME_SCREEN);
        }

        provider->mWidgets->Add(TO_IINTERFACE(widget));
        Int32 widgetCount = 0;
        provider->mWidgets->GetSize(&widgetCount);
        if (widgetCount == 1) {
            // Tell the provider that it's ready.
            SendEnableIntentLocked(provider);
        }

        // Send an update now -- We need this update now, and just for this appWidgetId.
        // It's less critical when the next one happens, so when we schedule the next one,
        // we add updatePeriodMillis to its start time. That time will have some slop,
        // but that's okay.
        AutoPtr< ArrayOf<Int32> > lstTmp = ArrayOf<Int32>::Alloc(1);
        lstTmp->Set(0, appWidgetId);
        SendUpdateIntentLocked(provider, lstTmp);

        // Schedule the future updates.
        RegisterForBroadcastsLocked(provider, GetWidgetIds(provider->mWidgets));
        SaveGroupStateAsync(userId);

        if (DEBUG) {
            Slogger::I(TAG, "Bound widget %d to provider %s", appWidgetId, TO_CSTR(provider->mId));
        }
    }

    *result = TRUE;
    return NOERROR;
}

ECode AppWidgetServiceImpl::GetAppWidgetIds(
    /* [in] */ IComponentName* componentName,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "getAppWidgetIds() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    String packageName;
    componentName->GetPackageName(&packageName);
    mSecurityPolicy->EnforceCallFromPackage(packageName);


    AutoPtr<ArrayOf<Int32> > widgetIds;
    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);

        // NOTE: The lookup is enforcing security across users by making
        // sure the caller can access only its providers.
        AutoPtr<ProviderId> providerId = new ProviderId(Binder::GetCallingUid(), componentName);
        AutoPtr<Provider> provider = LookupProviderLocked(providerId);

        if (provider != NULL) {
            widgetIds = GetWidgetIds(provider->mWidgets);
        }
        else
            widgetIds = ArrayOf<Int32>::Alloc(0);
    }
    *result = widgetIds;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AppWidgetServiceImpl::GetAppWidgetIdsForHost(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 hostId,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "getAppWidgetIdsForHost() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    mSecurityPolicy->EnforceCallFromPackage(callingPackage);

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);

        // NOTE: The lookup is enforcing security across users by making
        // sure the caller can only access its hosts.
        AutoPtr<HostId> id = new HostId(Binder::GetCallingUid(), hostId, callingPackage);
        AutoPtr<Host> host = LookupHostLocked(id);

        if (host != NULL) {
            *result = GetWidgetIds(host->mWidgets);
            REFCOUNT_ADD(*result);
            return NOERROR;
        }

        *result = ArrayOf<Int32>::Alloc(0);
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::BindRemoteViewsService(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IIntent* intent,
    /* [in] */ IBinder* callbacks)
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "bindRemoteViewsService() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    mSecurityPolicy->EnforceCallFromPackage(callingPackage);

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);

        // NOTE: The lookup is enforcing security across users by making
        // sure the caller can only access widgets it hosts or provides.
        AutoPtr<Widget> widget = LookupWidgetLocked(appWidgetId,
                Binder::GetCallingUid(), callingPackage);

        if (widget == NULL) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        // Make sure the widget has a provider.
        if (widget->mProvider == NULL) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        AutoPtr<IComponentName> componentName;
        intent->GetComponent((IComponentName**)&componentName);

        // Ensure that the service belongs to the same package as the provider.
        // But this is not enough as they may be under different users - see below...
        String providerPackage;
        widget->mProvider->mId->mComponentName->GetPackageName(&providerPackage);
        String servicePackage;
        componentName->GetPackageName(&servicePackage);
        if (!servicePackage.Equals(providerPackage)) {
            //throw new SecurityException("The taget service not in the same package"
            //        + " as the widget provider");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        // Make sure this service exists under the same user as the provider and
        // requires a permission which allows only the system to bind to it.
        Int32 userIdTmp = 0;
        widget->mProvider->GetUserId(&userIdTmp);
        mSecurityPolicy->EnforceServiceExistsAndRequiresBindRemoteViewsPermission(
                componentName, userIdTmp);

        // Good to go - the service pakcage is correct, it exists for the correct
        // user, and requires the bind permission.

        // If there is already a connection made for this service intent, then
        // disconnect from that first. (This does not allow multiple connections
        // to the same service under the same key).
        AutoPtr<ServiceConnectionProxy> connection;
        AutoPtr<IIntentFilterComparison> fc;
        CIntentFilterComparison::New(intent, (IIntentFilterComparison**)&fc);

        AutoPtr<IInteger32> firstTmp = CoreUtils::Convert(appWidgetId);
        AutoPtr<IPairHelper> pairHelper;
        CPairHelper::AcquireSingleton((IPairHelper**)&pairHelper);
        AutoPtr<IPair> key;
        pairHelper->Create(TO_IINTERFACE(firstTmp), TO_IINTERFACE(fc), (IPair**)&key);

        Boolean resTmp = FALSE;
        mBoundRemoteViewsServices->ContainsKey(key, &resTmp);
        if (resTmp) {
            AutoPtr<IInterface> interfaceTmp;
            mBoundRemoteViewsServices->Get(key, (IInterface**)&interfaceTmp);
            IObject* objTmp = IObject::Probe(interfaceTmp);
            connection = (ServiceConnectionProxy*)objTmp;
            connection->Disconnect();
            UnbindService(connection);
            mBoundRemoteViewsServices->Remove(TO_IINTERFACE(key));
        }

        // Bind to the RemoteViewsService (which will trigger a callback to the
        // RemoteViewsAdapter.onServiceConnected())
        connection = new ServiceConnectionProxy(callbacks);

        AutoPtr<IUserHandle> userHandle;
        widget->mProvider->mInfo->GetProfile((IUserHandle**)&userHandle);

        BindService(intent, connection, userHandle);
        mBoundRemoteViewsServices->Put(TO_IINTERFACE(key), TO_IINTERFACE(connection));

        // Add it to the mapping of RemoteViewsService to appWidgetIds so that we
        // can determine when we can call back to the RemoteViewsService later to
        // destroy associated factories.

        firstTmp = NULL;
        firstTmp = CoreUtils::Convert(widget->mProvider->mId->mUid);
        AutoPtr<IPair> serviceId;
        pairHelper->Create(TO_IINTERFACE(firstTmp), TO_IINTERFACE(fc), (IPair**)&serviceId);
        IncrementAppWidgetServiceRefCount(appWidgetId, serviceId);
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::UnbindRemoteViewsService(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "unbindRemoteViewsService() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    mSecurityPolicy->EnforceCallFromPackage(callingPackage);

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);

        // Unbind from the RemoteViewsService (which will trigger a callback to the bound
        // RemoteViewsAdapter)
        AutoPtr<IInteger32> firstTmp = CoreUtils::Convert(appWidgetId);
        AutoPtr<IIntentFilterComparison> secondTmp;
        CIntentFilterComparison::New(intent, (IIntentFilterComparison**)&secondTmp);
        AutoPtr<IPairHelper> pairHelper;
        CPairHelper::AcquireSingleton((IPairHelper**)&pairHelper);
        AutoPtr<IPair> key;
        pairHelper->Create(TO_IINTERFACE(firstTmp), TO_IINTERFACE(secondTmp), (IPair**)&key);

        Boolean resTmp = FALSE;
        mBoundRemoteViewsServices->ContainsKey(TO_IINTERFACE(key), &resTmp);
        if (resTmp) {
            // We don't need to use the appWidgetId until after we are sure there is something
            // to unbind. Note that this may mask certain issues with apps calling unbind()
            // more than necessary.

            // NOTE: The lookup is enforcing security across users by making
            // sure the caller can only access widgets it hosts or provides.
            AutoPtr<Widget> widget = LookupWidgetLocked(appWidgetId,
                    Binder::GetCallingUid(), callingPackage);

            if (widget == NULL) {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            AutoPtr<IInterface> interfaceTmp;
            mBoundRemoteViewsServices->Get(TO_IINTERFACE(key), (IInterface**)&interfaceTmp);
            IObject* objTmp = IObject::Probe(interfaceTmp);
            ServiceConnectionProxy* connection = (ServiceConnectionProxy*)objTmp;
            connection->Disconnect();
            mContext->UnbindService(IServiceConnection::Probe(connection));
            mBoundRemoteViewsServices->Remove(TO_IINTERFACE(key));
        }
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::DeleteHost(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 hostId)
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "deleteHost() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    mSecurityPolicy->EnforceCallFromPackage(callingPackage);

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);

        // NOTE: The lookup is enforcing security across users by making
        // sure the caller can only access hosts in its uid and package.
        AutoPtr<HostId> id = new HostId(Binder::GetCallingUid(), hostId, callingPackage);
        AutoPtr<Host> host = LookupHostLocked(id);
        if (host == NULL) {
            return NOERROR;
        }

        DeleteHostLocked(host);
        SaveGroupStateAsync(userId);
        if (DEBUG) {
            Slogger::I(TAG, "Deleted host %s", TO_CSTR(host->mId));
        }
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::DeleteAllHosts()
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "deleteAllHosts() %d", userId);
    }

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);
        Boolean changed = false;
        Int32 N = 0;
        mHosts->GetSize(&N);
        for (Int32 i = N - 1; i >= 0; --i) {
            AutoPtr<IInterface> interfaceTmp;
            mHosts->Get(i, (IInterface**)&interfaceTmp);
            IObject* objTmp = IObject::Probe(interfaceTmp);
            Host* host = (Host*)objTmp;
            // Delete only hosts in the calling uid.
            if (host->mId->mUid == Binder::GetCallingUid()) {
                DeleteHostLocked(host);
                changed = TRUE;
                if (DEBUG) {
                    Slogger::I(TAG, "Deleted host %s", TO_CSTR(host->mId));
                }
            }
        }

        if (changed) {
            SaveGroupStateAsync(userId);
        }
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::GetAppWidgetInfo(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 appWidgetId,
    /* [out] */ IAppWidgetProviderInfo** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "getAppWidgetInfo() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    mSecurityPolicy->EnforceCallFromPackage(callingPackage);

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);

        // NOTE: The lookup is enforcing security across users by making
        // sure the caller can only access widgets it hosts or provides.
        AutoPtr<Widget> widget = LookupWidgetLocked(appWidgetId,
                Binder::GetCallingUid(), callingPackage);

        if (widget != NULL && widget->mProvider != NULL && !widget->mProvider->mZombie) {
            AutoPtr<IAppWidgetProviderInfo> awpi = CloneIfLocalBinder(widget->mProvider->mInfo);
            *result = awpi.Get();
            REFCOUNT_ADD(*result);
            return NOERROR;
        }

        *result = NULL;
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::GetAppWidgetViews(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 appWidgetId,
    /* [out] */ IRemoteViews** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "getAppWidgetViews() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    mSecurityPolicy->EnforceCallFromPackage(callingPackage);

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);

        // NOTE: The lookup is enforcing security across users by making
        // sure the caller can only access widgets it hosts or provides.
        AutoPtr<Widget> widget = LookupWidgetLocked(appWidgetId,
                Binder::GetCallingUid(), callingPackage);

        if (widget != NULL) {
            AutoPtr<IRemoteViews> rv = CloneIfLocalBinder(widget->mViews);
            *result = rv.Get();
            REFCOUNT_ADD(*result);
            return NOERROR;
        }

        *result = NULL;
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::UpdateAppWidgetOptions(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IBundle* options)
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "updateAppWidgetOptions() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    mSecurityPolicy->EnforceCallFromPackage(callingPackage);

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);

        // NOTE: The lookup is enforcing security across users by making
        // sure the caller can only access widgets it hosts or provides.
        AutoPtr<Widget> widget = LookupWidgetLocked(appWidgetId,
                Binder::GetCallingUid(), callingPackage);

        if (widget == NULL) {
            return NOERROR;
        }

        // Merge the options.
        widget->mOptions->PutAll(options);

        // Send the broacast to notify the provider that options changed.
        SendOptionsChangedIntentLocked(widget);
        SaveGroupStateAsync(userId);
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::GetAppWidgetOptions(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 appWidgetId,
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "getAppWidgetOptions() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    mSecurityPolicy->EnforceCallFromPackage(callingPackage);

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);

        // NOTE: The lookup is enforcing security across users by making
        // sure the caller can only access widgets it hosts or provides.
        AutoPtr<Widget> widget = LookupWidgetLocked(appWidgetId,
                Binder::GetCallingUid(), callingPackage);

        if (widget != NULL && widget->mOptions != NULL) {
            AutoPtr<IBundle> rv = CloneIfLocalBinder(widget->mOptions);
            *result = rv.Get();
            REFCOUNT_ADD(*result)
            return NOERROR;
        }

        AutoPtr<IBundleHelper> bundleHelper;
        CBundleHelper::AcquireSingleton((IBundleHelper**)&bundleHelper);
        bundleHelper->GetEMPTY(result);
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::UpdateAppWidgetIds(
    /* [in] */ const String& callingPackage,
    /* [in] */ ArrayOf<Int32>* appWidgetIds,
    /* [in] */ IRemoteViews* views)
{
    if (DEBUG) {
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        Int32 userId = 0;
        helper->GetCallingUserId(&userId);
        Slogger::I(TAG, "updateAppWidgetIds() %d", userId);
    }

    UpdateAppWidgetIds(callingPackage, appWidgetIds, views, FALSE);
    return NOERROR;
}

ECode AppWidgetServiceImpl::PartiallyUpdateAppWidgetIds(
    /* [in] */ const String& callingPackage,
    /* [in] */ ArrayOf<Int32>* appWidgetIds,
    /* [in] */ IRemoteViews* views)
{
    if (DEBUG) {
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        Int32 userId = 0;
        helper->GetCallingUserId(&userId);
        Slogger::I(TAG, "partiallyUpdateAppWidgetIds() %d", userId);
    }

    UpdateAppWidgetIds(callingPackage, appWidgetIds, views, TRUE);
    return NOERROR;
}

ECode AppWidgetServiceImpl::NotifyAppWidgetViewDataChanged(
    /* [in] */ const String& callingPackage,
    /* [in] */ ArrayOf<Int32>* appWidgetIds,
    /* [in] */ Int32 viewId)
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "notifyAppWidgetViewDataChanged() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    mSecurityPolicy->EnforceCallFromPackage(callingPackage);

    if (appWidgetIds == NULL || appWidgetIds->GetLength() == 0) {
        return NOERROR;
    }

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);
        Int32 N = appWidgetIds->GetLength();
        Int32 appWidgetId = 0;
        AutoPtr<Widget> widget;
        for (Int32 i = 0; i < N; ++i) {
            appWidgetId = (*appWidgetIds)[i];

            // NOTE: The lookup is enforcing security across users by making
            // sure the caller can only access widgets it hosts or provides.
            widget = NULL;
            widget = LookupWidgetLocked(appWidgetId,
                    Binder::GetCallingUid(), callingPackage);
            if (widget != NULL) {
                ScheduleNotifyAppWidgetViewDataChanged(widget, viewId);
            }
        }
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::UpdateAppWidgetProvider(
    /* [in] */ IComponentName* componentName,
    /* [in] */ IRemoteViews* views)
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "updateAppWidgetProvider() %d", userId);
    }

    // Make sure the package runs under the caller uid.
    String packageName;
    componentName->GetPackageName(&packageName);
    mSecurityPolicy->EnforceCallFromPackage(packageName);

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);

        // NOTE: The lookup is enforcing security across users by making
        // sure the caller can access only its providers.
        AutoPtr<ProviderId> providerId = new ProviderId(Binder::GetCallingUid(), componentName);
        AutoPtr<Provider> provider = LookupProviderLocked(providerId);

        if (provider == NULL) {
            Slogger::W(TAG, "Provider doesn't exist %s", TO_CSTR(providerId));
            return NOERROR;
        }

        AutoPtr<IList> instances = provider->mWidgets;
        Int32 N = 0;
        instances->GetSize(&N);
        for (Int32 i = 0; i < N; ++i) {
            AutoPtr<IInterface> interfaceTmp;
            instances->Get(i, (IInterface**)&interfaceTmp);
            IObject* objTmp = IObject::Probe(interfaceTmp);
            Widget* widget = (Widget*)objTmp;
            UpdateAppWidgetInstanceLocked(widget, views, FALSE);
        }
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::GetInstalledProvidersForProfile(
    /* [in] */ Int32 categoryFilter,
    /* [in] */ Int32 profileId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (DEBUG) {
        Slogger::I(TAG, "getInstalledProvidersForProfiles() %d", userId);
    }

    // Ensure the profile is in the group and enabled.
    if (!mSecurityPolicy->IsEnabledGroupProfile(profileId)) {
        return NOERROR;
    }

    AutoLock syncLock(mLock);
    EnsureGroupStateLoadedLocked(userId);
    Int32 providerCount = 0;
    mProviders->GetSize(&providerCount);
    for (Int32 i = 0; i < providerCount; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mProviders->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Provider* provider = (Provider*)objTmp;
        IAppWidgetProviderInfo* info = provider->mInfo;
        // Ignore an invalid provider or one not matching the filter.
        Int32 widgetCategory = 0;
        info->GetWidgetCategory(&widgetCategory);
        if (provider->mZombie || (widgetCategory & categoryFilter) == 0) {
            continue;
        }

        // Add providers only for the requested profile that are white-listed.
        AutoPtr<IUserHandle> userHandle;
        info->GetProfile((IUserHandle**)&userHandle);
        Int32 providerProfileId = 0;
        userHandle->GetIdentifier(&providerProfileId);

        String packageName;
        provider->mId->mComponentName->GetPackageName(&packageName);
        Boolean resTmp = FALSE;
        mSecurityPolicy->IsProviderInCallerOrInProfileAndWhitelListed(packageName, providerProfileId, &resTmp);
        if (providerProfileId == profileId && resTmp) {
            if (*result == NULL) {
                CParcelableList::New(result);
            }
            (*result)->Add(TO_IINTERFACE(CloneIfLocalBinder(info)));
        }
    }

    return NOERROR;
}

ECode AppWidgetServiceImpl::SendOptionsChangedIntentLocked(
    /* [in] */ Widget* widget)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_OPTIONS_CHANGED, (IIntent**)&intent);
    AutoPtr<IComponentName> provider;
    widget->mProvider->mInfo->GetProvider((IComponentName**)&provider);
    intent->SetComponent(provider);
    intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_ID, widget->mAppWidgetId);
    intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_OPTIONS, widget->mOptions);
    AutoPtr<IUserHandle> profile;
    widget->mProvider->mInfo->GetProfile((IUserHandle**)&profile);
    SendBroadcastAsUser(intent, profile);
    return NOERROR;
}

ECode AppWidgetServiceImpl::GetWidgetParticipants(
    /* [in] */ Int32 userId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    return mBackupRestoreController->GetWidgetParticipants(userId, result);
}

ECode AppWidgetServiceImpl::GetWidgetState(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mBackupRestoreController->GetWidgetState(packageName, userId, result);
}

ECode AppWidgetServiceImpl::RestoreStarting(
    /* [in] */ Int32 userId)
{
    return mBackupRestoreController->RestoreStarting(userId);
}

ECode AppWidgetServiceImpl::RestoreWidgetState(
    /* [in] */ const String& packageName,
    /* [in] */ ArrayOf<Byte>* restoredState,
    /* [in] */ Int32 userId)
{
    return mBackupRestoreController->RestoreWidgetState(packageName, restoredState, userId);
}

ECode AppWidgetServiceImpl::RestoreFinished(
    /* [in] */ Int32 userId)
{
    return mBackupRestoreController->RestoreFinished(userId);
}

ECode AppWidgetServiceImpl::OnCrossProfileWidgetProvidersChanged(
    /* [in] */ Int32 userId,
    /* [in] */ IList* packages)
{
    VALIDATE_NOT_NULL(packages);

    Int32 parentId = 0;
    mSecurityPolicy->GetProfileParent(userId, &parentId);
    // We care only if the white-listed package is in a profile of
    // the group parent as only the parent can add widgets from the
    // profile and not the other way around.
    if (parentId != userId) {
        AutoLock syncLock(mLock);
        Boolean providersChanged = false;
        Int32 packageCount = 0;
        String packageName;
        packages->GetSize(&packageCount);
        for (Int32 i = 0; i < packageCount; ++i) {
            AutoPtr<IInterface> interfaceTmp;
            packages->Get(i, (IInterface**)&interfaceTmp);
            ICharSequence* charSequenceTmp = ICharSequence::Probe(interfaceTmp);
            charSequenceTmp->ToString(&packageName);
            providersChanged |= UpdateProvidersForPackageLocked(packageName,
                    userId, NULL);
        }

        if (providersChanged) {
            SaveGroupStateAsync(userId);
            ScheduleNotifyGroupHostsForProvidersChangedLocked(userId);
        }
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = "AppWidgetServiceImpl:: ";
    result->AppendFormat("%p", this);
    return NOERROR;
}

void AppWidgetServiceImpl::ComputeMaximumWidgetBitmapMemory()
{
    AutoPtr<IInterface> interfaceTmp;
    mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&interfaceTmp);
    IWindowManager* wm = IWindowManager::Probe(interfaceTmp);
    AutoPtr<IDisplay> display;
    wm->GetDefaultDisplay((IDisplay**)&display);
    AutoPtr<IPoint> size;
    CPoint::New((IPoint**)&size);
    display->GetRealSize(size);
    // Cap memory usage at 1.5 times the size of the display
    // 1.5 * 4 bytes/pixel * w * h ==> 6 * w * h
    Int32 x = 0, y = 0;
    size->GetX(&x);
    size->GetY(&y);
    mMaxWidgetBitmapMemory = 6 * x * y;
}

void AppWidgetServiceImpl::RegisterBroadcastReceiver()
{
    // Register for configuration changes so we can update the names
    // of the widgets when the locale changes.
    AutoPtr<IIntentFilter> configFilter;
    CIntentFilter::New((IIntentFilter**)&configFilter);
    configFilter->AddAction(IIntent::ACTION_CONFIGURATION_CHANGED);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> userHandleAllTmp;
    helper->GetALL((IUserHandle**)&userHandleAllTmp);

    AutoPtr<IIntent> intentTmp;
    mContext->RegisterReceiverAsUser(mBroadcastReceiver, userHandleAllTmp,
            configFilter, String(NULL), NULL, (IIntent**)&intentTmp);

    // Register for broadcasts about package install, etc., so we can
    // update the provider list.
    AutoPtr<IIntentFilter> packageFilter;
    CIntentFilter::New((IIntentFilter**)&packageFilter);
    packageFilter->AddAction(IIntent::ACTION_PACKAGE_ADDED);
    packageFilter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
    packageFilter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    packageFilter->AddDataScheme(String("package"));

    intentTmp = NULL;
    mContext->RegisterReceiverAsUser(mBroadcastReceiver, userHandleAllTmp,
            packageFilter, String(NULL), NULL, (IIntent**)&intentTmp);

    // Register for events related to sdcard installation.
    AutoPtr<IIntentFilter> sdFilter;
    CIntentFilter::New((IIntentFilter**)&sdFilter);
    sdFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE);
    sdFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE);

    intentTmp = NULL;
    mContext->RegisterReceiverAsUser(mBroadcastReceiver, userHandleAllTmp,
            sdFilter, String(NULL), NULL, (IIntent**)&intentTmp);

    AutoPtr<IIntentFilter> userFilter;
    CIntentFilter::New((IIntentFilter**)&userFilter);
    userFilter->AddAction(IIntent::ACTION_USER_STARTED);
    userFilter->AddAction(IIntent::ACTION_USER_STOPPED);

    intentTmp = NULL;
    mContext->RegisterReceiverAsUser(mBroadcastReceiver, userHandleAllTmp,
            userFilter, String(NULL), NULL, (IIntent**)&intentTmp);
}

void AppWidgetServiceImpl::RegisterOnCrossProfileProvidersChangedListener()
{
    Slogger::I(TAG, "TODO RegisterOnCrossProfileProvidersChangedListener");
    // AutoPtr<IInterface> interfaceTmp = LocalServices::GetService(EIID_IDevicePolicyManagerInternal);
    // IDevicePolicyManagerInternal* devicePolicyManager = IDevicePolicyManagerInternal::Probe(interfaceTmp);
    // // The device policy is an optional component.
    // if (devicePolicyManager NULL) {
    //     devicePolicyManager->AddOnCrossProfileWidgetProvidersChangeListener(this);
    // }
}

void AppWidgetServiceImpl::OnConfigurationChanged()
{
    if (DEBUG) {
        Slogger::I(TAG, "onConfigurationChanged()");
    }

    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> revised;
    helper->GetDefault((ILocale**)&revised);
    String strReviseTmp, strLocaleTmp;
    if (revised != NULL) {
        revised->ToString(&strReviseTmp);
    }
    if (mLocale != NULL) {
        mLocale->ToString(&strLocaleTmp);
    }
    if (revised == NULL || mLocale == NULL || !strReviseTmp.Equals(strLocaleTmp)) {
        mLocale = NULL;
        mLocale = revised;

        {
            AutoLock syncLock(mLock);
            AutoPtr<ISparseInt32Array> changedGroups;

            // Note: updateProvidersForPackageLocked() may remove providers, so we must copy the
            // list of installed providers and skip providers that we don't need to update.
            // Also note that remove the provider does not clear the Provider component data.
            AutoPtr<IArrayList> installedProviders = mProviders;
            AutoPtr<IHashSet> removedProviders;
            CHashSet::New((IHashSet**)&removedProviders);

            Int32 N = 0;
            installedProviders->GetSize(&N);
            Boolean resTmp = FALSE;
            String packageName;
            Int32 userIdTmp = 0;
            for (Int32 i = N - 1; i >= 0; --i) {
                AutoPtr<IInterface> interfaceTmp;
                installedProviders->Get(i, (IInterface**)&interfaceTmp);
                IObject* objTmp = IObject::Probe(interfaceTmp);
                Provider* provider = (Provider*)objTmp;
                provider->GetUserId(&userIdTmp);
                EnsureGroupStateLoadedLocked(userIdTmp);

                removedProviders->Contains(TO_IINTERFACE(provider->mId), &resTmp);
                if (!resTmp) {
                    provider->mId->mComponentName->GetPackageName(&packageName);
                    Boolean changed = UpdateProvidersForPackageLocked(packageName,
                            userIdTmp, ISet::Probe(removedProviders));

                    if (changed) {
                        if (changedGroups == NULL) {
                            CSparseInt32Array::New((ISparseInt32Array**)&changedGroups);
                        }
                        Int32 groupId = 0;
                        mSecurityPolicy->GetGroupParent(userIdTmp, &groupId);
                        changedGroups->Put(groupId, groupId);
                    }
                }
            }

            if (changedGroups != NULL) {
                Int32 groupCount = 0;
                changedGroups->GetSize(&groupCount);
                for (Int32 i = 0; i < groupCount; ++i) {
                    Int32 groupId = 0;
                    changedGroups->Get(i, &groupId);
                    SaveGroupStateAsync(groupId);
                }
            }
        }
    }
}

void AppWidgetServiceImpl::OnPackageBroadcastReceived(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 userId)
{
    String action;
    intent->GetAction(&action);
    Boolean added = FALSE;
    Boolean changed = FALSE;
    Boolean componentsModified = FALSE;

    AutoPtr< ArrayOf<String> > pkgList;
    if (IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE.Equals(action)) {
        intent->GetStringArrayExtra(IIntent::EXTRA_CHANGED_PACKAGE_LIST, (ArrayOf<String>**)&pkgList);
        added = TRUE;
    }
    else if (IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE.Equals(action)) {
        intent->GetStringArrayExtra(IIntent::EXTRA_CHANGED_PACKAGE_LIST, (ArrayOf<String>**)&pkgList);
        added = FALSE;
    }
    else {
        AutoPtr<IUri> uri;
        intent->GetData((IUri**)&uri);
        if (uri == NULL) {
            return;
        }
        String pkgName;
        uri->GetSchemeSpecificPart(&pkgName);
        if (pkgName.IsEmpty()) {
            return;
        }
        pkgList = ArrayOf<String>::Alloc(1);
        pkgList->Set(0, pkgName);
        added = IIntent::ACTION_PACKAGE_ADDED.Equals(action);
        changed = IIntent::ACTION_PACKAGE_CHANGED.Equals(action);
    }
    if (pkgList == NULL || pkgList->GetLength() == 0) {
        return;
    }

    {
        AutoLock syncLock(mLock);
        EnsureGroupStateLoadedLocked(userId);
        AutoPtr<IBundle> extras;
        intent->GetExtras((IBundle**)&extras);
        if (added || changed) {
            Boolean resTmp = FALSE;
            extras->GetBoolean(IIntent::EXTRA_REPLACING, FALSE, &resTmp);
            Boolean newPackageAdded = added && (extras == NULL || !resTmp);

            String pkgName;
            for (Int32 idx = 0; idx < pkgList->GetLength(); ++idx) {
                pkgName = (*pkgList)[idx];
                // Fix up the providers - add/remove/update.
                componentsModified |= UpdateProvidersForPackageLocked(pkgName, userId, NULL);

                // ... and see if these are hosts we've been awaiting.
                // NOTE: We are backing up and restoring only the owner.
                if (newPackageAdded && userId == IUserHandle::USER_OWNER) {
                    Int32 uid = GetUidForPackage(pkgName, userId);
                    if (uid >= 0 ) {
                        ResolveHostUidLocked(pkgName, uid);
                    }
                }
            }
        }
        else {
            // If the package is being updated, we'll receive a PACKAGE_ADDED
            // shortly, otherwise it is removed permanently.
            Boolean resTmp = FALSE;
            extras->GetBoolean(IIntent::EXTRA_REPLACING, FALSE, &resTmp);
            Boolean packageRemovedPermanently = (extras == NULL || !resTmp);
            if (packageRemovedPermanently) {
                String pkgName;
                for (Int32 idx = 0; idx < pkgList->GetLength(); ++idx) {
                    pkgName = (*pkgList)[idx];
                    componentsModified |= RemoveHostsAndProvidersForPackageLocked(pkgName, userId);
                }
            }
        }

        if (componentsModified) {
            SaveGroupStateAsync(userId);

            // If the set of providers has been modified, notify each active AppWidgetHost
            ScheduleNotifyGroupHostsForProvidersChangedLocked(userId);
        }
    }
}

void AppWidgetServiceImpl::ResolveHostUidLocked(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid)
{
    Int32 N = 0;
    mHosts->GetSize(&N);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mHosts->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Host* host = (Host*)objTmp;
        if (host->mId->mUid == UNKNOWN_UID && pkg.Equals(host->mId->mPackageName)) {
            if (DEBUG) {
                Slogger::I(TAG, "host %s resolved to uid %d", TO_CSTR(host->mId), uid);
            }
            host->mId = new HostId(uid, host->mId->mHostId, host->mId->mPackageName);
            return;
        }
    }
}

void AppWidgetServiceImpl::EnsureGroupStateLoadedLocked(
    /* [in] */ Int32 userId)
{
    AutoPtr<ArrayOf<Int32> > profileIds;
    mSecurityPolicy->GetEnabledGroupProfileIds(userId, (ArrayOf<Int32>**)&profileIds);

    // Careful lad, we may have already loaded the state for some
    // group members, so check before loading and read only the
    // state for the new member(s).
    Int32 newMemberCount = 0;
    Int32 profileIdCount = profileIds->GetLength();
    Int32 profileId = 0;
    Int32 intTmp = 0;
    for (Int32 i = 0; i < profileIdCount; ++i) {
        profileId = (*profileIds)[i];
        mLoadedUserIds->IndexOfKey(profileId, &intTmp);
        if (intTmp >= 0) {
            profileIds->Set(i, LOADED_PROFILE_ID);
        }
        else {
            ++newMemberCount;
        }
    }

    if (newMemberCount <= 0) {
        return;
    }

    Int32 newMemberIndex = 0;
    AutoPtr<ArrayOf<Int32> > newProfileIds = ArrayOf<Int32>::Alloc(newMemberCount);
    for (Int32 i = 0; i < profileIdCount; ++i) {
        profileId = (*profileIds)[i];
        if (profileId != LOADED_PROFILE_ID) {
            mLoadedUserIds->Put(profileId, profileId);
            newProfileIds->Set(newMemberIndex, profileId);
            ++newMemberIndex;
        }
    }

    ClearProvidersAndHostsTagsLocked();
    LoadGroupWidgetProvidersLocked(newProfileIds);
    LoadGroupStateLocked(newProfileIds);
}

ECode AppWidgetServiceImpl::UpdateAppWidgetIds(
    /* [in] */ const String& callingPackage,
    /* [in] */ ArrayOf<Int32>* appWidgetIds,
    /* [in] */ IRemoteViews* views,
    /* [in] */ Boolean partially)
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetCallingUserId(&userId);
    if (appWidgetIds == NULL || appWidgetIds->GetLength() == 0) {
        return NOERROR;
    }

    // Make sure the package runs under the caller uid.
    mSecurityPolicy->EnforceCallFromPackage(callingPackage);

    Int32 usageTmp = 0;
    views->EstimateMemoryUsage(&usageTmp);
    Int32 bitmapMemoryUsage = (views != NULL) ? usageTmp : 0;
    if (bitmapMemoryUsage > mMaxWidgetBitmapMemory) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock syncLock(mLock);
    EnsureGroupStateLoadedLocked(userId);
    Int32 N = appWidgetIds->GetLength();
    Int32 appWidgetId = 0;
    for (Int32 i = 0; i < N; ++i) {
        appWidgetId = (*appWidgetIds)[i];
        AutoPtr<Widget> widget = LookupWidgetLocked(appWidgetId, Binder::GetCallingUid(), callingPackage);
        if (widget != NULL) {
            UpdateAppWidgetInstanceLocked(widget, views, partially);
        }
    }

    return NOERROR;
}

Int32 AppWidgetServiceImpl::IncrementAndGetAppWidgetIdLocked(
    /* [in] */ Int32 userId)
{
    Int32 appWidgetId = PeekNextAppWidgetIdLocked(userId) + 1;
    mNextAppWidgetIds->Put(userId, appWidgetId);
    return appWidgetId;
}

void AppWidgetServiceImpl::SetMinAppWidgetIdLocked(
    /* [in] */ Int32 userId,
    /* [in] */ Int32 minWidgetId)
{
    Int32 nextAppWidgetId = PeekNextAppWidgetIdLocked(userId);
    if (nextAppWidgetId < minWidgetId) {
        mNextAppWidgetIds->Put(userId, minWidgetId);
    }
}

Int32 AppWidgetServiceImpl::PeekNextAppWidgetIdLocked(
    /* [in] */ Int32 userId)
{
    Int32 resTmp = 0;
    mNextAppWidgetIds->IndexOfKey(userId, &resTmp);
    if (resTmp < 0) {
        return IAppWidgetManager::INVALID_APPWIDGET_ID + 1;
    }
    else {
        mNextAppWidgetIds->Get(userId, &resTmp);
        return resTmp;
    }
}

AutoPtr<AppWidgetServiceImpl::Host> AppWidgetServiceImpl::LookupOrAddHostLocked(
    /* [in] */ HostId* id)
{
    AutoPtr<Host> host = LookupHostLocked(id);
    if (host != NULL) {
        return host;
    }

    host = new Host();
    host->mId = id;
    mHosts->Add(TO_IINTERFACE(host));
    return host;
}

void AppWidgetServiceImpl::DeleteHostLocked(
    /* [in] */ Host* host)
{
    Int32 N = 0;
    host->mWidgets->GetSize(&N);
    for (Int32 i = N - 1; i >= 0; --i) {
        AutoPtr<IInterface> interfaceTmp;
        host->mWidgets->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Widget* widget = (Widget*)objTmp;
        DeleteAppWidgetLocked(widget);
        host->mWidgets->Remove(interfaceTmp);
    }
    mHosts->Remove(TO_IINTERFACE(host));

    // it's gone or going away, abruptly drop the callback connection
    host->mCallbacks = NULL;
}

void AppWidgetServiceImpl::DeleteAppWidgetLocked(
    /* [in] */ Widget* widget)
{
    // We first unbind all services that are bound to this id
    UnbindAppWidgetRemoteViewsServicesLocked(widget);

    Host* host = widget->mHost;
    host->mWidgets->Remove(TO_IINTERFACE(widget));
    PruneHostLocked(host);

    mWidgets->Remove(TO_IINTERFACE(widget));
    Provider* provider = widget->mProvider;
    if (provider != NULL) {
        provider->mWidgets->Remove(TO_IINTERFACE(widget));
        if (!provider->mZombie) {
            // send the broacast saying that this appWidgetId has been deleted
            SendDeletedIntentLocked(widget);

            Boolean isEmpty = FALSE;
            provider->mWidgets->IsEmpty(&isEmpty);
            if (isEmpty) {
                // cancel the future updates
                CancelBroadcasts(provider);

                // send the broacast saying that the provider is not in use any more
                SendDisabledIntentLocked(provider);
            }
        }
    }
}

void AppWidgetServiceImpl::CancelBroadcasts(
    /* [in] */ Provider* provider)
{
    if (DEBUG) {
        Slogger::I(TAG, "cancelBroadcasts() for %s", TO_CSTR(provider));
    }
    if (provider->mBroadcast != NULL) {
        mAlarmManager->Cancel(provider->mBroadcast);
        Int64 token = Binder::ClearCallingIdentity();
        //try {
            provider->mBroadcast->Cancel();
        //} finally {
            Binder::RestoreCallingIdentity(token);
        //}
        provider->mBroadcast = NULL;
    }
}

void AppWidgetServiceImpl::UnbindAppWidgetRemoteViewsServicesLocked(
    /* [in] */ Widget* widget)
{
    Int32 appWidgetId = widget->mAppWidgetId;
    // Unbind all connections to Services bound to this AppWidgetId
    AutoPtr<ISet> keySet;
    mBoundRemoteViewsServices->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);
    Boolean hasNext = FALSE;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> interfaceTmp;
        it->GetNext((IInterface**)&interfaceTmp);
        IPair* key = IPair::Probe(interfaceTmp);

        AutoPtr<IInterface> firstTmp;
        key->GetFirst((IInterface**)&firstTmp);
        IInteger32* int32Tmp = IInteger32::Probe(firstTmp);
        Int32 first = 0;
        int32Tmp->GetValue(&first);
        if (first == appWidgetId) {
            AutoPtr<IInterface> interfaceProxyTmp;
            mBoundRemoteViewsServices->Get(interfaceTmp, (IInterface**)&interfaceProxyTmp);
            IObject* objProxyTmp = IObject::Probe(interfaceProxyTmp);
            ServiceConnectionProxy* conn = (ServiceConnectionProxy*)objProxyTmp;
            conn->Disconnect();
            mContext->UnbindService(IServiceConnection::Probe(conn));
            it->Remove();
        }
    }

    // Check if we need to destroy any services (if no other app widgets are
    // referencing the same service)
    DecrementAppWidgetServiceRefCount(widget);
}

void AppWidgetServiceImpl::DestroyRemoteViewsService(
    /* [in] */ IIntent* intent,
    /* [in] */ Widget* widget)
{
    AutoPtr<IServiceConnection> conn = new InnerServiceConnection(this, intent);
    // Bind to the service and remove the static intent->factory mapping in the
    // RemoteViewsService.
    Int64 token = Binder::ClearCallingIdentity();
    //try {
        AutoPtr<IUserHandle> userHandleTmp;
        widget->mProvider->mInfo->GetProfile((IUserHandle**)&userHandleTmp);
        Boolean resTmp;
        mContext->BindServiceAsUser(intent, conn, IContext::BIND_AUTO_CREATE, userHandleTmp, &resTmp);
    //} finally {
        Binder::RestoreCallingIdentity(token);
    //}
}

void AppWidgetServiceImpl::IncrementAppWidgetServiceRefCount(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IPair* serviceId)
{
    AutoPtr<IHashSet> appWidgetIds;
    Boolean resTmp = FALSE;
    mRemoteViewsServicesAppWidgets->ContainsKey(TO_IINTERFACE(serviceId), &resTmp);
    if (resTmp) {
        AutoPtr<IInterface> interfaceTmp;
        mRemoteViewsServicesAppWidgets->Get(serviceId, (IInterface**)&interfaceTmp);
        appWidgetIds = IHashSet::Probe(interfaceTmp);
    }
    else {
        CHashSet::New((IHashSet**)&appWidgetIds);
        mRemoteViewsServicesAppWidgets->Put(TO_IINTERFACE(serviceId), TO_IINTERFACE(appWidgetIds));
    }

    AutoPtr<IInteger32> idTmp = CoreUtils::Convert(appWidgetId);
    ISet::Probe(appWidgetIds)->Add(idTmp);
}

void AppWidgetServiceImpl::DecrementAppWidgetServiceRefCount(
    /* [in] */ Widget* widget)
{
    AutoPtr<ISet> keySet;
    mRemoteViewsServicesAppWidgets->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);
    Boolean hasNext = FALSE;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> interfaceTmp;
        it->GetNext((IInterface**)&interfaceTmp);
        IPair* key = IPair::Probe(interfaceTmp);

        AutoPtr<IInterface> interfaceServocesTmp;
        mRemoteViewsServicesAppWidgets->Get(interfaceTmp, (IInterface**)&interfaceServocesTmp);
        IHashSet* ids = IHashSet::Probe(interfaceServocesTmp);

        AutoPtr<IInteger32> appWidgetIdTmp = CoreUtils::Convert(widget->mAppWidgetId);
        Boolean resTmp = FALSE;
        ids->Remove(TO_IINTERFACE(appWidgetIdTmp), &resTmp);
        if (resTmp) {
            // If we have removed the last app widget referencing this service, then we
            // should destroy it and remove it from this set
            Boolean isEmpty = FALSE;
            ids->IsEmpty(&isEmpty);
            if (isEmpty) {
                AutoPtr<IInterface> secondTmp;
                key->GetSecond((IInterface**)&secondTmp);
                AutoPtr<IIntent> intent;
                IIntentFilterComparison::Probe(secondTmp)->GetIntent((IIntent**)&intent);
                DestroyRemoteViewsService(intent, widget);
                it->Remove();
            }
        }
    }
}

void AppWidgetServiceImpl::SaveGroupStateAsync(
    /* [in] */ Int32 groupId)
{
    AutoPtr<SaveStateRunnable> runnable = new SaveStateRunnable(groupId, this);
    Boolean resTmp;
    mSaveStateHandler->Post(IRunnable::Probe(runnable), &resTmp);
}

void AppWidgetServiceImpl::UpdateAppWidgetInstanceLocked(
    /* [in] */ Widget* widget,
    /* [in] */ IRemoteViews* views,
    /* [in] */ Boolean isPartialUpdate)
{
    if (widget != NULL && widget->mProvider != NULL
        && !widget->mProvider->mZombie && !widget->mHost->mZombie) {
        if (isPartialUpdate && widget->mViews != NULL) {
            // For a partial update, we merge the new RemoteViews with the old.
            widget->mViews->MergeRemoteViews(views);
        }
        else {
            // For a full update we replace the RemoteViews completely.
            widget->mViews = views;
        }
        ScheduleNotifyUpdateAppWidgetLocked(widget, views);
    }
}

void AppWidgetServiceImpl::ScheduleNotifyAppWidgetViewDataChanged(
    /* [in] */ Widget* widget,
    /* [in] */ Int32 viewId)
{
    if (widget == NULL || widget->mHost == NULL || widget->mHost->mZombie
            || widget->mHost->mCallbacks == NULL || widget->mProvider == NULL
            || widget->mProvider->mZombie) {
        return;
    }

    AutoPtr<ISomeArgsHelper> helper;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&helper);
    AutoPtr<ISomeArgs> args;
    helper->Obtain((ISomeArgs**)&args);
    args->SetObjectArg(1, TO_IINTERFACE(widget->mHost));
    args->SetObjectArg(2, TO_IINTERFACE(widget->mHost->mCallbacks));
    args->SetInt32Arg(1, widget->mAppWidgetId);
    args->SetInt32Arg(2, viewId);

    AutoPtr<IMessage> message;
    mCallbackHandler->ObtainMessage(CallbackHandler::MSG_NOTIFY_VIEW_DATA_CHANGED,
            TO_IINTERFACE(args), (IMessage**)&message);
    message->SendToTarget();
}

void AppWidgetServiceImpl::HandleNotifyAppWidgetViewDataChanged(
    /* [in] */ Host* host,
    /* [in] */ IIAppWidgetHost* callbacks,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ Int32 viewId)
{
    //try {
        callbacks->ViewDataChanged(appWidgetId, viewId);
    //} catch (RemoteException re) {
        // It failed; remove the callback. No need to prune because
        // we know that this host is still referenced by this instance.
        //callbacks = null;
    //}

    // If the host is unavailable, then we call the associated
    // RemoteViewsFactory.onDataSetChanged() directly

    AutoLock syncLock(mLock);
    if (callbacks == NULL) {
        host->mCallbacks = NULL;

        AutoPtr<ISet> keySet;
        mRemoteViewsServicesAppWidgets->GetKeySet((ISet**)&keySet);
        AutoPtr<IIterator> it;
        keySet->GetIterator((IIterator**)&it);
        Boolean hasNext = FALSE;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> interfaceTmp;
            it->GetNext((IInterface**)&interfaceTmp);
            IPair* key = IPair::Probe(interfaceTmp);

            AutoPtr<IInterface> getKeyTmp;
            mRemoteViewsServicesAppWidgets->Get(TO_IINTERFACE(key), (IInterface**)&getKeyTmp);
            IHashSet* valueTmp = IHashSet::Probe(getKeyTmp);
            AutoPtr<IInteger32> widgetIdTmp = CoreUtils::Convert(appWidgetId);
            Boolean resTmp = FALSE;
            valueTmp->Contains(TO_IINTERFACE(widgetIdTmp), &resTmp);
            if (resTmp) {
                AutoPtr<InnerServiceConnection1> connection = new InnerServiceConnection1(this);

                AutoPtr<IUserHandleHelper> helper;
                CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);

                AutoPtr<IInterface> keyFirstTmp;
                key->GetFirst((IInterface**)&keyFirstTmp);
                IInteger32* keyFirst1Tmp = IInteger32::Probe(keyFirstTmp);
                Int32 keyFirst = 0;
                keyFirst1Tmp->GetValue(&keyFirst);
                Int32 userId = 0;
                helper->GetUserId(keyFirst, &userId);

                AutoPtr<IInterface> keySecondTmp;
                key->GetSecond((IInterface**)&keySecondTmp);
                IIntentFilterComparison* keySecond1Tmp = IIntentFilterComparison::Probe(keySecondTmp);
                AutoPtr<IIntent> intent;
                keySecond1Tmp->GetIntent((IIntent**)&intent);

                // Bind to the service and call onDataSetChanged()
                AutoPtr<IUserHandle> userHandle;
                CUserHandle::New(userId, (IUserHandle**)&userHandle);
                BindService(intent, connection, userHandle);
            }
        }
    }
}

void AppWidgetServiceImpl::ScheduleNotifyUpdateAppWidgetLocked(
    /* [in] */ Widget* widget,
    /* [in] */ IRemoteViews* updateViews)
{
    if (widget == NULL || widget->mProvider == NULL || widget->mProvider->mZombie
            || widget->mHost->mCallbacks == NULL || widget->mHost->mZombie) {
        return;
    }

    AutoPtr<ISomeArgsHelper> helper;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&helper);
    AutoPtr<ISomeArgs> args;
    helper->Obtain((ISomeArgs**)&args);
    args->SetObjectArg(1, TO_IINTERFACE(widget->mHost));
    args->SetObjectArg(2, TO_IINTERFACE(widget->mHost->mCallbacks));
    args->SetObjectArg(3, TO_IINTERFACE(updateViews));
    args->SetInt32Arg(1, widget->mAppWidgetId);

    AutoPtr<IMessage> message;
    mCallbackHandler->ObtainMessage(CallbackHandler::MSG_NOTIFY_UPDATE_APP_WIDGET,
            TO_IINTERFACE(args), (IMessage**)&message);
    message->SendToTarget();
}

void AppWidgetServiceImpl::HandleNotifyUpdateAppWidget(
    /* [in] */ Host* host,
    /* [in] */ IIAppWidgetHost* callbacks,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IRemoteViews* views)
{
    //try {
        callbacks->UpdateAppWidget(appWidgetId, views);
    //} catch (RemoteException re) {
        //{    AutoLock syncLock(mLock);
        //    Slog.e(TAG, "Widget host dead: " + host.id, re);
        //    host.callbacks = null;
        //}
    //}
}

void AppWidgetServiceImpl::ScheduleNotifyProviderChangedLocked(
    /* [in] */ Widget* widget)
{
    if (widget == NULL || widget->mProvider == NULL || widget->mProvider->mZombie
            || widget->mHost->mCallbacks == NULL || widget->mHost->mZombie) {
        return;
    }

    AutoPtr<ISomeArgsHelper> helper;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&helper);
    AutoPtr<ISomeArgs> args;
    helper->Obtain((ISomeArgs**)&args);
    args->SetObjectArg(1, TO_IINTERFACE(widget->mHost));
    args->SetObjectArg(2, TO_IINTERFACE(widget->mHost->mCallbacks));
    args->SetObjectArg(3, TO_IINTERFACE(widget->mProvider->mInfo));
    args->SetInt32Arg(1, widget->mAppWidgetId);

    AutoPtr<IMessage> message;
    mCallbackHandler->ObtainMessage(CallbackHandler::MSG_NOTIFY_PROVIDER_CHANGED,
            TO_IINTERFACE(args), (IMessage**)&message);
    message->SendToTarget();
}

void AppWidgetServiceImpl::HandleNotifyProviderChanged(
    /* [in] */ Host* host,
    /* [in] */ IIAppWidgetHost* callbacks,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IAppWidgetProviderInfo* info)
{
    //try {
        callbacks->ProviderChanged(appWidgetId, info);
    //} catch (RemoteException re) {
        //{    AutoLock syncLock(mLock);
        //    Slog.e(TAG, "Widget host dead: " + host.id, re);
        //    host.callbacks = null;
        //}
    //}
}

void AppWidgetServiceImpl::ScheduleNotifyGroupHostsForProvidersChangedLocked(
    /* [in] */ Int32 userId)
{
    AutoPtr< ArrayOf<Int32> > profileIds;
    mSecurityPolicy->GetEnabledGroupProfileIds(userId, (ArrayOf<Int32>**)&profileIds);

    Int32 N = 0;
    mHosts->GetSize(&N);
    for (Int32 i = N - 1; i >= 0; --i) {
        AutoPtr<IInterface> interfaceTmp;
        mHosts->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Host* host = (Host*)objTmp;
        Boolean hostInGroup = FALSE;
        Int32 M = profileIds->GetLength();
        for (Int32 j = 0; j < M; ++j) {
            Int32 profileId = (*profileIds)[j];
            Int32 userIdTmp = 0;
            host->GetUserId(&userIdTmp);
            if (userIdTmp == profileId) {
                hostInGroup = TRUE;
                break;
            }
        }

        if (!hostInGroup) {
            continue;
        }

        if (host == NULL || host->mZombie || host->mCallbacks == NULL) {
            continue;
        }

        AutoPtr<ISomeArgsHelper> helper;
        CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&helper);
        AutoPtr<ISomeArgs> args;
        helper->Obtain((ISomeArgs**)&args);
        args->SetObjectArg(1, TO_IINTERFACE(host));
        args->SetObjectArg(2, TO_IINTERFACE(host->mCallbacks));

        AutoPtr<IMessage> message;
        mCallbackHandler->ObtainMessage(CallbackHandler::MSG_NOTIFY_PROVIDERS_CHANGED,
                TO_IINTERFACE(args), (IMessage**)&message);
        message->SendToTarget();
    }
}

void AppWidgetServiceImpl::HandleNotifyProvidersChanged(
    /* [in] */ Host* host,
    /* [in] */ IIAppWidgetHost* callbacks)
{
    //try {
        callbacks->ProvidersChanged();
    //} catch (RemoteException re) {
        //{    AutoLock syncLock(mLock);
        //    Slog.e(TAG, "Widget host dead: " + host.id, re);
        //    host.callbacks = null;
        //}
    //}
}

Boolean AppWidgetServiceImpl::IsLocalBinder()
{
    return Process::MyPid() == Binder::GetCallingPid();
}

AutoPtr<IRemoteViews> AppWidgetServiceImpl::CloneIfLocalBinder(
    /* [in] */ IRemoteViews* rv)
{
    if (IsLocalBinder() && rv != NULL) {
        AutoPtr<IRemoteViews> result;
        rv->Clone((IRemoteViews**)&result);
        return result;
    }
    return rv;
}

AutoPtr<IAppWidgetProviderInfo> AppWidgetServiceImpl::CloneIfLocalBinder(
    /* [in] */ IAppWidgetProviderInfo* info)
{
    if (IsLocalBinder() && info != NULL) {
        AutoPtr<IAppWidgetProviderInfo> result;
        info->Clone((IAppWidgetProviderInfo**)&result);
        return result;
    }
    return info;
}

AutoPtr<IBundle> AppWidgetServiceImpl::CloneIfLocalBinder(
    /* [in] */ IBundle* bundle)
{
    if (IsLocalBinder() && bundle != NULL) {
        AutoPtr<IInterface> resultTmp;
        ICloneable::Probe(bundle)->Clone((IInterface**)&resultTmp);
        AutoPtr<IBundle> result = IBundle::Probe(resultTmp);
        return result;
    }
    return bundle;
}

AutoPtr<AppWidgetServiceImpl::Widget> AppWidgetServiceImpl::LookupWidgetLocked(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    Int32 N = 0;
    mWidgets->GetSize(&N);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mWidgets->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Widget* widget = (Widget*)objTmp;
        Boolean resTmp = FALSE;
        mSecurityPolicy->CanAccessAppWidget(widget, uid, packageName, &resTmp);
        if (widget->mAppWidgetId == appWidgetId && resTmp) {
            return widget;
        }
    }
    return NULL;
}

AutoPtr<AppWidgetServiceImpl::Provider> AppWidgetServiceImpl::LookupProviderLocked(
    /* [in] */ ProviderId* id)
{
    Int32 N = 0;
    mProviders->GetSize(&N);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mProviders->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Provider* provider = (Provider*)objTmp;

        String strProviderId;
        provider->mId->ToString(&strProviderId);
        String strId;
        id->ToString(&strId);
        if (strProviderId.Equals(strId)) {
            return provider;
        }
    }
    return NULL;
}

AutoPtr<AppWidgetServiceImpl::Host> AppWidgetServiceImpl::LookupHostLocked(
    /* [in] */ HostId* hostId)
{
    Int32 N = 0;
    mHosts->GetSize(&N);
    for (int i = 0; i < N; i++) {
        AutoPtr<IInterface> interfaceTmp;
        mHosts->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Host* host = (Host*)objTmp;

        String strHostId;
        host->mId->ToString(&strHostId);
        String strHostId1;
        hostId->ToString(&strHostId1);
        if (strHostId.Equals(strHostId1)) {
            return host;
        }
    }
    return NULL;
}

void AppWidgetServiceImpl::PruneHostLocked(
    /* [in] */ Host* host)
{
    Int32 size = 0;
    host->mWidgets->GetSize(&size);
    if (size == 0 && host->mCallbacks == NULL) {
        if (DEBUG) {
            Slogger::I(TAG, "Pruning host %s", TO_CSTR(host->mId));
        }
        mHosts->Remove(TO_IINTERFACE(host));
    }
}

void AppWidgetServiceImpl::LoadGroupWidgetProvidersLocked(
    /* [in] */ ArrayOf<Int32>* profileIds)
{
    AutoPtr<IList> allReceivers;
    AutoPtr<IIntent> intent;
    CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_UPDATE, (IIntent**)&intent);
    Int32 profileCount = profileIds->GetLength();
    for (Int32 i = 0; i < profileCount; ++i) {
        Int32 profileId = (*profileIds)[i];
        AutoPtr<IList> receivers = QueryIntentReceivers(intent, profileId);
        Boolean resTmp = FALSE;
        receivers->IsEmpty(&resTmp);
        if (receivers != NULL && !resTmp) {
            if (allReceivers == NULL) {
                CArrayList::New((IList**)&allReceivers);
            }
            allReceivers->AddAll(ICollection::Probe(receivers));
        }
    }

    Int32 receiversSize = 0;
    Int32 N = (allReceivers == NULL) ? 0 : (allReceivers->GetSize(&receiversSize), receiversSize);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        allReceivers->Get(i, (IInterface**)&interfaceTmp);
        IResolveInfo* receiver = IResolveInfo::Probe(interfaceTmp);
        AddProviderLocked(receiver);
    }
}

Boolean AppWidgetServiceImpl::AddProviderLocked(
    /* [in] */ IResolveInfo* ri)
{
    AutoPtr<IActivityInfo> activityInfo;
    ri->GetActivityInfo((IActivityInfo**)&activityInfo);
    AutoPtr<IApplicationInfo> applicationInfo;
    IComponentInfo::Probe(activityInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    Int32 flags = 0;
    applicationInfo->GetFlags(&flags);
    if ((flags & IApplicationInfo::FLAG_EXTERNAL_STORAGE) != 0) {
        return FALSE;
    }

    Boolean isEnabled = FALSE;
    IComponentInfo::Probe(activityInfo)->IsEnabled(&isEnabled);
    if (!isEnabled) {
        return FALSE;
    }

    String packageName;
    IPackageItemInfo::Probe(activityInfo)->GetPackageName(&packageName);
    String name;
    IPackageItemInfo::Probe(activityInfo)->GetName(&name);
    AutoPtr<IComponentName> componentName;
    CComponentName::New(packageName, name, (IComponentName**)&componentName);

    Int32 applicationInfouid = 0;
    applicationInfo->GetUid(&applicationInfouid);
    AutoPtr<ProviderId> providerId = new ProviderId(applicationInfouid, componentName);
    AutoPtr<Provider> provider = ParseProviderInfoXml(providerId, ri);

    if (provider != NULL) {
        AutoPtr<Provider> existing = LookupProviderLocked(providerId);
        if (existing == NULL) {
            AutoPtr<ProviderId> restoredProviderId = new ProviderId(UNKNOWN_UID, componentName);
            existing = LookupProviderLocked(restoredProviderId);
        }

        if (existing != NULL) {
            if (existing->mZombie && !mSafeMode) {
                // it's a placeholder that was set up during an app restore
                existing->mId = providerId;
                existing->mZombie = FALSE;
                existing->mInfo = provider->mInfo; // the real one filled out from the ResolveInfo
                if (DEBUG) {
                    Slogger::I(TAG, "Provider placeholder now reified: %s", TO_CSTR(existing));
                }
            }
        }
        else {
            mProviders->Add(TO_IINTERFACE(provider));
        }
        return TRUE;
    }
    return FALSE;
}

void AppWidgetServiceImpl::DeleteProviderLocked(
    /* [in] */ Provider* provider)
{
    Int32 N = 0;
    provider->mWidgets->GetSize(&N);
    for (Int32 i = N - 1; i >= 0; --i) {
        AutoPtr<IInterface> interfaceTmp;
        provider->mWidgets->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Widget* widget = (Widget*)objTmp;
        provider->mWidgets->Remove(interfaceTmp);

        UpdateAppWidgetInstanceLocked(widget, NULL, FALSE);
        widget->mHost->mWidgets->Remove(TO_IINTERFACE(widget));
        mWidgets->Remove(TO_IINTERFACE(widget));
        widget->mProvider = NULL;
        PruneHostLocked(widget->mHost);
        widget->mHost = NULL;
    }
    mProviders->Remove(TO_IINTERFACE(provider));
    CancelBroadcasts(provider);
}

void AppWidgetServiceImpl::SendEnableIntentLocked(
    /* [in] */ Provider* p)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_ENABLED, (IIntent**)&intent);
    AutoPtr<IComponentName> componentName;
    p->mInfo->GetProvider((IComponentName**)&componentName);
    intent->SetComponent(componentName);

    AutoPtr<IUserHandle> profile;
    p->mInfo->GetProfile((IUserHandle**)&profile);
    SendBroadcastAsUser(intent, profile);
}

void AppWidgetServiceImpl::SendUpdateIntentLocked(
    /* [in] */ Provider* provider,
    /* [in] */ ArrayOf<Int32>* appWidgetIds)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_UPDATE, (IIntent**)&intent);
    intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_IDS, appWidgetIds);

    AutoPtr<IComponentName> componentName;
    provider->mInfo->GetProvider((IComponentName**)&componentName);
    intent->SetComponent(componentName);

    AutoPtr<IUserHandle> profile;
    provider->mInfo->GetProfile((IUserHandle**)&profile);
    SendBroadcastAsUser(intent, profile);
}

void AppWidgetServiceImpl::SendDeletedIntentLocked(
    /* [in] */ Widget* widget)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_DELETED, (IIntent**)&intent);

    AutoPtr<IComponentName> provider;
    widget->mProvider->mInfo->GetProvider((IComponentName**)&provider);
    intent->SetComponent(provider);

    intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_ID, widget->mAppWidgetId);
    AutoPtr<IUserHandle> profile;
    widget->mProvider->mInfo->GetProfile((IUserHandle**)&profile);
    SendBroadcastAsUser(intent, profile);
}

void AppWidgetServiceImpl::SendDisabledIntentLocked(
    /* [in] */ Provider* provider)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_DISABLED, (IIntent**)&intent);

    AutoPtr<IComponentName> componentName;
    provider->mInfo->GetProvider((IComponentName**)&componentName);
    intent->SetComponent(componentName);

    AutoPtr<IUserHandle> profile;
    provider->mInfo->GetProfile((IUserHandle**)&profile);
    SendBroadcastAsUser(intent, profile);
}

void AppWidgetServiceImpl::RegisterForBroadcastsLocked(
    /* [in] */ Provider* provider,
    /* [in] */ ArrayOf<Int32>* appWidgetIds)
{
    Int32 updatePeriodMillis = 0;
    provider->mInfo->GetUpdatePeriodMillis(&updatePeriodMillis);
    if (updatePeriodMillis > 0) {
        Boolean alreadyRegistered = provider->mBroadcast != NULL;

        AutoPtr<IIntent> intent;
        CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_UPDATE, (IIntent**)&intent);
        intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_IDS, appWidgetIds);

        AutoPtr<IComponentName> componentName;
        provider->mInfo->GetProvider((IComponentName**)&componentName);
        intent->SetComponent(componentName);

        Int64 token = Binder::ClearCallingIdentity();
        //try {
            AutoPtr<IPendingIntentHelper> helper;
            CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
            AutoPtr<IUserHandle> profile;
            provider->mInfo->GetProfile((IUserHandle**)&profile);
            provider->mBroadcast = NULL;
            helper->GetBroadcastAsUser(mContext, 1, intent, IPendingIntent::FLAG_UPDATE_CURRENT,
                profile, (IPendingIntent**)&(provider->mBroadcast));
        //} finally {
            Binder::RestoreCallingIdentity(token);
        //}
        if (!alreadyRegistered) {
            Int32 periodTmp = 0;
            provider->mInfo->GetUpdatePeriodMillis(&periodTmp);
            Int64 period = periodTmp;
            if (period < MIN_UPDATE_PERIOD) {
                period = MIN_UPDATE_PERIOD;
            }
            mAlarmManager->SetInexactRepeating(IAlarmManager::ELAPSED_REALTIME_WAKEUP,
                    SystemClock::GetElapsedRealtime() + period, period, provider->mBroadcast);
        }
    }
}

AutoPtr< ArrayOf<Int32> > AppWidgetServiceImpl::GetWidgetIds(
    /* [in] */ IList* widgets)
{
    Int32 instancesSize = 0;
    widgets->GetSize(&instancesSize);
    AutoPtr< ArrayOf<Int32> > appWidgetIds = ArrayOf<Int32>::Alloc(instancesSize);
    for (Int32 i = 0; i < instancesSize; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        widgets->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Widget* widget = (Widget*)objTmp;
        appWidgetIds->Set(i, widget->mAppWidgetId);
    }
    return appWidgetIds;
}

void AppWidgetServiceImpl::DumpProvider(
    /* [in] */ Provider* provider,
    /* [in] */ Int32 index,
    /* [in] */ IPrintWriter* pw)
{
    IAppWidgetProviderInfo* info = provider->mInfo;
    pw->Print(String("  ["));
    pw->Print(index);
    pw->Print(String("] provider "));

    String strTmp;
    provider->mId->ToString(&strTmp);
    pw->Println(strTmp);
    pw->Print(String("    min=("));

    Int32 minWidth = 0;
    info->GetMinWidth(&minWidth);
    pw->Print(minWidth);

    pw->Print(String("x"));
    Int32 minHeight = 0;
    info->GetMinHeight(&minHeight);
    pw->Print(minHeight);

    pw->Print(String(")   minResize=("));
    Int32 minResizeWidth = 0;
    info->GetMinResizeWidth(&minResizeWidth);
    pw->Print(minResizeWidth);

    pw->Print(String("x"));
    Int32 minResizeHeight = 0;
    info->GetMinResizeHeight(&minResizeHeight);
    pw->Print(minResizeHeight);

    pw->Print(String(") updatePeriodMillis="));
    Int32 updatePeriodMillis = 0;
    info->GetUpdatePeriodMillis(&updatePeriodMillis);
    pw->Print(updatePeriodMillis);

    pw->Print(String(" resizeMode="));
    Int32 resizeMode = 0;
    info->GetResizeMode(&resizeMode);
    pw->Print(resizeMode);

    Int32 widgetCategory = 0;
    info->GetWidgetCategory(&widgetCategory);
    pw->Print(widgetCategory);

    pw->Print(String(" autoAdvanceViewId="));
    Int32 autoAdvanceViewId = 0;
    info->GetAutoAdvanceViewId(&autoAdvanceViewId);
    pw->Print(autoAdvanceViewId);

    pw->Print(String(" initialLayout=#"));
    Int32 initialLayout = 0;
    info->GetInitialLayout(&initialLayout);
    pw->Print(StringUtils::ToHexString(initialLayout));

    pw->Print(String(" initialKeyguardLayout=#"));
    Int32 initialKeyguardLayout = 0;
    info->GetInitialKeyguardLayout(&initialKeyguardLayout);
    pw->Print(StringUtils::ToHexString(initialKeyguardLayout));

    pw->Print(String(" zombie="));
    pw->Println(provider->mZombie);
}

void AppWidgetServiceImpl::DumpHost(
    /* [in] */ Host* host,
    /* [in] */ Int32 index,
    /* [in] */ IPrintWriter* pw)
{
    pw->Print(String("  ["));
    pw->Print(index);

    pw->Print(String("] hostId="));
    String strTmp;
    host->mId->ToString(&strTmp);
    pw->Println(strTmp);

    pw->Print(String("    callbacks="));
    pw->Println(TO_IINTERFACE(host->mCallbacks));

    pw->Print(String("    widgets.size="));
    Int32 widgetSize = 0;
    host->mWidgets->GetSize(&widgetSize);
    pw->Print(widgetSize);
    pw->Print(String(" zombie="));
    pw->Println(host->mZombie);
}

void AppWidgetServiceImpl::DumpGrant(
    /* [in] */ IPair* grant,
    /* [in] */ Int32 index,
    /* [in] */ IPrintWriter* pw)
{
    pw->Print(String("  ["));
    pw->Print(index);
    pw->Print(']');

    pw->Print(String(" user="));
    AutoPtr<IInterface> firstTmp;
    grant->GetFirst((IInterface**)&firstTmp);
    IInteger32* first1Tmp = IInteger32::Probe(firstTmp);
    Int32 first = 0;
    first1Tmp->GetValue(&first);
    pw->Print(first);

    pw->Print(String(" package="));
    AutoPtr<IInterface> secondTmp;
    grant->GetSecond((IInterface**)&secondTmp);
    ICharSequence* second1Tmp = ICharSequence::Probe(secondTmp);
    String second;
    second1Tmp->ToString(&second);
    pw->Println(second);
}

void AppWidgetServiceImpl::DumpWidget(
    /* [in] */ Widget* widget,
    /* [in] */ Int32 index,
    /* [in] */ IPrintWriter* pw)
{
    pw->Print(String("  ["));
    pw->Print(index);
    pw->Print(String("] id="));
    pw->Println(widget->mAppWidgetId);
    pw->Print(String("    host="));
    String strTmp;
    widget->mHost->mId->ToString(&strTmp);
    pw->Println(strTmp);
    if (widget->mProvider != NULL) {
        pw->Print(String("    provider="));
        widget->mProvider->mId->ToString(&strTmp);
        pw->Println(strTmp);
    }
    if (widget->mHost != NULL) {
        pw->Print(String("    host.callbacks="));
        pw->Println(TO_IINTERFACE(widget->mHost->mCallbacks));
    }
    if (widget->mViews != NULL) {
        pw->Print(String("    views="));
        pw->Println(TO_IINTERFACE(widget->mViews));
    }
    if (widget->mOptions != NULL) {
        pw->Print(String("    options="));
        pw->Println(TO_IINTERFACE(widget->mOptions));
    }
}

void AppWidgetServiceImpl::SerializeProvider(
    /* [in] */ IXmlSerializer* out,
    /* [in] */ Provider* p)
{
    out->WriteStartTag(String(NULL), String("p"));

    AutoPtr<IComponentName> componentName;
    p->mInfo->GetProvider((IComponentName**)&componentName);
    String packageName;
    componentName->GetPackageName(&packageName);
    out->WriteAttribute(String(NULL), String("pkg"), packageName);

    String className;
    componentName->GetClassName(&className);
    out->WriteAttribute(String(NULL), String("cl"), className);
    out->WriteAttribute(String(NULL), String("tag"), StringUtils::ToHexString(p->mTag));
    out->WriteEndTag(String(NULL), String("p"));
}

void AppWidgetServiceImpl::SerializeHost(
    /* [in] */ IXmlSerializer* out,
    /* [in] */ Host* host)
{
    out->WriteStartTag(String(NULL), String("h"));
    out->WriteAttribute(String(NULL), String("pkg"), host->mId->mPackageName);
    out->WriteAttribute(String(NULL), String("id"), StringUtils::ToHexString(host->mId->mHostId));
    out->WriteAttribute(String(NULL), String("tag"), StringUtils::ToHexString(host->mTag));
    out->WriteEndTag(String(NULL), String("h"));
}

void AppWidgetServiceImpl::SerializeAppWidget(
    /* [in] */ IXmlSerializer* out,
    /* [in] */ Widget* widget)
{
    out->WriteStartTag(String(NULL), String("g"));
    out->WriteAttribute(String(NULL), String("id"), StringUtils::ToHexString(widget->mAppWidgetId));
    out->WriteAttribute(String(NULL), String("rid"), StringUtils::ToHexString(widget->mRestoredId));
    out->WriteAttribute(String(NULL), String("h"), StringUtils::ToHexString(widget->mHost->mTag));
    if (widget->mProvider != NULL) {
        out->WriteAttribute(String(NULL), String("p"), StringUtils::ToHexString(widget->mProvider->mTag));
    }
    if (widget->mOptions != NULL) {
        Int32 resTmp = 0;
        widget->mOptions->GetInt32(IAppWidgetManager::OPTION_APPWIDGET_MIN_WIDTH, &resTmp);
        out->WriteAttribute(String(NULL), String("min_width"), StringUtils::ToHexString(resTmp));

        widget->mOptions->GetInt32(IAppWidgetManager::OPTION_APPWIDGET_MIN_HEIGHT, &resTmp);
        out->WriteAttribute(String(NULL), String("min_height"), StringUtils::ToHexString(resTmp));

        widget->mOptions->GetInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_WIDTH, &resTmp);
        out->WriteAttribute(String(NULL), String("max_width"), StringUtils::ToHexString(resTmp));

        widget->mOptions->GetInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_HEIGHT, &resTmp);
        out->WriteAttribute(String(NULL), String("max_height"), StringUtils::ToHexString(resTmp));

        widget->mOptions->GetInt32(IAppWidgetManager::OPTION_APPWIDGET_HOST_CATEGORY, &resTmp);
        out->WriteAttribute(String(NULL), String("host_category"), StringUtils::ToHexString(resTmp));
    }
    out->WriteEndTag(String(NULL), String("g"));
}

AutoPtr<AppWidgetServiceImpl::Provider> AppWidgetServiceImpl::ParseProviderInfoXml(
    /* [in] */ ProviderId* providerId,
    /* [in] */ IResolveInfo* ri)
{
    AutoPtr<Provider> provider;
    AutoPtr<IActivityInfo> activityInfo;
    ri->GetActivityInfo((IActivityInfo**)&activityInfo);
    AutoPtr<IXmlResourceParser> parser;
    //try {
        AutoPtr<IPackageManager> packageManagerTmp;
        mContext->GetPackageManager((IPackageManager**)&packageManagerTmp);
        IPackageItemInfo::Probe(activityInfo)->LoadXmlMetaData(packageManagerTmp,
                IAppWidgetManager::META_DATA_APPWIDGET_PROVIDER, (IXmlResourceParser**)&parser);
        if (parser == NULL) {
            Slogger::W(TAG, "No %s meta-data for AppWidget provider \'%s\'",
                IAppWidgetManager::META_DATA_APPWIDGET_PROVIDER.string(), TO_CSTR(providerId));
            return NULL;
        }

        AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(IXmlPullParser::Probe(parser));
        Int32 type = 0;
        while ((IXmlPullParser::Probe(parser)->Next(&type), type) != IXmlPullParser::END_DOCUMENT
                && type != IXmlPullParser::START_TAG) {
            // drain whitespace, comments, etc.
        }

        String nodeName;
        IXmlPullParser::Probe(parser)->GetName(&nodeName);
        if (!String("appwidget-provider").Equals(nodeName)) {
            Slogger::W(TAG, "Meta-data does not start with appwidget-provider tag for"
                " AppWidget provider %s for user %d", TO_CSTR(providerId->mComponentName), providerId->mUid);
            return NULL;
        }

        provider = new Provider();
        provider->mId = providerId;

        provider->mInfo = NULL;
        CAppWidgetProviderInfo::New((IAppWidgetProviderInfo**)&(provider->mInfo));
        IAppWidgetProviderInfo* info = provider->mInfo;
        info->SetProvider(providerId->mComponentName);
        info->SetProviderInfo(activityInfo);

        AutoPtr<IResources> resources;
        Int64 identity = Binder::ClearCallingIdentity();
        //try {
            String packageName;
            IPackageItemInfo::Probe(activityInfo)->GetPackageName(&packageName);

            AutoPtr<IUserHandleHelper> helper;
            CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
            Int32 userId = 0;
            helper->GetUserId(providerId->mUid, &userId);
            packageManagerTmp->GetResourcesForApplicationAsUser(packageName, userId, (IResources**)&resources);
        //} finally {
            Binder::RestoreCallingIdentity(identity);
        //}

        AutoPtr< ArrayOf<Int32> > styleAttrsTmp = TO_ATTRS_ARRAYOF(R::styleable::AppWidgetProviderInfo);
        AutoPtr<ITypedArray> sa;
        resources->ObtainAttributes(attrs, styleAttrsTmp, (ITypedArray**)&sa);

        AutoPtr<ITypedValue> value;
        sa->PeekValue(R::styleable::AppWidgetProviderInfo_minWidth, (ITypedValue**)&value);
        Int32 intDataTmp = 0;
        if (value != NULL)
            value->GetData(&intDataTmp);
        info->SetMinWidth(intDataTmp);

        value = NULL;
        sa->PeekValue(R::styleable::AppWidgetProviderInfo_minHeight, (ITypedValue**)&value);
        intDataTmp = 0;
        if (value != NULL)
            value->GetData(&intDataTmp);
        info->SetMinHeight(intDataTmp);

        value = NULL;
        sa->PeekValue(R::styleable::AppWidgetProviderInfo_minResizeWidth, (ITypedValue**)&value);
        if (value != NULL)
            value->GetData(&intDataTmp);
        else
            info->GetMinWidth(&intDataTmp);
        info->SetMinResizeWidth(intDataTmp);

        value = NULL;
        sa->PeekValue(R::styleable::AppWidgetProviderInfo_minResizeHeight, (ITypedValue**)&value);
        if (value != NULL)
            value->GetData(&intDataTmp);
        else
            info->GetMinHeight(&intDataTmp);
        info->SetMinResizeHeight(intDataTmp);

        sa->GetInt32(R::styleable::AppWidgetProviderInfo_updatePeriodMillis, 0, &intDataTmp);
        info->SetUpdatePeriodMillis(intDataTmp);

        sa->GetResourceId(R::styleable::AppWidgetProviderInfo_initialLayout, 0, &intDataTmp);
        info->SetInitialLayout(intDataTmp);

        sa->GetResourceId(R::styleable::AppWidgetProviderInfo_initialKeyguardLayout, 0, &intDataTmp);
        info->SetInitialKeyguardLayout(intDataTmp);

        String className;
        sa->GetString(R::styleable::AppWidgetProviderInfo_configure, &className);
        if (className != NULL) {
            String packageName;
            providerId->mComponentName->GetPackageName(&packageName);
            AutoPtr<IComponentName> componentName;
            CComponentName::New(packageName, className, (IComponentName**)&componentName);
            info->SetConfigure(componentName);
        }

        AutoPtr<ICharSequence> charSequenceTmp;
        IPackageItemInfo::Probe(activityInfo)->LoadLabel(packageManagerTmp, (ICharSequence**)&charSequenceTmp);
        String strTmp;
        charSequenceTmp->ToString(&strTmp);
        info->SetLabel(strTmp);

        Int32 iconResourcesTmp = 0;
        ri->GetIconResource(&iconResourcesTmp);
        info->SetIcon(iconResourcesTmp);

        Int32 resourceIdTmp = 0;
        sa->GetResourceId(R::styleable::AppWidgetProviderInfo_previewImage, 0, &resourceIdTmp);
        info->SetPreviewImage(resourceIdTmp);

        sa->GetResourceId(R::styleable::AppWidgetProviderInfo_autoAdvanceViewId, -1, &resourceIdTmp);
        info->SetAutoAdvanceViewId(resourceIdTmp);

        Int32 intTmp = 0;
        sa->GetInt32(R::styleable::AppWidgetProviderInfo_resizeMode, IAppWidgetProviderInfo::RESIZE_NONE, &intTmp);
        info->SetResizeMode(intTmp);

        sa->GetInt32(R::styleable::AppWidgetProviderInfo_widgetCategory,
            IAppWidgetProviderInfo::WIDGET_CATEGORY_HOME_SCREEN, &intTmp);
        info->SetWidgetCategory(intTmp);

        sa->Recycle();
    //} catch (IOException | PackageManager.NameNotFoundException | XmlPullParserException e) {
    //    // Ok to catch Exception here, because anything going wrong because
    //    // of what a client process passes to us should not be fatal for the
    //    // system process.
    //    Slog.w(TAG, "XML parsing failed for AppWidget provider "
    //            + providerId.componentName + " for user " + providerId.uid, e);
    //    return null;
    //} finally {
        if (parser != NULL) {
            parser->Close();
        }
    //}
    return provider;
}

Int32 AppWidgetServiceImpl::GetUidForPackage(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId)
{
    AutoPtr<IPackageInfo> pkgInfo;

    Int64 identity = Binder::ClearCallingIdentity();
    //try {
        mPackageManager->GetPackageInfo(packageName, 0, userId, (IPackageInfo**)&pkgInfo);
    //} catch (RemoteException re) {
        // Shouldn't happen, local call
    //} finally {
        Binder::RestoreCallingIdentity(identity);
    //}

    AutoPtr<IApplicationInfo> applicationInfo;
    if (pkgInfo == NULL
        || (pkgInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo), applicationInfo == NULL)) {
        return -1;
    }

    Int32 result = 0;
    applicationInfo->GetUid(&result);
    return result;
}

AutoPtr<IActivityInfo> AppWidgetServiceImpl::GetProviderInfo(
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 userId)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_UPDATE, (IIntent**)&intent);
    intent->SetComponent(componentName);

    AutoPtr<IList> receivers = QueryIntentReceivers(intent, userId);
    // We are setting component, so there is only one or none.
    Boolean resTmp = FALSE;
    receivers->IsEmpty(&resTmp);
    if (!resTmp) {
        AutoPtr<IInterface> interfaceTmp;
        receivers->Get(0, (IInterface**)&interfaceTmp);
        IResolveInfo* resolveInfo = IResolveInfo::Probe(interfaceTmp);
        AutoPtr<IActivityInfo> result;
        resolveInfo->GetActivityInfo((IActivityInfo**)&result);
        return result;
    }
    return NULL;
}

AutoPtr<IList> AppWidgetServiceImpl::QueryIntentReceivers(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 userId)
{
    Int64 identity = Binder::ClearCallingIdentity();
    AutoPtr<IList> result;
    //try {
        Int32 flags = IPackageManager::GET_META_DATA;
        flags |= IPackageManager::GET_SHARED_LIBRARY_FILES;

        AutoPtr<IContentResolver> contentResolver;
        mContext->GetContentResolver((IContentResolver**)&contentResolver);
        String strTmp;
        intent->ResolveTypeIfNeeded(contentResolver, &strTmp);
        mPackageManager->QueryIntentReceivers(intent,
                strTmp, flags, userId, (IList**)&result);

    //} catch (RemoteException re) {
    //    return Collections.emptyList();
    //} finally {
        Binder::RestoreCallingIdentity(identity);
    //}
    return result;
}

void AppWidgetServiceImpl::OnUserStarted(
    /* [in] */ Int32 userId)
{
    AutoLock syncLock(mLock);
    EnsureGroupStateLoadedLocked(userId);
    Int32 N = 0;
    mProviders->GetSize(&N);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mProviders->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Provider* provider = (Provider*)objTmp;

        // Send broadcast only to the providers of the user.
        Int32 userId = 0;
        provider->GetUserId(&userId);
        if (userId != userId) {
            continue;
        }

        Int32 size = 0;
        provider->mWidgets->GetSize(&size);
        if (size > 0) {
            SendEnableIntentLocked(provider);
            AutoPtr< ArrayOf<Int32> > appWidgetIds = GetWidgetIds(provider->mWidgets);
            SendUpdateIntentLocked(provider, appWidgetIds);
            RegisterForBroadcastsLocked(provider, appWidgetIds);
        }
    }
}

void AppWidgetServiceImpl::LoadGroupStateLocked(
    /* [in] */ ArrayOf<Int32>* profileIds)
{
    // We can bind the widgets to host and providers only after
    // reading the host and providers for all users since a widget
    // can have a host and a provider in different users.
    AutoPtr<IList> loadedWidgets;
    CArrayList::New((IList**)&loadedWidgets);

    Int32 version = 0;
    Int32 profileIdCount = profileIds->GetLength();
    for (Int32 i = 0; i < profileIdCount; ++i) {
        Int32 profileId = (*profileIds)[i];

        // No file written for this user - nothing to do.
        AutoPtr<IAtomicFile> file = GetSavedStateFile(profileId);
        AutoPtr<IFileInputStream> stream;
        if (FAILED(file->OpenRead((IFileInputStream**)&stream))) {
            Slogger::W(TAG, "Failed to read state: ");
        }
        else {
            version = ReadProfileStateFromFileLocked(stream, profileId, loadedWidgets);
            AutoPtr<IIoUtils> ioUtils;
            CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
            ioUtils->CloseQuietly(ICloseable::Probe(stream));
        }
    }

    if (version >= 0) {
        BindLoadedWidgets(loadedWidgets);
        PerformUpgradeLocked(version);
    }
    else {
        Slogger::W(TAG, "Failed to read state, clearing widgets and hosts.");
        mWidgets->Clear();
        mHosts->Clear();
        Int32 N = 0;
        mProviders->GetSize(&N);
        for (Int32 i = 0; i < N; ++i) {
            AutoPtr<IInterface> interfaceTmp;
            mProviders->Get(i, (IInterface**)&interfaceTmp);
            Provider* provider = (Provider*)IObject::Probe(interfaceTmp);
            provider->mWidgets->Clear();
        }
    }
}

void AppWidgetServiceImpl::BindLoadedWidgets(
    /* [in] */ IList* loadedWidgets)
{
    Int32 loadedWidgetCount = 0;
    loadedWidgets->GetSize(&loadedWidgetCount);
    for (Int32 i = loadedWidgetCount - 1; i >= 0; --i) {
        AutoPtr<IInterface> interfaceTmp;
        loadedWidgets->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        LoadedWidgetState* loadedWidget = (LoadedWidgetState*)objTmp;
        loadedWidgets->Remove(interfaceTmp);
        Widget* widget = loadedWidget->mWidget;

        widget->mProvider = NULL;
        widget->mProvider = FindProviderByTag(loadedWidget->mProviderTag);
        if (widget->mProvider == NULL) {
            // This provider is gone. We just let the host figure out
            // that this happened when it fails to load it.
            continue;
        }

        widget->mHost = NULL;
        widget->mHost = FindHostByTag(loadedWidget->mHostTag);
        if (widget->mHost == NULL) {
            // This host is gone.
            continue;
        }

        widget->mProvider->mWidgets->Add(TO_IINTERFACE(widget));
        widget->mHost->mWidgets->Add(TO_IINTERFACE(widget));
        mWidgets->Add(TO_IINTERFACE(widget));
    }
}

AutoPtr<AppWidgetServiceImpl::Provider> AppWidgetServiceImpl::FindProviderByTag(
    /* [in] */ Int32 tag)
{
    if (tag < 0) {
        return NULL;
    }
    Int32 providerCount = 0;
    mProviders->GetSize(&providerCount);
    for (Int32 i = 0; i < providerCount; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mProviders->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Provider* provider = (Provider*)objTmp;
        if (provider->mTag == tag) {
            return provider;
        }
    }
    return NULL;
}

AutoPtr<AppWidgetServiceImpl::Host> AppWidgetServiceImpl::FindHostByTag(
    /* [in] */ Int32 tag)
{
    if (tag < 0) {
        return NULL;
    }
    Int32 hostCount = 0;
    mHosts->GetSize(&hostCount);
    for (Int32 i = 0; i < hostCount; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mHosts->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Host* host = (Host*)objTmp;
        if (host->mTag == tag) {
            return host;
        }
    }
    return NULL;
}

void AppWidgetServiceImpl::SaveStateLocked(
    /* [in] */ Int32 userId)
{
    TagProvidersAndHosts();
    AutoPtr< ArrayOf<Int32> > profileIds;
    mSecurityPolicy->GetEnabledGroupProfileIds(userId, (ArrayOf<Int32>**)&profileIds);
    Int32 profileCount = profileIds->GetLength();
    for (Int32 i = 0; i < profileCount; ++i) {
        Int32 profileId = (*profileIds)[i];
        AutoPtr<IAtomicFile> file = GetSavedStateFile(profileId);
        AutoPtr<IFileOutputStream> stream;
        //try {
            file->StartWrite((IFileOutputStream**)&stream);
            if (WriteProfileStateToFileLocked(stream, profileId)) {
                file->FinishWrite(stream);
            }
            else {
                file->FailWrite(stream);
                Slogger::W(TAG, "Failed to save state, restoring backup.");
            }
        //} catch (IOException e) {
            //Slog.w(TAG, "Failed open state file for write: " + e);
        //}
    }
}

void AppWidgetServiceImpl::TagProvidersAndHosts()
{
    Int32 providerCount = 0;
    mProviders->GetSize(&providerCount);
    for (Int32 i = 0; i < providerCount; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mProviders->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Provider* provider = (Provider*)objTmp;
        provider->mTag = i;
    }

    Int32 hostCount = 0;
    mHosts->GetSize(&hostCount);
    for (Int32 i = 0; i < hostCount; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mHosts->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Host* host = (Host*)objTmp;
        host->mTag = i;
    }
}

void AppWidgetServiceImpl::ClearProvidersAndHostsTagsLocked()
{
    Int32 providerCount = 0;
    mProviders->GetSize(&providerCount);
    for (Int32 i = 0; i < providerCount; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mProviders->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Provider* provider = (Provider*)objTmp;
        provider->mTag = TAG_UNDEFINED;
    }

    Int32 hostCount = 0;
    mHosts->GetSize(&hostCount);
    for (Int32 i = 0; i < hostCount; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mHosts->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Host* host = (Host*)objTmp;
        host->mTag = TAG_UNDEFINED;
    }
}

Boolean AppWidgetServiceImpl::WriteProfileStateToFileLocked(
    /* [in] */ IFileOutputStream* stream,
    /* [in] */ Int32 userId)
{
    Int32 N = 0;
    //try {
        AutoPtr<IXmlSerializer> out;
        CFastXmlSerializer::New((IXmlSerializer**)&out);
        out->SetOutput(IOutputStream::Probe(stream), String("utf-8"));
        out->StartDocument(String(NULL), TRUE);
        out->WriteStartTag(String(NULL), String("gs"));
        out->WriteAttribute(String(NULL), String("version"), StringUtils::ToString(CURRENT_VERSION));

        mProviders->GetSize(&N);
        Int32 userIdTmp = 0;
        for (Int32 i = 0; i < N; ++i) {
            AutoPtr<IInterface> interfaceTmp;
            mProviders->Get(i, (IInterface**)&interfaceTmp);
            IObject* objTmp = IObject::Probe(interfaceTmp);
            Provider* provider = (Provider*)objTmp;
            provider->GetUserId(&userIdTmp);
            if (userIdTmp != userId) {
                continue;
            }

            Int32 size = 0;
            provider->mWidgets->GetSize(&size);
            if (size > 0) {
                SerializeProvider(out, provider);
            }
        }

        mHosts->GetSize(&N);
        for (Int32 i = 0; i < N; ++i) {
            AutoPtr<IInterface> interfaceTmp;
            mHosts->Get(i, (IInterface**)&interfaceTmp);
            IObject* objTmp = IObject::Probe(interfaceTmp);
            Host* host = (Host*)objTmp;
            host->GetUserId(&userIdTmp);
            if (userIdTmp != userId) {
                continue;
            }
            SerializeHost(out, host);
        }

        mWidgets->GetSize(&N);
        for (Int32 i = 0; i < N; ++i) {
            AutoPtr<IInterface> interfaceTmp;
            mWidgets->Get(i, (IInterface**)&interfaceTmp);
            IObject* objTmp = IObject::Probe(interfaceTmp);
            Widget* widget = (Widget*)objTmp;
            widget->mHost->GetUserId(&userIdTmp);
            if (userIdTmp != userId) {
                continue;
            }
            SerializeAppWidget(out, widget);
        }

        AutoPtr<IIterator> it;
        ICollection::Probe(mPackagesWithBindWidgetPermission)->GetIterator((IIterator**)&it);
        Boolean hasNext = FALSE;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> interfaceTmp;
            it->GetNext((IInterface**)&interfaceTmp);
            IPair* binding = IPair::Probe(interfaceTmp);

            AutoPtr<IInterface> firstTmp, secondTmp;
            binding->GetFirst((IInterface**)&firstTmp);
            binding->GetSecond((IInterface**)&secondTmp);
            Int32 first = 0;
            IInteger32::Probe(firstTmp)->GetValue(&first);
            String second;
            ICharSequence::Probe(secondTmp)->ToString(&second);

            if (first != userId) {
                continue;
            }
            out->WriteStartTag(String(NULL), String("b"));
            out->WriteAttribute(String(NULL), String("packageName"), second);
            out->WriteEndTag(String(NULL), String("b"));
        }

        out->WriteEndTag(String(NULL), String("gs"));
        out->EndDocument();
        return TRUE;
    //} catch (IOException e) {
        //Slog.w(TAG, "Failed to write state: " + e);
        //return false;
    //}
}

Int32 AppWidgetServiceImpl::ReadProfileStateFromFileLocked(
    /* [in] */ IFileInputStream* stream,
    /* [in] */ Int32 userId,
    /* [in] */ IList* outLoadedWidgets)
{
    Int32 version = -1;
    String nullStr;
    //try {
    AutoPtr<IXmlPullParser> parser;
    Xml::NewPullParser((IXmlPullParser**)&parser);
    parser->SetInput(IInputStream::Probe(stream), nullStr);

    Int32 legacyProviderIndex = -1;
    Int32 legacyHostIndex = -1;
    Int32 type = 0;
    do {
        ECode ec = parser->Next(&type);
        if (FAILED(ec)) {
            Slogger::W(TAG, "Failed parsing ec = 0x%08x", ec);
            return -1;
        }
        if (type == IXmlPullParser::START_TAG) {
            String tag;
            ec = parser->GetName(&tag);
            if (String("gs").Equals(tag)) {
                String attributeValue;
                parser->GetAttributeValue(nullStr, String("version"), &attributeValue);
                //try {
                ECode ec = StringUtils::Parse(attributeValue, &version);
                if (FAILED(ec)) version = 0;
                //} catch (NumberFormatException e) {
                //    version = 0;
                //}
            }
            else if (String("p").Equals(tag)) {
                legacyProviderIndex++;
                String pkg;
                parser->GetAttributeValue(nullStr, String("pkg"), &pkg);
                String cl;
                parser->GetAttributeValue(nullStr, String("cl"), &cl);

                pkg = GetCanonicalPackageName(pkg, cl, userId);
                if (pkg.IsNull()) {
                    continue;
                }

                Int32 uid = GetUidForPackage(pkg, userId);
                if (uid < 0) {
                    continue;
                }

                AutoPtr<IComponentName> componentName;
                CComponentName::New(pkg, cl, (IComponentName**)&componentName);

                AutoPtr<IActivityInfo> providerInfo = GetProviderInfo(componentName, userId);
                if (providerInfo == NULL) {
                    continue;
                }

                AutoPtr<ProviderId> providerId = new ProviderId(uid, componentName);
                AutoPtr<Provider> provider = LookupProviderLocked(providerId);

                if (provider == NULL && mSafeMode) {
                    // if we're in safe mode, make a temporary one
                    provider = new Provider();
                    CAppWidgetProviderInfo::New((IAppWidgetProviderInfo**)&provider->mInfo);
                    provider->mInfo->SetProvider(providerId->mComponentName);
                    provider->mInfo->SetProviderInfo(providerInfo);
                    provider->mZombie = TRUE;
                    provider->mId = providerId;
                    mProviders->Add(TO_IINTERFACE(provider));
                }
                else if (provider == NULL) {
                    Slogger::W(TAG, "Failed parsing NullPointerException");
                    return -1;
                }

                String tagAttribute;
                parser->GetAttributeValue(nullStr, String("tag"), &tagAttribute);
                Int32 providerTag = !TextUtils::IsEmpty(tagAttribute)
                        ? StringUtils::ParseInt32(tagAttribute, 16) : legacyProviderIndex;
                provider->mTag = providerTag;
            }
            else if (String("h").Equals(tag)) {
                legacyHostIndex++;
                AutoPtr<Host> host = new Host();
                // TODO: do we need to check that this package has the same signature
                // as before?
                String pkg;
                parser->GetAttributeValue(nullStr, String("pkg"), &pkg);

                Int32 uid = GetUidForPackage(pkg, userId);
                if (uid < 0) {
                    host->mZombie = TRUE;
                }

                if (!host->mZombie || mSafeMode) {
                    // In safe mode, we don't discard the hosts we don't recognize
                    // so that they're not pruned from our list. Otherwise, we do.
                    String attrValue;
                    parser->GetAttributeValue(nullStr, String("id"), &attrValue);
                    Int32 hostId = StringUtils::ParseInt32(attrValue, 16);

                    String tagAttribute;
                    parser->GetAttributeValue(nullStr, String("tag"), &tagAttribute);
                    Int32 hostTag = !TextUtils::IsEmpty(tagAttribute)
                            ? StringUtils::ParseInt32(tagAttribute, 16) : legacyHostIndex;

                    host->mTag = hostTag;
                    host->mId = new HostId(uid, hostId, pkg);
                    mHosts->Add(TO_IINTERFACE(host));
                }
            }
            else if (String("b").Equals(tag)) {
                String packageName;
                parser->GetAttributeValue(nullStr, String("packageName"), &packageName);
                Int32 uid = GetUidForPackage(packageName, userId);
                if (uid >= 0) {
                    AutoPtr<IPairHelper> pairHelper;
                    CPairHelper::AcquireSingleton((IPairHelper**)&pairHelper);
                    AutoPtr<IPair> packageId;
                    pairHelper->Create(CoreUtils::Convert(userId), CoreUtils::Convert(packageName), (IPair**)&packageId);
                    ICollection::Probe(mPackagesWithBindWidgetPermission)->Add(packageId);
                }
            }
            else if (String("g").Equals(tag)) {
                AutoPtr<Widget> widget = new Widget();
                String attrValue;
                parser->GetAttributeValue(nullStr, String("id"), &attrValue);
                widget->mAppWidgetId = StringUtils::ParseInt32(attrValue, 16);
                SetMinAppWidgetIdLocked(userId, widget->mAppWidgetId + 1);

                // restored ID is allowed to be absent
                String restoredIdString;
                parser->GetAttributeValue(nullStr, String("rid"), &restoredIdString);
                widget->mRestoredId = (restoredIdString.IsNull()) ? 0
                        : StringUtils::ParseInt32(restoredIdString, 16);

                AutoPtr<IBundle> options;
                CBundle::New((IBundle**)&options);
                String minWidthString;
                parser->GetAttributeValue(nullStr, String("min_width"), &minWidthString);
                if (!minWidthString.IsNull()) {
                    options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MIN_WIDTH,
                            StringUtils::ParseInt32(minWidthString, 16));
                }

                String minHeightString;
                parser->GetAttributeValue(nullStr, String("min_height"), &minHeightString);
                if (!minHeightString.IsNull()) {
                    options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MIN_HEIGHT,
                            StringUtils::ParseInt32(minHeightString, 16));
                }

                String maxWidthString;
                parser->GetAttributeValue(nullStr, String("max_width"), &maxWidthString);
                if (!maxWidthString.IsNull()) {
                    options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_WIDTH,
                            StringUtils::ParseInt32(maxWidthString, 16));
                }

                String maxHeightString;
                parser->GetAttributeValue(nullStr, String("max_height"), &maxHeightString);
                if (!maxHeightString.IsNull()) {
                    options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_HEIGHT,
                            StringUtils::ParseInt32(maxHeightString, 16));
                }

                String categoryString;
                parser->GetAttributeValue(nullStr, String("host_category"), &categoryString);
                if (!categoryString.IsNull()) {
                    options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_HOST_CATEGORY,
                            StringUtils::ParseInt32(categoryString, 16));
                }
                widget->mOptions = options;

                parser->GetAttributeValue(nullStr, String("h"), &attrValue);
                Int32 hostTag = StringUtils::ParseInt32(attrValue, 16);

                parser->GetAttributeValue(nullStr, String("p"), &attrValue);
                Int32 providerTag = (!attrValue.IsNull()) ? StringUtils::ParseInt32(
                        attrValue, 16) : TAG_UNDEFINED;

                // We can match widgets with hosts and providers only after hosts
                // and providers for all users have been loaded since the widget
                // host and provider can be in different user profiles.
                AutoPtr<LoadedWidgetState> loadedWidgets = new LoadedWidgetState(widget,
                        hostTag, providerTag);
                outLoadedWidgets->Add(TO_IINTERFACE(loadedWidgets));
            }
        }
    } while (type != IXmlPullParser::END_DOCUMENT);
    //} catch (NullPointerException
    //        | NumberFormatException
    //        | XmlPullParserException
    //        | IOException
    //        | IndexOutOfBoundsException e) {
    //    Slog.w(TAG, "failed parsing " + e);
    //    return -1;
    //}
    return version;
}

ECode AppWidgetServiceImpl::PerformUpgradeLocked(
    /* [in] */ Int32 fromVersion)
{
    if (fromVersion < CURRENT_VERSION) {
        Slogger::V(TAG, "Upgrading widget database from %d to %d", fromVersion, CURRENT_VERSION);
    }

    Int32 version = fromVersion;

    // Update 1: keyguard moved from package "android" to "com.android.keyguard"
    if (version == 0) {
        AutoPtr<HostId> oldHostId = new HostId(Process::MyUid(), KEYGUARD_HOST_ID, OLD_KEYGUARD_HOST_PACKAGE);
        AutoPtr<Host> host = LookupHostLocked(oldHostId);
        if (host != NULL) {
            Int32 uid = GetUidForPackage(NEW_KEYGUARD_HOST_PACKAGE, IUserHandle::USER_OWNER);
            if (uid >= 0) {
                host->mId = new HostId(uid, KEYGUARD_HOST_ID, NEW_KEYGUARD_HOST_PACKAGE);
            }
        }
        version = 1;
    }

    if (version != CURRENT_VERSION) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<IFile> AppWidgetServiceImpl::GetStateFile(
    /* [in] */ Int32 userId)
{
    AutoPtr<IFile> result;
    CFile::New(Environment::GetUserSystemDirectory(userId), STATE_FILENAME, (IFile**)&result);
    return result;
}

AutoPtr<IAtomicFile> AppWidgetServiceImpl::GetSavedStateFile(
    /* [in] */ Int32 userId)
{
    AutoPtr<IFile> dir = Environment::GetUserSystemDirectory(userId);
    AutoPtr<IFile> settingsFile = GetStateFile(userId);
    Boolean exists = FALSE;
    settingsFile->Exists(&exists);
    if (!exists && userId == IUserHandle::USER_OWNER) {
        dir->Exists(&exists);
        if (!exists) {
            Boolean resTmp;
            dir->Mkdirs(&resTmp);
        }
        // Migrate old data
        AutoPtr<IFile> oldFile;
        CFile::New(String("/data/system/") + STATE_FILENAME, (IFile**)&oldFile);
        // Method doesn't throw an exception on failure. Ignore any errors
        // in moving the file (like non-existence)
        Boolean resTmp;
        oldFile->RenameTo(settingsFile, &resTmp);
    }
    AutoPtr<IAtomicFile> result;
    CAtomicFile::New(settingsFile, (IAtomicFile**)&result);
    return result;
}

void AppWidgetServiceImpl::OnUserStopped(
    /* [in] */ Int32 userId)
{
    AutoLock syncLock(mLock);
    Boolean providersChanged = FALSE;
    Boolean crossProfileWidgetsChanged = FALSE;

    // Remove widgets that have both host and provider in the user.
    Int32 widgetCount = 0;
    mWidgets->GetSize(&widgetCount);
    for (Int32 i = widgetCount - 1; i >= 0; --i) {
        AutoPtr<IInterface> interfaceTmp;
        mWidgets->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Widget* widget = (Widget*)objTmp;

        Int32 userIdTmp = 0;
        widget->mHost->GetUserId(&userIdTmp);
        Boolean hostInUser = userIdTmp == userId;
        Boolean hasProvider = widget->mProvider != NULL;

        widget->mProvider->GetUserId(&userIdTmp);
        Boolean providerInUser = hasProvider && userIdTmp == userId;

        // If both host and provider are in the user, just drop the widgets
        // as we do not want to make host callbacks and provider broadcasts
        // as the host and the provider will be killed.
        if (hostInUser && (!hasProvider || providerInUser)) {
            AutoPtr<IInterface> widgetTmp;
            mWidgets->Get(i, (IInterface**)&widgetTmp);
            mWidgets->Remove(TO_IINTERFACE(widgetTmp));

            widget->mHost->mWidgets->Remove(TO_IINTERFACE(widget));
            widget->mHost = NULL;
            if (hasProvider) {
                widget->mProvider->mWidgets->Remove(TO_IINTERFACE(widget));
                widget->mProvider = NULL;
            }
        }
    }

    // Remove hosts and notify providers in other profiles.
    Int32 hostCount = 0;
    mHosts->GetSize(&hostCount);
    for (Int32 i = hostCount - 1; i >= 0; --i) {
        AutoPtr<IInterface> interfaceTmp;
        mHosts->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Host* host = (Host*)objTmp;

        Int32 userIdTmp = 0;
        host->GetUserId(&userIdTmp);
        if (userIdTmp == userId) {
            Boolean resTmp = FALSE;
            host->mWidgets->IsEmpty(&resTmp);
            crossProfileWidgetsChanged |= !resTmp;
            DeleteHostLocked(host);
        }
    }

    // Remove the providers and notify hosts in other profiles.
    Int32 providerCount = 0;
    mProviders->GetSize(&providerCount);
    for (Int32 i = providerCount - 1; i >= 0; --i) {
        AutoPtr<IInterface> interfaceTmp;
        mProviders->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Provider* provider = (Provider*)objTmp;

        Int32 userIdTmp = 0;
        provider->GetUserId(&userIdTmp);
        if (userIdTmp == userId) {
            Boolean resTmp = FALSE;
            provider->mWidgets->IsEmpty(&resTmp);
            crossProfileWidgetsChanged |= !resTmp;
            providersChanged = TRUE;
            DeleteProviderLocked(provider);
        }
    }

    // Remove grants for this user.
    Int32 grantCount = 0;
    ICollection::Probe(mPackagesWithBindWidgetPermission)->GetSize(&grantCount);
    for (Int32 i = grantCount - 1; i >= 0; --i) {
        AutoPtr<IInterface> interfaceTmp;
        mPackagesWithBindWidgetPermission->GetValueAt(i, (IInterface**)&interfaceTmp);
        IPair* packageId = IPair::Probe(interfaceTmp);

        AutoPtr<IInterface> firstTmp;
        packageId->GetFirst((IInterface**)&firstTmp);
        IInteger32* first1Tmp = IInteger32::Probe(firstTmp);
        Int32 first = 0;
        first1Tmp->GetValue(&first);
        if (first == userId) {
            AutoPtr<IInterface> removeAtTmp;
            mPackagesWithBindWidgetPermission->RemoveAt(i, (IInterface**)&removeAtTmp);
        }
    }

    // Take a note we no longer have state for this user.
    Int32 userIndex = 0;
    mLoadedUserIds->IndexOfKey(userId, &userIndex);
    if (userIndex >= 0) {
        mLoadedUserIds->RemoveAt(userIndex);
    }

    // Remove the widget id counter.
    Int32 nextIdIndex = 0;
    mNextAppWidgetIds->IndexOfKey(userId, &nextIdIndex);
    if (nextIdIndex >= 0) {
        mNextAppWidgetIds->RemoveAt(nextIdIndex);
    }

    // Announce removed provider changes to all hosts in the group.
    if (providersChanged) {
        ScheduleNotifyGroupHostsForProvidersChangedLocked(userId);
    }

    // Save state if removing a profile changed the group state.
    // Nothing will be saved if the group parent was removed.
    if (crossProfileWidgetsChanged) {
        SaveGroupStateAsync(userId);
    }
}

Boolean AppWidgetServiceImpl::UpdateProvidersForPackageLocked(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [in] */ ISet* removedProviders)
{
    Boolean providersUpdated = FALSE;

    AutoPtr<IHashSet> keep;
    CHashSet::New((IHashSet**)&keep);
    AutoPtr<IIntent> intent;
    CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_UPDATE, (IIntent**)&intent);
    intent->SetPackage(packageName);
    AutoPtr<IList> broadcastReceivers = QueryIntentReceivers(intent, userId);

    // add the missing ones and collect which ones to keep
    Int32 N = 0;
    if (broadcastReceivers != NULL)
        broadcastReceivers->GetSize(&N);;
    for (int i = 0; i < N; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        broadcastReceivers->Get(i, (IInterface**)&interfaceTmp);
        IResolveInfo* ri = IResolveInfo::Probe(interfaceTmp);
        AutoPtr<IActivityInfo> ai;
        ri->GetActivityInfo((IActivityInfo**)&ai);

        AutoPtr<IApplicationInfo> applicationInfo;
        IComponentInfo::Probe(ai)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        Int32 flags = 0;
        applicationInfo->GetFlags(&flags);
        if ((flags & IApplicationInfo::FLAG_EXTERNAL_STORAGE) != 0) {
            continue;
        }

        String packageNameTmp;
        IPackageItemInfo::Probe(ai)->GetPackageName(&packageNameTmp);
        if (packageName.Equals(packageNameTmp)) {
            Int32 uidTmp = 0;
            applicationInfo->GetUid(&uidTmp);
            String name;
            IPackageItemInfo::Probe(ai)->GetName(&name);

            AutoPtr<IComponentName> componentName;
            CComponentName::New(packageNameTmp, name, (IComponentName**)&componentName);
            AutoPtr<ProviderId> providerId = new ProviderId(uidTmp, componentName);

            AutoPtr<Provider> provider = LookupProviderLocked(providerId);
            if (provider == NULL) {
                if (AddProviderLocked(ri)) {
                    keep->Add(TO_IINTERFACE(providerId));
                    providersUpdated = TRUE;
                }
            }
            else {
                AutoPtr<Provider> parsed = ParseProviderInfoXml(providerId, ri);
                if (parsed != NULL) {
                    keep->Add(TO_IINTERFACE(providerId));
                    // Use the new AppWidgetProviderInfo.
                    provider->mInfo = NULL;
                    provider->mInfo = parsed->mInfo;
                    // If it's enabled
                    Int32 M = 0;
                    provider->mWidgets->GetSize(&M);
                    if (M > 0) {
                        AutoPtr< ArrayOf<Int32> > appWidgetIds = GetWidgetIds(provider->mWidgets);
                        CancelBroadcasts(provider);
                        RegisterForBroadcastsLocked(provider, appWidgetIds);
                        for (Int32 j = 0; j < M; ++j) {
                            AutoPtr<IInterface> interfaceTmp;
                            provider->mWidgets->Get(i, (IInterface**)&interfaceTmp);
                            IObject* objTmp = IObject::Probe(interfaceTmp);
                            Widget* widget = (Widget*)objTmp;
                            widget->mViews = NULL;
                            ScheduleNotifyProviderChangedLocked(widget);
                        }
                        // Now that we've told the host, push out an update.
                        SendUpdateIntentLocked(provider, appWidgetIds);
                        providersUpdated = TRUE;
                    }
                }
            }
        }
    }

    // prune the ones we don't want to keep
    mProviders->GetSize(&N);
    for (Int32 i = N - 1; i >= 0; --i) {
        AutoPtr<IInterface> interfaceTmp;
        mProviders->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Provider* provider = (Provider*)objTmp;

        AutoPtr<IComponentName> componentNameTmp;
        provider->mInfo->GetProvider((IComponentName**)&componentNameTmp);
        String packageNameTmp;
        componentNameTmp->GetPackageName(&packageNameTmp);

        Int32 userIdTmp = 0;
        provider->GetUserId(&userIdTmp);
        Boolean contains = FALSE;
        ISet::Probe(keep)->Contains(TO_IINTERFACE(provider->mId), &contains);
        if (packageName.Equals(packageNameTmp) && userIdTmp == userId && !contains) {
            if (removedProviders != NULL) {
                removedProviders->Add(TO_IINTERFACE(provider->mId));
            }
            DeleteProviderLocked(provider);
            providersUpdated = TRUE;
        }
    }

    return providersUpdated;
}

Boolean AppWidgetServiceImpl::RemoveHostsAndProvidersForPackageLocked(
    /* [in] */ const String& pkgName,
    /* [in] */ Int32 userId)
{
    Boolean removed = FALSE;
    Int32 N = 0;
    mProviders->GetSize(&N);
    for (Int32 i = N - 1; i >= 0; --i) {
        AutoPtr<IInterface> interfaceTmp;
        mProviders->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Provider* provider = (Provider*)objTmp;

        AutoPtr<IComponentName> componentNameTmp;
        provider->mInfo->GetProvider((IComponentName**)&componentNameTmp);
        String packageNameTmp;
        componentNameTmp->GetPackageName(&packageNameTmp);

        Int32 userIdTmp = 0;
        provider->GetUserId(&userIdTmp);
        if (pkgName.Equals(packageNameTmp) && userIdTmp == userId) {
            DeleteProviderLocked(provider);
            removed = TRUE;
        }
    }

    // Delete the hosts for this package too
    // By now, we have removed any AppWidgets that were in any hosts here,
    // so we don't need to worry about sending DISABLE broadcasts to them.
    mHosts->GetSize(&N);
    for (Int32 i = N - 1; i >= 0; --i) {
        AutoPtr<IInterface> interfaceTmp;
        mHosts->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        Host* host = (Host*)objTmp;

        Int32 userIdTmp = 0;
        host->GetUserId(&userIdTmp);
        if (pkgName.Equals(host->mId->mPackageName) && userIdTmp == userId) {
            DeleteHostLocked(host);
            removed = TRUE;
        }
    }
    return removed;
}

String AppWidgetServiceImpl::GetCanonicalPackageName(
    /* [in] */ const String& packageName,
    /* [in] */ const String& className,
    /* [in] */ Int32 userId)
{
    Int64 identity = Binder::ClearCallingIdentity();
    //try {
        //try {
            AutoPtr<IIPackageManager> packageManager = AppGlobals::GetPackageManager();
            AutoPtr<IComponentName> conponentName;
            CComponentName::New(packageName, className, (IComponentName**)&conponentName);
            AutoPtr<IActivityInfo> activityInfo;
            packageManager->GetReceiverInfo(conponentName, 0, userId, (IActivityInfo**)&activityInfo);
            return packageName;
        //} catch (RemoteException re) {
        //    String[] packageNames = mContext.getPackageManager()
        //            .currentToCanonicalPackageNames(new String[]{packageName});
        //    if (packageNames != null && packageNames.length > 0) {
        //        return packageNames[0];
        //    }
        //}
    //} finally {
        Binder::RestoreCallingIdentity(identity);
    //}
    return String(NULL);
}

void AppWidgetServiceImpl::SendBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* userHandle)
{
    Int64 identity = Binder::ClearCallingIdentity();
    //try {
        mContext->SendBroadcastAsUser(intent, userHandle);
    //} finally {
        Binder::RestoreCallingIdentity(identity);
    //}
}

void AppWidgetServiceImpl::BindService(
    /* [in] */ IIntent* intent,
    /* [in] */ IServiceConnection* connection,
    /* [in] */ IUserHandle* userHandle)
{
    Int64 token = Binder::ClearCallingIdentity();
    //try {
        Boolean resTmp;
        mContext->BindServiceAsUser(intent, connection, IContext::BIND_AUTO_CREATE, userHandle, &resTmp);
    //} finally {
        Binder::RestoreCallingIdentity(token);
    //}
}

void AppWidgetServiceImpl::UnbindService(
    /* [in] */ IServiceConnection* connection)
{
    Int64 token = Binder::ClearCallingIdentity();
    //try {
        mContext->UnbindService(connection);
    //} finally {
        Binder::RestoreCallingIdentity(token);
    //}
}

Int32 AppWidgetServiceImpl::InitMinUpdatePeriod()
{
    return DEBUG ? 0 : 30 * 60 * 1000;
}

} // namespace AppWidget
} // namespace Server
} // namespace Droid
} // namespace Elastos
