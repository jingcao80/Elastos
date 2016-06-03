
#include "Elastos.Droid.Commands.Pm.h"
#include "Pm.h"
#include "CClearCacheObserver.h"
#include "CClearDataObserver.h"
#include "CLocalIntentSender.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Libcore.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringToIntegral.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::CIntentSender;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IFeatureInfo;
using Elastos::Droid::Content::Pm::IInstrumentationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageInstaller;
using Elastos::Droid::Content::Pm::IPackageInstallerSessionInfo;
using Elastos::Droid::Content::Pm::IPackageInstallerSessionParams;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageManagerHelper;
using Elastos::Droid::Content::Pm::CPackageManagerHelper;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Content::Pm::IPermissionGroupInfo;
using Elastos::Droid::Content::Pm::IPermissionInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::CVerificationParams;
using Elastos::Droid::Content::Pm::IVerificationParams;
using Elastos::Droid::Content::Pm::EIID_IIPackageDataObserver;
using Elastos::Droid::Content::Res::CAssetManager;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Internal::Content::IPackageHelper;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::ISizedInputStream;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringToIntegral;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::Concurrent::CSynchronousQueue;
using Elastos::Utility::Concurrent::IBlockingQueue;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Pm {

static AutoPtr<IPrintStream> InitPrintStream(
    /* [in] */ Int32 no)
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    static AutoPtr<IPrintStream> stream;
    if (no == 1) {
        system->GetOut((IPrintStream**)&stream);
    }
    else if (no == 2) {
        system->GetErr((IPrintStream**)&stream);
    }
    return stream;
}

AutoPtr<IPrintStream> Pm::System::Out = InitPrintStream(1);
AutoPtr<IPrintStream> Pm::System::Err = InitPrintStream(2);

ECode Pm::LocalPackageInstallObserver::OnPackageInstalled(
    /* [in] */ const String& name,
    /* [in] */ Int32 status,
    /* [in] */ const String& msg,
    /* [in] */ IBundle* extras)
{
    AutoLock lock(this);
    mFinished = TRUE;
    mResult = status;
    if (status == IPackageManager::INSTALL_FAILED_DUPLICATE_PERMISSION) {
        extras->GetString(
            IPackageManager::EXTRA_FAILURE_EXISTING_PERMISSION, &mExtraPermission);
        extras->GetString(
            IPackageManager::EXTRA_FAILURE_EXISTING_PACKAGE, &mExtraPackage);
    }
    NotifyAll();
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(Pm::ClearDataObserver, Object, IIPackageDataObserver, IBinder)

ECode Pm::ClearDataObserver::OnRemoveCompleted(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean succeeded)
{
    AutoLock lock(this);
    mFinished = TRUE;
    mResult = succeeded;
    NotifyAll();
    return NOERROR;
}

ECode Pm::ClearDataObserver::ToString(
    /* [in] */ String* str)
{
    return Object::ToString(str);
}

CAR_INTERFACE_IMPL_2(Pm::ClearCacheObserver, Object, IIPackageDataObserver, IBinder)

ECode Pm::ClearCacheObserver::OnRemoveCompleted(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean succeeded)
{
    AutoLock lock(this);
    mFinished = TRUE;
    mResult = succeeded;
    NotifyAll();
    return NOERROR;
}

ECode Pm::ClearCacheObserver::ToString(
    /* [in] */ String* str)
{
    return Object::ToString(str);
}

Pm::LocalIntentReceiver::LocalIntentReceiver()
{
    CSynchronousQueue::New((ISynchronousQueue**)&mResult);
    CLocalIntentSender::New(mResult, (IIIntentSender**)&mLocalSender);
}

AutoPtr<IIntentSender> Pm::LocalIntentReceiver::GetIntentSender()
{
    AutoPtr<IIntentSender> sender;
    CIntentSender::New(mLocalSender, (IIntentSender**)&sender);
    return sender;
}

AutoPtr<IIntent> Pm::LocalIntentReceiver::GetResult()
{
    AutoPtr<IInterface> intent;
    IBlockingQueue::Probe(mResult)->Take((IInterface**)&intent);
    return IIntent::Probe(intent);
}

const String Pm::TAG("PM");
const String Pm::PM_NOT_RUNNING_ERR("Error: Could not access the Package Manager.  Is the system running?");

CAR_INTERFACE_IMPL(Pm, Singleton, IPm);

Pm::Pm()
{}

ECode Pm::Main(
    /* [in] */ const ArrayOf<String>& args)
{
    ECode ec = Run(&const_cast<ArrayOf<String>&>(args));
    if (FAILED(ec)) {
        String e;
        e.AppendFormat("Error: 0x%08x", ec);
        Logger::E(TAG, e);
        System::Err->Println(e);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            System::Err->Println(PM_NOT_RUNNING_ERR);
        }
    }
    return NOERROR;
}

ECode Pm::Run(
    /* [in]*/ ArrayOf<String>* args)
{
    Boolean validCommand = FALSE;
    if (args->GetLength() < 1) {
        ShowUsage();
        return NOERROR;
    }

    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> service;
    sm->GetService(String("user"), (IInterface**)&service);
    mUm = IIUserManager::Probe(service);
    service = NULL;
    sm->GetService(String("package"), (IInterface**)&service);
    mPm = IIPackageManager::Probe(service);
    if (mPm == NULL) {
        System::Err->Println(PM_NOT_RUNNING_ERR);
        return NOERROR;
    }
    mPm->GetPackageInstaller((IIPackageInstaller**)&mInstaller);

    mArgs = args;
    String op = (*args)[0];
    mNextArg = 1;

    if (op.Equals("list")) {
        RunList();
        return NOERROR;
    }

    if (op.Equals("path")) {
        RunPath();
        return NOERROR;
    }

    if (op.Equals("dump")) {
        RunDump();
        return NOERROR;
    }

    if (op.Equals("install")) {
        return RunInstall();
    }

    if (op.Equals("install-create")) {
        return RunInstallCreate();
    }

    if (op.Equals("install-write")) {
        return RunInstallWrite();
    }

    if (op.Equals("install-commit")) {
        return RunInstallCommit();
    }

    if (op.Equals("install-abandon") || op.Equals("install-destroy")) {
        return RunInstallAbandon();
    }

    if (op.Equals("set-installer")) {
        return RunSetInstaller();
    }

    if (op.Equals("uninstall")) {
        return RunUninstall();
    }

    if (op.Equals("clear")) {
        RunClear();
        return NOERROR;
    }

    if (op.Equals("enable")) {
        RunSetEnabledSetting(IPackageManager::COMPONENT_ENABLED_STATE_ENABLED);
        return NOERROR;
    }

    if (op.Equals("disable")) {
        RunSetEnabledSetting(IPackageManager::COMPONENT_ENABLED_STATE_DISABLED);
        return NOERROR;
    }

    if (op.Equals("disable-user")) {
        RunSetEnabledSetting(IPackageManager::COMPONENT_ENABLED_STATE_DISABLED_USER);
        return NOERROR;
    }

    if (op.Equals("disable-until-used")) {
        RunSetEnabledSetting(IPackageManager::COMPONENT_ENABLED_STATE_DISABLED_UNTIL_USED);
        return NOERROR;
    }

    if (op.Equals("hide")) {
        RunSetHiddenSetting(TRUE);
        return NOERROR;
    }

    if (op.Equals("unhide")) {
        RunSetHiddenSetting(FALSE);
        return NOERROR;
    }

    if (op.Equals("grant")) {
        RunGrantRevokePermission(TRUE);
        return NOERROR;
    }

    if (op.Equals("revoke")) {
        RunGrantRevokePermission(FALSE);
        return NOERROR;
    }

    if (op.Equals("set-permission-enforced")) {
        RunSetPermissionEnforced();
        return NOERROR;
    }

    if (op.Equals("set-install-location")) {
        RunSetInstallLocation();
        return NOERROR;
    }

    if (op.Equals("get-install-location")) {
        RunGetInstallLocation();
        return NOERROR;
    }

    if (op.Equals("trim-caches")) {
        RunTrimCaches();
        return NOERROR;
    }

    if (op.Equals("create-user")) {
        RunCreateUser();
        return NOERROR;
    }

    if (op.Equals("remove-user")) {
        RunRemoveUser();
        return NOERROR;
    }

    if (op.Equals("get-max-users")) {
        RunGetMaxUsers();
        return NOERROR;
    }

    if (op.Equals("force-dex-opt")) {
        return RunForceDexOpt();
    }

    // try {
        if (mArgs->GetLength() == 1) {
            if ((*mArgs)[0].EqualsIgnoreCase("-l")) {
                validCommand = TRUE;
                RunListPackages(FALSE);
            }
            else if ((*mArgs)[0].EqualsIgnoreCase("-lf")){
                validCommand = TRUE;
                RunListPackages(TRUE);
            }
        }
        else if (mArgs->GetLength() == 2) {
            if ((*mArgs)[0].EqualsIgnoreCase("-p")) {
                validCommand = TRUE;
                DisplayPackageFilePath((*mArgs)[1]);
            }
        }
    // } finally {
        if (validCommand == FALSE) {
            if (op != NULL) {
                System::Err->Println(String("Error: unknown command '") + op + "'");
            }
            ShowUsage();
        }
    // }
    return NOERROR;
}

