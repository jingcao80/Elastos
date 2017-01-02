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

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_CERTVERIFYSTATUSANDROID_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_CERTVERIFYSTATUSANDROID_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.net;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {

class CertVerifyStatusElastos
    : public Object
{
public:
    static const Int32 VERIFY_OK = 0;
    static const Int32 VERIFY_FAILED = -1;
    static const Int32 VERIFY_NO_TRUSTED_ROOT = -2;
    static const Int32 VERIFY_EXPIRED = -3;
    static const Int32 VERIFY_NOT_YET_VALID = -4;
    static const Int32 VERIFY_UNABLE_TO_PARSE = -5;
    static const Int32 VERIFY_INCORRECT_KEY_USAGE = -6;
};

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_CERTVERIFYSTATUSANDROID_H_

