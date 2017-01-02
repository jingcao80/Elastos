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

/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings.applications;

using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Internal::App::IProcessStats;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;

public class ProcStatsEntry implements Parcelable {
    private static const String TAG = "ProcStatsEntry";
    private static Boolean DEBUG = ProcessStatsUi.DEBUG;

    final String mPackage;
    final Int32 mUid;
    final String mName;
    final ArrayList<String> mPackages = new ArrayList<String>();
    final Int64 mDuration;
    final Int64 mAvgPss;
    final Int64 mMaxPss;
    final Int64 mAvgUss;
    final Int64 mMaxUss;
    final Int64 mWeight;

    String mBestTargetPackage;

    ArrayMap<String, ArrayList<Service>> mServices = new ArrayMap<String, ArrayList<Service>>(1);

    public ApplicationInfo mUiTargetApp;
    public String mUiLabel;
    public String mUiBaseLabel;
    public String mUiPackage;

    public ProcStatsEntry(ProcessStats.ProcessState proc, String packageName,
            ProcessStats.ProcessDataCollection tmpTotals, Boolean useUss, Boolean weightWithTime) {
        ProcessStats->ComputeProcessData(proc, tmpTotals, 0);
        mPackage = proc.mPackage;
        mUid = proc.mUid;
        mName = proc.mName;
        mPackages->Add(packageName);
        mDuration = tmpTotals.totalTime;
        mAvgPss = tmpTotals.avgPss;
        mMaxPss = tmpTotals.maxPss;
        mAvgUss = tmpTotals.avgUss;
        mMaxUss = tmpTotals.maxUss;
        mWeight = (weightWithTime ? mDuration : 1) * (useUss ? mAvgUss : mAvgPss);
        if (DEBUG) Logger::D(TAG, "New proc entry " + proc.mName + ": dur=" + mDuration
                + " avgpss=" + mAvgPss + " weight=" + mWeight);
    }

    public ProcStatsEntry(Parcel in) {
        mPackage = in->ReadString();
        mUid = in->ReadInt();
        mName = in->ReadString();
        in->ReadStringList(mPackages);
        mDuration = in->ReadLong();
        mAvgPss = in->ReadLong();
        mMaxPss = in->ReadLong();
        mAvgUss = in->ReadLong();
        mMaxUss = in->ReadLong();
        mWeight = in->ReadLong();
        mBestTargetPackage = in->ReadString();
        final Int32 N = in->ReadInt();
        if (N > 0) {
            mServices->EnsureCapacity(N);
            for (Int32 i=0; i<N; i++) {
                String key = in->ReadString();
                ArrayList<Service> value = new ArrayList<Service>();
                in->ReadTypedList(value, Service.CREATOR);
                mServices->Append(key, value);
            }
        }
    }

    CARAPI AddPackage(String packageName) {
        mPackages->Add(packageName);
    }

