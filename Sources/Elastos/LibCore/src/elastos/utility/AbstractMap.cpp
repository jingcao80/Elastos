
#include "AbstractMap.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;
using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {

//==========================================================
//       AbstractMap
//==========================================================
CAR_INTERFACE_IMPL(AbstractMap, Object, IMap)

ECode AbstractMap::Clear()
{
    AutoPtr<ISet> outset;
    GetEntrySet((ISet**)&outset);
    return (ICollection::Probe(outset))->Clear();
}

ECode AbstractMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISet> outset;
    GetEntrySet((ISet**)&outset);
    AutoPtr<IIterator> it;
    (IIterable::Probe(outset))->GetIterator((IIterator**)&it);
    Boolean isflag = FALSE;
    if (key != NULL) {
        while ((it->HasNext(&isflag), isflag)) {
            AutoPtr<IInterface> outface;
            it->GetNext((IInterface**)&outface);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
            AutoPtr<IInterface> entrykey;
            entry->GetKey((IInterface**)&entrykey);
            if (Object::Equals(key, entrykey)) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    else {
        while ((it->HasNext(&isflag), isflag)) {
            AutoPtr<IInterface> outface;
            it->GetNext((IInterface**)&outface);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
            AutoPtr<IInterface> entrykey;
            entry->GetKey((IInterface**)&entrykey);
            if (entrykey == NULL) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode AbstractMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISet> entries;
    GetEntrySet((ISet**)&entries);
    AutoPtr<IIterator> it;
    (IIterable::Probe(entries))->GetIterator((IIterator**)&it);
    Boolean isflag = FALSE;
    if (value != NULL) {
        while ((it->HasNext(&isflag), isflag)) {
            AutoPtr<IInterface> outface;
            it->GetNext((IInterface**)&outface);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
            AutoPtr<IInterface> entryvalue;
            entry->GetValue((IInterface**)&entryvalue);
            if (Object::Equals(value, entryvalue)) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    else {
        while ((it->HasNext(&isflag), isflag)) {
            AutoPtr<IInterface> outface;
            it->GetNext((IInterface**)&outface);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
            AutoPtr<IInterface> entryvalue;
            entry->GetValue((IInterface**)&entryvalue);
            if (entryvalue == NULL) {
                *result = TRUE;
                return NOERROR;
            }
        }

    }
    *result = FALSE;
    return NOERROR;
}

ECode AbstractMap::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (this->Probe(EIID_IInterface) == object) {
        *result = TRUE;
        return NOERROR;
    }
    if (IMap::Probe(object)) {
        AutoPtr<IMap> map = (IMap*) object->Probe(EIID_IMap);
        Int32 len1 = 0;
        Int32 len2 = 0;
        if ((GetSize(&len1), len1) != (map->GetSize(&len2), len2)) {
            *result = FALSE;
            return NOERROR;
        }

        // try {
        AutoPtr< ArrayOf<IInterface*> > entries;
        AutoPtr<ISet> outset;
        GetEntrySet((ISet**)&outset);
        (ICollection::Probe(outset))->ToArray((ArrayOf<IInterface*>**)&entries);
        for (Int32 i = 0; i < entries->GetLength(); i++) {
            AutoPtr<IMapEntry> entry = IMapEntry::Probe((*entries)[i]);
            AutoPtr<IInterface> key;
            entry->GetKey((IInterface**)&key);
            AutoPtr<IInterface> mine;
            entry->GetValue((IInterface**)&mine);
            AutoPtr<IInterface> theirs;
            map->Get(key, (IInterface**)&theirs);
            Boolean isflag = FALSE;
            if (mine == NULL) {
                if (theirs != NULL || (map->ContainsKey(key, &isflag), !isflag)) {
                    *result = FALSE;
                    return NOERROR;
                }
            }
            else if (!Object::Equals(mine, theirs)) {
                *result = FALSE;
                return NOERROR;
            }
        }
        // } catch (NullPointerException ignored) {
        //     return false;
        // } catch (ClassCastException ignored) {
        //     return false;
        // }
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode AbstractMap::Get(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr<ISet> entries;
    GetEntrySet((ISet**)&entries);
    AutoPtr<IIterator> it;
    (IIterable::Probe(entries))->GetIterator((IIterator**)&it);
    Boolean isflag = FALSE;
    if (key != NULL) {
        while (it->HasNext(&isflag), isflag) {
            AutoPtr<IInterface> outface;
            it->GetNext((IInterface**)&outface);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
            AutoPtr<IInterface> entkey;
            entry->GetKey((IInterface**)&entkey);
            if (Object::Equals(key, entkey)) {
                AutoPtr<IInterface> entvalue;
                entry->GetValue((IInterface**)&entvalue);
                *value = entvalue;
                REFCOUNT_ADD(*value)
                return NOERROR;
            }
        }
    }
    else {
        while (it->HasNext(&isflag), isflag) {
            AutoPtr<IInterface> outface;
            it->GetNext((IInterface**)&outface);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
            AutoPtr<IInterface> entkey;
            entry->GetKey((IInterface**)&entkey);
            if (entkey == NULL) {
                AutoPtr<IInterface> entvalue;
                entry->GetValue((IInterface**)&entvalue);
                *value = entvalue;
                REFCOUNT_ADD(*value)
                return NOERROR;
            }
        }
    }
    *value = NULL;
    return NOERROR;
}

ECode AbstractMap::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    *hashCode = 0;
    AutoPtr<ISet> entries;
    GetEntrySet((ISet**)&entries);
    AutoPtr<IIterator> it;
    (IIterable::Probe(entries))->GetIterator((IIterator**)&it);
    Boolean isflag = FALSE;
    while (it->HasNext(&isflag), isflag) {
        Int32 codevalue = 0;
        AutoPtr<IInterface> outface;
        it->GetNext((IInterface**)&outface);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
        entry->GetHashCode(&codevalue);
        *hashCode += codevalue;
    }

    return NOERROR;
}

ECode AbstractMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 value = 0;
    *result = (GetSize(&value), value) == 0;
    return NOERROR;
}

ECode AbstractMap::GetKeySet(
    /* [out] */ ISet** keySet)
{
    VALIDATE_NOT_NULL(keySet)

    if (mKeySet == NULL) {
        mKeySet = (ISet*) new AbstractMapKeySet(this);
    }

    *keySet = mKeySet;
    REFCOUNT_ADD(*keySet)
    return NOERROR;
}

ECode AbstractMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* oldValue)
{
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode AbstractMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode AbstractMap::PutAll(
    /* [in] */ IMap* map)
{
    AutoPtr< ArrayOf<IInterface*> > entries;
    AutoPtr<ISet> outset;
    map->GetEntrySet((ISet**)&outset);
    (ICollection::Probe(outset))->ToArray((ArrayOf<IInterface*>**)&entries);
    for (Int32 i = 0; i < entries->GetLength(); i++) {
        AutoPtr<IMapEntry> entry = IMapEntry::Probe((*entries)[i]);
        AutoPtr<IInterface> entkey;
        AutoPtr<IInterface> entvalue;
        entry->GetKey((IInterface**)&entkey);
        entry->GetValue((IInterface**)&entvalue);
        AutoPtr<IInterface> oldValue;
        Put(entkey, entvalue, (IInterface**)&oldValue);
    }
    return NOERROR;
}

ECode AbstractMap::Remove(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr<ISet> entries;
    GetEntrySet((ISet**)&entries);
    AutoPtr<IIterator> it;
    (IIterable::Probe(entries))->GetIterator((IIterator**)&it);
    Boolean isflag = FALSE;
    if (key != NULL) {
        while (it->HasNext(&isflag), isflag) {
            AutoPtr<IInterface> outface;
            it->GetNext((IInterface**)&outface);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
            AutoPtr<IInterface> entkey;
            entry->GetKey((IInterface**)&entkey);
            if (Object::Equals(key, entkey)) {
                it->Remove();
                AutoPtr<IInterface> entvalue;
                entry->GetValue((IInterface**)&entvalue);
                *value = entvalue;
                REFCOUNT_ADD(*value)
                return NOERROR;
            }
        }
    }
    else {
        while (it->HasNext(&isflag), isflag) {
            AutoPtr<IInterface> outface;
            it->GetNext((IInterface**)&outface);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
            AutoPtr<IInterface> entkey;
            entry->GetKey((IInterface**)&entkey);
            if (entkey == NULL) {
                it->Remove();
                AutoPtr<IInterface> entvalue;
                entry->GetValue((IInterface**)&entvalue);
                *value = entvalue;
                REFCOUNT_ADD(*value)
                return NOERROR;
            }
        }
    }
    *value = NULL;
    return NOERROR;
}

ECode AbstractMap::Remove(
    /* [in] */ PInterface key)
{
    AutoPtr<ISet> entries;
    GetEntrySet((ISet**)&entries);
    AutoPtr<IIterator> it;
    (IIterable::Probe(entries))->GetIterator((IIterator**)&it);
    Boolean isflag = FALSE;
    if (key != NULL) {
        while (it->HasNext(&isflag), isflag) {
            AutoPtr<IInterface> outface;
            it->GetNext((IInterface**)&outface);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
            AutoPtr<IInterface> entkey;
            entry->GetKey((IInterface**)&entkey);
            if (Object::Equals(key, entkey)) {
                it->Remove();
                AutoPtr<IInterface> entvalue;
                entry->GetValue((IInterface**)&entvalue);
                return NOERROR;
            }
        }
    }
    else {
        while (it->HasNext(&isflag), isflag) {
            AutoPtr<IInterface> outface;
            it->GetNext((IInterface**)&outface);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
            AutoPtr<IInterface> entkey;
            entry->GetKey((IInterface**)&entkey);
            if (entkey == NULL) {
                it->Remove();
                AutoPtr<IInterface> entvalue;
                entry->GetValue((IInterface**)&entvalue);
                return NOERROR;
            }
        }
    }
    return NOERROR;
}

ECode AbstractMap::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)

    AutoPtr<ISet> entries;
    GetEntrySet((ISet**)&entries);
    return (ICollection::Probe(entries))->GetSize(size);
}

ECode AbstractMap::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    Boolean isflag = FALSE;
    if (IsEmpty(&isflag), isflag) {
        *str = "{}";
        return NOERROR;
    }

    StringBuilder buffer;
    buffer.AppendChar('{');
    AutoPtr<ISet> entries;
    GetEntrySet((ISet**)&entries);
    AutoPtr<IIterator> it;
    (IIterable::Probe(entries))->GetIterator((IIterator**)&it);
    while (it->HasNext(&isflag), isflag) {
        AutoPtr<IInterface> outface;
        it->GetNext((IInterface**)&outface);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
        AutoPtr<IInterface> key;
        entry->GetKey((IInterface**)&key);
        if (key.Get() != this->Probe(EIID_IInterface)) {
            buffer.Append(key);
        }
        else {
            buffer.Append("(this Map)");
        }
        buffer.Append('=');
        AutoPtr<IInterface> value;
        entry->GetValue((IInterface**)&value);
        if (value.Get() != this->Probe(EIID_IInterface)) {
            buffer.Append(value);
        }
        else {
            buffer.Append("(this Map)");
        }
        if (it->HasNext(&isflag), isflag) {
            buffer.Append(", ");
        }
    }
    buffer.Append('}');
    return buffer.ToString(str);
}

ECode AbstractMap::GetValues(
    /* [out] */ ICollection** value)
{
    VALIDATE_NOT_NULL(value)

    if (mValuesCollection == NULL) {
        mValuesCollection = (ICollection*) new AbstractMapValues(this);
    }
    *value = mValuesCollection;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

AbstractMap::AbstractMap()
{
}

ECode AbstractMap::Clone(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    assert(0 && "TODO");
    // AbstractMap<K, V> result = (AbstractMap<K, V>) super.clone();
    // result.keySet = null;
    // result.valuesCollection = null;
    // return result;
    return E_NOT_IMPLEMENTED;
}


//==========================================================
//       AbstractMap::SimpleImmutableEntry
//==========================================================
CAR_INTERFACE_IMPL_2(AbstractMap::SimpleImmutableEntry, Object, IMapEntry, ISerializable)

AbstractMap::SimpleImmutableEntry::SimpleImmutableEntry(
    /* [in] */ IInterface* theKey,
    /* [in] */ IInterface* theValue)
{
    mKey = theKey;
    mValue = theValue;
}

AbstractMap::SimpleImmutableEntry::SimpleImmutableEntry(
    /* [in] */ IMapEntry* copyFrom)
{
    copyFrom->GetKey((IInterface**)&mKey);
    copyFrom->GetValue((IInterface**)&mValue);
}

ECode AbstractMap::SimpleImmutableEntry::GetKey(
    /* [out] */ PInterface* key)
{
    VALIDATE_NOT_NULL(key)

    *key = mKey;
    REFCOUNT_ADD(*key)
    return NOERROR;
}

ECode AbstractMap::SimpleImmutableEntry::GetValue(
    /* [out] */ PInterface* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mValue;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode AbstractMap::SimpleImmutableEntry::SetValue(
    /* [in] */ IInterface* valueReplacer,
    /* [out] */ IInterface** valueReplacee)
{
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode AbstractMap::SimpleImmutableEntry::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (this->Probe(EIID_IInterface) == object) {
        *result = TRUE;
        return NOERROR;
    }
    if (IMapEntry::Probe(object)) {
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(object);
        AutoPtr<IInterface> keyface;
        AutoPtr<IInterface> valueface;
        entry->GetKey((IInterface**)&keyface);
        entry->GetValue((IInterface**)&valueface);
        *result = Object::Equals(mKey, keyface)
            && Object::Equals(mValue, valueface);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode AbstractMap::SimpleImmutableEntry::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    *hashCode = (mKey == NULL ? 0 : Object::GetHashCode(mKey))
            ^ (mValue == NULL ? 0 : Object::GetHashCode(mValue));
    return NOERROR;
}

ECode AbstractMap::SimpleImmutableEntry::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    *str = Object::ToString(mKey) + String("=") + Object::ToString(mValue);
    return NOERROR;
}


//==========================================================
//       AbstractMap::SimpleEntry
//==========================================================
CAR_INTERFACE_IMPL_2(AbstractMap::SimpleEntry, Object, IMapEntry, ISerializable)

AbstractMap::SimpleEntry::SimpleEntry(
    /* [in] */ IInterface* theKey,
    /* [in] */ IInterface* theValue)
{
    mKey = theKey;
    mValue = theValue;
}

AbstractMap::SimpleEntry::SimpleEntry(
    /* [in] */ IMapEntry* copyFrom)
{
    copyFrom->GetKey((IInterface**)&mKey);
    copyFrom->GetValue((IInterface**)&mValue);
}

ECode AbstractMap::SimpleEntry::GetKey(
    /* [out] */ PInterface* key)
{
    VALIDATE_NOT_NULL(key)

    *key = mKey;
    REFCOUNT_ADD(*key)
    return NOERROR;
}

ECode AbstractMap::SimpleEntry::GetValue(
    /* [out] */ PInterface* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mValue;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode AbstractMap::SimpleEntry::SetValue(
    /* [in] */ IInterface* valueReplacer,
    /* [out] */ IInterface** valueReplacee)
{
    VALIDATE_NOT_NULL(valueReplacee)

    AutoPtr<IInterface> result = mValue;
    mValue = valueReplacer;
    *valueReplacee = result;
    REFCOUNT_ADD(*valueReplacee)
    return NOERROR;
}

ECode AbstractMap::SimpleEntry::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (this->Probe(EIID_IInterface) == object) {
        *result = TRUE;
        return NOERROR;
    }
    if (IMapEntry::Probe(object)) {
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(object);
        AutoPtr<IInterface> keyface;
        AutoPtr<IInterface> valueface;
        entry->GetKey((IInterface**)&keyface);
        entry->GetValue((IInterface**)&valueface);
        *result = Object::Equals(mKey, keyface)
            && Object::Equals(mValue, valueface);;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode AbstractMap::SimpleEntry::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    *hashCode = (mKey == NULL ? 0 : Object::GetHashCode(mKey))
                ^ (mValue == NULL ? 0 : Object::GetHashCode(mValue));
    return NOERROR;
}

ECode AbstractMap::SimpleEntry::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    *str = Object::ToString(mKey) + String("=") + Object::ToString(mValue);
    return NOERROR;
}


//==========================================================
//       AbstractMap::AbstractMapKeySet
//==========================================================

CAR_INTERFACE_IMPL(AbstractMap::AbstractMapKeySet, Object, IIterator)

AbstractMap::AbstractMapKeySet::AbstractMapKeySet(
    /* [in] */ AbstractMap* hm)
{
    mMap = hm;
}

ECode AbstractMap::AbstractMapKeySet::GetSize(
    /* [out] */ Int32* value)
{
    return mMap->GetSize(value);
}

ECode AbstractMap::AbstractMapKeySet::Clear()
{
    return AbstractSet::Clear();
}

ECode AbstractMap::AbstractMapKeySet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    return AbstractSet::Remove(object, value);
}

ECode AbstractMap::AbstractMapKeySet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    return mMap->ContainsKey(object, value);
}

ECode AbstractMap::AbstractMapKeySet::GetIterator(
    /* [out] */ IIterator** outiter)
{
    VALIDATE_NOT_NULL(outiter)

    AutoPtr<IIterator> res = (IIterator*) new AbstractMapKeySetIterator(mMap);
    *outiter = res;
    REFCOUNT_ADD(*outiter)
    return NOERROR;
}

ECode AbstractMap::AbstractMapKeySet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractSet::IsEmpty(result);
}

ECode AbstractMap::AbstractMapKeySet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::Add(object, modified);
}

