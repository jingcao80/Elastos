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

#include "elastos/droid/webkit/CWebViewHitTestResult.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CWebViewHitTestResult, Object, IWebViewHitTestResult);

CAR_OBJECT_IMPL(CWebViewHitTestResult);

CWebViewHitTestResult::CWebViewHitTestResult()
    : mType(UNKNOWN_TYPE)
{
}

ECode CWebViewHitTestResult::constructor()
{
    return NOERROR;
}

ECode CWebViewHitTestResult::SetType(
    /* [in] */ Int32 type)
{
    mType = type;
    return NOERROR;
}

ECode CWebViewHitTestResult::SetExtra(
    /* [in] */ const String& extra)
{
    mExtra = extra;
    return NOERROR;
}

ECode CWebViewHitTestResult::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    return NOERROR;
}

ECode CWebViewHitTestResult::GetExtra(
    /* [out] */ String* extra)
{
    VALIDATE_NOT_NULL(extra);
    *extra = mExtra;
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos