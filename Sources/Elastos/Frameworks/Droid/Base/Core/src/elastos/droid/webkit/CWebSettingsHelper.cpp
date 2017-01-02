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

#include "elastos/droid/webkit/CWebSettingsHelper.h"
#include "elastos/droid/webkit/WebSettings.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CWebSettingsHelper, Object, IWebSettingsHelper);

CAR_SINGLETON_IMPL(CWebSettingsHelper);

ECode CWebSettingsHelper::GetDefaultUserAgent(
    /* [in] */ IContext* context,
    /* [out] */ String* ua)
{
    VALIDATE_NOT_NULL(ua);
    *ua = WebSettings::GetDefaultUserAgent(context);
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
