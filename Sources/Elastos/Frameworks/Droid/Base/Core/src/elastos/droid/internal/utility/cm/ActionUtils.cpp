#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CActivityOptions.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/internal/utility/cm/ActionUtils.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CActivityOptions;
using Elastos::Droid::App::IActivityManagerRunningAppProcessInfo;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Process;
using Elastos::Droid::R;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

const Boolean ActionUtils::DEBUG = FALSE;
const String ActionUtils::TAG("ActionUtils"); // = ActionUtils.class.getSimpleName();
const String ActionUtils::SYSTEMUI_PACKAGE("Elastos.Droid.SystemUI");
const String ActionUtils::DEFAULT_HOME_PACKAGE("Elastos.Droid.Launcher2");

ECode ActionUtils::KillForegroundApp(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    *result = KillForegroundAppInternal(context, userId);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Could not kill foreground app");
    // }
    return NOERROR;
}

ECode ActionUtils::SwitchToLastApp(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    *result = SwitchToLastAppInternal(context, userId);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Could not switch to last app");
    // }
    return NOERROR;
}

Boolean ActionUtils::KillForegroundAppInternal(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId)
{
    Logger::I(TAG, "### KillForegroundAppInternal");
    // try {
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    intent->AddCategory(IIntent::CATEGORY_HOME);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IResolveInfo> res;
    pm->ResolveActivity(intent, 0, (IResolveInfo**)&res);
    String homePackage = DEFAULT_HOME_PACKAGE;
    AutoPtr<IActivityInfo> ai;
    res->GetActivityInfo((IActivityInfo**)&ai);
    if (ai != NULL) {
        String pkgName;
        IPackageItemInfo::Probe(ai)->GetPackageName(&pkgName);
        if (!pkgName.Equals("android")) {
            homePackage = pkgName;
        }
    }

    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    AutoPtr<IList> apps;
    am->GetRunningAppProcesses((IList**)&apps);
    Int32 size;
    apps->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        apps->Get(i, (IInterface**)&obj);
        AutoPtr<IActivityManagerRunningAppProcessInfo> appInfo =
                IActivityManagerRunningAppProcessInfo::Probe(obj);

        Int32 uid;
        appInfo->GetUid(&uid);
        // Make sure it's a foreground user application (not system,
        // root, phone, etc.)
        Int32 importance;
        appInfo->GetImportance(&importance);
        if (uid >= IProcess::FIRST_APPLICATION_UID && uid <= IProcess::LAST_APPLICATION_UID
                && importance == IActivityManagerRunningAppProcessInfo::IMPORTANCE_FOREGROUND) {
            AutoPtr<ArrayOf<String> > pkgList;
            appInfo->GetPkgList((ArrayOf<String>**)&pkgList);
            if (pkgList != NULL) {
                if (pkgList->GetLength() > 0) {
                    for (Int32 i = 0; i < pkgList->GetLength(); i++) {
                        String pkg = (*pkgList)[i];
                        if (!pkg.Equals(SYSTEMUI_PACKAGE) && !pkg.Equals(homePackage)) {
                            am->ForceStopPackage(pkg, IUserHandle::USER_CURRENT);
                            return TRUE;
                        }
                    }
                }
            }
            else {
                Int32 pid;
                appInfo->GetPid(&pid);
                Process::KillProcess(pid);
                return TRUE;
            }
        }
    }
    // } catch (RemoteException remoteException) {
    //     // Do nothing; just let it go.
    // }
    return FALSE;
}

Boolean ActionUtils::SwitchToLastAppInternal(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId)
{
    AutoPtr<IActivityManagerRecentTaskInfo> lastTask = GetLastTask(context, userId);

    Int32 id;
    lastTask->GetId(&id);
    if (lastTask == NULL || id < 0) {
        return FALSE;
    }

    AutoPtr<IIntent> intent;
    lastTask->GetBaseIntent((IIntent**)&intent);
    AutoPtr<IComponentName> component;
    intent->GetComponent((IComponentName**)&component);
    String packageName;
    component->GetPackageName(&packageName);

    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    AutoPtr<IActivityOptions> opts;
    CActivityOptions::MakeCustomAnimation(context,
            R::anim::last_app_in, R::anim::last_app_out, (IActivityOptions**)&opts);

    if (DEBUG) Logger::D(TAG, "switching to %s", packageName.string());
    AutoPtr<IBundle> bundle;
    opts->ToBundle((IBundle**)&bundle);
    am->MoveTaskToFront(id, IActivityManager::MOVE_TASK_NO_USER_ACTION, bundle);

    return TRUE;
}

AutoPtr<IActivityManagerRecentTaskInfo> ActionUtils::GetLastTask(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId)
{
    String defaultHomePackage = ResolveCurrentLauncherPackage(context, userId);
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    AutoPtr<IList> tasks;
    am->GetRecentTasks(5, IActivityManager::RECENT_IGNORE_UNAVAILABLE, userId, (IList**)&tasks);

    Int32 size;
    tasks->GetSize(&size);
    for (Int32 i = 1; i < size; i++) {
        AutoPtr<IInterface> obj;
        tasks->Get(i, (IInterface**)&obj);
        AutoPtr<IActivityManagerRecentTaskInfo> task = IActivityManagerRecentTaskInfo::Probe(obj);
        AutoPtr<IComponentName> origActivity;
        task->GetOrigActivity((IComponentName**)&origActivity);
        AutoPtr<IIntent> intent;
        task->GetBaseIntent((IIntent**)&intent);
        if (origActivity != NULL) {
            intent->SetComponent(origActivity);
        }
        AutoPtr<IComponentName> cn;
        intent->GetComponent((IComponentName**)&cn);
        String packageName;
        cn->GetPackageName(&packageName);
        if (!packageName.Equals(defaultHomePackage)
                && !packageName.Equals(SYSTEMUI_PACKAGE)) {
            AutoPtr<IInterface> obj;
            tasks->Get(i, (IInterface**)&obj);
            return IActivityManagerRecentTaskInfo::Probe(obj);
        }
    }

    return NULL;
}

String ActionUtils::ResolveCurrentLauncherPackage(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId)
{
    AutoPtr<IIntent> launcherIntent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&launcherIntent);
    launcherIntent->AddCategory(IIntent::CATEGORY_HOME);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IResolveInfo> launcherInfo;
    pm->ResolveActivityAsUser(launcherIntent, 0, userId, (IResolveInfo**)&launcherInfo);
    AutoPtr<IActivityInfo> ai;
    launcherInfo->GetActivityInfo((IActivityInfo**)&ai);
    String pkgName;
    IPackageItemInfo::Probe(ai)->GetPackageName(&pkgName);
    return pkgName;
}

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos
