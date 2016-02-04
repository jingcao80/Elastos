
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/internal/content/PackageMonitor.h"
#include "elastos/droid/internal/os/BackgroundThread.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/os/CHandler.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Internal::Os::BackgroundThread;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::CHandler;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Content {

static AutoPtr<IIntentFilter> InitPackageFilt()
{
    AutoPtr<IIntentFilter> filter;
    ASSERT_SUCCEEDED(CIntentFilter::New((IIntentFilter**)&filter))
    ASSERT_SUCCEEDED(filter->AddAction(IIntent::ACTION_PACKAGE_ADDED))
    ASSERT_SUCCEEDED(filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED))
    ASSERT_SUCCEEDED(filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED))
    ASSERT_SUCCEEDED(filter->AddAction(IIntent::ACTION_QUERY_PACKAGE_RESTART))
    ASSERT_SUCCEEDED(filter->AddAction(IIntent::ACTION_PACKAGE_RESTARTED))
    ASSERT_SUCCEEDED(filter->AddAction(IIntent::ACTION_UID_REMOVED))
    ASSERT_SUCCEEDED(filter->AddDataScheme(String("package")))
    return filter;
}

static AutoPtr<IIntentFilter> InitNonDataFilt()
{
    AutoPtr<IIntentFilter> filter;
    ASSERT_SUCCEEDED(CIntentFilter::New((IIntentFilter**)&filter))
    ASSERT_SUCCEEDED(filter->AddAction(IIntent::ACTION_UID_REMOVED))
    ASSERT_SUCCEEDED(filter->AddAction(IIntent::ACTION_USER_STOPPED))
    return filter;
}

static AutoPtr<IIntentFilter> InitExternalFilt()
{
    AutoPtr<IIntentFilter> filter;
    ASSERT_SUCCEEDED(CIntentFilter::New((IIntentFilter**)&filter))
    ASSERT_SUCCEEDED(filter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE))
    ASSERT_SUCCEEDED(filter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE))
    return filter;
}

AutoPtr<IIntentFilter> PackageMonitor::sPackageFilt = InitPackageFilt();
AutoPtr<IIntentFilter> PackageMonitor::sNonDataFilt = InitNonDataFilt();
AutoPtr<IIntentFilter> PackageMonitor::sExternalFilt = InitExternalFilt();

PackageMonitor::PackageMonitor()
    : mChangeType(0)
    , mChangeUserId(IUserHandle::USER_NULL)
    , mSomePackagesChanged(FALSE)
{
    mTempArray = ArrayOf<String>::Alloc(1);
}

PackageMonitor::~PackageMonitor()
{
}

CAR_INTERFACE_IMPL(PackageMonitor, BroadcastReceiver, IPackageMonitor)

ECode PackageMonitor::Register(
    /* [in] */ IContext* context,
    /* [in] */ ILooper* thread,
    /* [in] */ Boolean externalStorage)
{
    return Register(context, thread, NULL, externalStorage);
}