/**
 * Execute the list sub-command.
 *
 * pm list [package | packages]
 * pm list permission-groups
 * pm list permissions
 * pm list features
 * pm list libraries
 * pm list instrumentation
 */
void Pm::RunList()
{
    String type = NextArg();
    if (type == NULL) {
        System::Err->Println(String("Error: didn't specify type of data to list"));
        return;
    }
    if (type.Equals("package") || type.Equals("packages")) {
        RunListPackages(FALSE);
    }
    else if (type.Equals("permission-groups")) {
        RunListPermissionGroups();
    }
    else if (type.Equals("permissions")) {
        RunListPermissions();
    }
    else if (type.Equals("features")) {
        RunListFeatures();
    }
    else if (type.Equals("libraries")) {
        RunListLibraries();
    }
    else if (type.Equals("instrumentation")) {
        RunListInstrumentation();
    }
    else if (type.Equals("users")) {
        RunListUsers();
    }
    else {
        System::Err->Println(String("Error: unknown list type '") + type + "'");
    }
}

/**
 * Lists all the installed packages.
 */
void Pm::RunListPackages(
    /* [in] */ Boolean showApplicationPackage)
{
    System::Err->Println(String("Pm::RunListPackages is not implemented!"));
    // Int32 getFlags = 0;
    // Boolean listDisabled = FALSE, listEnabled = FALSE;
    // Boolean listSystem = FALSE, listThirdParty = FALSE;
    // Boolean listInstaller = FALSE;
    // Int32 userId = IUserHandle::USER_OWNER;
    // try {
    //     String opt;
    //     while ((opt=NextOption()) != NULL) {
    //         if (opt.Equals("-l")) {
    //             // old compat
    //         }
    //         else if (opt.Equals("-lf")) {
    //             showApplicationPackage = TRUE;
    //         }
    //         else if (opt.Equals("-f")) {
    //             showApplicationPackage = TRUE;
    //         }
    //         else if (opt.Equals("-d")) {
    //             listDisabled = TRUE;
    //         }
    //         else if (opt.Equals("-e")) {
    //             listEnabled = TRUE;
    //         }
    //         else if (opt.Equals("-s")) {
    //             listSystem = TRUE;
    //         }
    //         else if (opt.Equals("-3")) {
    //             listThirdParty = TRUE;
    //         }
    //         else if (opt.Equals("-i")) {
    //             listInstaller = TRUE;
    //         }
    //         else if (opt.Equals("--user")) {
    //             userId = Integer.parseInt(NextArg());
    //         }
    //         else if (opt.Equals("-u")) {
    //             getFlags |= IPackageManager::GET_UNINSTALLED_PACKAGES;
    //         }
    //         else {
    //             System::Err->Println(String("Error: Unknown option: ") + opt);
    //             return;
    //         }
    //     }
    // } catch (RuntimeException ex) {
    //     System::Err->Println(String("Error: ") + ex.toString());
    //     return;
    // }

    // String filter = NextArg();

    // try {
    //     final List<PackageInfo> packages = getInstalledPackages(mPm, getFlags, userId);

    //     Int32 count = packages.size();
    //     for (Int32 p = 0 ; p < count ; p++) {
    //         PackageInfo info = packages.get(p);
    //         if (filter != NULL && !info.packageName.contains(filter)) {
    //             continue;
    //         }
    //         final Boolean isSystem =
    //                 (info.applicationInfo.flags&ApplicationInfo.FLAG_SYSTEM) != 0;
    //         if ((!listDisabled || !info.applicationInfo.enabled) &&
    //                 (!listEnabled || info.applicationInfo.enabled) &&
    //                 (!listSystem || isSystem) &&
    //                 (!listThirdParty || !isSystem)) {
    //             System::Out->Print("package:");
    //             if (showApplicationPackage) {
    //                 System::Out->Print(info.applicationInfo.sourceDir);
    //                 System::Out->Print("=");
    //             }
    //             System::Out->Print(info.packageName);
    //             if (listInstaller) {
    //                 System::Out->Print("  installer=");
    //                 System::Out->Print(mPm.getInstallerPackageName(info.packageName));
    //             }
    //             System::Out->Println();
    //         }
    //     }
    // } catch (RemoteException e) {
    //     System::Err->Println(e.toString());
    //     System::Err->Println(PM_NOT_RUNNING_ERR);
    // }
}

// @SuppressWarnings("unchecked")
ECode Pm::GetInstalledPackages(
    /* [in] */ IIPackageManager* pm,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IList** packages)
{
    VALIDATE_NOT_NULL(packages)
    *packages = NULL;
    AutoPtr<IParceledListSlice> slice;
    FAIL_RETURN(pm->GetInstalledPackages(flags, userId, (IParceledListSlice**)&slice))
    return slice->GetList(packages);
}

/**
 * Lists all of the features supported by the current device.
 *
 * pm list features
 */
void Pm::RunListFeatures()
{
    System::Err->Println(String("Pm::RunListFeatures is not implemented!"));
    // try {
    //     List<FeatureInfo> list = new ArrayList<FeatureInfo>();
    //     FeatureInfo[] rawList = mPm.getSystemAvailableFeatures();
    //     for (Int32 i=0; i<rawList.length; i++) {
    //         list.add(rawList[i]);
    //     }


    //     // Sort by name
    //     Collections.sort(list, new Comparator<FeatureInfo>()
    //     {
    //         public Int32 compare(FeatureInfo o1, FeatureInfo o2) {
    //             if (o1.name == o2.name) return 0;
    //             if (o1.name == NULL) return -1;
    //             if (o2.name == NULL) return 1;
    //             return o1.name.compareTo(o2.name);
    //         }
    //     });

    //     Int32 count = (list != NULL) ? list.size() : 0;
    //     for (Int32 p = 0; p < count; p++) {
    //         FeatureInfo fi = list.get(p);
    //         System::Out->Print("feature:");
    //         if (fi.name != NULL) System::Out->Println(fi.name);
    //         else System::Out->Println(String("reqGlEsVersion=0x)"
    //                 + Integer.toHexString(fi.reqGlEsVersion));
    //     }
    // } catch (RemoteException e) {
    //     System::Err->Println(e.toString());
    //     System::Err->Println(PM_NOT_RUNNING_ERR);
    // }
}

/**
 * Lists all of the libraries supported by the current device.
 *
 * pm list libraries
 */
void Pm::RunListLibraries()
{
    System::Err->Println(String("Pm::RunListLibraries is not implemented!"));
    // try {
    //     List<String> list = new ArrayList<String>();
    //     String[] rawList = mPm.getSystemSharedLibraryNames();
    //     for (Int32 i=0; i<rawList.length; i++) {
    //         list.add(rawList[i]);
    //     }


    //     // Sort by name
    //     Collections.sort(list, new Comparator<String>()
    //     {
    //         public Int32 compare(String o1, String o2) {
    //             if (o1 == o2) return 0;
    //             if (o1 == NULL) return -1;
    //             if (o2 == NULL) return 1;
    //             return o1.compareTo(o2);
    //         }
    //     });

    //     Int32 count = (list != NULL) ? list.size() : 0;
    //     for (Int32 p = 0; p < count; p++) {
    //         String lib = list.get(p);
    //         System::Out->Print("library:");
    //         System::Out->Println(lib);
    //     }
    // } catch (RemoteException e) {
    //     System::Err->Println(e.toString());
    //     System::Err->Println(PM_NOT_RUNNING_ERR);
    // }
}

/**
 * Lists all of the installed instrumentation, or all for a given package
 *
 * pm list instrumentation [package] [-f]
 */