    CARAPI EvaluateTargetPackage(PackageManager pm, ProcessStats stats,
            ProcessStats.ProcessDataCollection totals, Comparator<ProcStatsEntry> compare,
            Boolean useUss, Boolean weightWithTime) {
        mBestTargetPackage = NULL;
        if (mPackages->Size() == 1) {
            if (DEBUG) Logger::D(TAG, "Eval pkg of " + mName + ": single pkg " + mPackages->Get(0));
            mBestTargetPackage = mPackages->Get(0);
        } else {
            // See if there is one significant package that was running here.
            ArrayList<ProcStatsEntry> subProcs = new ArrayList<ProcStatsEntry>();
            for (Int32 ipkg=0; ipkg<mPackages->Size(); ipkg++) {
                SparseArray<ProcessStats.PackageState> vpkgs
                        = stats.mPackages->Get(mPackages->Get(ipkg), mUid);
                for (Int32 ivers=0;  ivers<vpkgs->Size(); ivers++) {
                    ProcessStats.PackageState pkgState = vpkgs->ValueAt(ivers);
                    if (DEBUG) Logger::D(TAG, "Eval pkg of " + mName + ", pkg "
                            + pkgState + ":");
                    if (pkgState == NULL) {
                        Logger::W(TAG, "No package state found for " + mPackages->Get(ipkg) + "/"
                                + mUid + " in process " + mName);
                        continue;
                    }
                    ProcessStats.ProcessState pkgProc = pkgState.mProcesses->Get(mName);
                    if (pkgProc == NULL) {
                        Logger::W(TAG, "No process " + mName + " found in package state "
                                + mPackages->Get(ipkg) + "/" + mUid);
                        continue;
                    }
                    subProcs->Add(new ProcStatsEntry(pkgProc, pkgState.mPackageName, totals, useUss,
                            weightWithTime));
                }
            }
            if (subProcs->Size() > 1) {
                Collections->Sort(subProcs, compare);
                if (subProcs->Get(0).mWeight > (subProcs->Get(1).mWeight*3)) {
                    if (DEBUG) Logger::D(TAG, "Eval pkg of " + mName + ": best pkg "
                            + subProcs->Get(0).mPackage + " weight " + subProcs->Get(0).mWeight
                            + " better than " + subProcs->Get(1).mPackage
                            + " weight " + subProcs->Get(1).mWeight);
                    mBestTargetPackage = subProcs->Get(0).mPackage;
                    return;
                }
                // Couldn't find one that is best by weight, let's decide on best another
                // way: the one that has the longest running service, accounts for at least
                // half of the maximum weight, and has specified an explicit app icon.
                Int64 maxWeight = subProcs->Get(0).mWeight;
                Int64 bestRunTime = -1;
                for (Int32 i=0; i<subProcs->Size(); i++) {
                    if (subProcs->Get(i).mWeight < (maxWeight/2)) {
                        if (DEBUG) Logger::D(TAG, "Eval pkg of " + mName + ": pkg "
                                + subProcs->Get(i).mPackage + " weight " + subProcs->Get(i).mWeight
                                + " too small");
                        continue;
                    }
                    try {
                        ApplicationInfo ai = pm->GetApplicationInfo(subProcs->Get(i).mPackage, 0);
                        if (ai.icon == 0) {
                            if (DEBUG) Logger::D(TAG, "Eval pkg of " + mName + ": pkg "
                                    + subProcs->Get(i).mPackage + " has no icon");
                            continue;
                        }
                    } catch (PackageManager.NameNotFoundException e) {
                        if (DEBUG) Logger::D(TAG, "Eval pkg of " + mName + ": pkg "
                                + subProcs->Get(i).mPackage + " failed finding app info");
                        continue;
                    }
                    ArrayList<Service> subProcServices = NULL;
                    for (Int32 isp=0, NSP=mServices->Size(); isp<NSP; isp++) {
                        ArrayList<Service> subServices = mServices->ValueAt(isp);
                        if (subServices->Get(0).mPackage->Equals(subProcs->Get(i).mPackage)) {
                            subProcServices = subServices;
                            break;
                        }
                    }
                    Int64 thisRunTime = 0;
                    if (subProcServices != NULL) {
                        for (Int32 iss=0, NSS=subProcServices->Size(); iss<NSS; iss++) {
                            Service service = subProcServices->Get(iss);
                            if (service.mDuration > thisRunTime) {
                                if (DEBUG) Logger::D(TAG, "Eval pkg of " + mName + ": pkg "
                                        + subProcs->Get(i).mPackage + " service " + service.mName
                                        + " run time is " + service.mDuration);
                                thisRunTime = service.mDuration;
                                break;
                            }
                        }
                    }
                    if (thisRunTime > bestRunTime) {
                        if (DEBUG) Logger::D(TAG, "Eval pkg of " + mName + ": pkg "
                                + subProcs->Get(i).mPackage + " new best run time " + thisRunTime);
                        mBestTargetPackage = subProcs->Get(i).mPackage;
                        bestRunTime = thisRunTime;
                    } else {
                        if (DEBUG) Logger::D(TAG, "Eval pkg of " + mName + ": pkg "
                                + subProcs->Get(i).mPackage + " run time " + thisRunTime
                                + " not as good as last " + bestRunTime);
                    }
                }
            } else if (subProcs->Size() == 1) {
                mBestTargetPackage = subProcs->Get(0).mPackage;
            }
        }
    }

    CARAPI RetrieveUiData(PackageManager pm) {
        mUiTargetApp = NULL;
        mUiLabel = mUiBaseLabel = mName;
        mUiPackage = mBestTargetPackage;
        if (mUiPackage != NULL) {
            // Only one app associated with this process.
            try {
                mUiTargetApp = pm->GetApplicationInfo(mUiPackage,
                        PackageManager.GET_DISABLED_COMPONENTS |
                        PackageManager.GET_DISABLED_UNTIL_USED_COMPONENTS |
                        PackageManager.GET_UNINSTALLED_PACKAGES);
                String name = mUiBaseLabel = mUiTargetApp->LoadLabel(pm).ToString();
                if (mName->Equals(mUiPackage)) {
                    mUiLabel = name;
                } else {
                    if (mName->StartsWith(mUiPackage)) {
                        Int32 off = mUiPackage->Length();
                        if (mName->Length() > off) {
                            off++;
                        }
                        mUiLabel = name + " (" + mName->Substring(off) + ")";
                    } else {
                        mUiLabel = name + " (" + mName + ")";
                    }
                }
            } catch (PackageManager.NameNotFoundException e) {
            }
        }
        if (mUiTargetApp == NULL) {
            String[] packages = pm->GetPackagesForUid(mUid);
            if (packages != NULL) {
                for (String curPkg : packages) {
                    try {
                        final PackageInfo pi = pm->GetPackageInfo(curPkg,
                                PackageManager.GET_DISABLED_COMPONENTS |
                                PackageManager.GET_DISABLED_UNTIL_USED_COMPONENTS |
                                PackageManager.GET_UNINSTALLED_PACKAGES);
                        if (pi.sharedUserLabel != 0) {
                            mUiTargetApp = pi.applicationInfo;
                            final CharSequence nm = pm->GetText(curPkg,
                                    pi.sharedUserLabel, pi.applicationInfo);
                            if (nm != NULL) {
                                mUiBaseLabel = nm->ToString();
                                mUiLabel = mUiBaseLabel + " (" + mName + ")";
                            } else {
                                mUiBaseLabel = mUiTargetApp->LoadLabel(pm).ToString();
                                mUiLabel = mUiBaseLabel + " (" + mName + ")";
                            }
                            break;
                        }
                    } catch (PackageManager.NameNotFoundException e) {
                    }
                }
            } else {
                // no current packages for this uid, typically because of uninstall
                Logger::I(TAG, "No package for uid " + mUid);
            }
        }
    }

    CARAPI AddService(ProcessStats.ServiceState svc) {
        ArrayList<Service> services = mServices->Get(svc.mPackage);
        if (services == NULL) {
            services = new ArrayList<Service>();
            mServices->Put(svc.mPackage, services);
        }
        services->Add(new Service(svc));
    }

    //@Override
    public Int32 DescribeContents() {
        return 0;
    }

    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteString(mPackage);
        dest->WriteInt(mUid);
        dest->WriteString(mName);
        dest->WriteStringList(mPackages);
        dest->WriteLong(mDuration);
        dest->WriteLong(mAvgPss);
        dest->WriteLong(mMaxPss);
        dest->WriteLong(mAvgUss);
        dest->WriteLong(mMaxUss);
        dest->WriteLong(mWeight);
        dest->WriteString(mBestTargetPackage);
        final Int32 N = mServices->Size();
        dest->WriteInt(N);
        for (Int32 i=0; i<N; i++) {
            dest->WriteString(mServices->KeyAt(i));
            dest->WriteTypedList(mServices->ValueAt(i));
        }
    }

    public static const Parcelable.Creator<ProcStatsEntry> CREATOR
            = new Parcelable.Creator<ProcStatsEntry>() {
        public ProcStatsEntry CreateFromParcel(Parcel in) {
            return new ProcStatsEntry(in);
        }

        public ProcStatsEntry[] NewArray(Int32 size) {
            return new ProcStatsEntry[size];
        }
    };

    public class Service implements Parcelable {
        final String mPackage;
        final String mName;
        final String mProcess;
        final Int64 mDuration;

        public Service(ProcessStats.ServiceState service) {
            mPackage = service.mPackage;
            mName = service.mName;
            mProcess = service.mProcessName;
            mDuration = ProcessStats->DumpSingleServiceTime(NULL, NULL, service,
                    ProcessStats.ServiceState.SERVICE_RUN,
                    ProcessStats.STATE_NOTHING, 0, 0);
        }

        public Service(Parcel in) {
            mPackage = in->ReadString();
            mName = in->ReadString();
            mProcess = in->ReadString();
            mDuration = in->ReadLong();
        }

        //@Override
        public Int32 DescribeContents() {
            return 0;
        }

        //@Override
        CARAPI WriteToParcel(Parcel dest, Int32 flags) {
            dest->WriteString(mPackage);
            dest->WriteString(mName);
            dest->WriteString(mProcess);
            dest->WriteLong(mDuration);
        }

        public static const Parcelable.Creator<Service> CREATOR
                = new Parcelable.Creator<Service>() {
            public Service CreateFromParcel(Parcel in) {
                return new Service(in);
            }

            public Service[] NewArray(Int32 size) {
                return new Service[size];
            }
        };
    }
}
