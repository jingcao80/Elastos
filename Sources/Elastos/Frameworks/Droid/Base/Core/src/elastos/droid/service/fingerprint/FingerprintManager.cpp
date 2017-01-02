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

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/CBinder.h"
#include "elastos/droid/service/fingerprint/FingerprintManager.h"
#include "elastos/droid/service/fingerprint/FingerprintUtils.h"
#include "elastos/droid/provider/Settings.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::Settings;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Fingerprint {

////////////////////////////////FingerprintManager::MHandler///////////////////////////////

FingerprintManager::MHandler::MHandler(
    /* [in] */ FingerprintManager* host)
    : mHost(host)
{
}

ECode FingerprintManager::MHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    if (mHost->mClientReceiver != NULL) {
        Int32 what;
        msg->GetWhat(&what);
        switch(what) {
            case FingerprintManager::MSG_ENROLL_RESULT:
            {
                Int32 arg1, arg2;
                msg->GetArg1(&arg1);
                msg->GetArg2(&arg2);
                mHost->mClientReceiver->OnEnrollResult(arg1, arg2);
                break;
            }
            case FingerprintManager::MSG_ACQUIRED:
            {
                Int32 arg1;
                msg->GetArg1(&arg1);
                mHost->mClientReceiver->OnAcquired(arg1);
                break;
            }
            case FingerprintManager::MSG_PROCESSED:
            {
                Int32 arg1;
                msg->GetArg1(&arg1);
                mHost->mClientReceiver->OnProcessed(arg1);
                break;
            }
            case FingerprintManager::MSG_ERROR:
            {
                Int32 arg1;
                msg->GetArg1(&arg1);
                mHost->mClientReceiver->OnError(arg1);
                break;
            }
            case FingerprintManager::MSG_REMOVED:
            {
                Int32 arg1;
                msg->GetArg1(&arg1);
                mHost->mClientReceiver->OnRemoved(arg1);
                break;
            }
        }
    }
    return NOERROR;
}

////////////////////////////////FingerprintManager::MServiceReceiver///////////////////////////////

CAR_INTERFACE_IMPL_2(FingerprintManager::MServiceReceiver, Object, IIFingerprintServiceReceiver, IBinder)

FingerprintManager::MServiceReceiver::MServiceReceiver(
    /* [in] */ FingerprintManager* host)
    : mHost(host)
{
}