void Pm::RunListInstrumentation()
{
    System::Err->Println(String("Pm::RunListInstrumentation is not implemented!"));
    // Int32 flags = 0;      // flags != 0 is only used to request meta-data
    // Boolean showPackage = FALSE;
    // String targetPackage = NULL;

    // try {
    //     String opt;
    //     while ((opt=NextArg()) != NULL) {
    //         if (opt.Equals("-f")) {
    //             showPackage = TRUE;
    //         }
    //         else if (opt.charAt(0) != '-') {
    //             targetPackage = opt;
    //         }
    //         else {
    //             System::Err->Println(String("Error: Unknown option: ") + opt);
    //             return;
    //         }
    //     }
    // } catch (RuntimeException ex) {
    //     System::Err->Println(String("Error: ") + ex.toString());
    //     return;
    // }

    // try {
    //     List<InstrumentationInfo> list = mPm.queryInstrumentation(targetPackage, flags);

    //     // Sort by target package
    //     Collections.sort(list, new Comparator<InstrumentationInfo>()
    //     {
    //         public Int32 compare(InstrumentationInfo o1, InstrumentationInfo o2) {
    //             return o1.targetPackage.compareTo(o2.targetPackage);
    //         }
    //     });

    //     Int32 count = (list != NULL) ? list.size() : 0;
    //     for (Int32 p = 0; p < count; p++) {
    //         InstrumentationInfo ii = list.get(p);
    //         System::Out->Print("instrumentation:");
    //         if (showPackage) {
    //             System::Out->Print(ii.sourceDir);
    //             System::Out->Print("=");
    //         }
    //         ComponentName cn = new ComponentName(ii.packageName, ii.name);
    //         System::Out->Print(cn.flattenToShortString());
    //         System::Out->Print(" (target=");
    //         System::Out->Print(ii.targetPackage);
    //         System::Out->Println(String(")"));
    //     }
    // } catch (RemoteException e) {
    //     System::Err->Println(e.toString());
    //     System::Err->Println(PM_NOT_RUNNING_ERR);
    // }
}

/**
 * Lists all the known permission groups.
 */
void Pm::RunListPermissionGroups()
{
    System::Err->Println(String("Pm::RunListPermissionGroups is not implemented!"));
    // try {
    //     List<PermissionGroupInfo> pgs = mPm.getAllPermissionGroups(0);

    //     Int32 count = pgs.size();
    //     for (Int32 p = 0 ; p < count ; p++) {
    //         PermissionGroupInfo pgi = pgs.get(p);
    //         System::Out->Print("permission group:");
    //         System::Out->Println(pgi.name);
    //     }
    // } catch (RemoteException e) {
    //     System::Err->Println(e.toString());
    //     System::Err->Println(PM_NOT_RUNNING_ERR);
    // }
}

String Pm::LoadText(
    /* [in] */ IPackageItemInfo* pii,
    /* [in] */ Int32 res,
    /* [in] */ ICharSequence* nonLocalized)
{
    String text;
    if (nonLocalized != NULL) {
        nonLocalized->ToString(&text);
        return text;
    }
    if (res != 0) {
        AutoPtr<IResources> r = GetResources(pii);
        if (r != NULL) {
            r->GetString(res, &text);
        }
    }
    return String(NULL);
}

/**
 * Lists all the permissions in a group.
 */
void Pm::RunListPermissions()
{
    System::Err->Println(String("Pm::RunListPermissions is not implemented!"));
    // try {
    //     Boolean labels = FALSE;
    //     Boolean groups = FALSE;
    //     Boolean userOnly = FALSE;
    //     Boolean summary = FALSE;
    //     Boolean dangerousOnly = FALSE;
    //     String opt;
    //     while ((opt=NextOption()) != NULL) {
    //         if (opt.Equals("-f")) {
    //             labels = TRUE;
    //         }
    //         else if (opt.Equals("-g")) {
    //             groups = TRUE;
    //         }
    //         else if (opt.Equals("-s")) {
    //             groups = TRUE;
    //             labels = TRUE;
    //             summary = TRUE;
    //         }
    //         else if (opt.Equals("-u")) {
    //             userOnly = TRUE;
    //         }
    //         else if (opt.Equals("-d")) {
    //             dangerousOnly = TRUE;
    //         }
    //         else {
    //             System::Err->Println(String("Error: Unknown option: ") + opt);
    //             return;
    //         }
    //     }

    //     String grp = NextOption();
    //     ArrayList<String> groupList = new ArrayList<String>();
    //     if (groups) {
    //         List<PermissionGroupInfo> infos =
    //                 mPm.getAllPermissionGroups(0);
    //         for (Int32 i=0; i<infos.size(); i++) {
    //             groupList.add(infos.get(i).name);
    //         }
    //         groupList.add(NULL);
    //     }
    //     else {
    //         groupList.add(grp);
    //     }

    //     if (dangerousOnly) {
    //         System::Out->Println(String("Dangerous Permissions:"));
    //         System::Out->Println(String(""));
    //         doListPermissions(groupList, groups, labels, summary,
    //                 PermissionInfo.PROTECTION_DANGEROUS,
    //                 PermissionInfo.PROTECTION_DANGEROUS);
    //         if (userOnly) {
    //             System::Out->Println(String("Normal Permissions:"));
    //             System::Out->Println(String(""));
    //             doListPermissions(groupList, groups, labels, summary,
    //                     PermissionInfo.PROTECTION_NORMAL,
    //                     PermissionInfo.PROTECTION_NORMAL);
    //         }
    //     }
    //     else if (userOnly) {
    //         System::Out->Println(String("Dangerous and Normal Permissions:"));
    //         System::Out->Println(String(""));
    //         doListPermissions(groupList, groups, labels, summary,
    //                 PermissionInfo.PROTECTION_NORMAL,
    //                 PermissionInfo.PROTECTION_DANGEROUS);
    //     }
    //     else {
    //         System::Out->Println(String("All Permissions:"));
    //         System::Out->Println(String(""));
    //         doListPermissions(groupList, groups, labels, summary,
    //                 -10000, 10000);
    //     }
    // } catch (RemoteException e) {
    //     System::Err->Println(e.toString());
    //     System::Err->Println(PM_NOT_RUNNING_ERR);
    // }
}

ECode Pm::DoListPermissions(
    /* [in] */ ArrayOf<String>* groupList,
    /* [in] */ Boolean groups,
    /* [in] */ Boolean labels,
    /* [in] */ Boolean summary,
    /* [in] */ Int32 startProtectionLevel,
    /* [in] */ Int32 endProtectionLevel)
{
    System::Err->Println(String("Pm::DoListPermissions is not implemented!"));
    // for (Int32 i=0; i<groupList.size(); i++) {
    //     String groupName = groupList.get(i);
    //     String prefix = "";
    //     if (groups) {
    //         if (i > 0) System::Out->Println(String(""));
    //         if (groupName != NULL) {
    //             PermissionGroupInfo pgi = mPm.getPermissionGroupInfo(
    //                     groupName, 0);
    //             if (summary) {
    //                 Resources res = getResources(pgi);
    //                 if (res != NULL) {
    //                     System::Out->Print(loadText(pgi, pgi.labelRes,
    //                             pgi.nonLocalizedLabel) + ": ");
    //                 }
    //                 else {
    //                     System::Out->Print(pgi.name + ": ");

    //                 }
    //             }
    //             else {
    //                 System::Out->Println((labels ? "+ " : "")
    //                         + "group:" + pgi.name);
    //                 if (labels) {
    //                     System::Out->Println(String("  package:") + pgi.packageName);
    //                     Resources res = getResources(pgi);
    //                     if (res != NULL) {
    //                         System::Out->Println(String("  label:)"
    //                                 + loadText(pgi, pgi.labelRes,
    //                                         pgi.nonLocalizedLabel));
    //                         System::Out->Println(String("  description:)"
    //                                 + loadText(pgi, pgi.descriptionRes,
    //                                         pgi.nonLocalizedDescription));
    //                     }
    //                 }
    //             }
    //         }
    //         else {
    //             System::Out->Println(((labels && !summary)
    //                     ? "+ " : "") + "ungrouped:");
    //         }
    //         prefix = "  ";
    //     }
    //     List<PermissionInfo> ps = mPm.queryPermissionsByGroup(
    //             groupList.get(i), 0);
    //     Int32 count = ps.size();
    //     Boolean first = TRUE;
    //     for (Int32 p = 0 ; p < count ; p++) {
    //         PermissionInfo pi = ps.get(p);
    //         if (groups && groupName == NULL && pi.group != NULL) {
    //             continue;
    //         }
    //         final Int32 base = pi.protectionLevel & PermissionInfo.PROTECTION_MASK_BASE;
    //         if (base < startProtectionLevel
    //                 || base > endProtectionLevel) {
    //             continue;
    //         }
    //         if (summary) {
    //             if (first) {
    //                 first = FALSE;
    //             }
    //             else {
    //                 System::Out->Print(", ");
    //             }
    //             Resources res = getResources(pi);
    //             if (res != NULL) {
    //                 System::Out->Print(loadText(pi, pi.labelRes,
    //                         pi.nonLocalizedLabel));
    //             }
    //             else {
    //                 System::Out->Print(pi.name);
    //             }
    //         }
    //         else {
    //             System::Out->Println(prefix + (labels ? "+ " : "")
    //                     + "permission:" + pi.name);
    //             if (labels) {
    //                 System::Out->Println(prefix + "  package:" + pi.packageName);
    //                 Resources res = getResources(pi);
    //                 if (res != NULL) {
    //                     System::Out->Println(prefix + "  label:"
    //                             + loadText(pi, pi.labelRes,
    //                                     pi.nonLocalizedLabel));
    //                     System::Out->Println(prefix + "  description:"
    //                             + loadText(pi, pi.descriptionRes,
    //                                     pi.nonLocalizedDescription));
    //                 }
    //                 System::Out->Println(prefix + "  protectionLevel:"
    //                         + PermissionInfo.protectionToString(pi.protectionLevel));
    //             }
    //         }
    //     }

    //     if (summary) {
    //         System::Out->Println(String(""));
    //     }
    // }
    return NOERROR;
}

