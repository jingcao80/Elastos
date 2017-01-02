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

#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_STOPREQUESTEXCEPTION_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_STOPREQUESTEXCEPTION_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

/**
 * Raised to indicate that the current request should be stopped immediately.
 *
 * Note the message passed to this exception will be logged and therefore must be guaranteed
 * not to contain any PII, meaning it generally can't include any information about the request
 * URI, headers, or destination filename.
 */
class StopRequestException
    : public Exception
{
public:
    StopRequestException(
        /* [in] */ Int32 finalStatus,
        /* [in] */ String message);

    StopRequestException(
        /* [in] */ Int32 finalStatus,
        /* [in] */ IThrowable* t);

    StopRequestException(
        /* [in] */ Int32 finalStatus,
        /* [in] */ String message,
        /* [in] */ IThrowable* t);

    CARAPI GetFinalStatus(
        /* [out] */ Int32* result);

    static CARAPI_(AutoPtr<IStopRequestException>) ThrowUnhandledHttpError(
        /* [in] */ Int32 code,
        /* [in] */ String message);

private:
    Int32 mFinalStatus;
};

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_STOPREQUESTEXCEPTION_H__