ECode AbstractMap::AbstractMapKeySet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::AddAll(collection, modified);
}

ECode AbstractMap::AbstractMapKeySet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::ContainsAll(collection, result);
}

ECode AbstractMap::AbstractMapKeySet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractSet::Equals(object, result);
}

ECode AbstractMap::AbstractMapKeySet::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractSet::GetHashCode(hashCode);
}

ECode AbstractMap::AbstractMapKeySet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::RemoveAll(collection, modified);
}

ECode AbstractMap::AbstractMapKeySet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::RetainAll(collection, modified);
}

ECode AbstractMap::AbstractMapKeySet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractSet::ToArray(array);
}

ECode AbstractMap::AbstractMapKeySet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractSet::ToArray(inArray, outArray);
}


//==========================================================
//       AbstractMap::AbstractMapKeySetIterator
//==========================================================
CAR_INTERFACE_IMPL(AbstractMap::AbstractMapKeySetIterator, Object, IIterator)

AbstractMap::AbstractMapKeySetIterator::AbstractMapKeySetIterator(
    /* [in] */ AbstractMap* hm)
{
    mMap = hm;
    AutoPtr<ISet> outset;
    mMap->GetEntrySet((ISet**)&outset);
    (IIterable::Probe(outset))->GetIterator((IIterator**)&mSetIterator);
}