void Pm::RunPath()
{
    String pkg = NextArg();
    if (pkg == NULL) {
        System::Err->Println(String("Error: no package specified"));
        return;
    }
    DisplayPackageFilePath(pkg);
}

void Pm::RunDump()
{
    String pkg = NextArg();
    if (pkg == NULL) {
        System::Err->Println(String("Error: no package specified"));
        return;
    }
    // ActivityManager->DumpPackageStateStatic(FileDescriptor.out, pkg);
}


/**
 * Converts a failure code into a string by using reflection to find a matching constant
 * in IPackageManager::
 */
String Pm::InstallFailureToString(
    /* [in] */ LocalPackageInstallObserver* obs)
{
    Int32 result = obs->mResult;

    AutoPtr<IPackageManagerHelper> pmHelper;
    CPackageManagerHelper::AcquireSingleton((IPackageManagerHelper**)&pmHelper);
    String strResult;
    pmHelper->InstallStatusToString(result, &strResult);

    if (strResult.StartWith("INSTALL")) {
        StringBuilder sb(64);
        sb.Append(strResult);
        if (obs->mExtraPermission != NULL) {
            sb.Append(" perm=");
            sb.Append(obs->mExtraPermission);
        }
        if (obs->mExtraPackage != NULL) {
            sb.Append(" pkg=");
            sb.Append(obs->mExtraPackage);
        }
        return sb.ToString();
    }

    // couldn't find a matching constant? return the value
    return strResult;
}

void Pm::RunSetInstallLocation()
{
    Int32 loc;

    String arg = NextArg();
    if (arg == NULL) {
        System::Err->Println(String("Error: no install location specified."));
        return;
    }
    if (FAILED(StringToIntegral::Parse(arg, &loc))) {
        System::Err->Println(String("Error: install location has to be a number."));
        return;
    }

    Boolean res;
    if (FAILED(mPm->SetInstallLocation(loc, &res))) {
        // System::Err->Println(e.toString());
        System::Err->Println(PM_NOT_RUNNING_ERR);
    }
    else if (!res) {
        System::Err->Println(String("Error: install location has to be a number."));
    }
}

void Pm::RunGetInstallLocation()
{
    Int32 loc;
    if (FAILED(mPm->GetInstallLocation(&loc))) {
        // System::Err->Println(e.toString());
        System::Err->Println(PM_NOT_RUNNING_ERR);
        return;
    }
    String locStr("invalid");
    if (loc == IPackageHelper::APP_INSTALL_AUTO) {
        locStr = "auto";
    }
    else if (loc == IPackageHelper::APP_INSTALL_INTERNAL) {
        locStr = "internal";
    }
    else if (loc == IPackageHelper::APP_INSTALL_EXTERNAL) {
        locStr = "external";
    }
    System::Out->Print(loc);
    System::Out->Print(String("["));
    System::Out->Print(locStr);
    System::Out->Println(String("]"));
}

ECode Pm::RunInstall()
{
    Int32 installFlags = 0;
    Int32 userId = IUserHandle::USER_ALL;
    String installerPackageName;

    String opt;

    String originatingUriString;
    String referrer;
    String abi;

    while ((opt=NextOption()) != NULL) {
        if (opt.Equals("-l")) {
            installFlags |= IPackageManager::INSTALL_FORWARD_LOCK;
        }
        else if (opt.Equals("-r")) {
            installFlags |= IPackageManager::INSTALL_REPLACE_EXISTING;
        }
        else if (opt.Equals("-i")) {
            installerPackageName = NextOptionData();
            if (installerPackageName == NULL) {
                System::Err->Println(String("Error: no value specified for -i"));
                return NOERROR;
            }
        }
        else if (opt.Equals("-t")) {
            installFlags |= IPackageManager::INSTALL_ALLOW_TEST;
        }
        else if (opt.Equals("-s")) {
            // Override if -s option is specified.
            installFlags |= IPackageManager::INSTALL_EXTERNAL;
        }
        else if (opt.Equals("-f")) {
            // Override if -s option is specified.
            installFlags |= IPackageManager::INSTALL_INTERNAL;
        }
        else if (opt.Equals("-d")) {
            installFlags |= IPackageManager::INSTALL_ALLOW_DOWNGRADE;
        }
        else if (opt.Equals("--originating-uri")) {
            originatingUriString = NextOptionData();
            if (originatingUriString == NULL) {
                System::Err->Println(String("Error: must supply argument for --originating-uri"));
                return NOERROR;
            }
        }
        else if (opt.Equals("--referrer")) {
            referrer = NextOptionData();
            if (referrer == NULL) {
                System::Err->Println(String("Error: must supply argument for --referrer"));
                return NOERROR;
            }
        }
        else if (opt.Equals("--abi")) {
             FAIL_RETURN(CheckAbiArgument(NextOptionData(), &abi))
        }
        else if (opt.Equals("--user")) {
            userId = StringUtils::ParseInt32(NextOptionData());
        }
        else {
            System::Err->Println(String("Error: Unknown option: ") + opt);
            return NOERROR;
        }
    }

    if (userId == IUserHandle::USER_ALL) {
        userId = IUserHandle::USER_OWNER;
        installFlags |= IPackageManager::INSTALL_ALL_USERS;
    }

    AutoPtr<IUri> verificationURI;
    AutoPtr<IUri> originatingURI;
    AutoPtr<IUri> referrerURI;

    if (originatingUriString != NULL) {
        Uri::Parse(originatingUriString, (IUri**)&originatingURI);
    }
    else {
        originatingURI = NULL;
    }

    if (referrer != NULL) {
        Uri::Parse(referrer, (IUri**)&referrerURI);
    }
    else {
        referrerURI = NULL;
    }

    // Populate apkURI, must be present
    String apkFilePath = NextArg();
    System::Err->Println(String("\tpkg: ") + apkFilePath);
    if (apkFilePath == NULL) {
        System::Err->Println(String("Error: no package specified"));
        return NOERROR;
    }

    // Populate verificationURI, optionally present
    String verificationFilePath = NextArg();
    if (verificationFilePath != NULL) {
        System::Err->Println(String("\tver: ") + verificationFilePath);
        AutoPtr<IFile> file;
        CFile::New(verificationFilePath, (IFile**)&file);
        Uri::FromFile(file, (IUri**)&verificationURI);
    }
    else {
        verificationURI = NULL;
    }

    AutoPtr<LocalPackageInstallObserver> obs = new LocalPackageInstallObserver();
    AutoPtr<IVerificationParams> verificationParams;
    CVerificationParams::New(verificationURI, originatingURI, referrerURI,
        IVerificationParams::NO_UID, NULL, (IVerificationParams**)&verificationParams);

    AutoPtr<IIPackageInstallObserver2> pio;
    obs->GetBinder((IIPackageInstallObserver2**)&pio);
    if (FAILED(mPm->InstallPackageAsUser(apkFilePath, pio, installFlags,
        installerPackageName, verificationParams, abi, userId))) {
        // System::Err->Println(e.toString());
        System::Err->Println(PM_NOT_RUNNING_ERR);
        return NOERROR;
    }

    AutoLock lock(obs);
    while (!obs->mFinished) {
        obs->Wait();
    }
    if (obs->mResult == IPackageManager::INSTALL_SUCCEEDED) {
        System::Out->Println(String("Success"));
    }
    else {
        System::Err->Println(String("Failure [") + InstallFailureToString(obs) + "]");
    }
    return NOERROR;
}

