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

#ifndef __ELASTOS_DROID_WEBKIT_CWEBVIEWCLIENT_H__
#define __ELASTOS_DROID_WEBKIT_CWEBVIEWCLIENT_H__

#include "_Elastos_Droid_Webkit_CWebViewClient.h"
#include "elastos/droid/webkit/WebViewClient.h"

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Net::Http::ISslError;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CWebViewClient), public WebViewClient
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CWEBVIEWCLIENT_H__
