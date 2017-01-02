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

#include "elastos/droid/animation/TypeConverter.h"

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL(TypeConverter, Object, ITypeConverter);

TypeConverter::TypeConverter()
{}

ECode TypeConverter::constructor(
    /* [in] */ const InterfaceID& fromClass,
    /* [in] */ const InterfaceID& toClass)
{
    mFromClass = fromClass;
    mToClass = toClass;
    return NOERROR;
}

ECode TypeConverter::GetTargetType(
    /* [out] */ InterfaceID* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mToClass;
    return NOERROR;
}

ECode TypeConverter::GetSourceType(
    /* [out] */ InterfaceID* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mFromClass;
    return NOERROR;
}

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