ECode Pm::RunInstallCreate()
{
    System::Err->Println(String("Pm::RunInstallCreate is not implemented!"));
    // Int32 userId = IUserHandle::USER_ALL;
    // String installerPackageName = NULL;

    // final SessionParams params = new SessionParams(SessionParams.MODE_FULL_INSTALL);

    // String opt;
    // while ((opt = NextOption()) != NULL) {
    //     if (opt.Equals("-l")) {
    //         params.installFlags |= IPackageManager::INSTALL_FORWARD_LOCK;
    //     }
    //     else if (opt.Equals("-r")) {
    //         params.installFlags |= IPackageManager::INSTALL_REPLACE_EXISTING;
    //     }
    //     else if (opt.Equals("-i")) {
    //         installerPackageName = NextArg();
    //         if (installerPackageName == NULL) {
    //             throw new IllegalArgumentException("Missing installer package");
    //         }
    //     }
    //     else if (opt.Equals("-t")) {
    //         params.installFlags |= IPackageManager::INSTALL_ALLOW_TEST;
    //     }
    //     else if (opt.Equals("-s")) {
    //         params.installFlags |= IPackageManager::INSTALL_EXTERNAL;
    //     }
    //     else if (opt.Equals("-f")) {
    //         params.installFlags |= IPackageManager::INSTALL_INTERNAL;
    //     }
    //     else if (opt.Equals("-d")) {
    //         params.installFlags |= IPackageManager::INSTALL_ALLOW_DOWNGRADE;
    //     }
    //     else if (opt.Equals("--originating-uri")) {
    //         params.originatingUri = Uri.parse(NextOptionData());
    //     }
    //     else if (opt.Equals("--referrer")) {
    //         params.referrerUri = Uri.parse(NextOptionData());
    //     }
    //     else if (opt.Equals("-p")) {
    //         params.mode = SessionParams.MODE_INHERIT_EXISTING;
    //         params.appPackageName = NextOptionData();
    //         if (params.appPackageName == NULL) {
    //             throw new IllegalArgumentException("Missing inherit package name");
    //         }
    //     }
    //     else if (opt.Equals("-S")) {
    //         params.setSize(Long.parseLong(NextOptionData()));
    //     }
    //     else if (opt.Equals("--abi")) {
    //         params.abiOverride = checkAbiArgument(NextOptionData());
    //     }
    //     else if (opt.Equals("--user")) {
    //         userId = Integer.parseInt(NextOptionData());
    //     }
    //     else {
    //         throw new IllegalArgumentException("Unknown option " + opt);
    //     }
    // }

    // if (userId == IUserHandle::USER_ALL) {
    //     userId = IUserHandle::USER_OWNER;
    //     params.installFlags |= IPackageManager::INSTALL_ALL_USERS;
    // }

    // final Int32 sessionId = mInstaller.createSession(params, installerPackageName, userId);

    // // NOTE: adb depends on parsing this string
    // System::Out->Println(String("Success: created install session [") + sessionId + "]");
    return NOERROR;
}

ECode Pm::RunInstallWrite()
{
    System::Err->Println(String("Pm::RunInstallWrite is not implemented!"));
    // long sizeBytes = -1;

    // String opt;
    // while ((opt = NextOption()) != NULL) {
    //     if (opt.Equals("-S")) {
    //         sizeBytes = Long.parseLong(NextOptionData());
    //     }
    //     else {
    //         throw new IllegalArgumentException("Unknown option: " + opt);
    //     }
    // }

    // final Int32 sessionId = Integer.parseInt(NextArg());
    // final String splitName = NextArg();

    // String path = NextArg();
    // if ("-".Equals(path)) {
    //     path = NULL;
    // }
    // else if (path != NULL) {
    //     final File file = new File(path);
    //     if (file.isFile()) {
    //         sizeBytes = file.length();
    //     }
    // }

    // final SessionInfo info = mInstaller.getSessionInfo(sessionId);

    // PackageInstaller.Session session = NULL;
    // InputStream in = NULL;
    // OutputStream out = NULL;
    // try {
    //     session = new PackageInstaller.Session(mInstaller.openSession(sessionId));

    //     if (path != NULL) {
    //         in = new FileInputStream(path);
    //     }
    //     else {
    //         in = new SizedInputStream(System.in, sizeBytes);
    //     }
    //     out = session.openWrite(splitName, 0, sizeBytes);

    //     Int32 total = 0;
    //     byte[] buffer = new byte[65536];
    //     Int32 c;
    //     while ((c = in.read(buffer)) != -1) {
    //         total += c;
    //         out.write(buffer, 0, c);

    //         if (info.sizeBytes > 0) {
    //             final float fraction = ((float) c / (float) info.sizeBytes);
    //             session.addProgress(fraction);
    //         }
    //     }
    //     session.fsync(out);

    //     System::Out->Println(String("Success: streamed ") + total + " bytes");
    // } finally {
    //     IoUtils.closeQuietly(out);
    //     IoUtils.closeQuietly(in);
    //     IoUtils.closeQuietly(session);
    // }
    return NOERROR;
}

ECode Pm::RunInstallCommit()
{
    System::Err->Println(String("Pm::RunInstallCommit is not implemented!"));
    // final Int32 sessionId = Integer.parseInt(NextArg());

    // PackageInstaller.Session session = NULL;
    // try {
    //     session = new PackageInstaller.Session(mInstaller.openSession(sessionId));

    //     final LocalIntentReceiver receiver = new LocalIntentReceiver();
    //     session.commit(receiver.getIntentSender());

    //     final Intent result = receiver.getResult();
    //     final Int32 status = result.getIntExtra(PackageInstaller.EXTRA_STATUS,
    //             PackageInstaller.STATUS_FAILURE);
    //     if (status == PackageInstaller.STATUS_SUCCESS) {
    //         System::Out->Println(String("Success"));
    //     }
    //     else {
    //         Log.e(TAG, "Failure details: " + result.getExtras());
    //         System::Out->Println(String("Failure ["
    //                 + result.getStringExtra(PackageInstaller.EXTRA_STATUS_MESSAGE) + "]");
    //         return;
    //     }
    // } finally {
    //     IoUtils.closeQuietly(session);
    // }
    return NOERROR;
}

ECode Pm::RunInstallAbandon()
{
    System::Err->Println(String("Pm::RunInstallAbandon is not implemented!"));
    // final Int32 sessionId = Integer.parseInt(NextArg());

    // PackageInstaller.Session session = NULL;
    // try {
    //     session = new PackageInstaller.Session(mInstaller.openSession(sessionId));
    //     session.abandon();
    //     System::Out->Println(String("Success"));
    // } finally {
    //     IoUtils.closeQuietly(session);
    // }
    return NOERROR;
}

ECode Pm::RunSetInstaller()
{
    String targetPackage = NextArg();
    String installerPackageName = NextArg();

    if (targetPackage == NULL || installerPackageName == NULL) {
        Logger::D(TAG, "must provide both target and installer package names");
        System::Err->Println(String("must provide both target and installer package names"));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(mPm->SetInstallerPackageName(targetPackage, installerPackageName))
    System::Out->Println(String("Success"));
    return NOERROR;
}

void Pm::RunCreateUser()
{
    System::Err->Println(String("Pm::RunCreateUser is not implemented!"));
    // String name;
    // Int32 userId = -1;
    // Int32 flags = 0;
    // String opt;
    // while ((opt = NextOption()) != NULL) {
    //     if ("--profileOf".Equals(opt)) {
    //         String optionData = NextOptionData();
    //         if (optionData == NULL || !isNumber(optionData)) {
    //             System::Err->Println(String("Error: no USER_ID specified"));
    //             ShowUsage();
    //             return;
    //         }
    //         else {
    //             userId = Integer.parseInt(optionData);
    //         }
    //     }
    //     else if ("--managed".Equals(opt)) {
    //         flags |= UserInfo.FLAG_MANAGED_PROFILE;
    //     }
    //     else {
    //         System::Err->Println(String("Error: unknown option ") + opt);
    //         ShowUsage();
    //         return;
    //     }
    // }
    // String arg = NextArg();
    // if (arg == NULL) {
    //     System::Err->Println(String("Error: no user name specified."));
    //     return;
    // }
    // name = arg;
    // try {
    //     UserInfo info = NULL;
    //     if (userId < 0) {
    //         info = mUm.createUser(name, flags);
    //     }
    //     else {
    //         info = mUm.createProfileForUser(name, flags, userId);
    //     }
    //     if (info != NULL) {
    //         System::Out->Println(String("Success: created user id ") + info.id);
    //     }
    //     else {
    //         System::Err->Println(String("Error: couldn't create User."));
    //     }
    // } catch (RemoteException e) {
    //     System::Err->Println(e.toString());
    //     System::Err->Println(PM_NOT_RUNNING_ERR);
    // }
}

void Pm::RunRemoveUser()
{
    System::Err->Println(String("Pm::RunRemoveUser is not implemented!"));
    // Int32 userId;
    // String arg = NextArg();
    // if (arg == NULL) {
    //     System::Err->Println(String("Error: no user id specified."));
    //     return;
    // }
    // try {
    //     userId = Integer.parseInt(arg);
    // } catch (NumberFormatException e) {
    //     System::Err->Println(String("Error: user id '") + arg + "' is not a number.");
    //     return;
    // }
    // try {
    //     if (mUm.removeUser(userId)) {
    //         System::Out->Println(String("Success: removed user"));
    //     }
    //     else {
    //         System::Err->Println(String("Error: couldn't remove user id ") + userId);
    //     }
    // } catch (RemoteException e) {
    //     System::Err->Println(e.toString());
    //     System::Err->Println(PM_NOT_RUNNING_ERR);
    // }
}

void Pm::RunListUsers()
{
    System::Err->Println(String("Pm::RunListUsers is not implemented!"));
    // try {
    //     IActivityManager am = ActivityManagerNative.getDefault();

    //     List<UserInfo> users = mUm.getUsers(FALSE);
    //     if (users == NULL) {
    //         System::Err->Println(String("Error: couldn't get users"));
    //     }
    //     else {
    //         System::Out->Println(String("Users:"));
    //         for (Int32 i = 0; i < users.size(); i++) {
    //             String Running = am.isUserRunning(users.get(i).id, FALSE) ? " Running" : "";
    //             System::Out->Println(String("\t") + users.get(i).toString() + Running);
    //         }
    //     }
    // } catch (RemoteException e) {
    //     System::Err->Println(e.toString());
    //     System::Err->Println(PM_NOT_RUNNING_ERR);
    // }
}

void Pm::RunGetMaxUsers()
{
    AutoPtr<IUserManagerHelper> umHelper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&umHelper);
    Int32 num;
    umHelper->GetMaxSupportedUsers(&num);
    System::Out->Print(String("Maximum supported users: "));
    System::Out->Println(num);
}