ECode PackageMonitor::Register(
    /* [in] */ IContext* context,
    /* [in] */ ILooper* thread,
    /* [in] */ IUserHandle* user,
    /* [in] */ Boolean externalStorage)
{
    if (NULL != mRegisteredContext) {
        // throw new IllegalStateException("Already registered");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mRegisteredContext = context;
    mRegisteredHandler = NULL;
    if (NULL == thread) {
        mRegisteredHandler = BackgroundThread::GetHandler();
    }
    else {
        CHandler::New(thread, (IHandler**)&mRegisteredHandler);
    }

    AutoPtr<IIntent> intent;
    if (NULL != user) {
        FAIL_RETURN(context->RegisterReceiverAsUser(THIS_PROBE(IBroadcastReceiver),
                user, sPackageFilt, String(NULL), mRegisteredHandler, (IIntent**)&intent))
        intent = NULL;
        FAIL_RETURN(context->RegisterReceiverAsUser(THIS_PROBE(IBroadcastReceiver),
                user, sNonDataFilt, String(NULL), mRegisteredHandler, (IIntent**)&intent))
        if (externalStorage) {
            intent = NULL;
            FAIL_RETURN(context->RegisterReceiverAsUser(THIS_PROBE(IBroadcastReceiver),
                    user, sExternalFilt, String(NULL), mRegisteredHandler, (IIntent**)&intent))
        }
    }
    else {
        FAIL_RETURN(context->RegisterReceiver(THIS_PROBE(IBroadcastReceiver),
                sPackageFilt, String(NULL), mRegisteredHandler, (IIntent**)&intent))
        intent = NULL;
        FAIL_RETURN(context->RegisterReceiver(THIS_PROBE(IBroadcastReceiver),
                sNonDataFilt, String(NULL), mRegisteredHandler, (IIntent**)&intent))
        if (externalStorage) {
            intent = NULL;
            FAIL_RETURN(context->RegisterReceiver(THIS_PROBE(IBroadcastReceiver),
                    sExternalFilt, String(NULL), mRegisteredHandler, (IIntent**)&intent))
        }
    }
    return NOERROR;
}

ECode PackageMonitor::GetRegisteredHandler(
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    *handler = mRegisteredHandler;
    REFCOUNT_ADD(*handler);
    return NOERROR;
}

ECode PackageMonitor::Unregister()
{
    if (NULL == mRegisteredContext) {
        // throw new IllegalStateException("Not registered");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    FAIL_RETURN(mRegisteredContext->UnregisterReceiver(THIS_PROBE(IBroadcastReceiver)))
    mRegisteredContext = NULL;
    return NOERROR;
}

Boolean PackageMonitor::IsPackageUpdating(
    /* [in] */ const String& packageName)
{
    AutoLock lock(mUpdatingPackagesLock);
    HashSet<String>::Iterator it = mUpdatingPackages.Find(packageName);
    if (it != mUpdatingPackages.End()) {
        return TRUE;
    }
    return FALSE;
}

ECode PackageMonitor::OnBeginPackageChanges()
{
    return NOERROR;
}

ECode PackageMonitor::OnPackageAdded(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    return NOERROR;
}

ECode PackageMonitor::OnPackageRemoved(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    return NOERROR;
}

ECode PackageMonitor::OnPackageRemovedAllUsers(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    return NOERROR;
}

ECode PackageMonitor::OnPackageUpdateStarted(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    return NOERROR;
}

ECode PackageMonitor::OnPackageUpdateFinished(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    return NOERROR;
}

ECode PackageMonitor::OnPackageChanged(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ ArrayOf<String>* components,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (components != NULL) {
        for (Int32 i = 0; i < components->GetLength(); i++) {
            String name = (*components)[i];
            if (packageName.Equals(name)) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode PackageMonitor::OnHandleForceStop(
    /* [in] */ IIntent* intent,
    /* [in] */ ArrayOf<String>* packages,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean doit,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode PackageMonitor::OnHandleUserStop(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 userHandle)
{
    return NOERROR;
}

ECode PackageMonitor::OnUidRemoved(
    /* [in] */ Int32 uid)
{
    return NOERROR;
}

ECode PackageMonitor::OnPackagesAvailable(
    /* [in] */ ArrayOf<String>* packages)
{
    return NOERROR;
}

ECode PackageMonitor::OnPackagesUnavailable(
    /* [in] */ ArrayOf<String>* packages)
{
    return NOERROR;
}

ECode PackageMonitor::OnPackageDisappeared(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 reason)
{
    return NOERROR;
}

ECode PackageMonitor::OnPackageAppeared(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 reason)
{
    return NOERROR;
}

ECode PackageMonitor::OnPackageModified(
    /* [in] */ const String& packageName)
{
    return NOERROR;
}

ECode PackageMonitor::DidSomePackagesChange(
    /* [out] */ Boolean* isChanged)
{
    VALIDATE_NOT_NULL(isChanged)
    *isChanged = mSomePackagesChanged;
    return NOERROR;
}

ECode PackageMonitor::IsPackageAppearing(
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (NULL != mAppearingPackages) {
        for (Int32 i = mAppearingPackages->GetLength() - 1; i >= 0; i--) {
            if (packageName.Equals((*mAppearingPackages)[i])) {
                *result = mChangeType;
                return NOERROR;
            }
        }
    }
    *result = IPackageMonitor::PACKAGE_UNCHANGED;
    return NOERROR;
}

ECode PackageMonitor::AnyPackagesAppearing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAppearingPackages != NULL;
    return NOERROR;
}

ECode PackageMonitor::IsPackageDisappearing(
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (NULL != mDisappearingPackages) {
        for (Int32 i = mDisappearingPackages->GetLength() - 1; i >= 0; i--) {
            if (packageName.Equals((*mDisappearingPackages)[i])) {
                *result = mChangeType;
                return NOERROR;
            }
        }
    }
    *result = IPackageMonitor::PACKAGE_UNCHANGED;
    return NOERROR;
}

ECode PackageMonitor::AnyPackagesDisappearing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDisappearingPackages != NULL;
    return NOERROR;
}

ECode PackageMonitor::IsReplacing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mChangeType == PACKAGE_UPDATING;
    return NOERROR;
}

ECode PackageMonitor::IsPackageModified(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* isModified)
{
    VALIDATE_NOT_NULL(isModified)
    if (NULL != mModifiedPackages) {
        for (Int32 i = mModifiedPackages->GetLength() - 1; i >= 0; i--) {
            if (packageName.Equals((*mModifiedPackages)[i])) {
                *isModified = TRUE;
                return NOERROR;
            }
        }
    }
    *isModified = FALSE;
    return NOERROR;
}

ECode PackageMonitor::OnSomePackagesChanged()
{
    return NOERROR;
}

ECode PackageMonitor::OnFinishPackageChanges()
{
    return NOERROR;
}

ECode PackageMonitor::GetChangingUserId(
    /* [out] */ Int32* userId)
{
    VALIDATE_NOT_NULL(userId)
    *userId = mChangeUserId;
    return NOERROR;
}

ECode PackageMonitor::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    FAIL_RETURN(intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_NULL, &mChangeUserId))
    if (mChangeUserId == IUserHandle::USER_NULL) {
        // throw new IllegalArgumentException("Intent broadcast does not contain user handle: " + intent);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(OnBeginPackageChanges())
    mDisappearingPackages = NULL;
    mAppearingPackages = NULL;
    mSomePackagesChanged = FALSE;
    String action;
    FAIL_RETURN(intent->GetAction(&action))
    if (IIntent::ACTION_PACKAGE_ADDED.Equals(action)) {
        String pkg = GetPackageName(intent);
        Int32 uid = 0;
        FAIL_RETURN(intent->GetInt32Extra(IIntent::EXTRA_UID, 0, &uid))
        // We consider something to have changed regardless of whether
        // this is just an update, because the update is now finished
        // and the contents of the package may have changed.
        mSomePackagesChanged = TRUE;
        if (!pkg.IsNull()) {
            mAppearingPackages = mTempArray;
            (*mTempArray)[0] = pkg;
            Boolean ret = FALSE;
            FAIL_RETURN(intent->GetBooleanExtra(IIntent::EXTRA_REPLACING, FALSE, &ret))
            if (ret) {
                mModifiedPackages = mTempArray;
                mChangeType = IPackageMonitor::PACKAGE_UPDATING;
                FAIL_RETURN(OnPackageUpdateFinished(pkg, uid))
                FAIL_RETURN(OnPackageModified(pkg))
            }
            else {
                mChangeType = IPackageMonitor::PACKAGE_PERMANENT_CHANGE;
                FAIL_RETURN(OnPackageAdded(pkg, uid))
            }
            FAIL_RETURN(OnPackageAppeared(pkg, mChangeType))
            if (mChangeType == IPackageMonitor::PACKAGE_UPDATING) {
                {
                    AutoLock lock(mUpdatingPackagesLock);
                    mUpdatingPackages.Erase(pkg);
                }
            }
        }
    }
    else if (IIntent::ACTION_PACKAGE_REMOVED.Equals(action)) {
        String pkg = GetPackageName(intent);
        Int32 uid = 0;
        FAIL_RETURN(intent->GetInt32Extra(IIntent::EXTRA_UID, 0, &uid))
        if (!pkg.IsNull()) {
            mDisappearingPackages = mTempArray;
            (*mTempArray)[0] = pkg;
            Boolean ret = FALSE;
            FAIL_RETURN(intent->GetBooleanExtra(IIntent::EXTRA_REPLACING, FALSE, &ret))
            if (ret) {
                mChangeType = IPackageMonitor::PACKAGE_UPDATING;
                {
                    AutoLock lock(mUpdatingPackagesLock);
                    //not used for now
                    //mUpdatingPackages.add(pkg);
                }
                FAIL_RETURN(OnPackageUpdateStarted(pkg, uid))
            }
            else {
                mChangeType = IPackageMonitor::PACKAGE_PERMANENT_CHANGE;
                // We only consider something to have changed if this is
                // not a replace; for a replace, we just need to consider
                // it when it is re-added.
                mSomePackagesChanged = TRUE;
                FAIL_RETURN(OnPackageRemoved(pkg, uid))
                FAIL_RETURN(intent->GetBooleanExtra(IIntent::EXTRA_REMOVED_FOR_ALL_USERS, FALSE, &ret))
                if (ret) {
                    FAIL_RETURN(OnPackageRemovedAllUsers(pkg, uid))
                }
            }
            FAIL_RETURN(OnPackageDisappeared(pkg, mChangeType))
        }
    }
    else if (IIntent::ACTION_PACKAGE_CHANGED.Equals(action)) {
        String pkg = GetPackageName(intent);
        Int32 uid = 0;
        FAIL_RETURN(intent->GetInt32Extra(IIntent::EXTRA_UID, 0, &uid))
        AutoPtr<ArrayOf<String> > components;
        FAIL_RETURN(intent->GetStringArrayExtra(IIntent::EXTRA_CHANGED_COMPONENT_NAME_LIST,
                (ArrayOf<String>**)&components))
        if (!pkg.IsNull()) {
            mModifiedPackages = mTempArray;
            (*mTempArray)[0] = pkg;
            mChangeType = PACKAGE_PERMANENT_CHANGE;
            Boolean res;
            OnPackageChanged(pkg, uid, components, &res);
            if (res) {
                mSomePackagesChanged = TRUE;
            }
            FAIL_RETURN(OnPackageModified(pkg))
        }
    }
    else if (IIntent::ACTION_QUERY_PACKAGE_RESTART.Equals(action)) {
        mDisappearingPackages = NULL;
        FAIL_RETURN(intent->GetStringArrayExtra(IIntent::EXTRA_PACKAGES, (ArrayOf<String>**)&mDisappearingPackages))
        mChangeType = IPackageMonitor::PACKAGE_TEMPORARY_CHANGE;
        Boolean canRestart = FALSE;
        Int32 uid = 0;
        FAIL_RETURN(intent->GetInt32Extra(IIntent::EXTRA_UID, 0, &uid))
        FAIL_RETURN(OnHandleForceStop(intent, mDisappearingPackages, uid, FALSE, &canRestart))
        if (canRestart) {
            FAIL_RETURN(BroadcastReceiver::SetResultCode(IActivity::RESULT_OK))
        }
    }
    else if (IIntent::ACTION_PACKAGE_RESTARTED.Equals(action)) {
        mDisappearingPackages = ArrayOf<String>::Alloc(1);
        String pkg = GetPackageName(intent);
        (*mDisappearingPackages)[0] = pkg;
        mChangeType = IPackageMonitor::PACKAGE_TEMPORARY_CHANGE;
        Int32 uid = 0;
        FAIL_RETURN(intent->GetInt32Extra(IIntent::EXTRA_UID, 0, &uid))
        Boolean ret = FALSE;
        FAIL_RETURN(OnHandleForceStop(intent, mDisappearingPackages, uid, TRUE, &ret))
    }
    else if (IIntent::ACTION_UID_REMOVED.Equals(action)) {
        Int32 uid = 0;
        FAIL_RETURN(intent->GetInt32Extra(IIntent::EXTRA_UID, 0, &uid))
        FAIL_RETURN(OnUidRemoved(uid))
    }
    else if (IIntent::ACTION_USER_STOPPED.Equals(action)) {
        Boolean ret = FALSE;
        FAIL_RETURN(intent->HasExtra(IIntent::EXTRA_USER_HANDLE, &ret))
        if (ret) {
            Int32 uid = 0;
            FAIL_RETURN(intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, 0, &uid))
            FAIL_RETURN(OnHandleUserStop(intent, uid))
        }
    } else if (IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE.Equals(action)) {
        AutoPtr<ArrayOf<String> > pkgList;
        FAIL_RETURN(intent->GetStringArrayExtra(IIntent::EXTRA_CHANGED_PACKAGE_LIST, (ArrayOf<String>**)&pkgList))
        mAppearingPackages = pkgList;
        Boolean res;
        intent->GetBooleanExtra(IIntent::EXTRA_REPLACING, FALSE, &res);
        mChangeType = res ? PACKAGE_UPDATING : PACKAGE_TEMPORARY_CHANGE;
        mSomePackagesChanged = TRUE;
        if (NULL != pkgList) {
            FAIL_RETURN(OnPackagesAvailable(pkgList))
            for (Int32 i = 0; i < pkgList->GetLength(); i++) {
                FAIL_RETURN(OnPackageAppeared((*pkgList)[i], mChangeType))
            }
        }
    }
    else if (IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE.Equals(action)) {
        AutoPtr<ArrayOf<String> > pkgList;
        FAIL_RETURN(intent->GetStringArrayExtra(IIntent::EXTRA_CHANGED_PACKAGE_LIST, (ArrayOf<String>**)&pkgList))
        mDisappearingPackages = pkgList;
        Boolean res;
        intent->GetBooleanExtra(IIntent::EXTRA_REPLACING, FALSE, &res);
        mChangeType = res ? PACKAGE_UPDATING : PACKAGE_TEMPORARY_CHANGE;
        mSomePackagesChanged = TRUE;
        if (NULL != pkgList) {
            FAIL_RETURN(OnPackagesUnavailable(pkgList))
            for (Int32 i = 0; i < pkgList->GetLength(); i++) {
                FAIL_RETURN(OnPackageDisappeared((*pkgList)[i], mChangeType))
            }
        }
    }

    if (mSomePackagesChanged) {
        FAIL_RETURN(OnSomePackagesChanged())
    }

    FAIL_RETURN(OnFinishPackageChanges())
    mChangeUserId = IUserHandle::USER_NULL;
    return NOERROR;
}

String PackageMonitor::GetPackageName(
    /* [in] */ IIntent* intent)
{
    String pkgName;
    AutoPtr<IUri> uri;
    intent->GetData((IUri**)&uri);
    if (NULL != uri) {
        uri->GetSchemeSpecificPart(&pkgName);
    }
    return pkgName;
}

ECode PackageMonitor::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("PackageMonitor:");
    (*info).AppendFormat("%p", this);
    return NOERROR;
}

}
}
}
}

