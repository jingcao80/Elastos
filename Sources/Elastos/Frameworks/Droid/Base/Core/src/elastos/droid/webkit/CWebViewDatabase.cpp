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

#include "elastos/droid/webkit/CWebViewDatabase.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

ECode CWebViewDatabase::HasUsernamePassword(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = WebViewDatabase::HasUsernamePassword();
    return NOERROR;
}

ECode CWebViewDatabase::ClearUsernamePassword()
{
    WebViewDatabase::ClearUsernamePassword();
    return NOERROR;
}

ECode CWebViewDatabase::HasHttpAuthUsernamePassword(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = WebViewDatabase::HasHttpAuthUsernamePassword();
    return NOERROR;
}

ECode CWebViewDatabase::ClearHttpAuthUsernamePassword()
{
    WebViewDatabase::ClearHttpAuthUsernamePassword();
    return NOERROR;
}

ECode CWebViewDatabase::HasFormData(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = WebViewDatabase::HasFormData();
    return NOERROR;
}

ECode CWebViewDatabase::ClearFormData()
{
    WebViewDatabase::ClearFormData();
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