ECode AbstractMap::AbstractMapKeySetIterator::HasNext(
    /* [out] */ Boolean* result)
{
    return mSetIterator->HasNext(result);
}

ECode AbstractMap::AbstractMapKeySetIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    AutoPtr<IInterface> outface;
    mSetIterator->GetNext((IInterface**)&outface);
    AutoPtr<IMapEntry> outent = IMapEntry::Probe(outface);
    VALIDATE_NOT_NULL(outent)

    return outent->GetKey(object);
}

ECode AbstractMap::AbstractMapKeySetIterator::Remove()
{
    return mSetIterator->Remove();
}


//==========================================================
//       AbstractMap::AbstractMapValues
//==========================================================

AbstractMap::AbstractMapValues::AbstractMapValues(
    /* [in] */ AbstractMap* hm)
{
    mMap = hm;
}

ECode AbstractMap::AbstractMapValues::GetSize(
    /* [out] */ Int32* value)
{
    return mMap->GetSize(value);
}

ECode AbstractMap::AbstractMapValues::Clear()
{
    return AbstractCollection::Clear();
}

ECode AbstractMap::AbstractMapValues::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    return AbstractCollection::Remove(object, value);
}

ECode AbstractMap::AbstractMapValues::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    return mMap->ContainsValue(object, value);
}

