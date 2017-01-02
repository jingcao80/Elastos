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

#ifndef __ELASTOS_DROID_WEBKIT_CWEBVIEWHITTESTRESULT_H__
#define __ELASTOS_DROID_WEBKIT_CWEBVIEWHITTESTRESULT_H__

#include "_Elastos_Droid_Webkit_CWebViewHitTestResult.h"
#include "elastos/droid/webkit/WebView.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CWebViewHitTestResult)
    , public Object
    , public IWebViewHitTestResult
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * @hide Only for use by WebViewProvider implementations
     */
    CWebViewHitTestResult();

    CARAPI constructor();

    /**
     * @hide Only for use by WebViewProvider implementations
     */
    CARAPI SetType(
        /* [in] */ Int32 type);

    /**
     * @hide Only for use by WebViewProvider implementations
     */
    CARAPI SetExtra(
        /* [in] */ const String& extra);

    /**
     * Gets the type of the hit test result. See the XXX_TYPE constants
     * defined in this class.
     *
     * @return the type of the hit test result
     */
    CARAPI GetType(
        /* [out] */ Int32* type);

    /**
     * Gets additional type-dependant information about the result. See
     * {@link WebView#getHitTestResult()} for details. May either be null
     * or contain extra information about this result.
     *
     * @return additional type-dependant information about the result
     */
    CARAPI GetExtra(
        /* [out] */ String* extra);

private:
    Int32 mType;
    String mExtra;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CWEBVIEWHITTESTRESULT_H__
