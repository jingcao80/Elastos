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

#include "CAnnotation.h"
#include "StringBuilder.h"
#include "StringUtils.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CAnnotation, Object, IAnnotation)

CAR_OBJECT_IMPL(CAnnotation)

ECode CAnnotation::constructor(
    /* [in] */ IInterface * attribute)
{
    mValue = attribute;
    return NOERROR;
}

ECode CAnnotation::GetValue(
    /* [out] */ IInterface ** value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CAnnotation::ToString(
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CAnnotation[value=");
    IObject* obj = IObject::Probe(mValue);
    String objInfo;
    if (obj != NULL) {
        obj->ToString(&objInfo);
    }
    else {
        objInfo = StringUtils::ToHexString((Int32)mValue.Get());
    }
    sb.Append(objInfo);

    *str = sb.ToString();
    return NOERROR;
}

} // namespace Text
} // namespace Elastos

