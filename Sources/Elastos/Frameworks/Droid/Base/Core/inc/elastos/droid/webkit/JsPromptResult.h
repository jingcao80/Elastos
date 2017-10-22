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

#ifndef __ELASTOS_DROID_WEBKIT_JSPROMPTRESULT_H__
#define __ELASTOS_DROID_WEBKIT_JSPROMPTRESULT_H__

#include "elastos/droid/webkit/JsResult.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * Public class for handling JavaScript prompt requests. The WebChromeClient will receive a
 * {@link WebChromeClient#onJsPrompt(WebView, String, String, String, JsPromptResult)} call with a
 * JsPromptResult instance as a parameter. This parameter is used to return the result of this user
 * dialog prompt back to the WebView instance. The client can call cancel() to cancel the dialog or
 * confirm() with the user's input to confirm the dialog.
 */
class JsPromptResult
    : public JsResult
    , public IJsPromptResult
{
public:
    CAR_INTERFACE_DECL()

    JsPromptResult();

    /**
     * @hide Only for use by WebViewProvider implementations
     */
    JsPromptResult(
        /* [in] */ IJsResultReceiver* receiver);

    CARAPI constructor(
        /* [in] */ IJsResultReceiver* receiver);

    using JsResult::Confirm;

    /**
     * Handle a confirmation response from the user.
     */
    CARAPI Confirm(
        /* [in] */ const String& result);

    /**
     * @hide Only for use by WebViewProvider implementations
     */
    CARAPI GetStringResult(
        /* [out] */ String* result);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    // String result of the prompt
    String mStringResult;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_JSPROMPTRESULT_H__
