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

#include "elastos/droid/internal/telephony/dataconnection/DcFailBringUp.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/internal/telephony/dataconnection/DcFailCause.h"
#include <Elastos.Droid.Content.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

const String DcFailBringUp::INTENT_BASE("Elastos.Droid.Internal.Telephony.DataConnection");
const String DcFailBringUp::LOG__TAG("DcFailBringUp");
const Boolean DcFailBringUp::DBG = TRUE;
const String DcFailBringUp::ACTION_FAIL_BRINGUP("action_fail_bringup");
const String DcFailBringUp::COUNTER("counter");
const Int32 DcFailBringUp::DEFAULT_COUNTER = 1;
const String DcFailBringUp::FAIL_CAUSE("fail_cause");
const EnumDcFailCause DcFailBringUp::DEFAULT_FAIL_CAUSE = DcFailCause_ERROR_UNSPECIFIED;
const String DcFailBringUp::SUGGESTED_RETRY_TIME("suggested_retry_time");
const Int32 DcFailBringUp::DEFAULT_SUGGESTED_RETRY_TIME = -1;

ECode DcFailBringUp::SaveParameters(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& s)
{
    String action;
    intent->GetAction(&action);
    if (DBG) Log("%s.saveParameters: action=%s", s.string(), action.string());
    intent->GetInt32Extra(COUNTER, DEFAULT_COUNTER, &mCounter);
    Int32 enumValue;
    intent->GetInt32Extra(FAIL_CAUSE, DEFAULT_FAIL_CAUSE, &enumValue);
    DcFailCause::FromInt32(enumValue, (IDcFailCause**)&mFailCause);
    intent->GetInt32Extra(SUGGESTED_RETRY_TIME, DEFAULT_SUGGESTED_RETRY_TIME, &mSuggestedRetryTime);
    if (DBG) {
        Log("%s.saveParameters: %s", s.string(), TO_CSTR(TO_IINTERFACE(this)));
    }
    return NOERROR;
}

ECode DcFailBringUp::SaveParameters(
    /* [in] */ Int32 counter,
    /* [in] */ Int32 failCause,
    /* [in] */ Int32 suggestedRetryTime)
{
    mCounter = counter;
    DcFailCause::FromInt32(failCause, (IDcFailCause**)&mFailCause);
    mSuggestedRetryTime = suggestedRetryTime;
    return NOERROR;
}

ECode DcFailBringUp::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String rev;
    rev.AppendFormat("{mCounter=%d mFailCause=%s mSuggestedRetryTime=%d}",
            mCounter, TO_CSTR(mFailCause), mSuggestedRetryTime);
    *result = rev;
    return NOERROR;
}

#define MSG_BUF_SIZE    1024
ECode DcFailBringUp::Log(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return Logger::D(LOG__TAG, msgBuf);
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
