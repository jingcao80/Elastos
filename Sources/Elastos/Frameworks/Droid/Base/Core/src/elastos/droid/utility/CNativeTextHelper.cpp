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

#include "elastos/droid/utility/CNativeTextHelper.h"
#include "elastos/droid/utility/NativeTextHelper.h"

using Elastos::Droid::Utility::EIID_INativeTextHelper;
using Elastos::Droid::Utility::NativeTextHelper;

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_SINGLETON_IMPL(CNativeTextHelper)

CAR_INTERFACE_IMPL(CNativeTextHelper, Singleton, INativeTextHelper)

ECode CNativeTextHelper::GetLocalString(
    /* [in] */ IContext* context,
    /* [in] */ const String& originalString,
    /* [in] */ Int32 originNamesId,
    /* [in] */ Int32 localNamesId,
    /* [out] */ String* result)
{
    return NativeTextHelper::GetLocalString(context, originalString, originNamesId, localNamesId, result);
}

ECode CNativeTextHelper::GetInternalLocalString(
    /* [in] */ IContext* context,
    /* [in] */ const String& originalString,
    /* [in] */ Int32 originNamesId,
    /* [in] */ Int32 localNamesId,
    /* [out] */ String* result)
{
    return NativeTextHelper::GetInternalLocalString(context, originalString, originNamesId, localNamesId, result);
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
