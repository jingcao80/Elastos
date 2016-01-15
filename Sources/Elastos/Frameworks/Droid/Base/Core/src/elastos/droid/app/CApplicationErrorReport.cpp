
#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/app/CApplicationErrorReport.h"
#include "elastos/droid/app/CApplicationErrorReportCrashInfo.h"
#include "elastos/droid/app/CApplicationErrorReportAnrInfo.h"
#include "elastos/droid/app/CApplicationErrorReportBatteryInfo.h"
#include "elastos/droid/app/CApplicationErrorReportRunningServiceInfo.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CComponentName.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;

namespace Elastos {
namespace Droid {
namespace App {

const String TAG("CApplicationErrorReport");

// System property defining error report receiver for system apps
const String SYSTEM_APPS_ERROR_RECEIVER_PROPERTY("ro.error.receiver.system.apps");

// System property defining default error report receiver
const String DEFAULT_ERROR_RECEIVER_PROPERTY("ro.error.receiver.default");

CAR_INTERFACE_IMPL_2(CApplicationErrorReport, Object, IApplicationErrorReport, IParcelable)

CAR_OBJECT_IMPL(CApplicationErrorReport)

CApplicationErrorReport::CApplicationErrorReport()
    : mType(0)
    , mTime(0)
    , mSystemApp(FALSE)
{
}

/**
 * Create an uninitialized instance of {@link ApplicationErrorReport}.
 */
ECode CApplicationErrorReport::constructor()
{
    return NOERROR;
}

ECode CApplicationErrorReport::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    return NOERROR;
}

ECode CApplicationErrorReport::SetType(
    /* [in] */ Int32 type)
{
    mType = type;
    return NOERROR;
}

ECode CApplicationErrorReport::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = mPackageName;
    return NOERROR;
}

ECode CApplicationErrorReport::SetPackageName(
    /* [in] */ const String& packageName)
{
    mPackageName = packageName;
    return NOERROR;
}

ECode CApplicationErrorReport::GetInstallerPackageName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName);
    *pkgName = mInstallerPackageName;
    return NOERROR;
}

ECode CApplicationErrorReport::SetInstallerPackageName(
    /* [in] */ const String& pkgName)
{
    mInstallerPackageName = pkgName;
    return NOERROR;
}

ECode CApplicationErrorReport::GetProcessName(
    /* [out] */ String* processName)
{
    VALIDATE_NOT_NULL(processName);
    *processName = mProcessName;
    return NOERROR;
}

ECode CApplicationErrorReport::SetProcessName(
    /* [in] */ const String& processName)
{
    mProcessName = processName;
    return NOERROR;
}

ECode CApplicationErrorReport::GetTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mTime;
    return NOERROR;
}

ECode CApplicationErrorReport::SetTime(
    /* [in] */ Int64 time)
{
    mTime = time;
    return NOERROR;
}

ECode CApplicationErrorReport::GetSystemApp(
    /* [out] */ Boolean* systemApp)
{
    VALIDATE_NOT_NULL(systemApp);
    *systemApp = mSystemApp;
    return NOERROR;
}

ECode CApplicationErrorReport::SetSystemApp(
    /* [in] */ Boolean systemApp)
{
    mSystemApp = systemApp;
    return NOERROR;
}

ECode CApplicationErrorReport::GetCrashInfo(
    /* [out] */ IApplicationErrorReportCrashInfo** crashInfo)
{
    VALIDATE_NOT_NULL(crashInfo);
    *crashInfo = mCrashInfo;
    return NOERROR;
}

ECode CApplicationErrorReport::SetCrashInfo(
    /* [in] */ IApplicationErrorReportCrashInfo* crashInfo)
{
    mCrashInfo = crashInfo;
    return NOERROR;
}

ECode CApplicationErrorReport::GetAnrInfo(
    /* [out] */ IApplicationErrorReportAnrInfo** anrInfo)
{
    VALIDATE_NOT_NULL(anrInfo);
    *anrInfo = mAnrInfo;
    return NOERROR;
}

ECode CApplicationErrorReport::SetAnrInfo(
    /* [in] */ IApplicationErrorReportAnrInfo* anrInfo)
{
    mAnrInfo = anrInfo;
    return NOERROR;
}

ECode CApplicationErrorReport::GetBatteryInfo(
    /* [out] */ IApplicationErrorReportBatteryInfo** batteryInfo)
{
    VALIDATE_NOT_NULL(batteryInfo);
    *batteryInfo = mBatteryInfo;
    return NOERROR;
}

ECode CApplicationErrorReport::SetBatteryInfo(
    /* [in] */ IApplicationErrorReportBatteryInfo* batteryInfo)
{
    mBatteryInfo = batteryInfo;
    return NOERROR;
}