ECode Pm::RunForceDexOpt()
{
    String packageName = NextArg();
    if (FAILED(mPm->ForceDexOpt(packageName))) {
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode Pm::RunUninstall()
{
    Int32 flags = 0;
    Int32 userId = IUserHandle::USER_ALL;

    String opt;
    while ((opt=NextOption()) != NULL) {
        if (opt.Equals("-k")) {
            flags |= IPackageManager::DELETE_KEEP_DATA;
        }
        else if (opt.Equals("--user")) {
            String param = NextArg();
            if (IsNumber(param)) {
                userId = StringUtils::ParseInt32(param);
            }
            else {
                ShowUsage();
                System::Err->Println(String("Error: Invalid user: ") + param);
                return NOERROR;
            }
        }
        else {
            System::Err->Println(String("Error: Unknown option: ") + opt);
            return NOERROR;
        }
    }

    String pkg = NextArg();
    if (pkg == NULL) {
        System::Err->Println(String("Error: no package specified"));
        ShowUsage();
        return NOERROR;
    }

    if (userId == IUserHandle::USER_ALL) {
        userId = IUserHandle::USER_OWNER;
        flags |= IPackageManager::DELETE_ALL_USERS;
    }
    else {
        AutoPtr<IPackageInfo> info;
        if (FAILED(mPm->GetPackageInfo(pkg, 0, userId, (IPackageInfo**)&info))) {
            // System::Err->Println(e.toString());
            System::Err->Println(PM_NOT_RUNNING_ERR);
            return NOERROR;
        }
        if (info == NULL) {
            System::Err->Print(String("Failure - not installed for "));
            System::Err->Println(userId);
            return NOERROR;
        }
        AutoPtr<IApplicationInfo> applicationInfo;
        info->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        Int32 flags;
        applicationInfo->GetFlags(&flags);
        Boolean isSystem = (flags & IApplicationInfo::FLAG_SYSTEM) != 0;
        // If we are being asked to delete a system app for just one
        // user set flag so it disables rather than reverting to system
        // version of the app.
        if (isSystem) {
            flags |= IPackageManager::DELETE_SYSTEM_APP;
        }
    }

    AutoPtr<LocalIntentReceiver> receiver = new LocalIntentReceiver();
    FAIL_RETURN(mInstaller->Uninstall(pkg, flags, receiver->GetIntentSender(), userId));

    AutoPtr<IIntent> result = receiver->GetResult();
    Int32 status;
    result->GetInt32Extra(IPackageInstaller::EXTRA_STATUS,
        IPackageInstaller::STATUS_FAILURE, &status);
    if (status == IPackageInstaller::STATUS_SUCCESS) {
        System::Out->Println(String("Success"));
    }
    else {
        AutoPtr<IBundle> extras;
        result->GetExtras((IBundle**)&extras);
        Logger::E(TAG, "Failure details: %s", TO_CSTR(extras));
        String strExtra;
        result->GetStringExtra(IPackageInstaller::EXTRA_STATUS_MESSAGE, &strExtra);
        System::Out->Println(String("Failure [") + strExtra + "]");
    }
    return NOERROR;
}

void Pm::RunClear()
{
    System::Err->Println(String("Pm::RunClear is not implemented!"));
    // Int32 userId = 0;
    // String option = NextOption();
    // if (option != NULL && option.Equals("--user")) {
    //     String optionData = NextOptionData();
    //     if (optionData == NULL || !isNumber(optionData)) {
    //         System::Err->Println(String("Error: no USER_ID specified"));
    //         ShowUsage();
    //         return;
    //     }
    //     else {
    //         userId = Integer.parseInt(optionData);
    //     }
    // }

    // String pkg = NextArg();
    // if (pkg == NULL) {
    //     System::Err->Println(String("Error: no package specified"));
    //     ShowUsage();
    //     return;
    // }

    // ClearDataObserver obs = new ClearDataObserver();
    // try {
    //     ActivityManagerNative.getDefault().clearApplicationUserData(pkg, obs, userId);
    //     synchronized (obs) {
    //         while (!obs.finished) {
    //             try {
    //                 obs.wait();
    //             } catch (InterruptedException e) {
    //             }
    //         }
    //     }

    //     if (obs.result) {
    //         System::Err->Println(String("Success"));
    //     }
    //     else {
    //         System::Err->Println(String("Failed"));
    //     }
    // } catch (RemoteException e) {
    //     System::Err->Println(e.toString());
    //     System::Err->Println(PM_NOT_RUNNING_ERR);
    // }
}

String Pm::EnabledSettingToString(
    /* [in] */ Int32 state)
{
    switch (state) {
        case IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT:
            return String("default");
        case IPackageManager::COMPONENT_ENABLED_STATE_ENABLED:
            return String("enabled");
        case IPackageManager::COMPONENT_ENABLED_STATE_DISABLED:
            return String("disabled");
        case IPackageManager::COMPONENT_ENABLED_STATE_DISABLED_USER:
            return String("disabled-user");
        case IPackageManager::COMPONENT_ENABLED_STATE_DISABLED_UNTIL_USED:
            return String("disabled-until-used");
    }
    return String("unknown");
}

Boolean Pm::IsNumber(
    /* [in] */ const String& s)
{
    Int32 result;
    if (FAILED(StringToIntegral::Parse(s, &result))) {
        return FALSE;
    }
    return TRUE;
}

void Pm::RunSetEnabledSetting(
    /* [in] */ Int32 state)
{
    System::Err->Println(String("Pm::RunSetEnabledSetting is not implemented!"));
    // Int32 userId = 0;
    // String option = NextOption();
    // if (option != NULL && option.Equals("--user")) {
    //     String optionData = NextOptionData();
    //     if (optionData == NULL || !isNumber(optionData)) {
    //         System::Err->Println(String("Error: no USER_ID specified"));
    //         ShowUsage();
    //         return;
    //     }
    //     else {
    //         userId = Integer.parseInt(optionData);
    //     }
    // }

    // String pkg = NextArg();
    // if (pkg == NULL) {
    //     System::Err->Println(String("Error: no package or component specified"));
    //     ShowUsage();
    //     return;
    // }
    // ComponentName cn = ComponentName.unflattenFromString(pkg);
    // if (cn == NULL) {
    //     try {
    //         mPm.setApplicationEnabledSetting(pkg, state, 0, userId,
    //                 "shell:" + android.os.Process.myUid());
    //         System::Err->Println(String("Package ") + pkg + " new state: "
    //                 + enabledSettingToString(
    //                 mPm.getApplicationEnabledSetting(pkg, userId)));
    //     } catch (RemoteException e) {
    //         System::Err->Println(e.toString());
    //         System::Err->Println(PM_NOT_RUNNING_ERR);
    //     }
    // }
    // else {
    //     try {
    //         mPm.setComponentEnabledSetting(cn, state, 0, userId);
    //         System::Err->Println(String("Component ") + cn.toShortString() + " new state: "
    //                 + enabledSettingToString(
    //                 mPm.getComponentEnabledSetting(cn, userId)));
    //     } catch (RemoteException e) {
    //         System::Err->Println(e.toString());
    //         System::Err->Println(PM_NOT_RUNNING_ERR);
    //     }
    // }
}

void Pm::RunSetHiddenSetting(
    /* [in] */ Boolean state)
{
    System::Err->Println(String("Pm::RunSetHiddenSetting is not implemented!"));
    // Int32 userId = 0;
    // String option = NextOption();
    // if (option != NULL && option.Equals("--user")) {
    //     String optionData = NextOptionData();
    //     if (optionData == NULL || !isNumber(optionData)) {
    //         System::Err->Println(String("Error: no USER_ID specified"));
    //         ShowUsage();
    //         return;
    //     }
    //     else {
    //         userId = Integer.parseInt(optionData);
    //     }
    // }

    // String pkg = NextArg();
    // if (pkg == NULL) {
    //     System::Err->Println(String("Error: no package or component specified"));
    //     ShowUsage();
    //     return;
    // }
    // try {
    //     mPm.setApplicationHiddenSettingAsUser(pkg, state, userId);
    //     System::Err->Println(String("Package ") + pkg + " new hidden state: "
    //             + mPm.getApplicationHiddenSettingAsUser(pkg, userId));
    // } catch (RemoteException e) {
    //     System::Err->Println(e.toString());
    //     System::Err->Println(PM_NOT_RUNNING_ERR);
    // }
}

void Pm::RunGrantRevokePermission(
    /* [in] */ Boolean grant)
{
    System::Err->Println(String("Pm::RunGrantRevokePermission is not implemented!"));
    // String pkg = NextArg();
    // if (pkg == NULL) {
    //     System::Err->Println(String("Error: no package specified"));
    //     ShowUsage();
    //     return;
    // }
    // String perm = NextArg();
    // if (perm == NULL) {
    //     System::Err->Println(String("Error: no permission specified"));
    //     ShowUsage();
    //     return;
    // }
    // try {
    //     if (grant) {
    //         mPm.grantPermission(pkg, perm);
    //     }
    //     else {
    //         mPm.revokePermission(pkg, perm);
    //     }
    // } catch (RemoteException e) {
    //     System::Err->Println(e.toString());
    //     System::Err->Println(PM_NOT_RUNNING_ERR);
    // } catch (IllegalArgumentException e) {
    //     System::Err->Println(String("Bad argument: ") + e.toString());
    //     ShowUsage();
    // } catch (SecurityException e) {
    //     System::Err->Println(String("Operation not allowed: ") + e.toString());
    // }
}

void Pm::RunSetPermissionEnforced()
{
    System::Err->Println(String("Pm::RunSetPermissionEnforced is not implemented!"));
    // final String permission = NextArg();
    // if (permission == NULL) {
    //     System::Err->Println(String("Error: no permission specified"));
    //     ShowUsage();
    //     return;
    // }
    // final String enforcedRaw = NextArg();
    // if (enforcedRaw == NULL) {
    //     System::Err->Println(String("Error: no enforcement specified"));
    //     ShowUsage();
    //     return;
    // }
    // final Boolean enforced = Boolean.parseBoolean(enforcedRaw);
    // try {
    //     mPm.setPermissionEnforced(permission, enforced);
    // } catch (RemoteException e) {
    //     System::Err->Println(e.toString());
    //     System::Err->Println(PM_NOT_RUNNING_ERR);
    // } catch (IllegalArgumentException e) {
    //     System::Err->Println(String("Bad argument: ") + e.toString());
    //     ShowUsage();
    // } catch (SecurityException e) {
    //     System::Err->Println(String("Operation not allowed: ") + e.toString());
    // }
}

void Pm::RunTrimCaches()
{
    System::Err->Println(String("Pm::RunTrimCaches is not implemented!"));
    // String size = NextArg();
    // if (size == NULL) {
    //     System::Err->Println(String("Error: no size specified"));
    //     ShowUsage();
    //     return;
    // }
    // Int32 len = size.length();
    // long multiplier = 1;
    // if (len > 1) {
    //     char c = size.charAt(len-1);
    //     if (c == 'K' || c == 'k') {
    //         multiplier = 1024L;
    //     }
    //     else if (c == 'M' || c == 'm') {
    //         multiplier = 1024L*1024L;
    //     }
    //     else if (c == 'G' || c == 'g') {
    //         multiplier = 1024L*1024L*1024L;
    //     }
    //     else {
    //         System::Err->Println(String("Invalid suffix: ") + c);
    //         ShowUsage();
    //         return;
    //     }
    //     size = size.substring(0, len-1);
    // }
    // long sizeVal;
    // try {
    //     sizeVal = Long.parseLong(size) * multiplier;
    // } catch (NumberFormatException e) {
    //     System::Err->Println(String("Error: expected number at: ") + size);
    //     ShowUsage();
    //     return;
    // }
    // ClearDataObserver obs = new ClearDataObserver();
    // try {
    //     mPm.freeStorageAndNotify(sizeVal, obs);
    //     synchronized (obs) {
    //         while (!obs.finished) {
    //             try {
    //                 obs.wait();
    //             } catch (InterruptedException e) {
    //             }
    //         }
    //     }
    // } catch (RemoteException e) {
    //     System::Err->Println(e.toString());
    //     System::Err->Println(PM_NOT_RUNNING_ERR);
    // } catch (IllegalArgumentException e) {
    //     System::Err->Println(String("Bad argument: ") + e.toString());
    //     ShowUsage();
    // } catch (SecurityException e) {
    //     System::Err->Println(String("Operation not allowed: ") + e.toString());
    // }
}

/**
 * Displays the package file for a package.
 * @param pckg
 */
void Pm::DisplayPackageFilePath(
    /* [in] */ const String& pckg)
{
    System::Err->Println(String("Pm::DisplayPackageFilePath is not implemented!"));
    // try {
    //     PackageInfo info = mPm.getPackageInfo(pckg, 0, 0);
    //     if (info != NULL && info.applicationInfo != NULL) {
    //         System::Out->Print("package:");
    //         System::Out->Println(info.applicationInfo.sourceDir);
    //         if (!ArrayUtils.isEmpty(info.applicationInfo.splitSourceDirs)) {
    //             for (String splitSourceDir : info.applicationInfo.splitSourceDirs) {
    //                 System::Out->Print("package:");
    //                 System::Out->Println(splitSourceDir);
    //             }
    //         }
    //     }
    // } catch (RemoteException e) {
    //     System::Err->Println(e.toString());
    //     System::Err->Println(PM_NOT_RUNNING_ERR);
    // }
}

AutoPtr<IResources> Pm::GetResources(
    /* [in] */ IPackageItemInfo* pii)
{
    String packageName;
    pii->GetPackageName(&packageName);
    AutoPtr<IResources> res = mResourceCache[packageName];
    if (res != NULL)
        return res;

    AutoPtr<IApplicationInfo> ai;
    if (FAILED(mPm->GetApplicationInfo(packageName, 0, 0, (IApplicationInfo**)&ai))) {
        // System::Err->Println(e.toString());
        System::Err->Println(PM_NOT_RUNNING_ERR);
        return NULL;
    }
    AutoPtr<IAssetManager> am;
    CAssetManager::New((IAssetManager**)&am);
    String publicSourceDir;
    ai->GetPublicSourceDir(&publicSourceDir);
    Int32 result;
    am->AddAssetPath(publicSourceDir, &result);
    CResources::New(am, NULL, NULL, (IResources**)&res);
    mResourceCache[packageName] = res;
    return res;
}

ECode Pm::CheckAbiArgument(
    /* [in] */ const String& abi,
    /* [out] */ String* outAbi)
{
    VALIDATE_NOT_NULL(outAbi)
    *outAbi = NULL;
    if (TextUtils::IsEmpty(abi)) {
        Logger::E(TAG, "Missing ABI argument");
        System::Err->Println(String("Missing ABI argument"));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (abi.Equals("-")) {
        *outAbi = abi;
        return NOERROR;
    }

    AutoPtr<ArrayOf<String> > supportedAbis = Build::SUPPORTED_ABIS;
    for (Int32 i = 0; i < supportedAbis->GetLength(); i++) {
        String supportedAbi = (*supportedAbis)[i];
        if (supportedAbi.Equals(abi)) {
            *outAbi = abi;
            return NOERROR;
        }
    }

    StringBuilder sb("ABI");
    sb += abi;
    sb += " not supported on this device";
    Logger::E(TAG, sb.ToString());
    System::Err->Println(sb.ToString());
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

String Pm::NextOption()
{
    if (mNextArg >= mArgs->GetLength()) {
        return String(NULL);
    }
    String arg = (*mArgs)[mNextArg];
    if (!arg.StartWith("-")) {
        return String(NULL);
    }
    mNextArg++;
    if (arg.Equals("--")) {
        return String(NULL);
    }
    if (arg.GetLength() > 1 && arg.GetChar(1) != '-') {
        if (arg.GetLength() > 2) {
            mCurArgData = arg.Substring(2);
            return arg.Substring(0, 2);
        }
        else {
            mCurArgData = NULL;
            return arg;
        }
    }
    mCurArgData = NULL;
    return arg;
}

String Pm::NextOptionData()
{
    if (mCurArgData != NULL) {
        return mCurArgData;
    }
    if (mNextArg >= mArgs->GetLength()) {
        return String(NULL);
    }
    String data = (*mArgs)[mNextArg];
    mNextArg++;
    return data;
}

String Pm::NextArg()
{
    if (mNextArg >= mArgs->GetLength()) {
        return String(NULL);
    }
    String arg = (*mArgs)[mNextArg];
    mNextArg++;
    return arg;
}

void Pm::ShowUsage()
{
    System::Err->Println(String("usage: pm list packages [-f] [-d] [-e] [-s] [-3] [-i] [-u] [--user USER_ID] [FILTER]"));
    System::Err->Println(String("       pm list permission-groups"));
    System::Err->Println(String("       pm list permissions [-g] [-f] [-d] [-u] [GROUP]"));
    System::Err->Println(String("       pm list instrumentation [-f] [TARGET-PACKAGE]"));
    System::Err->Println(String("       pm list features"));
    System::Err->Println(String("       pm list libraries"));
    System::Err->Println(String("       pm list users"));
    System::Err->Println(String("       pm path PACKAGE"));
    System::Err->Println(String("       pm dump PACKAGE"));
    System::Err->Println(String("       pm install [-lrtsfd] [-i PACKAGE] [PATH]"));
    System::Err->Println(String("       pm install-create [-lrtsfdp] [-i PACKAGE] [-S BYTES]"));
    System::Err->Println(String("       pm install-write [-S BYTES] SESSION_ID SPLIT_NAME [PATH]"));
    System::Err->Println(String("       pm install-commit SESSION_ID"));
    System::Err->Println(String("       pm install-abandon SESSION_ID"));
    System::Err->Println(String("       pm uninstall [-k] [--user USER_ID] PACKAGE"));
    System::Err->Println(String("       pm set-installer PACKAGE INSTALLER"));
    System::Err->Println(String("       pm clear [--user USER_ID] PACKAGE"));
    System::Err->Println(String("       pm enable [--user USER_ID] PACKAGE_OR_COMPONENT"));
    System::Err->Println(String("       pm disable [--user USER_ID] PACKAGE_OR_COMPONENT"));
    System::Err->Println(String("       pm disable-user [--user USER_ID] PACKAGE_OR_COMPONENT"));
    System::Err->Println(String("       pm disable-until-used [--user USER_ID] PACKAGE_OR_COMPONENT"));
    System::Err->Println(String("       pm hide [--user USER_ID] PACKAGE_OR_COMPONENT"));
    System::Err->Println(String("       pm unhide [--user USER_ID] PACKAGE_OR_COMPONENT"));
    System::Err->Println(String("       pm grant PACKAGE PERMISSION"));
    System::Err->Println(String("       pm revoke PACKAGE PERMISSION"));
    System::Err->Println(String("       pm set-install-location [0/auto] [1/internal] [2/external]"));
    System::Err->Println(String("       pm get-install-location"));
    System::Err->Println(String("       pm set-permission-enforced PERMISSION [TRUE|FALSE]"));
    System::Err->Println(String("       pm trim-caches DESIRED_FREE_SPACE"));
    System::Err->Println(String("       pm create-user [--profileOf USER_ID] [--managed] USER_NAME"));
    System::Err->Println(String("       pm remove-user USER_ID"));
    System::Err->Println(String("       pm get-max-users"));
    System::Err->Println(String(""));
    System::Err->Println(String("pm list packages: prints all packages, optionally only"));
    System::Err->Println(String("  those whose package name contains the text in FILTER.  Options:"));
    System::Err->Println(String("    -f: see their associated file."));
    System::Err->Println(String("    -d: filter to only show disbled packages."));
    System::Err->Println(String("    -e: filter to only show enabled packages."));
    System::Err->Println(String("    -s: filter to only show system packages."));
    System::Err->Println(String("    -3: filter to only show third party packages."));
    System::Err->Println(String("    -i: see the installer for the packages."));
    System::Err->Println(String("    -u: also include uninstalled packages."));
    System::Err->Println(String(""));
    System::Err->Println(String("pm list permission-groups: prints all known permission groups."));
    System::Err->Println(String(""));
    System::Err->Println(String("pm list permissions: prints all known permissions, optionally only"));
    System::Err->Println(String("  those in GROUP.  Options:"));
    System::Err->Println(String("    -g: organize by group."));
    System::Err->Println(String("    -f: print all information."));
    System::Err->Println(String("    -s: short summary."));
    System::Err->Println(String("    -d: only list dangerous permissions."));
    System::Err->Println(String("    -u: list only the permissions users will see."));
    System::Err->Println(String(""));
    System::Err->Println(String("pm list instrumentation: use to list all test packages; optionally"));
    System::Err->Println(String("  supply <TARGET-PACKAGE> to list the test packages for a particular"));
    System::Err->Println(String("  application.  Options:"));
    System::Err->Println(String("    -f: list the .apk file for the test package."));
    System::Err->Println(String(""));
    System::Err->Println(String("pm list features: prints all features of the system."));
    System::Err->Println(String(""));
    System::Err->Println(String("pm list users: prints all users on the system."));
    System::Err->Println(String(""));
    System::Err->Println(String("pm path: print the path to the .apk of the given PACKAGE."));
    System::Err->Println(String(""));
    System::Err->Println(String("pm dump: print system state associated with the given PACKAGE."));
    System::Err->Println(String(""));
    System::Err->Println(String("pm install: install a single legacy package"));
    System::Err->Println(String("pm install-create: create an install session"));
    System::Err->Println(String("    -l: forward lock application"));
    System::Err->Println(String("    -r: replace existing application"));
    System::Err->Println(String("    -t: allow test packages"));
    System::Err->Println(String("    -i: specify the installer package name"));
    System::Err->Println(String("    -s: install application on sdcard"));
    System::Err->Println(String("    -f: install application on internal flash"));
    System::Err->Println(String("    -d: allow version code downgrade"));
    System::Err->Println(String("    -p: partial application install"));
    System::Err->Println(String("    -S: size in bytes of entire session"));
    System::Err->Println(String(""));
    System::Err->Println(String("pm install-write: write a package into existing session; path may"));
    System::Err->Println(String("  be '-' to read from stdin"));
    System::Err->Println(String("    -S: size in bytes of package, required for stdin"));
    System::Err->Println(String(""));
    System::Err->Println(String("pm install-commit: perform install of fully staged session"));
    System::Err->Println(String("pm install-abandon: abandon session"));
    System::Err->Println(String(""));
    System::Err->Println(String("pm set-installer: set installer package name"));
    System::Err->Println(String(""));
    System::Err->Println(String("pm uninstall: removes a package from the system. Options:"));
    System::Err->Println(String("    -k: keep the data and cache directories around after package removal."));
    System::Err->Println(String(""));
    System::Err->Println(String("pm clear: deletes all data associated with a package."));
    System::Err->Println(String(""));
    System::Err->Println(String("pm enable, disable, disable-user, disable-until-used: these commands"));
    System::Err->Println(String("  change the enabled state of a given package or component (written"));
    System::Err->Println(String("  as \"package/class\")."));
    System::Err->Println(String(""));
    System::Err->Println(String("pm grant, revoke: these commands either grant or revoke permissions"));
    System::Err->Println(String("  to applications.  Only optional permissions the application has"));
    System::Err->Println(String("  declared can be granted or revoked."));
    System::Err->Println(String(""));
    System::Err->Println(String("pm get-install-location: returns the current install location."));
    System::Err->Println(String("    0 [auto]: Let system decide the best location"));
    System::Err->Println(String("    1 [internal]: Install on internal device storage"));
    System::Err->Println(String("    2 [external]: Install on external media"));
    System::Err->Println(String(""));
    System::Err->Println(String("pm set-install-location: changes the default install location."));
    System::Err->Println(String("  NOTE: this is only intended for debugging; using this can cause"));
    System::Err->Println(String("  applications to break and other undersireable behavior."));
    System::Err->Println(String("    0 [auto]: Let system decide the best location"));
    System::Err->Println(String("    1 [internal]: Install on internal device storage"));
    System::Err->Println(String("    2 [external]: Install on external media"));
    System::Err->Println(String(""));
    System::Err->Println(String("pm trim-caches: trim cache files to reach the given free space."));
    System::Err->Println(String(""));
    System::Err->Println(String("pm create-user: create a new user with the given USER_NAME,"));
    System::Err->Println(String("  printing the new user identifier of the user."));
    System::Err->Println(String(""));
    System::Err->Println(String("pm remove-user: remove the user with the given USER_IDENTIFIER,"));
    System::Err->Println(String("  deleting all data associated with that user"));
    System::Err->Println(String(""));
}

}
}
}
}

