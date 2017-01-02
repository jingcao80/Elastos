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

#ifndef __ELASTOS_DROID_WEBKIT_JSRESULT_H__
#define __ELASTOS_DROID_WEBKIT_JSRESULT_H__

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * An instance of this class is passed as a parameter in various {@link WebChromeClient} action
 * notifications. The object is used as a handle onto the underlying JavaScript-originated request,
 * and provides a means for the client to indicate whether this action should proceed.
 */
class JsResult
    : public Object
    , public IJsResult
{
public:
    CAR_INTERFACE_DECL()

    JsResult();

    /**
     * @hide Only for use by WebViewProvider implementations
     */
    JsResult(
        /* [in] */ IJsResultReceiver* receiver);

    CARAPI constructor(
        /* [in] */ IJsResultReceiver* receiver);

    /**
     * Handle the result if the user cancelled the dialog.
     */
    CARAPI Cancel();

    /**
     * Handle a confirmation response from the user.
     */
    CARAPI Confirm();

    /**
     * @hide Only for use by WebViewProvider implementations
     */
    CARAPI GetResult(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* info);

protected:
    /* Notify the caller that the JsResult has completed */
    virtual CARAPI_(void) WakeUp();

protected:
    // This is the caller of the prompt and is the object that is waiting.
    AutoPtr<IJsResultReceiver> mReceiver;

private:
    // This is a basic result of a confirm or prompt dialog.
    Boolean mResult;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_JSRESULT_H__
