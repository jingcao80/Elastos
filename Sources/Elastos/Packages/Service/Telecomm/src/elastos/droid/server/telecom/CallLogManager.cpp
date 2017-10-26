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

#include "R.h"
#include "elastos/droid/server/telecom/CallLogManager.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/Log.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Telephony.h>
#include <elastos/droid/R.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Net::EIID_IUri;
using Elastos::Droid::Provider::CCalls;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Telecom::ICallState;
using Elastos::Droid::Telecom::IDisconnectCause;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telecom::IVideoProfileVideoState;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Core::CArrayOf;
using Elastos::Core::IArrayOf;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// CallLogManager::AddCallArgs
//=============================================================================
CallLogManager::AddCallArgs::AddCallArgs(
    /* [in] */ IContext* context,
    /* [in] */ ICallerInfo* callerInfo,
    /* [in] */ const String& number,
    /* [in] */ Int32 presentation,
    /* [in] */ Int32 callType,
    /* [in] */ Int32 features,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ Int64 creationDate,
    /* [in] */ Int64 durationInMillis,
    /* [in] */ IInteger64* dataUsage)
    : mContext(context)
    , mCallerInfo(callerInfo)
    , mNumber(number)
    , mPresentation(presentation)
    , mCallType(callType)
    , mFeatures(features)
    , mAccountHandle(accountHandle)
    , mTimestamp(creationDate)
    , mDurationInSec((Int32)(durationInMillis / 1000))
    , mDataUsage(dataUsage)
{}

//=============================================================================
// CallLogManager::LogCallAsyncTask
//=============================================================================
ECode CallLogManager::LogCallAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* callList,
    /* [out, callee] */ IInterface** _result)
{
    VALIDATE_NOT_NULL(_result)

    Int32 count = callList->GetLength();
    AutoPtr<ArrayOf<IUri*> > result = ArrayOf<IUri*>::Alloc(count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<AddCallArgs> c = (AddCallArgs*) IObject::Probe((*callList)[i]);
        // try {
        ECode ec;
        do {
            // May block.
            AutoPtr<ICalls> helper;
            CCalls::AcquireSingleton((ICalls**)&helper);
            AutoPtr<IUri> uri;
            Int64 i64DataUsage = -1;
            if (c->mDataUsage != NULL)
                c->mDataUsage->GetValue(&i64DataUsage);
            ec = helper->AddCall(c->mCallerInfo, c->mContext, c->mNumber, c->mPresentation,
                    c->mCallType, c->mFeatures, c->mAccountHandle, c->mTimestamp, c->mDurationInSec,
                    i64DataUsage, TRUE /* addForAllUsers */, (IUri**)&uri);
            if (FAILED(ec)) break;
            result->Set(i, uri);
        } while(FALSE);
        // } catch (Exception e) {
        if (FAILED(ec)) {
            // This is very rare but may happen in legitimate cases.
            // E.g. If the phone is encrypted and thus write request fails, it may cause
            // some kind of Exception (right now it is IllegalArgumentException, but this
            // might change).
            //
            // We don't want to crash the whole process just because of that, so just log
            // it instead.
            Log::E("CallLogManager", "Exception raised during adding CallLog entry. %d", ec);
            (*result)[i] = NULL;
        }
        // }
    }
    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IUri, result->GetLength(), (IArrayOf**)&array);
    for (Int32 i = 0; i < result->GetLength(); ++i) {
        array->Set(i, (*result)[i]);
    }
    *_result = array;
    REFCOUNT_ADD(*_result)
    return NOERROR;
}

ECode CallLogManager::LogCallAsyncTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    AutoPtr<IArrayOf> uriArray = IArrayOf::Probe(result);
    Int32 N;
    uriArray->GetLength(&N);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInterface> uri;
        uriArray->Get(i, (IInterface**)&uri);
        if (uri == NULL) {
            Log::W("CallLogManager", "Failed to write call to the log.");
        }
    }
    return NOERROR;
}

//=============================================================================
// CallLogManager
//=============================================================================
const String CallLogManager::TAG("CallLogManager");
const String CallLogManager::ACTION_CALLS_TABLE_ADD_ENTRY("com.android.server.telecom.intent.action.CALLS_ADD_ENTRY");
const String CallLogManager::PERMISSION_PROCESS_CALLLOG_INFO("android.permission.PROCESS_CALLLOG_INFO");
const String CallLogManager::CALL_TYPE("callType");
const String CallLogManager::CALL_DURATION("duration");

ECode CallLogManager::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

ECode CallLogManager::OnCallStateChanged(
    /* [in] */ ICall* call,
    /* [in] */ Int32 oldState,
    /* [in] */ Int32 newState)
{
    Boolean isNewlyDisconnected =
            newState == ICallState::DISCONNECTED || newState == ICallState::ABORTED;
    AutoPtr<IDisconnectCause> disconnectCause;
    ((Call*) call)->GetDisconnectCause((IDisconnectCause**)&disconnectCause);
    Int32 code;
    disconnectCause->GetCode(&code);
    Boolean isCallCanceled = isNewlyDisconnected &&
            code == IDisconnectCause::CANCELED;
    // Log newly disconnected calls only if:
    // 1) It was not in the "choose account" phase when disconnected
    // 2) It is a conference call
    // 3) Call was not explicitly canceled
    Boolean isConference;
    ((Call*) call)->IsConference(&isConference);
    if (isNewlyDisconnected &&
            (oldState != ICallState::PRE_DIAL_WAIT &&
             !isConference &&
             !isCallCanceled)) {
        Int32 type;
        Boolean isIncoming;
        ((Call*) call)->IsIncoming(&isIncoming);
        if (!isIncoming) {
            type = ICalls::OUTGOING_TYPE;
        } else if (oldState == ICallState::RINGING) {
            type = ICalls::MISSED_TYPE;
        } else {
            type = ICalls::INCOMING_TYPE;
        }
        LogCall(call, type);
    }
    return NOERROR;
}

