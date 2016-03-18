
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include <elastos/droid/DroidRuntime.h>
#include "elastos/droid/app/ApplicationPackageManager.h"
#include "elastos/droid/app/CContextImpl.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/CUserManager.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/content/pm/PackageItemInfo.h"
#include "elastos/droid/content/pm/CPackageInstaller.h"
#include "elastos/droid/content/pm/CVerificationParams.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/drawable/CBitmapDrawable.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/internal/utility/UserIcons.h"
#include "elastos/droid/R.h"
#include "elastos/core/CoreUtils.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::DroidRuntime;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::ECLSID_CApplicationInfo;
using Elastos::Droid::Content::Pm::ECLSID_CPackageInfo;
using Elastos::Droid::Content::Pm::EIID_IPackageManager;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Content::Pm::CVerificationParams;
using Elastos::Droid::Content::Pm::CPackageInstaller;
using Elastos::Droid::Content::Pm::IIPackageInstaller;
using Elastos::Droid::Content::Pm::PackageItemInfo;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Droid::Internal::Utility::UserIcons;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableConstantState;

using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::IList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

const String ApplicationPackageManager::TAG("ApplicationPackageManager");
const Boolean ApplicationPackageManager::DEBUG = FALSE;
const Boolean ApplicationPackageManager::DEBUG_ICONS = FALSE;
Int32 ApplicationPackageManager::sDefaultFlags = IPackageManager::GET_SHARED_LIBRARY_FILES;

Object ApplicationPackageManager::sSync;
HashMap<AutoPtr<ApplicationPackageManager::ResourceName>, AutoPtr<IWeakReference> > ApplicationPackageManager::sIconCache;
HashMap<AutoPtr<ApplicationPackageManager::ResourceName>, AutoPtr<IWeakReference> > ApplicationPackageManager::sStringCache;

ApplicationPackageManager::ApplicationPackageManager(
    /* [in] */ IContextImpl* context,
    /* [in] */ IIPackageManager* pm)
    : mContext(IContext::Probe(context))
    , mPM(pm)
    , mCachedSafeMode(-1)
{}

AutoPtr<IUserManager> ApplicationPackageManager::GetUserManager()
{
    synchronized(mLock) {
        if (mUserManager == NULL) {
            mUserManager = CUserManager::Get(mContext);
        }
    }
    return mUserManager;
}

ECode ApplicationPackageManager::GetPackageInfo(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 flags,
    /* [out] */ IPackageInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

//    try {
    Int32 id;
    mContext->GetUserId(&id);
    AutoPtr<IPackageInfo> pi;
    FAIL_RETURN(mPM->GetPackageInfo(packageName, flags, id, (IPackageInfo**)&pi));
    if (pi != NULL) {
        *info = pi;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }
//    } catch (RemoteException e) {
//        throw new RuntimeException("Package manager has died", e);
//    }

//    throw new NameNotFoundException(packageName);
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::CurrentToCanonicalPackageNames(
    /* [in] */ const ArrayOf<String>& cuNames,
    /* [out, callee] */ ArrayOf<String>** caNames)
{
    VALIDATE_NOT_NULL(caNames);
//    try {
    return mPM->CurrentToCanonicalPackageNames(cuNames, caNames);
//    } catch (RemoteException e) {
//        throw new RuntimeException("Package manager has died", e);
//    }
}

ECode ApplicationPackageManager::CanonicalToCurrentPackageNames(
    /* [in] */ const ArrayOf<String>& caNames,
    /* [out, callee] */ ArrayOf<String>** cuNames)
{
//    try {
    return mPM->CanonicalToCurrentPackageNames(caNames, cuNames);
//    } catch (RemoteException e) {
//        throw new RuntimeException("Package manager has died", e);
//    }
}

ECode ApplicationPackageManager::GetLaunchIntentForPackage(
    /* [in] */ const String& packageName,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    // First see if the package has an INFO activity; the existence of
    // such an activity is implied to be the desired front-door for the
    // overall package (such as if it has multiple launcher entries).
    AutoPtr<IIntent> intentToResolve;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intentToResolve);
    intentToResolve->AddCategory(IIntent::CATEGORY_INFO);
    intentToResolve->SetPackage(packageName);
    AutoPtr<IList> ris;
    QueryIntentActivities(intentToResolve, 0, (IList**)&ris);

    // Otherwise, try to find a main launcher activity.
    Int32 size;
    if (ris == NULL ||(ris->GetSize(&size), size <= 0)) {
        // reuse the intent instance
        intentToResolve->RemoveCategory(IIntent::CATEGORY_INFO);
        intentToResolve->AddCategory(IIntent::CATEGORY_LAUNCHER);
        intentToResolve->SetPackage(packageName);
        ris = NULL;
        QueryIntentActivities(intentToResolve, 0, (IList**)&ris);
    }
    if (ris == NULL ||(ris->GetSize(&size), size <= 0)) {
        *intent = NULL;
        return NOERROR;
    }
    AutoPtr<IIntent> newIntent;
    CIntent::New(intentToResolve, (IIntent**)&newIntent);
    newIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

    AutoPtr<IInterface> obj;
    ris->Get(0, (IInterface**)&obj);
    AutoPtr<IResolveInfo> resInfo = IResolveInfo::Probe(obj);
    AutoPtr<IActivityInfo> aInfo;
    resInfo->GetActivityInfo((IActivityInfo**)&aInfo);
    String pkgName, name;
    IPackageItemInfo::Probe(aInfo)->GetPackageName(&pkgName);
    IPackageItemInfo::Probe(aInfo)->GetName(&name);
    newIntent->SetClassName(pkgName, name);
    *intent = newIntent;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode ApplicationPackageManager::GetLeanbackLaunchIntentForPackage(
    /* [in] */ const String& packageName,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = NULL;

    // Try to find a main leanback_launcher activity.
    AutoPtr<IIntent> intentToResolve;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intentToResolve);
    intentToResolve->AddCategory(IIntent::CATEGORY_LEANBACK_LAUNCHER);
    intentToResolve->SetPackage(packageName);

    AutoPtr<IList> ris;
    QueryIntentActivities(intentToResolve, 0, (IList**)&ris);

    Int32 size;
    if (ris == NULL ||(ris->GetSize(&size), size <= 0)) {
        *intent = NULL;
        return NOERROR;
    }

    AutoPtr<IInterface> obj;
    ris->Get(0, (IInterface**)&obj);
    AutoPtr<IResolveInfo> resInfo = IResolveInfo::Probe(obj);
    AutoPtr<IActivityInfo> aInfo;
    resInfo->GetActivityInfo((IActivityInfo**)&aInfo);
    String pkgName, name;
    IPackageItemInfo::Probe(aInfo)->GetPackageName(&pkgName);
    IPackageItemInfo::Probe(aInfo)->GetName(&name);

    AutoPtr<IIntent> newIntent;
    CIntent::New(intentToResolve, (IIntent**)&newIntent);
    newIntent->SetClassName(pkgName, name);
    *intent = newIntent;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode ApplicationPackageManager::GetPackageGids(
    /* [in] */ const String& packageName,
    /* [out, callee] */ ArrayOf<Int32>** pgids)
{
    VALIDATE_NOT_NULL(pgids);
    *pgids = NULL;

//    try {
    AutoPtr< ArrayOf<Int32> > gids;
    FAIL_RETURN(mPM->GetPackageGids(packageName, (ArrayOf<Int32>**)&gids));
    if (gids == NULL || gids->GetLength() > 0) {
        *pgids = gids;
        REFCOUNT_ADD(*pgids);
        return NOERROR;
    }
//    } catch (RemoteException e) {
//        throw new RuntimeException("Package manager has died", e);
//    }

//    throw new NameNotFoundException(packageName);
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::GetPermissionInfo(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags,
    /* [out] */ IPermissionInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

//     try {
    AutoPtr<IPermissionInfo> pi;
    FAIL_RETURN(mPM->GetPermissionInfo(name, flags, (IPermissionInfo**)&pi));
    if (pi != NULL) {
        *info = pi;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }

//     throw new NameNotFoundException(name);
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::GetPackageUid(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid)
    *uid = -1;
    // try {
    Int32 id;
    ECode ec = mPM->GetPackageUid(packageName, userHandle, &id);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        return E_RUNTIME_EXCEPTION;
    }

    if (id >= 0) {
        *uid = id;
        return NOERROR;
    }

    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::QueryPermissionsByGroup(
    /* [in] */ const String& group,
    /* [in] */ Int32 flags,
    /* [out] */ IList** permissions)
{
    VALIDATE_NOT_NULL(permissions);
    *permissions = NULL;

//     try {
    AutoPtr<IList> pi;
    FAIL_RETURN(mPM->QueryPermissionsByGroup(group, flags, (IList**)&pi));
    if (pi != NULL) {
        *permissions = pi;
        REFCOUNT_ADD(*permissions);
        return NOERROR;
    }
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }

//     throw new NameNotFoundException(group);
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::GetPermissionGroupInfo(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags,
    /* [out] */ IPermissionGroupInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

//     try {
    AutoPtr<IPermissionGroupInfo> pgi;
    FAIL_RETURN(mPM->GetPermissionGroupInfo(name, flags, (IPermissionGroupInfo**)&pgi));
    if (pgi != NULL) {
        *info = pgi;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }

//     throw new NameNotFoundException(name);
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::GetAllPermissionGroups(
    /* [in] */ Int32 flags,
    /* [out] */ IList** groups)
{
    VALIDATE_NOT_NULL(groups);
//     try {
    return mPM->GetAllPermissionGroups(flags, groups);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::GetApplicationInfo(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 flags,
    /* [out] */ IApplicationInfo** info)
{
    VALIDATE_NOT_NULL(info)
    AutoPtr<IApplicationInfo> ai;
    #if 0
    Int32 id;
    mContext->GetUserId(&id);
    FAIL_RETURN(mPM->GetApplicationInfo(packageName, flags, id, (IApplicationInfo**)&ai));
    #else
        FAIL_RETURN(mPM->GetApplicationInfo(packageName, flags, 0, (IApplicationInfo**)&ai));
    #endif
    if (ai != NULL) {
        // This is a temporary hack. Callers must use
        // createPackageContext(packageName).getApplicationInfo() to
        // get the right paths.
        MaybeAdjustApplicationInfo(ai);

        *info = ai;
        REFCOUNT_ADD(*info)
        return NOERROR;
    }
    return E_NAME_NOT_FOUND_EXCEPTION;
}

void ApplicationPackageManager::MaybeAdjustApplicationInfo(
    /* [in] */ IApplicationInfo* info)
{
    // If we're dealing with a multi-arch application that has both
    // 32 and 64 bit shared libraries, we might need to choose the secondary
    // depending on what the current runtime's instruction set is.
    String pca, sca;
    info->GetPrimaryCpuAbi(&pca);
    info->GetSecondaryCpuAbi(&sca);
    if (!pca.IsNull() && !sca.IsNull()) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        String runtimeIsa = DroidRuntime::GetRuntime()->GetInstructionSetString();
        String secondaryIsa;
        system->GetInstructionSet(sca, &secondaryIsa);

        // If the runtimeIsa is the same as the primary isa, then we do nothing.
        // Everything will be set up correctly because info.nativeLibraryDir will
        // correspond to the right ISA.
        if (runtimeIsa.Equals(secondaryIsa)) {
            String dir;
            info->GetSecondaryNativeLibraryDir(&dir);
            info->SetNativeLibraryDir(dir);
        }
    }
}

ECode ApplicationPackageManager::GetActivityInfo(
    /* [in] */ IComponentName* component,
    /* [in] */ Int32 flags,
    /* [out] */ IActivityInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

//     try {
    Int32 id;
    mContext->GetUserId(&id);
    AutoPtr<IActivityInfo> ai;
    FAIL_RETURN(mPM->GetActivityInfo(component, flags, id, (IActivityInfo**)&ai));
    if (ai != NULL) {
        *info = ai;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }

//     throw new NameNotFoundException(className.toString());
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::GetReceiverInfo(
    /* [in] */ IComponentName* component,
    /* [in] */ Int32 flags,
    /* [out] */ IActivityInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

//     try {
    Int32 id;
    mContext->GetUserId(&id);
    AutoPtr<IActivityInfo> ai;
    FAIL_RETURN(mPM->GetReceiverInfo(component, flags, id, (IActivityInfo**)&ai));
    if (ai != NULL) {
        *info = ai;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }

//     throw new NameNotFoundException(className.toString());
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::GetServiceInfo(
    /* [in] */ IComponentName* component,
    /* [in] */ Int32 flags,
    /* [out] */ IServiceInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

//     try {
    Int32 id;
    mContext->GetUserId(&id);
    AutoPtr<IServiceInfo> si;
    FAIL_RETURN(mPM->GetServiceInfo(component, flags, id, (IServiceInfo**)&si));
    if (si != NULL) {
        *info = si;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }

//     throw new NameNotFoundException(className.toString());
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::GetProviderInfo(
    /* [in] */ IComponentName* component,
    /* [in] */ Int32 flags,
    /* [out] */ IProviderInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

//     try {
    Int32 id;
    mContext->GetUserId(&id);
    AutoPtr<IProviderInfo> pi;
    FAIL_RETURN(mPM->GetProviderInfo(component, flags, id, (IProviderInfo**)&pi));
    if (pi != NULL) {
        *info = pi;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }

//     throw new NameNotFoundException(className.toString());
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::GetSystemSharedLibraryNames(
    /* [out, callee] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names);
//     try {
    return mPM->GetSystemSharedLibraryNames(names);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::GetSystemAvailableFeatures(
    /* [out, callee] */ ArrayOf<IFeatureInfo*>** features)
{
    VALIDATE_NOT_NULL(features);
//     try {
    return mPM->GetSystemAvailableFeatures(features);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::HasSystemFeature(
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mPM->HasSystemFeature(name, result);
}

ECode ApplicationPackageManager::CheckPermission(
    /* [in] */ const String& permName,
    /* [in] */ const String& pkgName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
//     try {
    return mPM->CheckPermission(permName, pkgName, result);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::AddPermission(
    /* [in] */ IPermissionInfo* info,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
//     try {
    return mPM->AddPermission(info, result);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::AddPermissionAsync(
    /* [in] */ IPermissionInfo* info,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
//     try {
    return mPM->AddPermissionAsync(info, result);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::RemovePermission(
    /* [in] */ const String& name)
{
//     try {
    return mPM->RemovePermission(name);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::GrantPermission(
    /* [in] */ const String& packageName,
    /* [in] */ const String& permissionName)
{
//     try {
    return mPM->GrantPermission(packageName, permissionName);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::RevokePermission(
    /* [in] */ const String& packageName,
    /* [in] */ const String& permissionName)
{
//     try {
    return mPM->RevokePermission(packageName, permissionName);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::CheckSignatures(
    /* [in] */ const String& pkg1,
    /* [in] */ const String& pkg2,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
//     try {
    return mPM->CheckSignatures(pkg1, pkg2, result);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::CheckSignatures(
    /* [in] */ Int32 uid1,
    /* [in] */ Int32 uid2,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
//     try {
    return mPM->CheckUidSignatures(uid1, uid2, result);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::GetPackagesForUid(
    /* [in] */ Int32 uid,
    /* [out, callee] */ ArrayOf<String>** packages)
{
    VALIDATE_NOT_NULL(packages);
//     try {
    return mPM->GetPackagesForUid(uid, packages);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::GetNameForUid(
    /* [in] */ Int32 uid,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
//     try {
    return mPM->GetNameForUid(uid, name);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::GetUidForSharedUser(
    /* [in] */ const String& sharedUserName,
    /* [out] */ Int32* useruid)
{
    VALIDATE_NOT_NULL(useruid);
//     try {
    Int32 uid;
    FAIL_RETURN(mPM->GetUidForSharedUser(sharedUserName, &uid));
    if(uid != -1) {
        *useruid = uid;
        return NOERROR;
    }
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
//     throw new NameNotFoundException("No shared userid for user:"+sharedUserName);
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::GetInstalledPackages(
    /* [in] */ Int32 flags,
    /* [out] */ IList** infos)
{
    VALIDATE_NOT_NULL(infos);
    Int32 id;
    mContext->GetUserId(&id);
    return GetInstalledPackages(flags, id, infos);
}

ECode ApplicationPackageManager::GetInstalledPackages(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IList** infos)
{
    VALIDATE_NOT_NULL(infos);
    *infos = NULL;

    Int32 id;
    mContext->GetUserId(&id);
    AutoPtr<IParceledListSlice> slice;
    ECode ec = mPM->GetInstalledPackages(
        flags, userId, (IParceledListSlice**)&slice);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "Package manager has died");
        return E_RUNTIME_EXCEPTION;
    }

    return slice->GetList(infos);
}

ECode ApplicationPackageManager::GetPackagesHoldingPermissions(
    /* [in] */ ArrayOf<String>* permissions,
    /* [in] */ Int32 flags,
    /* [out] */ IList** apps)
{
    VALIDATE_NOT_NULL(apps);
    *apps = NULL;

    Int32 userId;
    mContext->GetUserId(&userId);
    AutoPtr<IParceledListSlice> slice;
    ECode ec = mPM->GetPackagesHoldingPermissions(
        permissions, flags, userId, (IParceledListSlice**)&slice);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "Package manager has died");
        return E_RUNTIME_EXCEPTION;
    }

    return slice->GetList(apps);
}

ECode ApplicationPackageManager::GetInstalledApplications(
    /* [in] */ Int32 flags,
    /* [out] */ IList** apps)
{
    VALIDATE_NOT_NULL(apps);
    *apps = NULL;

    Int32 userId = 0;
    mContext->GetUserId(&userId);

    AutoPtr<IParceledListSlice> slice;
    ECode ec = mPM->GetInstalledApplications(
        flags, userId, (IParceledListSlice**)&slice);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "Package manager has died");
        return E_RUNTIME_EXCEPTION;
    }
    return slice->GetList(apps);
}

ECode ApplicationPackageManager::ResolveActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [out] */ IResolveInfo** resolve)
{
    VALIDATE_NOT_NULL(resolve);
    Int32 id;
    mContext->GetUserId(&id);
    return ResolveActivityAsUser(intent, flags, id, resolve);
}

ECode ApplicationPackageManager::ResolveActivityAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IResolveInfo** resolve)
{
    VALIDATE_NOT_NULL(resolve);
//     try {
    AutoPtr<IContentResolver> cResolver;
    FAIL_RETURN(mContext->GetContentResolver((IContentResolver**)&cResolver));
    String type;
    FAIL_RETURN(intent->ResolveTypeIfNeeded(cResolver, &type));
    return mPM->ResolveIntent(
            intent,
            type,
            flags,
            userId, resolve);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::QueryIntentActivities(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [out] */ IList** resolves)
{
    VALIDATE_NOT_NULL(resolves);
    Int32 id;
    mContext->GetUserId(&id);
    return QueryIntentActivitiesAsUser(intent, flags, id, resolves);
}

ECode ApplicationPackageManager::QueryIntentActivitiesAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IList** resolves)
{
    VALIDATE_NOT_NULL(resolves);
//     try {
    AutoPtr<IContentResolver> cResolver;
    FAIL_RETURN(mContext->GetContentResolver((IContentResolver**)&cResolver));
    String type;
    FAIL_RETURN(intent->ResolveTypeIfNeeded(cResolver, &type));
    return mPM->QueryIntentActivities(
            intent,
            type,
            flags,
            userId,
            resolves);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::QueryIntentActivityOptions(
    /* [in] */ IComponentName* caller,
    /* [in] */ ArrayOf<IIntent*>* specifics,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [out] */ IList** resolves)
{
    VALIDATE_NOT_NULL(resolves);
    AutoPtr<IContentResolver> resolver;
    FAIL_RETURN(mContext->GetContentResolver((IContentResolver**)&resolver));

    AutoPtr<ArrayOf<String> > specificTypes;
    if (specifics != NULL) {
        const Int32 N = specifics->GetLength();
        for (Int32 i=0; i<N; i++) {
            AutoPtr<IIntent> sp = (*specifics)[i];
            if (sp != NULL) {
                String t;
                sp->ResolveTypeIfNeeded(resolver, &t);
                if (t != NULL) {
                    if (specificTypes == NULL) {
                        specificTypes = ArrayOf<String>::Alloc(N);
                    }
                    (*specificTypes)[i] = t;
                }
            }
        }
    }

//     try {
    String type;
    FAIL_RETURN(intent->ResolveTypeIfNeeded(resolver, &type));
    Int32 id;
    mContext->GetUserId(&id);
    return mPM->QueryIntentActivityOptions(caller, specifics,
                                              specificTypes, intent, type,
                                              flags, id, resolves);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::QueryBroadcastReceivers(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IList** resolves)
{
    VALIDATE_NOT_NULL(resolves);
//     try {
    AutoPtr<IContentResolver> resolver;
    FAIL_RETURN(mContext->GetContentResolver((IContentResolver**)&resolver));
    String type;
    FAIL_RETURN(intent->ResolveTypeIfNeeded(resolver, &type));
    return mPM->QueryIntentReceivers(
            intent,
            type,
            flags,
            userId,
            resolves);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::QueryBroadcastReceivers(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [out] */ IList** resolves)
{
    VALIDATE_NOT_NULL(resolves);

    Int32 id;
    mContext->GetUserId(&id);
    return QueryBroadcastReceivers(intent, flags, id, resolves);
}

ECode ApplicationPackageManager::ResolveService(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [out] */ IResolveInfo** resolve)
{
    VALIDATE_NOT_NULL(resolve);
//     try {
    AutoPtr<IContentResolver> resolver;
    FAIL_RETURN(mContext->GetContentResolver((IContentResolver**)&resolver));
    String type;
    FAIL_RETURN(intent->ResolveTypeIfNeeded(resolver, &type));
    Int32 id;
    FAIL_RETURN(mContext->GetUserId(&id));
    return mPM->ResolveService(
            intent,
            type,
            flags,
            id,
            resolve);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::QueryIntentServicesAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IList** resolves)
{
    VALIDATE_NOT_NULL(resolves);
    // try {
    AutoPtr<IContentResolver> resolver;
    FAIL_RETURN(mContext->GetContentResolver((IContentResolver**)&resolver));
    String type;
    FAIL_RETURN(intent->ResolveTypeIfNeeded(resolver, &type));
    return mPM->QueryIntentServices(
        intent,
        type,
        flags,
        userId,
        resolves);
    // } catch (RemoteException e) {
    //     throw new RuntimeException("Package manager has died", e);
    // }
}

ECode ApplicationPackageManager::QueryIntentServices(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [out] */ IList** resolves)
{
    VALIDATE_NOT_NULL(resolves);
    Int32 id;
    mContext->GetUserId(&id);
    return QueryIntentServicesAsUser(intent, flags, id, resolves);
}

ECode ApplicationPackageManager::QueryIntentContentProvidersAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IList** resolveInfos) //List<ResolveInfo>
{
    VALIDATE_NOT_NULL(resolveInfos)
    *resolveInfos = NULL;
    AutoPtr<IContentResolver> cResolver;
    FAIL_RETURN(mContext->GetContentResolver((IContentResolver**)&cResolver));
    String type;
    FAIL_RETURN(intent->ResolveTypeIfNeeded(cResolver, &type));
    return mPM->QueryIntentContentProviders(
        intent, type, flags, userId, resolveInfos);
}

ECode ApplicationPackageManager::QueryIntentContentProviders(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [out] */ IList** resolveInfos) //List<ResolveInfo>
{
    Int32 id;
    mContext->GetUserId(&id);
    return QueryIntentContentProvidersAsUser(intent, flags, id, resolveInfos);
}

ECode ApplicationPackageManager::ResolveContentProvider(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags,
    /* [out] */ IProviderInfo** provider)
{
    Int32 id;
    mContext->GetUserId(&id);
    return ResolveContentProvider(name, flags, id, provider);
}

ECode ApplicationPackageManager::ResolveContentProvider(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IProviderInfo** provider)
{
    return mPM->ResolveContentProvider(name, flags, userId, provider);
}

ECode ApplicationPackageManager::QueryContentProviders(
    /* [in] */ const String& processName,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 flags,
    /* [out] */ IList** providers)
{
//     try {
    return mPM->QueryContentProviders(processName, uid, flags, providers);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::GetInstrumentationInfo(
    /* [in] */ IComponentName* className,
    /* [in] */ Int32 flags,
    /* [out] */ IInstrumentationInfo** instrumentation)
{
    VALIDATE_NOT_NULL(instrumentation);
    *instrumentation = NULL;
//     try {
    AutoPtr<IInstrumentationInfo> ii;
    mPM->GetInstrumentationInfo(className, flags, (IInstrumentationInfo**)&ii);
    if (ii != NULL) {
        *instrumentation = ii;
        REFCOUNT_ADD(*instrumentation);
        return NOERROR;
    }
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }

//     throw new NameNotFoundException(className.toString());
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::QueryInstrumentation(
    /* [in] */ const String& targetPackage,
    /* [in] */ Int32 flags,
    /* [out] */ IList** instrumentations)
{
//     try {
    return mPM->QueryInstrumentation(targetPackage, flags, instrumentations);
//     } catch (RemoteException e) {
//         throw new RuntimeException("Package manager has died", e);
//     }
}

ECode ApplicationPackageManager::GetDrawable(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 resid,
    /* [in] */ IApplicationInfo* appInfo,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);

    AutoPtr<ResourceName> name = new ResourceName(packageName, resid);
    AutoPtr<IDrawable> dr = GetCachedIcon(name);
    if (dr != NULL) {
        *drawable = dr;
        REFCOUNT_ADD(*drawable);
        return NOERROR;
    }
    if (appInfo == NULL) {
        if (FAILED(GetApplicationInfo(packageName, sDefaultFlags, &appInfo))) {
            *drawable = NULL;
            return NOERROR;
        }
    }

    AutoPtr<IResources> r;
    FAIL_GOTO(GetResourcesForApplication(appInfo, (IResources**)&r), EXCEPTION);
    FAIL_GOTO(r->GetDrawable(resid, (IDrawable**)&dr), EXCEPTION);

    PutCachedIcon(name, dr);
    *drawable = dr;
    REFCOUNT_ADD(*drawable);
    return NOERROR;

EXCEPTION:
    *drawable = NULL;
    return NOERROR;
}

ECode ApplicationPackageManager::GetActivityIcon(
    /* [in] */ IComponentName* activityName,
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon);
    AutoPtr<IActivityInfo> aInfo;
    GetActivityInfo(activityName, sDefaultFlags, (IActivityInfo**)&aInfo);
    FAIL_RETURN(IPackageItemInfo::Probe(aInfo)->LoadIcon(this, icon));
    return NOERROR;
}

ECode ApplicationPackageManager::GetActivityIcon(
    /* [in] */ IIntent* intent,
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon);
    AutoPtr<IComponentName> component;
    intent->GetComponent((IComponentName**)&component);
    if (component != NULL) {
        return GetActivityIcon(component, icon);
    }

    AutoPtr<IResolveInfo> info;
    ResolveActivity(
        intent, IPackageManager::MATCH_DEFAULT_ONLY, (IResolveInfo**)&info);
    if (info != NULL) {
        AutoPtr<IActivityInfo> aInfo;
        info->GetActivityInfo((IActivityInfo**)&aInfo);
        return IPackageItemInfo::Probe(aInfo)->LoadIcon(this, icon);
    }

//     throw new NameNotFoundException(intent.toUri(0));
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::GetDefaultActivityIcon(
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon);
    AutoPtr<IResources> resources = CResources::GetSystem();
    FAIL_RETURN(resources->GetDrawable(
           R::drawable::sym_def_app_icon, icon));
    return NOERROR;
}

ECode ApplicationPackageManager::GetApplicationIcon(
    /* [in] */ IApplicationInfo* info,
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon);
    return IPackageItemInfo::Probe(info)->LoadIcon(this, icon);
}

ECode ApplicationPackageManager::GetApplicationIcon(
    /* [in] */ const String& packageName,
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon);
    AutoPtr<IApplicationInfo> appInfo;
    GetApplicationInfo(packageName, sDefaultFlags, (IApplicationInfo**)&appInfo);
    return GetApplicationIcon(appInfo, icon);
}

ECode ApplicationPackageManager::GetActivityBanner(
    /* [in] */ IComponentName* activityName,
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon);
    AutoPtr<IActivityInfo> info;
    GetActivityInfo(activityName, sDefaultFlags, (IActivityInfo**)&info);
    return  IPackageItemInfo::Probe(info)->LoadBanner(this, icon);
}

ECode ApplicationPackageManager::GetActivityBanner(
    /* [in] */ IIntent* intent,
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon)
    *icon = NULL;

    AutoPtr<IComponentName>  cn;
    intent->GetComponent((IComponentName**)&cn);
    if (cn != NULL) {
        return GetActivityBanner(cn, icon);
    }

    AutoPtr<IResolveInfo> info;
    ResolveActivity(intent, IPackageManager::MATCH_DEFAULT_ONLY, (IResolveInfo**)&info);
    if (info != NULL) {
        AutoPtr<IActivityInfo> ai;
        info->GetActivityInfo((IActivityInfo**)&ai);
        return  IPackageItemInfo::Probe(info)->LoadBanner(this, icon);
    }

    // throw new NameNotFoundException(intent.toUri(0));
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::GetApplicationBanner(
    /* [in] */ IApplicationInfo* info,
    /* [out] */ IDrawable** icon)
{
    return IPackageItemInfo::Probe(info)->LoadBanner(this, icon);
}

ECode ApplicationPackageManager::GetApplicationBanner(
    /* [in] */ const String& packageName,
    /* [out] */ IDrawable** icon)
{
    AutoPtr<IApplicationInfo> info;
    GetApplicationInfo(packageName, sDefaultFlags, (IApplicationInfo**)&info);
    return GetApplicationBanner(info, icon);
}

ECode ApplicationPackageManager::GetActivityLogo(
    /* [in] */ IComponentName* activityName,
    /* [out] */ IDrawable** logo)
{
    VALIDATE_NOT_NULL(logo);
    AutoPtr<IActivityInfo> aInfo;
    GetActivityInfo(activityName, sDefaultFlags, (IActivityInfo**)&aInfo);
    return IPackageItemInfo::Probe(aInfo)->LoadLogo(this, logo);
}

ECode ApplicationPackageManager::GetActivityLogo(
    /* [in] */ IIntent* intent,
    /* [out] */ IDrawable** logo)
{
    AutoPtr<IComponentName> component;
    intent->GetComponent((IComponentName**)&component);
    if (component != NULL) {
        return GetActivityLogo(component, logo);
    }

    AutoPtr<IResolveInfo> info;
    ResolveActivity(
        intent, IPackageManager::MATCH_DEFAULT_ONLY, (IResolveInfo**)&info);
    if (info != NULL) {
        AutoPtr<IActivityInfo> aInfo;
        info->GetActivityInfo((IActivityInfo**)&aInfo);
        return IPackageItemInfo::Probe(aInfo)->LoadLogo(this, logo);
    }

//     throw new NameNotFoundException(intent.toUri(0));
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::GetApplicationLogo(
    /* [in] */ IApplicationInfo* info,
    /* [out] */ IDrawable** logo)
{
    VALIDATE_NOT_NULL(logo);

    return IPackageItemInfo::Probe(info)->LoadLogo(this, logo);
}

ECode ApplicationPackageManager::GetApplicationLogo(
    /* [in] */ const String& packageName,
    /* [out] */ IDrawable** logo)
{
    VALIDATE_NOT_NULL(logo);

    AutoPtr<IApplicationInfo> info;
    GetApplicationInfo(packageName, sDefaultFlags, (IApplicationInfo**)&info);
    return GetApplicationLogo(info, logo);
}

ECode ApplicationPackageManager::GetResourcesForActivity(
    /* [in] */ IComponentName* activityName,
    /* [out] */ IResources** res)
{
     VALIDATE_NOT_NULL(res);

     AutoPtr<IActivityInfo> aInfo;
     GetActivityInfo(activityName, sDefaultFlags, (IActivityInfo**)&aInfo);
     AutoPtr<IApplicationInfo> info;
     IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&info);
     return GetResourcesForApplication(info, res);
}

ECode ApplicationPackageManager::GetUserBadgedIcon(
    /* [in] */ IDrawable* icon,
    /* [in] */ IUserHandle* user,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    *drawable = NULL;
    Int32 userId;
    user->GetIdentifier(&userId);
    Int32 badgeResId = GetBadgeResIdForUser(userId);
    if (badgeResId == 0) {
        *drawable = icon;
        REFCOUNT_ADD(*drawable)
        return NOERROR;
    }
    AutoPtr<IDrawable> badgeIcon;
    GetDrawable(String("system"), badgeResId, NULL, (IDrawable**)&badgeIcon);
    AutoPtr<IDrawable> tmp = GetBadgedDrawable(icon, badgeIcon, NULL, TRUE);
    *drawable = tmp;
    REFCOUNT_ADD(*drawable)
    return NOERROR;
}

ECode ApplicationPackageManager::GetUserBadgedDrawableForDensity(
    /* [in] */ IDrawable* drawable,
    /* [in] */ IUserHandle* user,
    /* [in] */ IRect* badgeLocation,
    /* [in] */ Int32 badgeDensity,
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IDrawable> badgeDrawable;
    GetUserBadgeForDensity(user, badgeDensity, (IDrawable**)&badgeDrawable);
    if (badgeDrawable == NULL) {
        *result = drawable;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    AutoPtr<IDrawable> tmp = GetBadgedDrawable(drawable, badgeDrawable, badgeLocation, TRUE);
    *result = tmp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ApplicationPackageManager::GetUserBadgeForDensity(
    /* [in] */ IUserHandle* user,
    /* [in] */ Int32 density,
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 id;
    user->GetIdentifier(&id);
    AutoPtr<IUserInfo> userInfo = GetUserIfProfile(id);
    Boolean bval;
    if (userInfo != NULL && (userInfo->IsManagedProfile(&bval), bval)) {
        Int32 d = density;
        if (d <= 0) {
            AutoPtr<IResources> resources;
            mContext->GetResources((IResources**)&resources);
            AutoPtr<IDisplayMetrics> dm;
            resources->GetDisplayMetrics((IDisplayMetrics**)&dm);
            dm->GetDensityDpi(&d);
        }

        AutoPtr<IResources> systemRes = CResources::GetSystem();
        return systemRes->GetDrawableForDensity(
                R::drawable::ic_corp_badge, d, result);
    }
    return NOERROR;
}

ECode ApplicationPackageManager::GetUserBadgedLabel(
    /* [in] */ ICharSequence* label,
    /* [in] */ IUserHandle* user,
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq)
    *csq = NULL;

    Int32 id;
    user->GetIdentifier(&id);
    AutoPtr<IUserInfo> userInfo = GetUserIfProfile(id);
    Boolean bval;
    if (userInfo != NULL && (userInfo->IsManagedProfile(&bval), bval)) {
        AutoPtr<IResources> systemRes = CResources::GetSystem();
        AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, label);
        String str;
        systemRes->GetString(
                R::string::managed_profile_label_badge, args, &str);
        AutoPtr<ICharSequence> tmp = CoreUtils::Convert(str);
        *csq = tmp;
        REFCOUNT_ADD(*csq);
        return NOERROR;
    }

    *csq = label;
    REFCOUNT_ADD(*csq)
    return NOERROR;
}

ECode ApplicationPackageManager::GetResourcesForApplication(
    /* [in] */ IApplicationInfo* app,
    /* [out] */ IResources** res)
{
    VALIDATE_NOT_NULL(res);
    *res = NULL;

    String pkgName;
    IPackageItemInfo::Probe(app)->GetPackageName(&pkgName);
    if (pkgName.Equals("system")) {
        AutoPtr<IContextImpl> sysContent;
        CContextImpl* ci = (CContextImpl*)IContextImpl::Probe(mContext);
        ci->mMainThread->GetSystemContext((IContextImpl**)&sysContent);
        return IContext::Probe(sysContent)->GetResources(res);
    }

    Int32 appUid;
    app->GetUid(&appUid);
    Boolean sameUid = (appUid == Process::MyUid());

    String sourceDir, publicSourceDir;
    AutoPtr< ArrayOf<String> > splitSourceDirs, splitPublicSourceDirs, resourceDirs, sharedLibraryFiles;
    app->GetSourceDir(&sourceDir);
    app->GetPublicSourceDir(&publicSourceDir);
    app->GetSplitSourceDirs((ArrayOf<String>**)&splitSourceDirs);
    app->GetSplitPublicSourceDirs((ArrayOf<String>**)&splitPublicSourceDirs);
    app->GetResourceDirs((ArrayOf<String>**)&resourceDirs);
    app->GetSharedLibraryFiles((ArrayOf<String>**)&sharedLibraryFiles);

    AutoPtr<IResources> r;
    CContextImpl* ci = (CContextImpl*)IContextImpl::Probe(mContext);
    String packageName;
    IPackageItemInfo::Probe(app)->GetPackageName(&packageName);
    ci->mMainThread->GetTopLevelResources(
            sameUid ? sourceDir : publicSourceDir,
            sameUid ? splitSourceDirs : splitPublicSourceDirs,
            resourceDirs, sharedLibraryFiles, IDisplay::DEFAULT_DISPLAY,
            NULL, ci->mPackageInfo, mContext, packageName, (IResources**)&r);
    if (r != NULL) {
        *res = r;
        REFCOUNT_ADD(*res)
        return NOERROR;
    }

    Slogger::E(TAG, "Unable to open [%s], ApplicationInfo:[%s]", publicSourceDir.string(), TO_CSTR(app));
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::GetResourcesForApplication(
    /* [in] */ const String& appPackageName,
    /* [out] */ IResources** res)
{
    VALIDATE_NOT_NULL(res);

    AutoPtr<IApplicationInfo> info;
    GetApplicationInfo(appPackageName, sDefaultFlags, (IApplicationInfo**)&info);
    return GetResourcesForApplication(info, res);
}

ECode ApplicationPackageManager::GetResourcesForApplicationAsUser(
    /* [in] */ const String& appPackageName,
    /* [in] */ Int32 userId,
    /* [out] */ IResources** res)
{
    VALIDATE_NOT_NULL(res);
    *res = NULL;

    if (userId < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (appPackageName.Equals("system")) {
        AutoPtr<IContextImpl> ctx;
        CContextImpl* ci = (CContextImpl*)IContextImpl::Probe(mContext);
        ci->mMainThread->GetSystemContext((IContextImpl**)&ctx);
        return IContext::Probe(ctx)->GetResources(res);
    }

    // try {
    AutoPtr<IApplicationInfo> ai;
    if (FAILED(mPM->GetApplicationInfo(appPackageName, sDefaultFlags, userId, (IApplicationInfo**)&ai))) {
        return E_RUNTIME_EXCEPTION;
    }

    if (ai != NULL) {
        return GetResourcesForApplication(ai, res);
    }
    // } catch (RemoteException e) {
    //     throw new RuntimeException("Package manager has died", e);
    // }

    Slogger::E(TAG, "UPackage [%s] doesn't exist",  appPackageName.string());
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode ApplicationPackageManager::GetThemedResourcesForApplication(
    /* [in] */ IApplicationInfo* app,
    /* [in] */ const String& themePkgName,
    /* [out] */ IResources** res)
{
    return E_NOT_IMPLEMENTED;
}

ECode ApplicationPackageManager::GetThemedResourcesForApplication(
    /* [in] */ const String& appPackageName,
    /* [in] */ const String& themePkgName,
    /* [out] */ IResources** res)
{
    return E_NOT_IMPLEMENTED;
}

ECode ApplicationPackageManager::GetThemedResourcesForApplicationAsUser(
    /* [in] */ const String& appPackageName,
    /* [in] */ const String& themePkgName,
    /* [in] */ Int32 userId,
    /* [out] */ IResources** res)
{
    return E_NOT_IMPLEMENTED;
}

ECode ApplicationPackageManager::IsSafeMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    // try {
    if (mCachedSafeMode < 0) {
        Boolean isSafeMode = FALSE;
        if (FAILED(mPM->IsSafeMode(&isSafeMode))) {
            Slogger::E(TAG, "Package manager has died");
            return E_RUNTIME_EXCEPTION;
        }
        mCachedSafeMode = isSafeMode ? 1 : 0;
    }
    *result = mCachedSafeMode != 0;
    return NOERROR;
    // } catch (RemoteException e) {
    //     throw new RuntimeException("Package manager has died", e);
    // }
}

void ApplicationPackageManager::ConfigurationChanged()
{
    AutoLock lock(sSync);
    sIconCache.Clear();
    sStringCache.Clear();
}

AutoPtr<IDrawable> ApplicationPackageManager::GetCachedIcon(
    /* [in] */ ResourceName* name)
{
    AutoLock lock(sSync);
    HashMap<AutoPtr<ResourceName>, AutoPtr<IWeakReference> >::Iterator it = sIconCache.Find(name);
    if (it != sIconCache.End()) {
        AutoPtr<IWeakReference> wr = it->mSecond;
        AutoPtr<IDrawableConstantState> state;
        wr->Resolve(EIID_IDrawableConstantState, (IInterface**)&state);

        if (state != NULL) {
            if (DEBUG_ICONS) {
                String info;
                name->ToString(&info);
                Logger::V(TAG, "Get cached drawable state for %s : %s",
                    info.string(), TO_CSTR(state));
            }
            // Note: It's okay here to not use the newDrawable(Resources) variant
            //       of the API. The ConstantState comes from a drawable that was
            //       originally created by passing the proper app Resources instance
            //       which means the state should already contain the proper
            //       resources specific information (like density.) See
            //       BitmapDrawable.BitmapState for instance.
            AutoPtr<IDrawable> dr;
            state->NewDrawable((IDrawable**)&dr);
            return dr;
        }

        // our entry has been purged
        sIconCache.Erase(it);
    }
    return NULL;
}

void ApplicationPackageManager::PutCachedIcon(
    /* [in] */ ResourceName* name,
    /* [in] */ IDrawable* dr)
{
    assert(dr != NULL);
    AutoLock lock(sSync);
    AutoPtr<IDrawableConstantState> state;
    dr->GetConstantState((IDrawableConstantState**)&state);

    IWeakReferenceSource* source = IWeakReferenceSource::Probe(state);
    if (source != NULL) {
        AutoPtr<IWeakReference> wr;
        source->GetWeakReference((IWeakReference**)&wr);
        sIconCache[name] = wr;
        if (DEBUG_ICONS) {
            String info;
            name->ToString(&info);
            Logger::V(TAG, "Added cached drawable state for %s : %s",
                info.string(), TO_CSTR(state));
        }
    }
}

void ApplicationPackageManager::HandlePackageBroadcast(
    /* [in] */ Int32 cmd,
    /* [in] */ ArrayOf<String>* pkgList,
    /* [in] */ Boolean hasPkgInfo)
{
    Boolean immediateGc = FALSE;
    if (cmd == IApplicationThread::EXTERNAL_STORAGE_UNAVAILABLE) {
        immediateGc = TRUE;
    }

    if (pkgList != NULL && (pkgList->GetLength() > 0)) {
        Boolean needCleanup = FALSE;
        String ssp;
        for (Int32 i = 0; i < pkgList->GetLength(); ++i) {
            ssp = (*pkgList)[i];
            {
                AutoLock lock(sSync);
                if (!sIconCache.IsEmpty()) {
                    HashMap<AutoPtr<ResourceName>, AutoPtr<IWeakReference> >::Iterator it;
                    for (it = sIconCache.Begin(); it != sIconCache.End();) {
                        AutoPtr<ResourceName> nm = it->mFirst;
                        if (nm->mPackageName.Equals(ssp)) {
                            if (DEBUG_ICONS) {
                                String info;
                                nm->ToString(&info);
                                Logger::V(TAG, "Removing cached drawable for %s", info.string());
                            }
                            sIconCache.Erase(it++);
                            needCleanup = true;
                        }
                        else {
                            ++it;
                        }
                    }
                }

                if (!sStringCache.IsEmpty()) {
                    HashMap<AutoPtr<ResourceName>, AutoPtr<IWeakReference> >::Iterator it;
                    for (it = sStringCache.Begin(); it != sStringCache.End();) {
                        AutoPtr<ResourceName> nm = it->mFirst;
                        if (nm->mPackageName.Equals(ssp)) {
                            if (DEBUG_ICONS) {
                                String info;
                                nm->ToString(&info);
                                Logger::V(TAG, "Removing cached string for %s", info.string());
                            }
                            sStringCache.Erase(it++);
                            needCleanup = true;
                        }
                        else {
                            ++it;
                        }
                    }
                }
            }
        }

        if (needCleanup || hasPkgInfo) {
            if (immediateGc) {
                // Schedule an immediate gc.
                // Runtime.getRuntime().gc();
            } else {
                // ActivityThread.currentActivityThread().scheduleGcIdler();
            }
        }
    }
}

AutoPtr<ICharSequence> ApplicationPackageManager::GetCachedString(
    /* [in] */ ResourceName* name)
{
    AutoLock lock(sSync);

    HashMap<AutoPtr<ResourceName>, AutoPtr<IWeakReference> >::Iterator it = sStringCache.Find(name);
    if (it != sIconCache.End()) {
        AutoPtr<IWeakReference> wr = it->mSecond;
        AutoPtr<ICharSequence> cs;
        wr->Resolve(EIID_ICharSequence, (IInterface**)&cs);
        if (cs != NULL) {
            if (DEBUG_ICONS) {
                String str;
                cs->ToString(&str);
                String info;
                name->ToString(&info);
                Logger::V(TAG, "Get cached string for %s : %s wr:%p",
                    info.string(), str.string(), wr.Get());
            }

            return cs;
        }

        // our entry has been purged
        sStringCache.Erase(it);
    }

    return NULL;
}

void ApplicationPackageManager::PutCachedString(
    /* [in] */ ResourceName* name,
    /* [in] */ ICharSequence* cs)
{
    AutoLock lock(sSync);

    AutoPtr<IWeakReferenceSource> source = IWeakReferenceSource::Probe(cs);
    if (source != NULL) {
        AutoPtr<IWeakReference> wr;
        source->GetWeakReference((IWeakReference**)&wr);
        sStringCache[name] = wr;

        if (DEBUG_ICONS) {
            String str;
            cs->ToString(&str);
            String info;
            name->ToString(&info);
            Logger::V(TAG, "Added cached string for %s : %s wr:%p",
                info.string(), str.string(), wr.Get());
        }
    }
}

ECode ApplicationPackageManager::GetText(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 resid,
    /* [in] */ IApplicationInfo* appInfo,
    /* [out] */ ICharSequence** _text)
{
    VALIDATE_NOT_NULL(_text);
    *_text = NULL;

    AutoPtr<ResourceName> name = new ResourceName(packageName, resid);
    AutoPtr<ICharSequence> text = GetCachedString(name);
    if (text != NULL) {
        *_text = text;
        REFCOUNT_ADD(*_text);
        return NOERROR;
    }

    ECode ec;
    if (appInfo == NULL) {
        ec = GetApplicationInfo(packageName, sDefaultFlags, (IApplicationInfo**)&appInfo);
        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
            return NOERROR;
        }
    }

    AutoPtr<IResources> r;
    FAIL_GOTO((ec = GetResourcesForApplication(appInfo, (IResources**)&r)), ERROR)
    FAIL_GOTO((ec = r->GetText(resid, (ICharSequence**)&text)), ERROR)
    PutCachedString(name, text);
    *_text = text;
    REFCOUNT_ADD(*_text)
    return NOERROR;

ERROR:
    switch(ec) {
        case E_NAME_NOT_FOUND_EXCEPTION:
        {
            String pkgName;
            IPackageItemInfo::Probe(appInfo)->GetPackageName(&pkgName);
            Slogger::W("PackageManager", "Failure retrieving resources for %s",
                pkgName.string());
            ec = NOERROR;
            break;
        }
        case E_RUNTIME_EXCEPTION:
        {
           // If an exception was thrown, fall through to return
           // default icon.
            Slogger::W("PackageManager", "Failure retrieving text 0x%08x  in package %s",
                resid, packageName.string());
            ec = NOERROR;
            break;
        }
    }
    return ec;
}

ECode ApplicationPackageManager::GetXml(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 resid,
    /* [in] */ IApplicationInfo* _appInfo,
    /* [out] */ IXmlResourceParser** parser)
{
    VALIDATE_NOT_NULL(parser);
    AutoPtr<IApplicationInfo> appInfo = _appInfo;
    if (appInfo == NULL) {
        // try {
        ECode ec = GetApplicationInfo(packageName, sDefaultFlags, (IApplicationInfo**)&appInfo);
        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
            *parser = NULL;
            return ec;
        }
        // } catch (NameNotFoundException e) {
        //     return NULL;
        // }
    }

    // try {
    AutoPtr<IResources> r;
    GetResourcesForApplication(appInfo, (IResources**)&r);
    return r->GetXml(resid, parser);
    // } catch (RuntimeException e) {
    //     // If an exception was thrown, fall through to return
    //     // default icon.
    //     Log.w("PackageManager", "Failure retrieving xml 0x"
    //           + Integer.toHexString(resid) + " in package "
    //           + packageName, e);
    // } catch (NameNotFoundException e) {
    //     Log.w("PackageManager", "Failure retrieving resources for "
    //           + appInfo.packageName);
    // }
    // return NULL;
}

ECode ApplicationPackageManager::GetApplicationLabel(
    /* [in] */ IApplicationInfo* info,
    /* [out] */ ICharSequence** label)
{
    assert(info != NULL);
    return IPackageItemInfo::Probe(info)->LoadLabel((IPackageManager*)this, label);
}

ECode ApplicationPackageManager::InstallPackage(
    /* [in] */ IUri* packageURI,
    /* [in] */ IIPackageInstallObserver* observer,
    /* [in] */ Int32 flags,
    /* [in] */ const String& installerPackageName)
{
    AutoPtr<IVerificationParams> verificationParams;
    CVerificationParams::New(NULL, NULL,
        NULL, IVerificationParams::NO_UID, NULL,
        (IVerificationParams**)&verificationParams);

    AutoPtr<IPackageInstallObserver> lpio = new LegacyPackageInstallObserver(observer);
    return InstallCommon(packageURI, lpio, flags,
            installerPackageName, verificationParams, NULL);
}

ECode ApplicationPackageManager::InstallPackageWithVerification(
    /* [in] */ IUri* packageURI,
    /* [in] */ IIPackageInstallObserver* observer,
    /* [in] */ Int32 flags,
    /* [in] */ const String& installerPackageName,
    /* [in] */ IUri* verificationURI,
    /* [in] */ IManifestDigest* manifestDigest,
    /* [in] */ IContainerEncryptionParams* encryptionParams)
{
    AutoPtr<IVerificationParams> verificationParams;
    CVerificationParams::New(verificationURI, NULL,
        NULL, IVerificationParams::NO_UID, manifestDigest,
        (IVerificationParams**)&verificationParams);

    AutoPtr<IPackageInstallObserver> lpio = new LegacyPackageInstallObserver(observer);
    return InstallCommon(packageURI, lpio, flags,
            installerPackageName, verificationParams, encryptionParams);
}

ECode ApplicationPackageManager::InstallPackageWithVerificationAndEncryption(
    /* [in] */ IUri* packageURI,
    /* [in] */ IIPackageInstallObserver* observer,
    /* [in] */ Int32 flags,
    /* [in] */ const String& installerPackageName,
    /* [in] */ IVerificationParams* verificationParams,
    /* [in] */ IContainerEncryptionParams* encryptionParams)
{
    AutoPtr<IPackageInstallObserver> lpio = new LegacyPackageInstallObserver(observer);
    return InstallCommon(packageURI, lpio, flags,
            installerPackageName, verificationParams, encryptionParams);
}

ECode ApplicationPackageManager::InstallCommon(
    /* [in] */ IUri* packageURI,
    /* [in] */ IPackageInstallObserver* observer,
    /* [in] */ Int32 flags,
    /* [in] */ const String& installerPackageName,
    /* [in] */ IVerificationParams* verificationParams,
    /* [in] */ IContainerEncryptionParams* encryptionParams)
{
    String scheme;
    packageURI->GetScheme(&scheme);
    if (!scheme.Equals("file")) {
        Logger::E(TAG, "Only file:// URIs are supported");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    if (encryptionParams != NULL) {
        Logger::E(TAG, "ContainerEncryptionParams not supported");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    String originPath;
    packageURI->GetPath(&originPath);
    AutoPtr<IIPackageInstallObserver2> binder;
    observer->GetBinder((IIPackageInstallObserver2**)&binder);
    return mPM->InstallPackage(originPath, binder, flags, installerPackageName,
            verificationParams, String(NULL));
}


ECode ApplicationPackageManager::InstallExistingPackage(
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    ECode ec = mPM->InstallExistingPackageAsUser(packageName, UserHandle::GetMyUserId(), result);
    if (FAILED(ec)) {
        *result = 0;
        Slogger::W("PackageManager", "Package %s doesn't exist", packageName.string());
        return E_NAME_NOT_FOUND_EXCEPTION;
    }

    if (*result == INSTALL_FAILED_INVALID_URI) {
        Slogger::W("PackageManager", "Package %s doesn't exist", packageName.string());
        return E_NAME_NOT_FOUND_EXCEPTION;
    }
    return NOERROR;
}

ECode ApplicationPackageManager::VerifyPendingInstall(
    /* [in] */ Int32 id,
    /* [in] */ Int32 verificationCode)
{
    return mPM->VerifyPendingInstall(id, verificationCode);
}

ECode ApplicationPackageManager::ExtendVerificationTimeout(
    /* [in] */ Int32 id,
    /* [in] */ Int32 verificationCodeAtTimeout,
    /* [in] */ Int64 millisecondsToDelay)
{
    return mPM->ExtendVerificationTimeout(id, verificationCodeAtTimeout, millisecondsToDelay);
}

ECode ApplicationPackageManager::SetInstallerPackageName(
    /* [in] */ const String& targetPackage,
    /* [in] */ const String& installerPackageName)
{
    return mPM->SetInstallerPackageName(targetPackage, installerPackageName);
}

ECode ApplicationPackageManager::MovePackage(
    /* [in] */ const String& packageName,
    /* [in] */ IIPackageMoveObserver* observer,
    /* [in] */ Int32 flags)
{
    return mPM->MovePackage(packageName, observer, flags);
}

ECode ApplicationPackageManager::GetInstallerPackageName(
    /* [in] */ const String& packageName,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    return mPM->GetInstallerPackageName(packageName, name);
}

ECode ApplicationPackageManager::DeletePackage(
    /* [in] */ const String& packageName,
    /* [in] */ IIPackageDeleteObserver* observer,
    /* [in] */ Int32 flags)
{
    return mPM->DeletePackageAsUser(packageName, observer, UserHandle::GetMyUserId(), flags);
}

ECode ApplicationPackageManager::ClearApplicationUserData(
    /* [in] */ const String& packageName,
    /* [in] */ IIPackageDataObserver* observer)
{
    Int32 uid = 0;
    mContext->GetUserId(&uid);
    return mPM->ClearApplicationUserData(packageName, observer, uid);
}

ECode ApplicationPackageManager::DeleteApplicationCacheFiles(
    /* [in] */ const String& packageName,
    /* [in] */ IIPackageDataObserver* observer)
{
    return mPM->DeleteApplicationCacheFiles(packageName, observer);
}

ECode ApplicationPackageManager::FreeStorageAndNotify(
    /* [in] */ Int64 freeStorageSize,
    /* [in] */ IIPackageDataObserver* observer)
{
    return mPM->FreeStorageAndNotify(freeStorageSize, observer);
}

ECode ApplicationPackageManager::FreeStorage(
    /* [in] */ Int64 freeStorageSize,
    /* [in] */ IIntentSender* pi)
{
    return mPM->FreeStorage(freeStorageSize, pi);
}

ECode ApplicationPackageManager::GetPackageSizeInfo(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userHandle,
    /* [in] */ IIPackageStatsObserver* observer)
{
    return mPM->GetPackageSizeInfo(packageName, userHandle, observer);
}

ECode ApplicationPackageManager::AddPackageToPreferred(
    /* [in] */ const String& packageName)
{
    return mPM->AddPackageToPreferred(packageName);
}

ECode ApplicationPackageManager::RemovePackageFromPreferred(
    /* [in] */ const String& packageName)
{
    return mPM->RemovePackageFromPreferred(packageName);
}

ECode ApplicationPackageManager::GetPreferredPackages(
    /* [in] */ Int32 flags,
    /* [out] */ IList** packages)
{
    VALIDATE_NOT_NULL(packages);
    ECode ec = mPM->GetPreferredPackages(flags, packages);
    if (FAILED(ec)) {
        CArrayList::New(packages);
    }

    return NOERROR;
}

ECode ApplicationPackageManager::AddPreferredActivity(
    /* [in] */ IIntentFilter* filter,
    /* [in] */ Int32 match,
    /* [in] */ ArrayOf<IComponentName*>* set,
    /* [in] */ IComponentName* activity)
{
    Int32 uid = 0;
    mContext->GetUserId(&uid);
    return mPM->AddPreferredActivity(filter, match, set, activity, uid);
}

ECode ApplicationPackageManager::AddPreferredActivity(
    /* [in] */ IIntentFilter* filter,
    /* [in] */ Int32 match,
    /* [in] */ ArrayOf<IComponentName*>* set,
    /* [in] */ IComponentName* activity,
    /* [in] */ Int32 userId)
{
    return mPM->AddPreferredActivity(filter, match, set, activity, userId);
}

ECode ApplicationPackageManager::ReplacePreferredActivity(
    /* [in] */ IIntentFilter* filter,
    /* [in] */ Int32 match,
    /* [in] */ ArrayOf<IComponentName*>* set,
    /* [in] */ IComponentName* activity)
{
    return mPM->ReplacePreferredActivity(filter, match, set, activity, UserHandle::GetMyUserId());
}

ECode ApplicationPackageManager::ReplacePreferredActivityAsUser(
    /* [in] */ IIntentFilter* filter,
    /* [in] */ Int32 match,
    /* [in] */ ArrayOf<IComponentName*>* set,
    /* [in] */ IComponentName* activity,
    /* [in] */ Int32 userId)
{
    // try {
    return mPM->ReplacePreferredActivity(filter, match, set, activity, userId);
    // } catch (RemoteException e) {
    //     // Should never happen!
    // }
}

ECode ApplicationPackageManager::ClearPackagePreferredActivities(
    /* [in] */ const String& packageName)
{
    return mPM->ClearPackagePreferredActivities(packageName);
}

ECode ApplicationPackageManager::GetPreferredActivities(
    /* [in] */ IList* outFilters,
    /* [in] */ IList* outActivities,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    ECode ec = mPM->GetPreferredActivities(outFilters, outActivities, packageName, num);
    if (FAILED(ec)) {
        *num = 0;
    }

    return NOERROR;
}

ECode ApplicationPackageManager::GetHomeActivities(
    /* [in] */ IList* /*<ResolveInfo>*/ outActivities,
    /* [out] */ IComponentName** cn)
{
    // try {
    return mPM->GetHomeActivities(outActivities, cn);
    // } catch (RemoteException e) {
    //     // Should never happen!
    // }
    // return NULL;
}

ECode ApplicationPackageManager::SetComponentEnabledSetting(
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 newState,
    /* [in] */ Int32 flags)
{
    Int32 uid = 0;
    mContext->GetUserId(&uid);
    return mPM->SetComponentEnabledSetting(componentName, newState, flags, uid);
}

ECode ApplicationPackageManager::GetComponentEnabledSetting(
    /* [in] */ IComponentName* componentName,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    Int32 uid = 0;
    mContext->GetUserId(&uid);
    ECode ec = mPM->GetComponentEnabledSetting(componentName, uid, value);
    if (FAILED(ec)) {
        *value = IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT;
    }

    return NOERROR;
}

ECode ApplicationPackageManager::SetApplicationEnabledSetting(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 newState,
    /* [in] */ Int32 flags)
{
    Int32 uid = 0;
    mContext->GetUserId(&uid);
    String pkgName;
    mContext->GetOpPackageName(&pkgName);
    return mPM->SetApplicationEnabledSetting(
        packageName, newState, flags, uid, pkgName);
}

ECode ApplicationPackageManager::GetApplicationEnabledSetting(
    /* [in] */ const String& packageName,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    Int32 uid = 0;
    mContext->GetUserId(&uid);
    ECode ec = mPM->GetApplicationEnabledSetting(packageName, uid, value);
    if (FAILED(ec)) {
        *value = IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT;
    }

    return NOERROR;
}

ECode ApplicationPackageManager::SetApplicationHiddenSettingAsUser(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean hidden,
    /* [in] */ IUserHandle* user,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 userId;
    user->GetIdentifier(&userId);

    return mPM->SetApplicationHiddenSettingAsUser(packageName, hidden,
        userId, result);
}

ECode ApplicationPackageManager::GetApplicationHiddenSettingAsUser(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 userId;
    user->GetIdentifier(&userId);
    return mPM->GetApplicationHiddenSettingAsUser(packageName, userId, result);
}

ECode ApplicationPackageManager::GetKeySetByAlias(
    /* [in] */ const String& packageName,
    /* [in] */ const String& alias,
    /* [out] */ IKeySet** keySet)
{
    VALIDATE_NOT_NULL(keySet)
    *keySet = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(packageName))
    FAIL_RETURN(Preconditions::CheckNotNull(alias))

    return mPM->GetKeySetByAlias(packageName, alias, keySet);
}

ECode ApplicationPackageManager::GetSigningKeySet(
    /* [in] */ const String& packageName,
    /* [out] */ IKeySet** keySet)
{
    VALIDATE_NOT_NULL(keySet)
    *keySet = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(packageName))
    return mPM->GetSigningKeySet(packageName, keySet);
}

ECode ApplicationPackageManager::IsSignedBy(
    /* [in] */ const String& packageName,
    /* [in] */ IKeySet* ks,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    FAIL_RETURN(Preconditions::CheckNotNull(packageName))
    FAIL_RETURN(Preconditions::CheckNotNull(ks))

    return mPM->IsPackageSignedByKeySet(packageName, ks, result);
}

ECode ApplicationPackageManager::IsSignedByExactly(
    /* [in] */ const String& packageName,
    /* [in] */ IKeySet* ks,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    FAIL_RETURN(Preconditions::CheckNotNull(packageName))
    FAIL_RETURN(Preconditions::CheckNotNull(ks))

    return mPM->IsPackageSignedByKeySetExactly(packageName, ks, result);
}

ECode ApplicationPackageManager::GetVerifierDeviceIdentity(
    /* [out] */ IVerifierDeviceIdentity** identity)
{
    VALIDATE_NOT_NULL(identity);
    ECode ec = mPM->GetVerifierDeviceIdentity(identity);
    if (FAILED(ec)) {
        *identity = NULL;
    }

    return NOERROR;
}

ECode ApplicationPackageManager::SetComponentProtectedSetting(
    /* [in] */ IComponentName* componentName,
    /* [in] */ Boolean newState)
{
    return E_NOT_IMPLEMENTED;
}

ECode ApplicationPackageManager::GetPackageInstaller(
    /* [out] */ IPackageInstaller** installer)
{
    VALIDATE_NOT_NULL(installer)
    *installer = NULL;

    synchronized(mLock) {
        if (mInstaller == NULL) {
            // try {
            AutoPtr<IIPackageInstaller> pi;
            ECode ec = mPM->GetPackageInstaller((IIPackageInstaller**)&pi);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                return E_RUNTIME_EXCEPTION;
            }

            Int32 userId;
            mContext->GetUserId(&userId);
            String pkgName;
            mContext->GetPackageName(&pkgName);
            CPackageInstaller::New(mContext, this, pi,
                pkgName, userId, (IPackageInstaller**)&mInstaller);

            // } catch (RemoteException e) {
            //     throw e.rethrowAsRuntimeException();
            // }
        }
        *installer = mInstaller;
        REFCOUNT_ADD(*installer)
    }
    return NOERROR;
}

ECode ApplicationPackageManager::IsPackageAvailable(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    // try {
    Int32 userId;
    mContext->GetUserId(&userId);
    ECode ec = mPM->IsPackageAvailable(packageName, userId, result);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        return E_RUNTIME_EXCEPTION;
    }
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
    return ec;
}

ECode ApplicationPackageManager::AddCrossProfileIntentFilter(
    /* [in] */ IIntentFilter* filter,
    /* [in] */ Int32 sourceUserId,
    /* [in] */ Int32 targetUserId,
    /* [in] */ Int32 flags)
{
    // try {
    String pkgName;
    mContext->GetOpPackageName(&pkgName);
    Int32 userId;
    mContext->GetUserId(&userId);
    return mPM->AddCrossProfileIntentFilter(filter,pkgName,
        userId, sourceUserId, targetUserId, flags);
    // } catch (RemoteException e) {
    //     // Should never happen!
    // }
}

ECode ApplicationPackageManager::ClearCrossProfileIntentFilters(
    /* [in] */ Int32 sourceUserId)
{
    // try {
    String pkgName;
    mContext->GetOpPackageName(&pkgName);
    Int32 userId;
    mContext->GetUserId(&userId);
    return mPM->ClearCrossProfileIntentFilters(sourceUserId, pkgName, userId);
    // } catch (RemoteException e) {
    //     // Should never happen!
    // }
}

ECode ApplicationPackageManager::LoadItemIcon(
    /* [in] */ IPackageItemInfo* pii,
    /* [in] */ IApplicationInfo* appInfo,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    *drawable = NULL;

    PackageItemInfo* itemInfo = (PackageItemInfo*)pii;
    if (itemInfo->mShowUserIcon != IUserHandle::USER_NULL) {
        AutoPtr<IBitmap> bitmap;
        GetUserManager()->GetUserIcon(itemInfo->mShowUserIcon, (IBitmap**)&bitmap);
        if (bitmap == NULL) {
            return UserIcons::GetDefaultUserIcon(itemInfo->mShowUserIcon, /* light= */ FALSE, drawable);
        }

        return CBitmapDrawable::New(bitmap, drawable);
    }

    AutoPtr<IDrawable> dr;
    if (!itemInfo->mPackageName.IsNull()) {
        GetDrawable(itemInfo->mPackageName, itemInfo->mIcon, appInfo, (IDrawable**)&dr);
    }
    if (dr == NULL) {
        itemInfo->LoadDefaultIcon((IPackageManager*)this, (IDrawable**)&dr);
    }
    Int32 userId;
    mContext->GetUserId(&userId);
    AutoPtr<IUserHandle> user;
    CUserHandle::New(userId, (IUserHandle**)&user);
    return GetUserBadgedIcon(dr, user, drawable);
}

AutoPtr<IDrawable> ApplicationPackageManager::GetBadgedDrawable(
    /* [in] */ IDrawable* drawable,
    /* [in] */ IDrawable* badgeDrawable,
    /* [in] */ IRect* badgeLocation,
    /* [in] */ Boolean tryBadgeInPlace)
{
    Int32 badgedWidth;
    drawable->GetIntrinsicWidth(&badgedWidth);
    Int32 badgedHeight;
    drawable->GetIntrinsicHeight(&badgedHeight);
    Boolean canBadgeInPlace = tryBadgeInPlace;
    IBitmapDrawable* bd = IBitmapDrawable::Probe(drawable);
    if (!canBadgeInPlace && bd) {
        AutoPtr<IBitmap> bmp;
        bd->GetBitmap((IBitmap**)&bmp);
        bmp->IsMutable(&canBadgeInPlace);
    }

    AutoPtr<IBitmap> bitmap;
    if (canBadgeInPlace) {
        bd->GetBitmap((IBitmap**)&bitmap);
    }
    else {
        CBitmap::CreateBitmap(badgedWidth, badgedHeight, BitmapConfig_ARGB_8888, (IBitmap**)&bitmap);
    }
    AutoPtr<ICanvas> canvas;
    CCanvas::New(bitmap, (ICanvas**)&canvas);

    if (!canBadgeInPlace) {
        drawable->SetBounds(0, 0, badgedWidth, badgedHeight);
        drawable->Draw(canvas);
    }

    if (badgeLocation != NULL) {
        Int32 l, t, w, h;
        badgeLocation->GetLeft(&l);
        badgeLocation->GetTop(&t);
        badgeLocation->GetWidth(&w);
        badgeLocation->GetHeight(&h);
        if (l < 0 || t < 0
                || w > badgedWidth || h > badgedHeight) {
            // throw new IllegalArgumentException("Badge location " + badgeLocation
            //         + " not in badged drawable bounds "
            //         + new Rect(0, 0, badgedWidth, badgedHeight));
            // return E_ILLEGAL_ARGUMENT_EXCEPTION;
            return NULL;
        }
        badgeDrawable->SetBounds(0, 0, w, h);

        Int32 ival;
        canvas->Save(&ival);
        canvas->Translate(l, t);
        badgeDrawable->Draw(canvas);
        canvas->Restore();
    }
    else {
        badgeDrawable->SetBounds(0, 0, badgedWidth, badgedHeight);
        badgeDrawable->Draw(canvas);
    }

    if (!canBadgeInPlace) {
        AutoPtr<IBitmapDrawable> mergedDrawable;
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        CBitmapDrawable::New(res, bitmap, (IBitmapDrawable**)&mergedDrawable);

        IBitmapDrawable* bitmapDrawable = IBitmapDrawable::Probe(drawable);
        if (bitmapDrawable) {
            AutoPtr<IBitmap> tmp;
            bitmapDrawable->GetBitmap((IBitmap**)&tmp);
            Int32 density;
            tmp->GetDensity(&density);
            mergedDrawable->SetTargetDensity(density);
        }

        return IDrawable::Probe(mergedDrawable);
    }

    return drawable;
}

Int32 ApplicationPackageManager::GetBadgeResIdForUser(
    /* [in] */ Int32 userHandle)
{
    // Return the framework-provided badge.
    AutoPtr<IUserInfo> userInfo = GetUserIfProfile(userHandle);
    Boolean bval;
    if (userInfo != NULL && (userInfo->IsManagedProfile(&bval), bval)) {
        return R::drawable::ic_corp_icon_badge;
    }
    return 0;
}

AutoPtr<IUserInfo> ApplicationPackageManager::GetUserIfProfile(
    /* [in] */ Int32 userHandle)
{
    AutoPtr<IList> userProfiles;
    GetUserManager()->GetProfiles(UserHandle::GetMyUserId(), (IList**)&userProfiles);
    AutoPtr<IIterator> it;
    userProfiles->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), &hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IUserInfo* user = IUserInfo::Probe(obj);
        Int32 id;
        user->GetId(&id);
        if (id == userHandle) {
            return user;
        }
    }
    return NULL;
}

ECode ApplicationPackageManager::UpdateIconMaps(
    /* [in] */ const String& pkgName)
{
    return E_NOT_IMPLEMENTED;
}

ECode ApplicationPackageManager::ProcessThemeResources(
    /* [in] */ const String& themePkgName,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
}

//==========================================================================
// ApplicationPackageManager::ResourceName
//==========================================================================

ApplicationPackageManager::ResourceName::ResourceName(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 iconId)
    : mPackageName(packageName)
    , mIconId(iconId)
{}

ApplicationPackageManager::ResourceName::ResourceName(
    /* [in] */ IApplicationInfo* aInfo,
    /* [in] */ Int32 iconId)
{
    String pkgName;
    IPackageItemInfo::Probe(aInfo)->GetPackageName(&pkgName);
    ResourceName(pkgName, iconId);
}

ApplicationPackageManager::ResourceName::ResourceName(
    /* [in] */ IComponentInfo* cInfo,
    /* [in] */ Int32 iconId)
{
    AutoPtr<IApplicationInfo> appInfo;
    cInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
    String pkgName;
    IPackageItemInfo::Probe(appInfo)->GetPackageName(&pkgName);
    ResourceName(pkgName, iconId);
}

ApplicationPackageManager::ResourceName::ResourceName(
    /* [in] */ IResolveInfo* rInfo,
    /* [in] */ Int32 iconId)
{
    AutoPtr<IActivityInfo> acInfo;
    rInfo->GetActivityInfo((IActivityInfo**)&acInfo);
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(acInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    String pkgName;
    IPackageItemInfo::Probe(appInfo)->GetPackageName(&pkgName);
    ResourceName(pkgName, iconId);
}

Boolean ApplicationPackageManager::ResourceName::Equals(
    /* [in] */ ResourceName* o)
{
    if (this == o) return TRUE;

    if (mIconId != o->mIconId) return FALSE;
    return !(!mPackageName.IsNull() ?
        !mPackageName.Equals(o->mPackageName) : !o->mPackageName.IsNull());

}

ECode ApplicationPackageManager::ResourceName::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    Int32 result;
    result = mPackageName.GetHashCode();
    result = 31 * result + mIconId;
    *hash = result;
    return NOERROR;
}

ECode ApplicationPackageManager::ResourceName::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("{ResourceName ");
    sb += mPackageName;
    sb += " / ";
    sb += mIconId;
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
