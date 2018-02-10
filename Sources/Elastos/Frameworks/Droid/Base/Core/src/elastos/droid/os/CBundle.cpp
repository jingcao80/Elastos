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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/CBundle.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include "elastos/droid/utility/CArrayMap.h"
#include <binder/Parcel.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Utility::CArrayMap;

namespace Elastos {
namespace Droid {
namespace Os {

static AutoPtr<IBundle> InitEMPTY()
{
    AutoPtr<CBundle> empty;
    CBundle::NewByFriend((CBundle**)&empty);
    empty->mMap = CArrayMap::EMPTY;
    return empty;
}

AutoPtr<IBundle> CBundle::EMPTY = InitEMPTY();

CAR_INTERFACE_IMPL_3(CBundle, BaseBundle, IBundle, ICloneable, IParcelable)

CAR_OBJECT_IMPL(CBundle)

CBundle::CBundle()
    : mHasFds(FALSE)
    , mFdsKnown(TRUE)
    , mAllowFds(TRUE)
{
}

CBundle::~CBundle()
{
    // TODO: for debug
    mParcelledData = NULL;
    mClassLoader = NULL;

    mMap = NULL;
}

ECode CBundle::constructor()
{
    return BaseBundle::constructor();
}

ECode CBundle::constructor(
    /* [in] */ IParcel* parcelledData)
{
    FAIL_RETURN(BaseBundle::constructor(parcelledData))

    mParcelledData->HasFileDescriptors(&mHasFds);
    mFdsKnown = TRUE;
    return NOERROR;
}

ECode CBundle::constructor(
    /* [in] */ IParcel* parcelledData,
    /* [in] */ Int32 length)
{
    FAIL_RETURN(BaseBundle::constructor(parcelledData, length))

    mParcelledData->HasFileDescriptors(&mHasFds);
    mFdsKnown = TRUE;
    return NOERROR;
}

ECode CBundle::constructor(
    /* [in] */ IClassLoader* loader)
{
    return BaseBundle::constructor(loader);
}

ECode CBundle::constructor(
    /* [in] */ Int32 capacity)
{
    return BaseBundle::constructor(capacity);
}

ECode CBundle::constructor(
    /* [in] */ IBundle* bundle)
{
    FAIL_RETURN(BaseBundle::constructor(IBaseBundle::Probe(bundle)))

    CBundle* b = (CBundle*)bundle;

    mHasFds = b->mHasFds;
    mFdsKnown = b->mFdsKnown;
    return NOERROR;
}

ECode CBundle::constructor(
    /* [in] */ IPersistableBundle* bundle)
{
    return BaseBundle::constructor(IBaseBundle::Probe(bundle));
}

AutoPtr<IBundle> CBundle::ForPair(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    AutoPtr<IBundle> b;
    CBundle::New(1, (IBundle**)&b);
    b->PutString(key, value);
    return b;
}

ECode CBundle::SetAllowFds(
    /* [in] */ Boolean allowFds)
{
    Boolean prev;
    return SetAllowFds(allowFds, &prev);
}

ECode CBundle::SetAllowFds(
    /* [in] */ Boolean allowFds,
    /* [out] */ Boolean* prev)
{
    VALIDATE_NOT_NULL(prev);
    *prev = mAllowFds;
    mAllowFds = allowFds;
    return NOERROR;
}

ECode CBundle::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<IBundle> bundle;
    CBundle::New(this, (IBundle**)&bundle);
    *obj = bundle.Get();
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CBundle::Clear()
{
    FAIL_RETURN(BaseBundle::Clear())

    mHasFds = FALSE;
    mFdsKnown = TRUE;
    return NOERROR;
}

ECode CBundle::PutAll(
    /* [in] */ IBundle* b)
{
    Unparcel();

    CBundle* bundle = (CBundle*)b;
    bundle->Unparcel();
    mMap->PutAll(IMap::Probe(bundle->mMap));

    // fd state is now known if and only if both bundles already knew
    mHasFds |= bundle->mHasFds;
    mFdsKnown = mFdsKnown && bundle->mFdsKnown;
    return NOERROR;
}

ECode CBundle::HasFileDescriptors(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (!mFdsKnown) {
        Boolean fdFound = FALSE;    // keep going until we find one or run out of data

        if (mParcelledData != NULL) {
            Boolean bval;
            mParcelledData->HasFileDescriptors(&bval);
            if (bval) {
                fdFound = TRUE;
            }
        }
        else {
            // It's been unparcelled, so we need to walk the map

        //     AutoPtr<ISet> outset;
        //     IMap::Probe(map)->GetEntrySet((ISet**)&outset);
        //     AutoPtr<IIterator> it;
        //     outset->GetIterator((IIterator**)&it);
        //     Boolean hasNext = FALSE;
        //     String key;
        //     while ((it->HasNext(&hasNext), hasNext)) {
        //         AutoPtr<IInterface> outface;
        //         it->GetNext((IInterface**)&outface);
        //         AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
        //         AutoPtr<IInterface> obj;
        //         entry->GetValue((IInterface**)&obj);

        //         if (IParcelable::Probe(obj) != NULL) {
        //             if ((((Parcelable)obj).describeContents()
        //                     & Parcelable.CONTENTS_FILE_DESCRIPTOR) != 0) {
        //                 fdFound = TRUE;
        //                 break;
        //             }
        //         }
        //         else if (obj instanceof Parcelable[]) {
        //             Parcelable[] array = (Parcelable[]) obj;
        //             for (int n = array.length - 1; n >= 0; n--) {
        //                 if ((array[n].describeContents()
        //                         & Parcelable.CONTENTS_FILE_DESCRIPTOR) != 0) {
        //                     fdFound = TRUE;
        //                     break;
        //                 }
        //             }
        //         }
        //         else if (obj instanceof SparseArray) {
        //             SparseArray<? extends Parcelable> array =
        //                     (SparseArray<? extends Parcelable>) obj;
        //             for (int n = array.size() - 1; n >= 0; n--) {
        //                 if ((array.valueAt(n).describeContents()
        //                         & Parcelable.CONTENTS_FILE_DESCRIPTOR) != 0) {
        //                     fdFound = TRUE;
        //                     break;
        //                 }
        //             }
        //         }
        //         else if (obj instanceof ArrayList) {
        //             ArrayList array = (ArrayList) obj;
        //             // an ArrayList here might contain either Strings or
        //             // Parcelables; only look inside for Parcelables
        //             if (!array.isEmpty() && (array.get(0) instanceof Parcelable)) {
        //                 for (int n = array.size() - 1; n >= 0; n--) {
        //                     Parcelable p = (Parcelable) array.get(n);
        //                     if (p != null && ((p.describeContents()
        //                             & Parcelable.CONTENTS_FILE_DESCRIPTOR) != 0)) {
        //                         fdFound = TRUE;
        //                         break;
        //                     }
        //                 }
        //             }
        //         }
        //     }
        }

        mHasFds = fdFound;
        mFdsKnown = TRUE;
    }

    *result = mHasFds;
    return NOERROR;
}

ECode CBundle::PutParcelable(
    /* [in] */ const String& key,
    /* [in] */ IParcelable* value)
{
    assert(key != NULL);
    Unparcel();
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    mMap->Put(keyObj.Get(), value);
    mFdsKnown = FALSE;
    return NOERROR;
}

ECode CBundle::PutSize(
    /* [in] */ const String& key,
    /* [in] */ ISize* value)
{
    assert(key != NULL);
    Unparcel();
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    mMap->Put(keyObj.Get(), value);
    return NOERROR;
}

ECode CBundle::PutSizeF(
    /* [in] */ const String& key,
    /* [in] */ ISizeF* value)
{
    assert(key != NULL);
    Unparcel();
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    mMap->Put(keyObj.Get(), value);
    return NOERROR;
}

ECode CBundle::PutParcelableArray(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<IParcelable*>* value)
{
    assert(key != NULL);
    Unparcel();
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    AutoPtr<IArrayOf> valueObj = CoreUtils::Convert(value, EIID_IParcelable);
    mMap->Put(keyObj.Get(), valueObj.Get());
    mFdsKnown = FALSE;
    return NOERROR;
}

ECode CBundle::PutParcelableArrayList(
    /* [in] */ const String& key,
    /* [in] */ IArrayList* value) //ArrayList<? extends Parcelable> value
{
    assert(key != NULL);
    Unparcel();
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    mMap->Put(keyObj.Get(), value);
    mFdsKnown = FALSE;
    return NOERROR;
}

ECode CBundle::PutParcelableList(
    /* [in] */ const String& key,
    /* [in] */ IList* value)//List<? extends Parcelable> value
{
    assert(key != NULL);
    Unparcel();
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    mMap->Put(keyObj.Get(), value);
    mFdsKnown = FALSE;
    return NOERROR;
}

ECode CBundle::PutSparseParcelableArray(
    /* [in] */ const String& key,
    /* [in] */ ISparseArray* value)//SparseArray<? extends Parcelable> value)
{
    assert(key != NULL);
    Unparcel();
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    mMap->Put(keyObj.Get(), value);
    mFdsKnown = FALSE;
    return NOERROR;
}

ECode CBundle::PutBundle(
    /* [in] */ const String& key,
    /* [in] */ IBundle* value)
{
    assert(key != NULL);
    Unparcel();
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    mMap->Put(keyObj.Get(), value);
    return NOERROR;
}

ECode CBundle::PutBinder(
    /* [in] */ const String& key,
    /* [in] */ IBinder* value)
{
    assert(key != NULL);
    Unparcel();
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    mMap->Put(keyObj.Get(), value);
    return NOERROR;
}

ECode CBundle::PutIBinder(
    /* [in] */ const String& key,
    /* [in] */ IBinder* value)
{
    assert(key != NULL);
    Unparcel();
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    mMap->Put(keyObj.Get(), value);
    return NOERROR;
}

ECode CBundle::GetSize(
    /* [in] */ const String& key,
    /* [out] */ ISize** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    Unparcel();

    AutoPtr<IInterface> o = GetValue(key);
    if (o == NULL) {
        return NOERROR;
    }

    ISize* obj = ISize::Probe(o);
    if (obj == NULL) {
        TypeWarning(key, String("ISize"));
        return NOERROR;
    }

    *value = obj;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CBundle::GetSizeF(
    /* [in] */ const String& key,
    /* [out] */ ISizeF** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    Unparcel();

    AutoPtr<IInterface> o = GetValue(key);
    if (o == NULL) {
        return NOERROR;
    }

    ISizeF* obj = ISizeF::Probe(o);
    if (obj == NULL) {
        TypeWarning(key, String("ISizeF"));
        return NOERROR;
    }

    *value = obj;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CBundle::GetBundle(
    /* [in] */ const String& key,
    /* [out] */ IBundle** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    Unparcel();

    AutoPtr<IInterface> o = GetValue(key);
    if (o == NULL) {
        return NOERROR;
    }

    IBundle* obj = IBundle::Probe(o);
    if (obj == NULL) {
        TypeWarning(key, String("IBundle"));
        return NOERROR;
    }

    *value = obj;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CBundle::GetParcelable(
    /* [in] */ const String& key,
    /* [out] */ IParcelable** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    Unparcel();

    AutoPtr<IInterface> o = GetValue(key);
    if (o == NULL) {
        return NOERROR;
    }

    IParcelable* obj = IParcelable::Probe(o);
    if (obj == NULL) {
        TypeWarning(key, String("IParcelable"));
        return NOERROR;
    }

    *value = obj;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CBundle::GetParcelableArray(
    /* [in] */ const String& key,
    /* [out, callee] */ ArrayOf<IParcelable*>** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    Unparcel();
    AutoPtr<IInterface> o = GetValue(key);
    if (o == NULL) {
        return NOERROR;
    }

    IArrayOf* arrayObj = IArrayOf::Probe(o);
    if (arrayObj == NULL) {
        TypeWarning(key, String("IArrayOf<IParcelable>"));
        return NOERROR;
    }

    Int32 length = 0;
    arrayObj->GetLength(&length);
    AutoPtr<ArrayOf<IParcelable*> > array = ArrayOf<IParcelable*>::Alloc(length);
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IInterface> obj;
        arrayObj->Get(i, (IInterface**)&obj);
        if (obj) {
            assert(IParcelable::Probe(obj) != NULL);
        }
        array->Set(i, IParcelable::Probe(obj));
    }

    *value = array;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode CBundle::GetParcelableArrayList(
    /* [in] */ const String& key,
    /* [out] */ IArrayList** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    Unparcel();
    AutoPtr<IInterface> o = GetValue(key);
    if (o == NULL) {
        return NOERROR;
    }

    IArrayList* obj = IArrayList::Probe(o);
    if (obj == NULL) {
        TypeWarning(key, String("IArrayList"));
        return NOERROR;
    }

    *value = obj;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CBundle::GetSparseParcelableArray(
    /* [in] */ const String& key,
    /* [out] */ ISparseArray** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    Unparcel();
    AutoPtr<IInterface> o = GetValue(key);
    if (o == NULL) {
        return NOERROR;
    }

    ISparseArray* obj = ISparseArray::Probe(o);
    if (obj == NULL) {
        TypeWarning(key, String("ISparseArray"));
        return NOERROR;
    }

    *value = obj;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CBundle::GetBinder(
    /* [in] */ const String& key,
    /* [out] */ IBinder** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    Unparcel();
    AutoPtr<IInterface> o = GetValue(key);
    if (o == NULL) {
        return NOERROR;
    }

    IBinder* obj = IBinder::Probe(o);
    if (obj == NULL) {
        TypeWarning(key, String("IBinder"));
        return NOERROR;
    }

    *value = obj;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CBundle::GetIBinder(
    /* [in] */ const String& key,
    /* [out] */ IBinder** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    Unparcel();
    AutoPtr<IInterface> o = GetValue(key);
    if (o == NULL) {
        return NOERROR;
    }

    IBinder* obj = IBinder::Probe(o);
    if (obj == NULL) {
        TypeWarning(key, String("IBinder"));
        return NOERROR;
    }

    *value = obj;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CBundle::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    FAIL_RETURN(BaseBundle::ReadFromParcelInner(parcel))
    mParcelledData->HasFileDescriptors(&mHasFds);
    mFdsKnown = TRUE;
    return NOERROR;
}

ECode CBundle::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    android::Parcel* p;
    parcel->GetDataPayload((HANDLE*)&p);
    Boolean oldAllowFds = p->pushAllowFds(mAllowFds);
    ECode ec = BaseBundle::WriteToParcelInner(parcel);
    p->restoreAllowFds(oldAllowFds);
    return ec;
}

ECode CBundle::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    if (mParcelledData != NULL) {
        if (mParcelledData == EMPTY_PARCEL) {
            *str = String("Bundle[EMPTY_PARCEL]");
            return NOERROR;
        }
        else {
            Int32 length;
            mParcelledData->GetDataSize(&length);
            StringBuilder sb("Bundle[mParcelledData.dataSize=");
            sb.Append(length);
            sb.Append("]");
            *str = sb.ToString();
            return NOERROR;
        }
    }

    StringBuilder sb("Bundle[");
    sb.Append(Object::ToString(mMap));
    sb.Append("]");
    *str = sb.ToString();
    return NOERROR;
}

//
// From IBaseBundle
//

ECode CBundle::SetClassLoader(
    /* [in] */ IClassLoader* loader)
{
    return BaseBundle::SetClassLoader(loader);
}

ECode CBundle::GetClassLoader(
    /* [out] */ IClassLoader** loader)
{
    return BaseBundle::GetClassLoader(loader);
}

ECode CBundle::IsParcelled(
    /* [out] */ Boolean* empty)
{
    return BaseBundle::IsParcelled(empty);
}

ECode CBundle::GetSize(
    /* [out] */ Int32 * size)
{
    return BaseBundle::GetSize(size);
}

ECode CBundle::IsEmpty(
    /* [out] */ Boolean* empty)
{
    return BaseBundle::IsEmpty(empty);
}

ECode CBundle::ContainsKey(
    /* [in] */ const String& key,
    /* [out] */ Boolean* result)
{
    return BaseBundle::ContainsKey(key, result);
}

ECode CBundle::Get(
    /* [in] */ const String& key,
    /* [out] */ IInterface** obj)
{
    return BaseBundle::Get(key, obj);
}

ECode CBundle::Remove(
    /* [in] */ const String& key)
{
    return BaseBundle::Remove(key);
}

ECode CBundle::PutAll(
    /* [in] */ IPersistableBundle* bundle)
{
    return BaseBundle::PutAll(bundle);
}

ECode CBundle::GetKeySet(
    /* [out] */ ISet** set)
{
    return BaseBundle::GetKeySet(set);
}

ECode CBundle::PutBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean value)
{
    return BaseBundle::PutBoolean(key, value);
}

ECode CBundle::PutByte(
    /* [in] */ const String& key,
    /* [in] */ Byte value)
{
    return BaseBundle::PutByte(key, value);
}

ECode CBundle::PutChar(
    /* [in] */ const String& key,
    /* [in] */ Char32 value)
{
    return BaseBundle::PutChar(key, value);
}

ECode CBundle::PutInt16(
    /* [in] */ const String& key,
    /* [in] */ Int16 value)
{
    return BaseBundle::PutInt16(key, value);
}

ECode CBundle::PutInt32(
    /* [in] */ const String& key,
    /* [in] */ Int32 value)
{
    return BaseBundle::PutInt32(key, value);
}

ECode CBundle::PutInt64(
    /* [in] */ const String& key,
    /* [in] */ Int64 value)
{
    return BaseBundle::PutInt64(key, value);
}

ECode CBundle::PutFloat(
    /* [in] */ const String& key,
    /* [in] */ Float value)
{
    return BaseBundle::PutFloat(key, value);
}

ECode CBundle::PutDouble(
    /* [in] */ const String& key,
    /* [in] */ Double value)
{
    return BaseBundle::PutDouble(key, value);
}

ECode CBundle::PutString(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    return BaseBundle::PutString(key, value);
}

ECode CBundle::PutCharSequence(
    /* [in] */ const String& key,
    /* [in] */ ICharSequence* value)
{
    return BaseBundle::PutCharSequence(key, value);
}

ECode CBundle::PutIntegerArrayList(
    /* [in] */ const String& key,
    /* [in] */ IArrayList* value)
{
    return BaseBundle::PutIntegerArrayList(key, value);
}

ECode CBundle::PutStringArrayList(
    /* [in] */ const String& key,
    /* [in] */ IArrayList* value)
{
    return BaseBundle::PutStringArrayList(key, value);
}

ECode CBundle::PutCharSequenceArrayList(
    /* [in] */ const String& key,
    /* [in] */ IArrayList* value)
{
    return BaseBundle::PutCharSequenceArrayList(key, value);
}

ECode CBundle::PutSerializable(
    /* [in] */ const String& key,
    /* [in] */ ISerializable* value)
{
    return BaseBundle::PutSerializable(key, value);
}

ECode CBundle::PutBooleanArray(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Boolean>* value)
{
    return BaseBundle::PutBooleanArray(key, value);
}

ECode CBundle::PutByteArray(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Byte>* value)
{
    return BaseBundle::PutByteArray(key, value);
}

ECode CBundle::PutCharArray(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Char32>* value)
{
    return BaseBundle::PutCharArray(key, value);
}

ECode CBundle::PutInt16Array(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Int16>* value)
{
    return BaseBundle::PutInt16Array(key, value);
}

ECode CBundle::PutInt32Array(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Int32>* value)
{
    return BaseBundle::PutInt32Array(key, value);
}

ECode CBundle::PutInt64Array(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Int64>* value)
{
    return BaseBundle::PutInt64Array(key, value);
}

ECode CBundle::PutFloatArray(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Float>* value)
{
    return BaseBundle::PutFloatArray(key, value);
}

ECode CBundle::PutDoubleArray(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Double>* value)
{
    return BaseBundle::PutDoubleArray(key, value);
}

ECode CBundle::PutStringArray(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<String>* value)
{
    return BaseBundle::PutStringArray(key, value);
}

ECode CBundle::PutCharSequenceArray(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<ICharSequence*>* value)
{
    return BaseBundle::PutCharSequenceArray(key, value);
}

ECode CBundle::GetBoolean(
    /* [in] */ const String& key,
    /* [out] */ Boolean* value)
{
    return BaseBundle::GetBoolean(key, value);
}

ECode CBundle::GetBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean defaultValue,
    /* [out] */ Boolean* value)
{
    return BaseBundle::GetBoolean(key, defaultValue, value);
}

ECode CBundle::GetByte(
    /* [in] */ const String& key,
    /* [out] */ Byte* value)
{
    return BaseBundle::GetByte(key, value);
}

ECode CBundle::GetByte(
    /* [in] */ const String& key,
    /* [in] */ Byte defaultValue,
    /* [out] */ Byte* value)
{
    return BaseBundle::GetByte(key, defaultValue, value);
}

ECode CBundle::GetChar(
    /* [in] */ const String& key,
    /* [out] */ Char32* value)
{
    return BaseBundle::GetChar(key, value);
}

ECode CBundle::GetChar(
    /* [in] */ const String& key,
    /* [in] */ Char32 defaultValue,
    /* [out] */ Char32* value)
{
    return BaseBundle::GetChar(key, defaultValue, value);
}

ECode CBundle::GetInt16(
    /* [in] */ const String& key,
    /* [out] */ Int16* value)
{
    return BaseBundle::GetInt16(key, value);
}

ECode CBundle::GetInt16(
    /* [in] */ const String& key,
    /* [in] */ Int16 defaultValue,
    /* [out] */ Int16* value)
{
    return BaseBundle::GetInt16(key, defaultValue, value);
}

ECode CBundle::GetInt32(
    /* [in] */ const String& key,
    /* [out] */ Int32* value)
{
    return BaseBundle::GetInt32(key, value);
}

ECode CBundle::GetInt32(
    /* [in] */ const String& key,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* value)
{
    return BaseBundle::GetInt32(key, defaultValue, value);
}

ECode CBundle::GetInt64(
    /* [in] */ const String& key,
    /* [out] */ Int64* value)
{
    return BaseBundle::GetInt64(key, value);
}

ECode CBundle::GetInt64(
    /* [in] */ const String& key,
    /* [in] */ Int64 defaultValue,
    /* [out] */ Int64* value)
{
    return BaseBundle::GetInt64(key, defaultValue, value);
}

ECode CBundle::GetFloat(
    /* [in] */ const String& key,
    /* [out] */ Float* value)
{
    return BaseBundle::GetFloat(key, value);
}

ECode CBundle::GetFloat(
    /* [in] */ const String& key,
    /* [in] */ Float defaultValue,
    /* [out] */ Float* value)
{
    return BaseBundle::GetFloat(key, defaultValue, value);
}

ECode CBundle::GetDouble(
    /* [in] */ const String& key,
    /* [out] */ Double* value)
{
    return BaseBundle::GetDouble(key, value);
}

ECode CBundle::GetDouble(
    /* [in] */ const String& key,
    /* [in] */ Double defaultValue,
    /* [out] */ Double* value)
{
    return BaseBundle::GetDouble(key, defaultValue, value);
}

ECode CBundle::GetString(
    /* [in] */ const String& key,
    /* [out] */ String* value)
{
    return BaseBundle::GetString(key, value);
}

ECode CBundle::GetString(
    /* [in] */ const String& key,
    /* [in] */ const String& defaultValue,
    /* [out] */ String* value)
{
    return BaseBundle::GetString(key, defaultValue, value);
}

ECode CBundle::GetCharSequence(
    /* [in] */ const String& key,
    /* [out] */ ICharSequence** value)
{
    return BaseBundle::GetCharSequence(key, value);
}

ECode CBundle::GetCharSequence(
    /* [in] */ const String& key,
    /* [in] */ ICharSequence* defaultValue,
    /* [out] */ ICharSequence** value)
{
    return BaseBundle::GetCharSequence(key, defaultValue, value);
}

ECode CBundle::GetSerializable(
    /* [in] */ const String& key,
    /* [out] */ ISerializable** value)
{
    return BaseBundle::GetSerializable(key, value);
}

ECode CBundle::GetIntegerArrayList(
    /* [in] */ const String& key,
    /* [out] */ IArrayList** value)
{
    return BaseBundle::GetIntegerArrayList(key, value);
}

ECode CBundle::GetStringArrayList(
    /* [in] */ const String& key,
    /* [out] */ IArrayList** value)
{
    return BaseBundle::GetStringArrayList(key, value);
}

ECode CBundle::GetCharSequenceArrayList(
    /* [in] */ const String& key,
    /* [out] */ IArrayList** value)
{
    return BaseBundle::GetCharSequenceArrayList(key, value);
}

ECode CBundle::GetBooleanArray(
    /* [in] */ const String& key,
    /* [out, callee] */ ArrayOf<Boolean>** value)
{
    return BaseBundle::GetBooleanArray(key, value);
}

ECode CBundle::GetByteArray(
    /* [in] */ const String& key,
    /* [out, callee] */ ArrayOf<Byte>** value)
{
    return BaseBundle::GetByteArray(key, value);
}

ECode CBundle::GetInt16Array(
    /* [in] */ const String& key,
    /* [out, callee] */ ArrayOf<Int16>** value)
{
    return BaseBundle::GetInt16Array(key, value);
}

ECode CBundle::GetCharArray(
    /* [in] */ const String& key,
    /* [out, callee] */ ArrayOf<Char32>** value)
{
    return BaseBundle::GetCharArray(key, value);
}

ECode CBundle::GetInt32Array(
    /* [in] */ const String& key,
    /* [out, callee] */ ArrayOf<Int32>** value)
{
    return BaseBundle::GetInt32Array(key, value);
}

ECode CBundle::GetInt64Array(
    /* [in] */ const String& key,
    /* [out, callee] */ ArrayOf<Int64>** value)
{
    return BaseBundle::GetInt64Array(key, value);
}

ECode CBundle::GetFloatArray(
    /* [in] */ const String& key,
    /* [out, callee] */ ArrayOf<Float>** value)
{
    return BaseBundle::GetFloatArray(key, value);
}

ECode CBundle::GetDoubleArray(
    /* [in] */ const String& key,
    /* [out, callee] */ ArrayOf<Double>** value)
{
    return BaseBundle::GetDoubleArray(key, value);
}

ECode CBundle::GetStringArray(
    /* [in] */ const String& key,
    /* [out, callee] */ ArrayOf<String>** value)
{
    return BaseBundle::GetStringArray(key, value);
}

ECode CBundle::GetCharSequenceArray(
    /* [in] */ const String& key,
    /* [out, callee] */ ArrayOf<ICharSequence*>** value)
{
    return BaseBundle::GetCharSequenceArray(key, value);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
