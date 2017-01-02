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

#ifndef __ELASTOS_DROID_APP_CACTIVITYMANAGERPRUNNINGAPPPROCESSINFO_H__
#define __ELASTOS_DROID_APP_CACTIVITYMANAGERPRUNNINGAPPPROCESSINFO_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_App_CActivityManagerRunningAppProcessInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Information you can retrieve about a running process.
 */
CarClass(CActivityManagerRunningAppProcessInfo)
    , public Object
    , public IActivityManagerRunningAppProcessInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CActivityManagerRunningAppProcessInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& pProcessName,
        /* [in] */ Int32 pPid,
        /* [in] */ ArrayOf<String>* pArr);

    /** @hide */
    static Int32 ProcStateToImportance(
        /* [in] */ Int32 procState);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * The name of the process that this object is associated with
     */
    CARAPI GetProcessName(
        /* [out] */ String* processName);

    CARAPI SetProcessName(
        /* [in] */ const String& processName);

    /**
     * The pid of this process; 0 if none
     */
    CARAPI GetPid(
        /* [out] */ Int32* pid);

    CARAPI SetPid(
        /* [in] */ Int32 pid);

    /**
     * The user id of this process.
     */
    CARAPI GetUid(
        /* [out] */ Int32* uid);

    CARAPI SetUid(
        /* [in] */ Int32 uid);

    /**
     * All packages that have been loaded into the process.
     */
    CARAPI GetPkgList(
        /* [out, callee] */ ArrayOf<String>** pkgList);

    CARAPI SetPkgList(
        /* [in] */ ArrayOf<String>* pkgList);

    /**
     * Flags of information.  May be any of
     * {@link #FLAG_CANT_SAVE_STATE}.
     * @hide
     */
    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    /**
     * Last memory trim level reported to the process: corresponds to
     * the values supplied to {@link android.content.ComponentCallbacks2#onTrimMemory(int)
     * ComponentCallbacks2.onTrimMemory(int)}.
     */
    CARAPI GetLastTrimLevel(
        /* [out] */ Int32* level);

    CARAPI SetLastTrimLevel(
        /* [in] */ Int32 level);

    /**
     * The relative importance level that the system places on this
     * process.  May be one of {@link #IMPORTANCE_FOREGROUND},
     * {@link #IMPORTANCE_VISIBLE}, {@link #IMPORTANCE_SERVICE},
     * {@link #IMPORTANCE_BACKGROUND}, or {@link #IMPORTANCE_EMPTY}.  These
     * constants are numbered so that "more important" values are always
     * smaller than "less important" values.
     */
    CARAPI GetImportance(
        /* [out] */ Int32* importance);

    CARAPI SetImportance(
        /* [in] */ Int32 importance);

    /**
     * An additional ordering within a particular {@link #importance}
     * category, providing finer-grained information about the relative
     * utility of processes within a category.  This number means nothing
     * except that a smaller values are more recently used (and thus
     * more important).  Currently an LRU value is only maintained for
     * the {@link #IMPORTANCE_BACKGROUND} category, though others may
     * be maintained in the future.
     */
    CARAPI GetLru(
        /* [out] */ Int32* lru);

    CARAPI SetLru(
        /* [in] */ Int32 lru);

    /**
     * The reason for {@link #importance}, if any.
     */
    CARAPI GetImportanceReasonCode(
        /* [out] */ Int32* code);

    CARAPI SetImportanceReasonCode(
        /* [in] */ Int32 code);

    /**
     * For the specified values of {@link #importanceReasonCode}, this
     * is the process ID of the other process that is a client of this
     * process.  This will be 0 if no other process is using this one.
     */
    CARAPI GetImportanceReasonPid(
        /* [out] */ Int32* pid);

    CARAPI SetImportanceReasonPid(
        /* [in] */ Int32 pid);

    /**
     * For the specified values of {@link #importanceReasonCode}, this
     * is the name of the component that is being used in this process.
     */
    CARAPI GetImportanceReasonComponent(
        /* [out] */ IComponentName** component);

    CARAPI SetImportanceReasonComponent(
        /* [in] */ IComponentName* component);

    /**
     * When {@link importanceReasonPid} is non-0, this is the importance
     * of the other pid. @hide
     */
    CARAPI GetImportanceReasonImportance(
        /* [out] */ Int32* importance);

    CARAPI SetImportanceReasonImportance(
        /* [in] */ Int32 importance);

    /**
     * Current process state, as per PROCESS_STATE_* constants.
     * @hide
     */
    CARAPI GetProcessState(
        /* [out] */ Int32* processState);

    CARAPI SetProcessState(
        /* [in] */ Int32 processState);

private:
    String mProcessName;

    Int32 mPid;

    Int32 mUid;

    AutoPtr< ArrayOf<String> > mPkgList;

    Int32 mFlags;

    Int32 mLastTrimLevel;

    Int32 mImportance;

    Int32 mLru;

    Int32 mImportanceReasonCode;

    Int32 mImportanceReasonPid;

    AutoPtr<IComponentName> mImportanceReasonComponent;

    Int32 mImportanceReasonImportance;

   /**
     * Current process state, as per PROCESS_STATE_* constants.
     * @hide
     */
    Int32 mProcessState;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACTIVITYMANAGERPRUNNINGAPPPROCESSINFO_H__
