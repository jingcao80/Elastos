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

#include "elastos/droid/providers/downloads/StopRequestException.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

StopRequestException::StopRequestException(
    /* [in] */ Int32 finalStatus,
    /* [in] */ String message)
{
    StopRequestException(message);
    mFinalStatus = finalStatus;
}

StopRequestException::StopRequestException(
    /* [in] */ Int32 finalStatus,
    /* [in] */ IThrowable* t)
{
    String msg;
    t->GetMessage(&msg);
    StopRequestException(finalStatus, msg);
    InitCause(t);
}

StopRequestException::StopRequestException(
    /* [in] */ Int32 finalStatus,
    /* [in] */ String message,
    /* [in] */ IThrowable* t)
{
    StopRequestException(finalStatus, message);
    InitCause(t);
}

ECode StopRequestException::GetFinalStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFinalStatus;
    return NOERROR;
}

AutoPtr<IStopRequestException> StopRequestException::ThrowUnhandledHttpError(
    /* [in] */ Int32 code,
    /* [in] */ String message)
{
    String error("Unhandled HTTP response: ");
    error += code;
    error += " ";
    error += message;
    if (code >= 400 && code < 600) {
        // throw new StopRequestException(code, error);
        return NULL;
    }
    else if (code >= 300 && code < 400) {
        // throw new StopRequestException(STATUS_UNHANDLED_REDIRECT, error);
        return NULL;
    }
    else {
        // throw new StopRequestException(STATUS_UNHANDLED_HTTP_CODE, error);
        return NULL;
    }
}

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos
