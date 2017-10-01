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

#include "AbstractSet.h"

using Elastos::Utility::ISet;
using Elastos::Utility::EIID_ISet;

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL(AbstractSet, AbstractCollection, ISet)

AbstractSet::AbstractSet()
{
}

AbstractSet::~AbstractSet()
{
}

ECode AbstractSet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    if (TO_IINTERFACE(this) == object) {
        *value = TRUE;
        return NOERROR;
    }
    if (ISet::Probe(object)) {
        AutoPtr<ISet> s = (ISet*) object->Probe(EIID_ISet);
        // try {
        Int32 value1 = 0;
        Int32 value2 = 0;
        GetSize(&value1);
        (ICollection::Probe(s))->GetSize(&value2);
        Boolean isflag = FALSE;
        *value = value1 == value2 && (ContainsAll(ICollection::Probe(s), &isflag), isflag);
        return NOERROR;
        // } catch (NullPointerException ignored) {
        //     return false;
        // } catch (ClassCastException ignored) {
        //     return false;
        // }
    }
    *value = FALSE;
    return NOERROR;
}

ECode AbstractSet::GetHashCode(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    Int32 result = 0;
    AutoPtr<IIterator> it;
    GetIterator((IIterator**)&it);
    Boolean isflag = FALSE;
    while (it->HasNext(&isflag), isflag) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        result += next == NULL ? 0 : Object::GetHashCode(next);
    }
    *value = result;
    return NOERROR;
}

ECode AbstractSet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    Boolean result = FALSE;
    Int32 value1 = 0;
    Int32 value2 = 0;
    if ((GetSize(&value1), value1) <= (collection->GetSize(&value2), value2)) {
        AutoPtr<IIterator> it;
        GetIterator((IIterator**)&it);
        Boolean isflag = FALSE;
        while (it->HasNext(&isflag), isflag) {
            AutoPtr<IInterface> next;
            it->GetNext((IInterface**)&next);
            if (collection->Contains(next, &isflag), isflag) {
                it->Remove();
                result = TRUE;
            }
        }
    }
    else {
        AutoPtr<IIterator> it;
        (IIterable::Probe(collection))->GetIterator((IIterator**)&it);
        Boolean isflag = FALSE;
        while (it->HasNext(&isflag), isflag) {
            AutoPtr<IInterface> next;
            it->GetNext((IInterface**)&next);
            Boolean rmflag = FALSE;
            result = (Remove(next, &rmflag), rmflag) || result;
        }
    }
    *value = result;
    return NOERROR;
}

ECode AbstractSet::GetIterator(
    /* [out] */ IIterator** it)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode AbstractSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::Add(object, modified);
}

ECode AbstractSet::Add(
    /* [in] */ IInterface* object)
{
    return AbstractCollection::Add(object);
}

ECode AbstractSet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::AddAll(collection, modified);
}

ECode AbstractSet::AddAll(
    /* [in] */ ICollection* collection)
{
    return AbstractCollection::AddAll(collection);
}

ECode AbstractSet::Clear()
{
    return AbstractCollection::Clear();
}

ECode AbstractSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::Contains(object, result);
}

ECode AbstractSet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::ContainsAll(collection, result);
}

ECode AbstractSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractCollection::IsEmpty(result);
}

ECode AbstractSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::Remove(object, modified);
}

ECode AbstractSet::Remove(
    /* [in] */ IInterface* object)
{
    return AbstractCollection::Remove(object);
}

ECode AbstractSet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return AbstractCollection::RemoveAll(collection);
}

ECode AbstractSet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::RetainAll(collection, modified);
}

ECode AbstractSet::RetainAll(
    /* [in] */ ICollection* collection)
{
    return AbstractCollection::RetainAll(collection);
}

ECode AbstractSet::GetSize(
    /* [out] */ Int32* size)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode AbstractSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractCollection::ToArray(array);
}

ECode AbstractSet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractCollection::ToArray(inArray, outArray);
}

} // namespace Utility
} // namespace Elastos
