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

#ifndef __ELASTOS_UTILITY_EVENTOBJECT_H__
#define __ELASTOS_UTILITY_EVENTOBJECT_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Utility {

class EventObject
    : public Object
    , public IEventObject
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    EventObject();

    virtual ~EventObject();

    /**
     * Constructs a new instance of this class.
     *
     * @param source
     *            the object which fired the event.
     */
    CARAPI constructor(
        /* [in] */ IObject* source);

    /**
     * Returns the object which fired the event.
     */
    virtual CARAPI GetSource(
        /* [out] */ IInterface** object);

    /**
     * Returns the string representation of this {@code EventObject}.
     */
    CARAPI ToString(
        /* [in] */ String* str);

protected:
    /*transient*/ AutoPtr<IObject> mSource;
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_EVENTOBJECT_H__
