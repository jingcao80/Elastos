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

#include "elastos/droid/webkit/JsPromptResult.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(JsPromptResult, Object, IJsPromptResult);

JsPromptResult::JsPromptResult()
{
}

JsPromptResult::JsPromptResult(
    /* [in] */ IJsResultReceiver* receiver)
    : JsResult(receiver)
{
}

ECode JsPromptResult::constructor(
    /* [in] */ IJsResultReceiver* receiver)
{
    return JsResult::constructor(receiver);
}


/**
 * Handle a confirmation response from the user.
 */
ECode JsPromptResult::Confirm(
    /* [in] */ const String& result)
{
    mStringResult = result;
    return JsResult::Confirm();
}

/**
 * @hide Only for use by WebViewProvider implementations
 */
ECode JsPromptResult::GetStringResult(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStringResult;
    return NOERROR;
}

ECode JsPromptResult::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "JsPromptResult";
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos