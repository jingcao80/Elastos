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

#include "NodeChangeEvent.h"

namespace Elastos {
namespace Utility {
namespace Prefs {


CAR_INTERFACE_IMPL(NodeChangeEvent, EventObject, INodeChangeEvent)

NodeChangeEvent::NodeChangeEvent()
{
}

NodeChangeEvent::~NodeChangeEvent()
{
}

ECode NodeChangeEvent::constructor(
    /* [in] */ IPreferences* p,
    /* [in] */ IPreferences* c)
{
    FAIL_RETURN(EventObject::constructor(IObject::Probe(p)))

    mParent = p;
    mChild = c;
    return NOERROR;
}

ECode NodeChangeEvent::GetParent(
    /* [out] */ IPreferences** parent)
{
    VALIDATE_NOT_NULL(parent);
    *parent = mParent;
    REFCOUNT_ADD(*parent);
    return NOERROR;
}

ECode NodeChangeEvent::GetChild(
    /* [out] */ IPreferences** child)
{
    VALIDATE_NOT_NULL(child);
    *child = mChild;
    REFCOUNT_ADD(*child);
    return NOERROR;
}

ECode NodeChangeEvent::WriteObject(
    /* [in] */ IObjectOutputStream* out)/* throws IOException */
{
    return E_NOT_SERIALIZABLE_EXCEPTION;
}

/**
 * This method always throws a <code>NotSerializableException</code>,
 * because this object cannot be serialized,
 */
ECode NodeChangeEvent::ReadObject(
    /* [in] */ IObjectInputStream* in) /*throws IOException, ClassNotFoundException*/
{
    return E_NOT_SERIALIZABLE_EXCEPTION;
}

} // namespace Prefs
} // namespace Utility
} // namespace Elastos
