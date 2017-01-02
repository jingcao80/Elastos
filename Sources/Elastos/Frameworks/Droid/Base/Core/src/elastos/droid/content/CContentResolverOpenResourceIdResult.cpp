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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/content/CContentResolverOpenResourceIdResult.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CContentResolverOpenResourceIdResult, Object, IContentResolverOpenResourceIdResult)

CAR_OBJECT_IMPL(CContentResolverOpenResourceIdResult)

CContentResolverOpenResourceIdResult::CContentResolverOpenResourceIdResult()
    : mRes(NULL)
    , mId(0)
{}

ECode CContentResolverOpenResourceIdResult::constructor(
    /* [in] */ IResources* res,
    /* [in] */ Int32 id)
{
    mRes = res;
    mId = id;
    return NOERROR;
}

ECode CContentResolverOpenResourceIdResult::GetResources(
    /* [out] */ IResources** resources)
{
    VALIDATE_NOT_NULL(resources)
    *resources = mRes;
    REFCOUNT_ADD(*resources)
    return NOERROR;
}

ECode CContentResolverOpenResourceIdResult::SetResources(
    /* [in] */ IResources* resources)
{
    mRes = resources;
    return NOERROR;
}

ECode CContentResolverOpenResourceIdResult::GetResourceId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mId;
    return NOERROR;
}

ECode CContentResolverOpenResourceIdResult::SetResourceId(
    /* [in] */ Int32 id)
{
    mId = id;
    return NOERROR;
}

}
}
}

