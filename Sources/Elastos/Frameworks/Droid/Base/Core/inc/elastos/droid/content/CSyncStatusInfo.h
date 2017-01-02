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

#ifndef __ELASTOS_DROID_CONTENT_CSYNCSTATUSINFO_H__
#define __ELASTOS_DROID_CONTENT_CSYNCSTATUSINFO_H__

#include "_Elastos_Droid_Content_CSyncStatusInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CSyncStatusInfo)
    , public Object
    , public ISyncStatusInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSyncStatusInfo();

    virtual ~CSyncStatusInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 authorityId);

    CARAPI constructor(
        /* [in] */ ISyncStatusInfo* other);

    CARAPI GetAuthorityId(
        /* [out] */ Int32* authorityId);

    CARAPI GetTotalElapsedTime(
        /* [out] */ Int64* totalElapsedTime);

    CARAPI SetTotalElapsedTime(
        /* [in] */ Int64 totalElapsedTime);

    CARAPI GetNumSyncs(
        /* [out] */ Int32* numSyncs);

    CARAPI SetNumSyncs(
        /* [in] */ Int32 numSyncs);

    CARAPI GetNumSourcePoll(
        /* [out] */ Int32* numSourcePoll);

    CARAPI SetNumSourcePoll(
        /* [in] */ Int32 numSourcePoll);

    CARAPI GetNumSourceServer(
        /* [out] */ Int32* numSourceServer);

    CARAPI SetNumSourceServer(
        /* [in] */ Int32 numSourceServer);

    CARAPI GetNumSourceLocal(
        /* [out] */ Int32* numSourceLocal);

    CARAPI SetNumSourceLocal(
        /* [in] */ Int32 numSourceLocal);

    CARAPI GetNumSourceUser(
        /* [out] */ Int32* numSourceUser);

    CARAPI SetNumSourceUser(
        /* [in] */ Int32 numSourceUser);

    CARAPI GetNumSourcePeriodic(
        /* [out] */ Int32* numSourcePeriodic);

    CARAPI SetNumSourcePeriodic(
        /* [in] */ Int32 numSourcePeriodic);

    CARAPI GetLastSuccessTime(
        /* [out] */ Int64* lastSuccessTime);

    CARAPI SetLastSuccessTime(
        /* [in] */ Int64 lastSuccessTime);

    CARAPI GetLastSuccessSource(
        /* [out] */ Int32* lastSuccessSource);

    CARAPI SetLastSuccessSource(
        /* [in] */ Int32 lastSuccessSource);

    CARAPI GetLastFailureTime(
        /* [out] */ Int64* lastFailureTime);

    CARAPI SetLastFailureTime(
        /* [in] */ Int64 lastFailureTime);

    CARAPI GetLastFailureSource(
        /* [out] */ Int32* lastFailureSource);

    CARAPI SetLastFailureSource(
        /* [in] */ Int32 lastFailureSource);

    CARAPI GetLastFailureMesg(
        /* [out] */ String* lastFailureMesg);

    CARAPI SetLastFailureMesg(
        /* [in] */ const String& lastFailureMesg);

    CARAPI GetInitialFailureTime(
        /* [out] */ Int64* initialFailureTime);

    CARAPI SetInitialFailureTime(
        /* [in] */ Int64 initialFailureTime);

    CARAPI GetPending(
        /* [out] */ Boolean* pending);

    CARAPI SetPending(
        /* [in] */ Boolean pending);

    CARAPI GetInitialize(
        /* [out] */ Boolean* initialize);

    CARAPI SetInitialize(
        /* [in] */ Boolean initialize);

    CARAPI GetLastFailureMesgAsInt(
        /* [in] */ Int32 def,
        /* [out] */ Int32* msg);

    CARAPI SetPeriodicSyncTime(
        /* [in] */ Int32 index,
        /* [in] */ Int64 when);

    CARAPI GetPeriodicSyncTime(
        /* [in] */ Int32 index,
        /* [out] */ Int64* time);

    CARAPI RemovePeriodicSyncTime(
        /* [in] */ Int32 index);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    CARAPI EnsurePeriodicSyncTimeSize(
        /* [in] */ Int32 index);

private:
    static const String TAG;
    static const Int32 VERSION = 2;

private:
    Int32 mAuthorityId;
    Int64 mTotalElapsedTime;
    Int32 mNumSyncs;
    Int32 mNumSourcePoll;
    Int32 mNumSourceServer;
    Int32 mNumSourceLocal;
    Int32 mNumSourceUser;
    Int32 mNumSourcePeriodic;
    Int64 mLastSuccessTime;
    Int32 mLastSuccessSource;
    Int64 mLastFailureTime;
    Int32 mLastFailureSource;
    String mLastFailureMesg;
    Int64 mInitialFailureTime;
    Boolean mPending;
    Boolean mInitialize;

    // Warning: It is up to the external caller to ensure there are
    // no race conditions when accessing this list
    AutoPtr<ArrayOf<Int64> > mPeriodicSyncTimes;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CSYNCSTATUSINFO_H__