ECode CallLogManager::LogCall(
    /* [in] */ ICall* call,
    /* [in] */ Int32 callLogType)
{
    Int64 creationTime;
    ((Call*) call)->GetCreationTimeMillis(&creationTime);
    Int64 age;
    ((Call*) call)->GetDurationMillis(&age);
    String logNumber;
    GetLogNumber(call, &logNumber);
    Log::D(TAG, "logNumber set to: %s", Log::Pii(StringUtils::ParseCharSequence(logNumber)).string());
    Int32 presentation;
    GetPresentation(call, &presentation);
    AutoPtr<IPhoneAccountHandle> accountHandle;
    ((Call*) call)->GetTargetPhoneAccount((IPhoneAccountHandle**)&accountHandle);
    // TODO(vt): Once data usage is available, wire it up here.
    Int32 videoStateHistory;
    call->GetVideoStateHistory(&videoStateHistory);
    Int32 callFeatures;
    GetCallFeatures(videoStateHistory, &callFeatures);
    AutoPtr<ICallerInfo> callerInfo;
    ((Call*) call)->GetCallerInfo((ICallerInfo**)&callerInfo);
    LogCall(callerInfo, logNumber, presentation, callLogType, callFeatures,
            accountHandle, creationTime, age, NULL);
    return NOERROR;
}

ECode CallLogManager::LogCall(
    /* [in] */ ICallerInfo* callerInfo,
    /* [in] */ const String& number,
    /* [in] */ Int32 presentation,
    /* [in] */ Int32 callType,
    /* [in] */ Int32 features,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ Int64 start,
    /* [in] */ Int64 duration,
    /* [in] */ IInteger64* dataUsage)
{
    AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
    Boolean isEmergencyNumber;
    phoneNumberUtilsHelper->IsLocalEmergencyNumber(mContext, number, &isEmergencyNumber);
    // On some devices, to avoid accidental redialing of emergency numbers, we *never* log
    // emergency calls to the Call Log.  (This behavior is set on a per-product basis, based
    // on carrier requirements.)
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Boolean okToLogEmergencyNumber;
    resources->GetBoolean(R::bool_::allow_emergency_numbers_in_call_log, &okToLogEmergencyNumber);
    // Don't log emergency numbers if the device doesn't allow it.
    const Boolean isOkToLogThisCall = !isEmergencyNumber || okToLogEmergencyNumber;
    SendAddCallBroadcast(callType, duration);
    if (isOkToLogThisCall) {
        Log::D(TAG, "Logging Calllog entry: %s, %s, %d, %d, %lld, %lld", TO_CSTR(callerInfo),
                Log::Pii(StringUtils::ParseCharSequence(number)).string(), presentation, callType, start, duration);
        AutoPtr<AddCallArgs> args = new AddCallArgs(mContext, callerInfo, number, presentation,
                callType, features, accountHandle, start, duration, dataUsage);
        AutoPtr<AsyncTask> task;
        LogCallAsync(args, (AsyncTask**)&task);
    } else {
      Log::D(TAG, "Not adding emergency call to call log.");
    }
    return NOERROR;
}

ECode CallLogManager::GetCallFeatures(
    /* [in] */ Int32 videoState,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if ((videoState & IVideoProfileVideoState::TX_ENABLED)
            == IVideoProfileVideoState::TX_ENABLED) {
        *result = ICalls::FEATURES_VIDEO;
        return NOERROR;
    }
    *result = 0;
    return NOERROR;
}

ECode CallLogManager::GetLogNumber(
    /* [in] */ ICall* call,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IUri> handle;
    call->GetOriginalHandle((IUri**)&handle);
    if (handle == NULL) {
        *result = String(NULL);
        return NOERROR;
    }
    String handleString;
    handle->GetSchemeSpecificPart(&handleString);
    AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
    Boolean isUriNumber;
    phoneNumberUtilsHelper->IsUriNumber(handleString, &isUriNumber);
    if (!isUriNumber) {
        phoneNumberUtilsHelper->StripSeparators(handleString, &handleString);
    }
    *result = handleString;
    return NOERROR;
}

ECode CallLogManager::GetPresentation(
    /* [in] */ ICall* call,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IPhoneConstants::PRESENTATION_ALLOWED;
    return NOERROR;
}

ECode CallLogManager::LogCallAsync(
    /* [in] */ AddCallArgs* args,
    /* [out] */ AsyncTask** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<AsyncTask> asyncTask = new LogCallAsyncTask();
    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
    array->Set(0, TO_IINTERFACE(args));
    asyncTask->Execute(array);
    *result = asyncTask;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallLogManager::SendAddCallBroadcast(
    /* [in] */ Int32 callType,
    /* [in] */ Int64 duration)
{
    AutoPtr<IIntent> callAddIntent;
    CIntent::New(ACTION_CALLS_TABLE_ADD_ENTRY, (IIntent**)&callAddIntent);
    callAddIntent->PutExtra(CALL_TYPE, callType);
    callAddIntent->PutExtra(CALL_DURATION, duration);
    mContext->SendBroadcast(callAddIntent, PERMISSION_PROCESS_CALLLOG_INFO);
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
