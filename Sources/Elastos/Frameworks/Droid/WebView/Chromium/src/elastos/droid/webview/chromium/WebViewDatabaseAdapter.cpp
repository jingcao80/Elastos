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

#include "elastos/droid/webkit/webview/chromium/WebViewDatabaseAdapter.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

//=====================================================================
//                        WebViewDatabaseAdapter
//=====================================================================
CAR_INTERFACE_IMPL(WebViewDatabaseAdapter, Object, IWebViewDatabase)

WebViewDatabaseAdapter::WebViewDatabaseAdapter(
    /* [in] */ AwFormDatabase* formDatabase,
    /* [in] */ HttpAuthDatabase* httpAuthDatabase)
    : mFormDatabase(formDatabase)
    , mHttpAuthDatabase(httpAuthDatabase)
{
    // ==================before translated======================
    // mFormDatabase = formDatabase;
    // mHttpAuthDatabase = httpAuthDatabase;
}

ECode WebViewDatabaseAdapter::HasUsernamePassword(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // This is a deprecated API: intentional no-op.
    // return false;

    *result = FALSE;
    return NOERROR;
}

ECode WebViewDatabaseAdapter::ClearUsernamePassword()
{
    // ==================before translated======================
    // // This is a deprecated API: intentional no-op.

    return NOERROR;
}

ECode WebViewDatabaseAdapter::HasHttpAuthUsernamePassword(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mHttpAuthDatabase.hasHttpAuthUsernamePassword();

    *result = mHttpAuthDatabase->HasHttpAuthUsernamePassword();
    return NOERROR;
}

ECode WebViewDatabaseAdapter::ClearHttpAuthUsernamePassword()
{
    // ==================before translated======================
    // mHttpAuthDatabase.clearHttpAuthUsernamePassword();

    mHttpAuthDatabase->ClearHttpAuthUsernamePassword();
    return NOERROR;
}

ECode WebViewDatabaseAdapter::HasFormData(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mFormDatabase.hasFormData();

    *result = mFormDatabase->HasFormData();
    return NOERROR;
}

ECode WebViewDatabaseAdapter::ClearFormData()
{
    // ==================before translated======================
    // mFormDatabase.clearFormData();

    mFormDatabase->ClearFormData();
    return NOERROR;
}

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


