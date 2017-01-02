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

#include "elastos/droid/utility/CParcelableList.h"

using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_OBJECT_IMPL(CParcelableList)

CAR_INTERFACE_IMPL(CParcelableList, ArrayList, IParcelable)

ECode CParcelableList::constructor(
    /* [in] */ IList* list)
{
    return ArrayList::constructor(ICollection::Probe(list));
}

ECode CParcelableList::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Int32 size;
    GetSize(&size);
    dest->WriteInt32(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        Get(i, (IInterface**)&item);
        dest->WriteInterfacePtr(item);
    }

    return NOERROR;
}

ECode CParcelableList::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 size;
    source->ReadInt32(&size);
    EnsureCapacity(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        source->ReadInterfacePtr((Handle32*)&item);
        Add(item);
    }

    return NOERROR;
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos

