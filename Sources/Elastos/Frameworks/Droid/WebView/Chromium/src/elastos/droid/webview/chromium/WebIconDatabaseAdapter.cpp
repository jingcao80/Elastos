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

#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContents.h"
#include "elastos/droid/webkit/webview/chromium/WebIconDatabaseAdapter.h"

using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwContents;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

//=====================================================================
//                        WebIconDatabaseAdapter
//=====================================================================
CAR_INTERFACE_IMPL(WebIconDatabaseAdapter, Object, IWebIconDatabase)

ECode WebIconDatabaseAdapter::Open(
    /* [in] */ const String& path)
{
    // ==================before translated======================
    // AwContents.setShouldDownloadFavicons();

    AwContents::SetShouldDownloadFavicons();
    return NOERROR;
}

ECode WebIconDatabaseAdapter::Close()
{
    // ==================before translated======================
    // // Intentional no-op.

    return NOERROR;
}

ECode WebIconDatabaseAdapter::RemoveAllIcons()
{
    // ==================before translated======================
    // // Intentional no-op: we have no database so nothing to remove.

    return NOERROR;
}

ECode WebIconDatabaseAdapter::RequestIconForPageUrl(
    /* [in] */ const String& url,
    /* [in] */ IWebIconDatabaseIconListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // // Intentional no-op.

    return NOERROR;
}

ECode WebIconDatabaseAdapter::BulkRequestIconForPageUrl(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& where,
    /* [in] */ IWebIconDatabaseIconListener* listener)
{
    VALIDATE_NOT_NULL(cr);
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // // Intentional no-op: hidden in base class.

    return NOERROR;
}

ECode WebIconDatabaseAdapter::RetainIconForPageUrl(
    /* [in] */ const String& url)
{
    // ==================before translated======================
    // // Intentional no-op.

    return NOERROR;
}

ECode WebIconDatabaseAdapter::ReleaseIconForPageUrl(
    /* [in] */ const String& url)
{
    // ==================before translated======================
    // // Intentional no-op.

    return NOERROR;
}

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


