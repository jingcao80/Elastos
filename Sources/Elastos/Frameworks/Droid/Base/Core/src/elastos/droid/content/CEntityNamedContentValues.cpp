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

#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/CEntityNamedContentValues.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CEntityNamedContentValues, Object, IEntityNamedContentValues)

CAR_OBJECT_IMPL(CEntityNamedContentValues)

CEntityNamedContentValues::CEntityNamedContentValues()
{}

CEntityNamedContentValues::~CEntityNamedContentValues()
{}

ECode CEntityNamedContentValues::constructor(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values)
{
    mUri = uri;
    mContentValues = values;
    return NOERROR;
}

ECode CEntityNamedContentValues::GetUri(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = mUri;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CEntityNamedContentValues::GetValues(
    /* [out] */ IContentValues** values)
{
    VALIDATE_NOT_NULL(values)
    *values = mContentValues;
    REFCOUNT_ADD(*values);
    return NOERROR;
}

}
}
}