ECode FingerprintManager::MServiceReceiver::OnEnrollResult(
    /* [in] */ Int32 fingerprintId,
    /* [in] */ Int32 remaining)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(FingerprintManager::MSG_ENROLL_RESULT, fingerprintId, remaining, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode FingerprintManager::MServiceReceiver::OnAcquired(
    /* [in] */ Int32 acquireInfo)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(FingerprintManager::MSG_ACQUIRED, acquireInfo, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode FingerprintManager::MServiceReceiver::OnProcessed(
    /* [in] */ Int32 fingerprintId)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(FingerprintManager::MSG_PROCESSED, fingerprintId, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode FingerprintManager::MServiceReceiver::OnError(
    /* [in] */ Int32 error)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(FingerprintManager::MSG_ERROR, error, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode FingerprintManager::MServiceReceiver::OnRemoved(
    /* [in] */ Int32 fingerprintId)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(FingerprintManager::MSG_REMOVED, fingerprintId, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

////////////////////////////////FingerprintManager///////////////////////////////

const String FingerprintManager::TAG("FingerprintManager");
const Boolean FingerprintManager::DEBUG = TRUE;
const Int32 FingerprintManager::MSG_ENROLL_RESULT;
const Int32 FingerprintManager::MSG_ACQUIRED;
const Int32 FingerprintManager::MSG_PROCESSED;
const Int32 FingerprintManager::MSG_ERROR;
const Int32 FingerprintManager::MSG_REMOVED;

CAR_INTERFACE_IMPL(FingerprintManager, Object, IFingerprintManager)

FingerprintManager::FingerprintManager()
{
}

ECode FingerprintManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIFingerprintService* service)
{
    mContext = context;
    mService = service;
    if (mService == NULL) {
        Slogger::V(TAG, "FingerprintManagerService was null");
    }

    CBinder::New((IBinder**)&mToken);
    AutoPtr<MHandler> mh = new MHandler(this);
    mh->constructor();
    mHandler = IHandler::Probe(mh.Get());
    AutoPtr<MServiceReceiver> ms = new MServiceReceiver(this);
    ms->constructor();
    mServiceReceiver = IIFingerprintServiceReceiver::Probe(ms.Get());
    return NOERROR;
}

ECode FingerprintManager::EnrolledAndEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContentResolver> res;
    mContext->GetContentResolver((IContentResolver**)&res);
    Int32 i;
    Settings::Secure::GetInt32(res, String("fingerprint_enabled"), 0, &i);
    Int32 useid;
    GetCurrentUserId(&useid);
    AutoPtr<ArrayOf<Int32> > array;
    FingerprintUtils::GetFingerprintIdsForUser(res, useid, (ArrayOf<Int32>**)&array);
    *result = i != 0 && array->GetLength() > 0;
    return NOERROR;
}

ECode FingerprintManager::Enroll(
    /* [in] */ Int64 timeout)
{
    if (mServiceReceiver == NULL) {
        SendError(IFingerprintManager::FINGERPRINT_ERROR_NO_RECEIVER, 0, 0);
        return E_NULL_POINTER_EXCEPTION;
    }
    if (mService != NULL) {
        Int32 useid;
        GetCurrentUserId(&useid);
        ECode ec = mService->Enroll(mToken, timeout, useid);
        if (FAILED(ec)) {
            Logger::V(TAG, "Remote exception while enrolling: %d");
            SendError(IFingerprintManager::FINGERPRINT_ERROR_HW_UNAVAILABLE, 0, 0);
            return E_REMOTE_EXCEPTION;
        }
    }
    return NOERROR;
}


ECode FingerprintManager::Remove(
    /* [in] */ Int32 fingerprintId)
{
    if (mServiceReceiver == NULL) {
        SendError(IFingerprintManager::FINGERPRINT_ERROR_NO_RECEIVER, 0, 0);
        return E_NULL_POINTER_EXCEPTION;
    }
    if (mService != NULL) {
        Int32 useid;
        GetCurrentUserId(&useid);
        ECode ec = mService->Remove(mToken, fingerprintId, useid);
        if (FAILED(ec)) {
            Logger::V(TAG, "Remote exception during remove of fingerprintId: %d", fingerprintId);
            return E_REMOTE_EXCEPTION;
        }
    }
    else {
        Logger::W(TAG, "remove(): Service not connected!");
        SendError(IFingerprintManager::FINGERPRINT_ERROR_HW_UNAVAILABLE, 0, 0);
    }
    return NOERROR;
}


ECode FingerprintManager::StartListening(
    /* [in] */ IFingerprintManagerReceiver* receiver)
{
    mClientReceiver = receiver;
    if (mService != NULL) {
        Int32 useid;
        GetCurrentUserId(&useid);
        ECode ec = mService->StartListening(mToken, mServiceReceiver, useid);
        if (FAILED(ec)) {
            Logger::V(TAG, "Remote exception in startListening(): %d", ec);
            return E_REMOTE_EXCEPTION;
        }
    }
    else {
        Logger::W(TAG, "startListening(): Service not connected!");
        SendError(IFingerprintManager::FINGERPRINT_ERROR_HW_UNAVAILABLE, 0, 0);
    }
    return NOERROR;
}


ECode FingerprintManager::GetCurrentUserId(
    /* [out] */ Int32* userId)
{
    VALIDATE_NOT_NULL(userId)
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    AutoPtr<IUserInfo> info;
    ECode ec = am->GetCurrentUser((IUserInfo**)&info);
    if (FAILED(ec)) {
        Logger::W(TAG, "Failed to get current user id\n");
        *userId = IUserHandle::USER_NULL;
        return E_REMOTE_EXCEPTION;
    }
    info->GetId(userId);
    return NOERROR;
}

ECode FingerprintManager::StopListening()
{
    if (mService != NULL) {
        Int32 useid;
        GetCurrentUserId(&useid);
        ECode ec = mService->StopListening(mToken, useid);
        mClientReceiver = NULL;
        if (FAILED(ec)) {
            Logger::V(TAG, "Remote exception in stopListening(): %d", ec);
            return E_REMOTE_EXCEPTION;
        }
    }
    else {
        Logger::W(TAG, "stopListening(): Service not connected!");
        SendError(IFingerprintManager::FINGERPRINT_ERROR_HW_UNAVAILABLE, 0, 0);
    }
    return NOERROR;
}

ECode FingerprintManager::EnrollCancel()
{
    if (mServiceReceiver == NULL) {
        SendError(IFingerprintManager::FINGERPRINT_ERROR_NO_RECEIVER, 0, 0);
        return E_NULL_POINTER_EXCEPTION;
    }
    if (mService != NULL) {
            Int32 useid;
            GetCurrentUserId(&useid);
            ECode ec = mService->EnrollCancel(mToken, useid);
            mClientReceiver = NULL;
            if (FAILED(ec)) {
                Logger::V(TAG, "Remote exception in enrollCancel(): %d", ec);
                SendError(IFingerprintManager::FINGERPRINT_ERROR_HW_UNAVAILABLE, 0, 0);
                return E_REMOTE_EXCEPTION;
            }
    }
    else {
        Logger::W(TAG, "enrollCancel(): Service not connected!");
    }
    return NOERROR;
}

ECode FingerprintManager::SendError(
    /* [in] */ Int32 msg,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2)
{
    AutoPtr<IMessage> message;
    return mHandler->ObtainMessage(msg, arg1, arg2, (IMessage**)&message);
}

} // namespace Fingerprint
} // namespace Service
} // namepsace Droid
} // namespace Elastos