ECode AbstractMap::AbstractMapValues::GetIterator(
    /* [out] */ IIterator** outiter)
{
    VALIDATE_NOT_NULL(outiter)

    AutoPtr<IIterator> res = (IIterator*) new AbstractMapValuesIterator(mMap);
    *outiter = res;
    REFCOUNT_ADD(*outiter)
    return NOERROR;
}

ECode AbstractMap::AbstractMapValues::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractCollection::IsEmpty(result);
}

ECode AbstractMap::AbstractMapValues::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::Add(object, modified);
}

ECode AbstractMap::AbstractMapValues::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::AddAll(collection, modified);
}

ECode AbstractMap::AbstractMapValues::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::ContainsAll(collection, result);
}

ECode AbstractMap::AbstractMapValues::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = Object::Equals(this, object);
    return NOERROR;
}

ECode AbstractMap::AbstractMapValues::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    *hashCode = Object::GetHashCode(this);
    return NOERROR;
}

ECode AbstractMap::AbstractMapValues::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::RemoveAll(collection, modified);
}

ECode AbstractMap::AbstractMapValues::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::RetainAll(collection, modified);
}

ECode AbstractMap::AbstractMapValues::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractCollection::ToArray(array);
}

ECode AbstractMap::AbstractMapValues::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractCollection::ToArray(inArray, outArray);
}


//==========================================================
//       AbstractMap::AbstractMapValuesIterator
//==========================================================
CAR_INTERFACE_IMPL(AbstractMap::AbstractMapValuesIterator, Object, IIterator)

AbstractMap::AbstractMapValuesIterator::AbstractMapValuesIterator(
    /* [in] */ AbstractMap* hm)
{
    mMap = hm;
    AutoPtr<ISet> outset;
    mMap->GetEntrySet((ISet**)&outset);
    (IIterable::Probe(outset))->GetIterator((IIterator**)&mSetIterator);
}

ECode AbstractMap::AbstractMapValuesIterator::HasNext(
    /* [out] */ Boolean* result)
{
    return mSetIterator->HasNext(result);
}

ECode AbstractMap::AbstractMapValuesIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    AutoPtr<IInterface> outface;
    mSetIterator->GetNext((IInterface**)&outface);
    AutoPtr<IMapEntry> outent = IMapEntry::Probe(outface);
    VALIDATE_NOT_NULL(outent)

    return outent->GetValue(object);
}

ECode AbstractMap::AbstractMapValuesIterator::Remove()
{
    return mSetIterator->Remove();
}

} // namespace Utility
} // namespace Elastos
