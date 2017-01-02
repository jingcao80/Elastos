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

#include "EventObject.h"
#include "StringUtils.h"

using Elastos::Core::StringUtils;
using Elastos::IO::EIID_ISerializable;

namespace Elastos{
namespace Utility{

CAR_INTERFACE_IMPL_2(EventObject, Object, IEventObject, ISerializable)

EventObject::EventObject()
{
}

EventObject::~EventObject()
{
}

ECode EventObject::constructor(
    /* [in] */ IObject* source)
{
    VALIDATE_NOT_NULL(source)
    mSource = source;
    return NOERROR;
}

ECode EventObject::GetSource(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    *object = mSource;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode EventObject::ToString(
    /* [in] */ String* str)
{
    // return getClass().getName() + "[source=" + source + ']';
    VALIDATE_NOT_NULL(str);
    str->AppendFormat("EventObject:[source=%s]", Object::ToString(mSource).string());
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