ECode CApplicationErrorReport::GetRunningServiceInfo(
    /* [out] */ IApplicationErrorReportRunningServiceInfo** runningServiceInfo)
{
    VALIDATE_NOT_NULL(runningServiceInfo);
    *runningServiceInfo = mRunningServiceInfo;
    return NOERROR;
}

ECode CApplicationErrorReport::SetRunningServiceInfo(
    /* [in] */ IApplicationErrorReportRunningServiceInfo* runningServiceInfo)
{
    mRunningServiceInfo = runningServiceInfo;
    return NOERROR;
}

/**
 * Return activity in receiverPackage that handles ACTION_APP_ERROR.
 *
 * @param pm PackageManager instance
 * @param errorPackage package which caused the error
 * @param receiverPackage candidate package to receive the error
 * @return activity component within receiverPackage which handles
 * ACTION_APP_ERROR, or null if not found
 */
AutoPtr<IComponentName> CApplicationErrorReport::GetErrorReportReceiver(
    /* [in] */ IPackageManager* pm,
    /* [in] */ const String& errorPackage,
    /* [in] */ const String& receiverPackage)
{
    if (pm == NULL) {
        return NULL;
    }

    if (receiverPackage.IsNullOrEmpty()) {
        return NULL;
    }

    // break the loop if it's the error report receiver package that crashed
    if (receiverPackage == errorPackage) {
        return NULL;
    }

    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_APP_ERROR, (IIntent**)&intent);
    intent->SetPackage(receiverPackage);

    AutoPtr<IResolveInfo> info;
    pm->ResolveActivity(intent, 0, (IResolveInfo**)&info);
    if (info == NULL) {
        return NULL;
    }

    AutoPtr<IActivityInfo> activityInfo;
    info->GetActivityInfo((IActivityInfo**)&activityInfo);
    if (activityInfo == NULL) {
        return NULL;
    }

    String activityName;
    IPackageItemInfo::Probe(activityInfo)->GetName(&activityName);

    AutoPtr<IComponentName> compName;
    CComponentName::New(receiverPackage, activityName, (IComponentName**)&compName);
    return compName;
}

AutoPtr<IComponentName> CApplicationErrorReport::GetErrorReportReceiver(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 appFlags)
{
    if (context == NULL) {
        return NULL;
    }

//  TODO
//    // check if error reporting is enabled in secure settings
//    int enabled = Settings.Global.getInt(context.getContentResolver(),
//            Settings.Global.SEND_ACTION_APP_ERROR, 0);
//    if (enabled == 0) {
//        return null;
//    }
//
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);

    // look for receiver in the installer package
    String candidate;
    pm->GetInstallerPackageName(packageName, &candidate);
    AutoPtr<IComponentName> result = GetErrorReportReceiver(pm, packageName, candidate);
    if (result != NULL) {
        return result;
    }

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    // if the error app is on the system image, look for system apps
    // error receiver
    if ((appFlags & IApplicationInfo::FLAG_SYSTEM) != 0) {
        system->GetEnv(SYSTEM_APPS_ERROR_RECEIVER_PROPERTY, &candidate);
        result = GetErrorReportReceiver(pm, packageName, candidate);
        if (result != NULL) {
            return result;
        }
    }

    // if there is a default receiver, try that
    system->GetEnv(DEFAULT_ERROR_RECEIVER_PROPERTY, &candidate);
    return GetErrorReportReceiver(pm, packageName, candidate);
}

ECode CApplicationErrorReport::Dump(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    VALIDATE_NOT_NULL(pw);

    StringBuilder sb(64);
    sb.Append(prefix);
    sb.Append("type:");
    sb.Append(mType);
    String str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("packageName:");
    sb.Append(mPackageName);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("installerPackageName:");
    sb.Append(mInstallerPackageName);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("processName:");
    sb.Append(mProcessName);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("time:");
    sb.Append(mTime);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("systemApp:");
    sb.Append(mSystemApp);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    switch (mType) {
        case IApplicationErrorReport::TYPE_CRASH:
            return mCrashInfo->Dump(pw, prefix);

        case IApplicationErrorReport::TYPE_ANR:
            return mAnrInfo->Dump(pw, prefix);

        case IApplicationErrorReport::TYPE_BATTERY:
            return mBatteryInfo->Dump(pw, prefix);

        case IApplicationErrorReport::TYPE_RUNNING_SERVICE:
            return mRunningServiceInfo->Dump(pw, prefix);

        default:
            Logger::W(TAG, "WriteToParcel: Invalid Error type!");
            break;
    }

    return NOERROR;
}

ECode CApplicationErrorReport::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    Int32 start;
    dest->GetDataPosition(&start);
    FAIL_RETURN(dest->WriteInt32(mType));
    FAIL_RETURN(dest->WriteString(mPackageName));
    FAIL_RETURN(dest->WriteString(mInstallerPackageName));
    FAIL_RETURN(dest->WriteString(mProcessName));
    FAIL_RETURN(dest->WriteInt64(mTime));
    FAIL_RETURN(dest->WriteInt32(mSystemApp ? 1 : 0));

    switch (mType) {
        case IApplicationErrorReport::TYPE_CRASH:
            return IParcelable::Probe(mCrashInfo)->WriteToParcel(dest);

        case IApplicationErrorReport::TYPE_ANR:
            return IParcelable::Probe(mAnrInfo)->WriteToParcel(dest);

        case IApplicationErrorReport::TYPE_BATTERY:
            return IParcelable::Probe(mBatteryInfo)->WriteToParcel(dest);

        case IApplicationErrorReport::TYPE_RUNNING_SERVICE:
            return IParcelable::Probe(mRunningServiceInfo)->WriteToParcel(dest);

        default:
            Logger::W(TAG, "WriteToParcel: Invalid Error type!");
            break;
    }

    Int32 total;
    dest->GetDataPosition(&total);
    total = total - start;
    if (total > 20*1024) {
        Slogger::D("Error", "ERR: InstallerPackageName=%s", mInstallerPackageName.string());
        // Slogger.D("Error", "ERR: exClass=" + exceptionClassName);
        // Slogger.D("Error", "ERR: exMsg=" + exceptionMessage);
        // Slogger.D("Error", "ERR: file=" + throwFileName);
        // Slogger.D("Error", "ERR: class=" + throwClassName);
        // Slogger.D("Error", "ERR: method=" + throwMethodName + " line=" + throwLineNumber);
        // Slogger.D("Error", "ERR: stack=" + stackTrace);
        // Slogger.D("Error", "ERR: TOTAL BYTES WRITTEN: " + (dest.dataPosition()-start));
    }

    return NOERROR;
}

ECode CApplicationErrorReport::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);

    FAIL_RETURN(parcel->ReadInt32(&mType));
    FAIL_RETURN(parcel->ReadString(&mPackageName));
    FAIL_RETURN(parcel->ReadString(&mInstallerPackageName));
    FAIL_RETURN(parcel->ReadString(&mProcessName));
    FAIL_RETURN(parcel->ReadInt64(&mTime));
    Int32 sa;
    FAIL_RETURN(parcel->ReadInt32(&sa));
    mSystemApp =  (sa == 1);

    IParcelable* parcelable;
    switch (mType) {
        case IApplicationErrorReport::TYPE_CRASH:
            CApplicationErrorReportCrashInfo::New(
                    (IApplicationErrorReportCrashInfo**)&mCrashInfo);
            parcelable = (IParcelable*)mCrashInfo->Probe(EIID_IParcelable);
            if (parcelable) {
                parcelable->ReadFromParcel(parcel);
            }

            mAnrInfo = NULL;
            mBatteryInfo = NULL;
            mRunningServiceInfo = NULL;
            break;
        case IApplicationErrorReport::TYPE_ANR:
            CApplicationErrorReportAnrInfo::New(
                    (IApplicationErrorReportAnrInfo**)&mAnrInfo);
            parcelable = (IParcelable*)mAnrInfo->Probe(EIID_IParcelable);
            if (parcelable) {
                parcelable->ReadFromParcel(parcel);
            }

            mCrashInfo = NULL;
            mBatteryInfo = NULL;
            mRunningServiceInfo = NULL;
            break;
        case IApplicationErrorReport::TYPE_BATTERY:
            CApplicationErrorReportBatteryInfo::New(
                    (IApplicationErrorReportBatteryInfo**)&mBatteryInfo);
            parcelable = (IParcelable*)mBatteryInfo->Probe(EIID_IParcelable);
            if (parcelable) {
                parcelable->ReadFromParcel(parcel);
            }

            mAnrInfo = NULL;
            mCrashInfo = NULL;
            mRunningServiceInfo = NULL;
            break;
        case IApplicationErrorReport::TYPE_RUNNING_SERVICE:
            CApplicationErrorReportRunningServiceInfo::New(
                    (IApplicationErrorReportRunningServiceInfo**)&mRunningServiceInfo);
            parcelable = (IParcelable*)mRunningServiceInfo->Probe(EIID_IParcelable);
            if (parcelable) {
                parcelable->ReadFromParcel(parcel);
            }

            mAnrInfo = NULL;
            mCrashInfo = NULL;
            mBatteryInfo = NULL;
            break;

        default:
            Logger::W(TAG, "WriteToParcel: Invalid Error type!");
            break;
    }

    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
