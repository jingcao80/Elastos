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

#ifndef __ELASTOS_DROID_APP_CAPPLICATIONERRORREPORT_H__
#define __ELASTOS_DROID_APP_CAPPLICATIONERRORREPORT_H__

#include "_Elastos_Droid_App_CApplicationErrorReport.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IPrinter;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IPackageManager;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CApplicationErrorReport)
    , public Object
    , public IApplicationErrorReport
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CApplicationErrorReport();

    /**
     * Create an uninitialized instance of {@link ApplicationErrorReport}.
     */
    CARAPI constructor();

    /**
     * Return activity in receiverPackage that handles ACTION_APP_ERROR.
     *
     * @param pm PackageManager instance
     * @param errorPackage package which caused the error
     * @param receiverPackage candidate package to receive the error
     * @return activity component within receiverPackage which handles
     * ACTION_APP_ERROR, or null if not found
     */
    static CARAPI_(AutoPtr<IComponentName>) GetErrorReportReceiver(
        /* [in] */ IPackageManager* pm,
        /* [in] */ const String& errorPackage,
        /* [in] */ const String& receiverPackage);

    static CARAPI_(AutoPtr<IComponentName>) GetErrorReportReceiver(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 appFlags);

    /**
     * Dump the report to a Printer.
     */
    CARAPI Dump(
        /* [in] */ IPrinter* pw,
        /* [in] */ const String& prefix);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    /**
     * Type of this report. Can be one of {@link #TYPE_NONE},
     * {@link #TYPE_CRASH}, {@link #TYPE_ANR}, {@link #TYPE_BATTERY},
     * or {@link #TYPE_RUNNING_SERVICE}.
     */
    CARAPI GetType(
        /* [out] */ Int32* type);

    CARAPI SetType(
        /* [in] */ Int32 type);

    /**
     * Package name of the application.
     */
    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI SetPackageName(
        /* [in] */ const String& packageName);

    /**
     * Package name of the application which installed the application this
     * report pertains to.
     * This identifies which market the application came from.
     */
    CARAPI GetInstallerPackageName(
        /* [out] */ String* pkgName);

    CARAPI SetInstallerPackageName(
        /* [in] */ const String& pkgName);

    /**
     * Process name of the application.
     */
    CARAPI GetProcessName(
        /* [out] */ String* processName);

    CARAPI SetProcessName(
        /* [in] */ const String& processName);

    /**
     * Time at which the error occurred.
     */
    CARAPI GetTime(
        /* [out] */ Int64* time);

    CARAPI SetTime(
        /* [in] */ Int64 time);

    /**
     * Set if the app is on the system image.
     */
    CARAPI GetSystemApp(
        /* [out] */ Boolean* systemApp);

    CARAPI SetSystemApp(
        /* [in] */ Boolean systemApp);

    /**
     * If this report is of type {@link #TYPE_CRASH}, contains an instance
     * of CrashInfo describing the crash; otherwise null.
     */
    CARAPI GetCrashInfo(
        /* [out] */ IApplicationErrorReportCrashInfo** crashInfo);

    CARAPI SetCrashInfo(
        /* [in] */ IApplicationErrorReportCrashInfo* crashInfo);

    /**
     * If this report is of type {@link #TYPE_ANR}, contains an instance
     * of AnrInfo describing the ANR; otherwise null.
     */
    CARAPI GetAnrInfo(
        /* [out] */ IApplicationErrorReportAnrInfo** anrInfo);

    CARAPI SetAnrInfo(
        /* [in] */ IApplicationErrorReportAnrInfo* anrInfo);

    /**
     * If this report is of type {@link #TYPE_BATTERY}, contains an instance
     * of BatteryInfo; otherwise null.
     */
    CARAPI GetBatteryInfo(
        /* [out] */ IApplicationErrorReportBatteryInfo** batteryInfo);

    CARAPI SetBatteryInfo(
        /* [in] */ IApplicationErrorReportBatteryInfo* batteryInfo);

    /**
     * If this report is of type {@link #TYPE_RUNNING_SERVICE}, contains an instance
     * of RunningServiceInfo; otherwise null.
     */
    CARAPI GetRunningServiceInfo(
        /* [out] */ IApplicationErrorReportRunningServiceInfo** runningServiceInfo);

    CARAPI SetRunningServiceInfo(
        /* [in] */ IApplicationErrorReportRunningServiceInfo* runningServiceInfo);

private:
    /**
     * Type of this report. Can be one of {@link #TYPE_NONE},
     * {@link #TYPE_CRASH}, {@link #TYPE_ANR}, {@link #TYPE_BATTERY},
     * or {@link #TYPE_RUNNING_SERVICE}.
     */
    Int32 mType;

    /**
     * Package name of the application.
     */
    String mPackageName;

    /**
     * Package name of the application which installed the application this
     * report pertains to.
     * This identifies which market the application came from.
     */
    String mInstallerPackageName;

    /**
     * Process name of the application.
     */
    String mProcessName;

    /**
     * Time at which the error occurred.
     */
    Int64 mTime;

    /**
     * Set if the app is on the system image.
     */
    Boolean mSystemApp;

    /**
     * If this report is of type {@link #TYPE_CRASH}, contains an instance
     * of CrashInfo describing the crash; otherwise null.
     */
    AutoPtr<IApplicationErrorReportCrashInfo> mCrashInfo;

    /**
     * If this report is of type {@link #TYPE_ANR}, contains an instance
     * of AnrInfo describing the ANR; otherwise null.
     */
    AutoPtr<IApplicationErrorReportAnrInfo> mAnrInfo;

    /**
      * If this report is of type {@link #TYPE_BATTERY}, contains an instance
      * of BatteryInfo; otherwise null.
      */
    AutoPtr<IApplicationErrorReportBatteryInfo> mBatteryInfo;

    /**
      * If this report is of type {@link #TYPE_RUNNING_SERVICE}, contains an instance
      * of RunningServiceInfo; otherwise null.
      */
    AutoPtr<IApplicationErrorReportRunningServiceInfo> mRunningServiceInfo;
};


} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CAPPLICATIONERRORREPORT_H__
