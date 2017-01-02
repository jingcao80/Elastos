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

#include "elastos/droid/webkit/webview/chromium/native/net/GURLUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/net/api/GURLUtils_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {

//=====================================================================
//                              GURLUtils
//=====================================================================
String GURLUtils::GetOrigin(
    /* [in] */ const String& url)
{
    // ==================before translated======================
    // return nativeGetOrigin(url);

    return NativeGetOrigin(url);
}

String GURLUtils::GetScheme(
    /* [in] */ const String& url)
{
    // ==================before translated======================
    // return nativeGetScheme(url);

    return NativeGetScheme(url);
}

String GURLUtils::NativeGetOrigin(
    /* [in] */ const String& url)
{
    return Elastos_GURLUtils_nativeGetOrigin(url);
}

String GURLUtils::NativeGetScheme(
    /* [in] */ const String& url)
{
    return Elastos_GURLUtils_nativeGetScheme(url);
}

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


