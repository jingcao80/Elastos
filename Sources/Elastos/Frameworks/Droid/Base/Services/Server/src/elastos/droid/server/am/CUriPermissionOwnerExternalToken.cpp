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

#include "elastos/droid/server/am/CUriPermissionOwnerExternalToken.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL_2(CUriPermissionOwnerExternalToken, Object, IUriPermissionOwnerExternalToken, IBinder)

CAR_OBJECT_IMPL(CUriPermissionOwnerExternalToken)

ECode CUriPermissionOwnerExternalToken::constructor(
    /* [in] */ IObject* owner)
{
    mUriOwner = owner;
    return NOERROR;
}

ECode CUriPermissionOwnerExternalToken::GetOwner(
    /* [out] */ IObject** owner)
{
    VALIDATE_NOT_NULL(owner)
    *owner = mUriOwner;
    REFCOUNT_ADD(*owner)
    return NOERROR;
}

ECode CUriPermissionOwnerExternalToken::ToString(
    /* [out] */ String *str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb("UriPermissionOwner::ExternalToken: ");
    sb += (Int32)this;
    *str = sb.ToString();
    return NOERROR;
}

} // namespace Am
} // namespace Droid
} // namespace Droid
} // namespace Elastos