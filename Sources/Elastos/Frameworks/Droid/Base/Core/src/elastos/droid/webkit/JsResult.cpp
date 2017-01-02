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

#include "elastos/droid/webkit/JsResult.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(JsResult, Object, IJsResult);

JsResult::JsResult()
    : mResult(FALSE)
{
}

JsResult::JsResult(
    /* [in] */ IJsResultReceiver* receiver)
    : mReceiver(receiver)
    , mResult(FALSE)
{
}

ECode JsResult::constructor(
    /* [in] */ IJsResultReceiver* receiver)
{
    mReceiver = receiver;
    mResult = FALSE;
    return NOERROR;
}

/**
 * Handle the result if the user cancelled the dialog.
 */
ECode JsResult::Cancel()
{
    mResult = FALSE;
    WakeUp();
    return NOERROR;
}

/**
 * Handle a confirmation response from the user.
 */
ECode JsResult::Confirm()
{
    mResult = TRUE;
    WakeUp();
    return NOERROR;
}

/**
 * @hide Only for use by WebViewProvider implementations
 */
ECode JsResult::GetResult(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mResult;
    return NOERROR;
}

/* Notify the caller that the JsResult has completed */
void JsResult::WakeUp()
{
    mReceiver->OnJsResultComplete(IJsResult::Probe(this));
}

ECode JsResult::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "JsResult";
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
