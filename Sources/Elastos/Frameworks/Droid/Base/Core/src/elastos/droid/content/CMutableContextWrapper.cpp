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

#include "elastos/droid/content/CMutableContextWrapper.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CMutableContextWrapper, ContextWrapper, IMutableContextWrapper)

CAR_OBJECT_IMPL(CMutableContextWrapper)

ECode CMutableContextWrapper::GetBaseContext(
    /* [out] */ IContext** context)
{
    return ContextWrapper::GetBaseContext(context);
}

ECode CMutableContextWrapper::SetBaseContext(
    /* [in] */ IContext* base)
{
    mBase = base;
    return NOERROR;
}

ECode CMutableContextWrapper::constructor(
    /* [in] */ IContext* base)
{
    return ContextWrapper::constructor(base);
}

}
}
}

