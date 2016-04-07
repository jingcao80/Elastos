
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/content/CContentValues.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::INumber;
using Elastos::Core::CByte;
using Elastos::Core::CString;
using Elastos::Core::CInteger16;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::CFloat;
using Elastos::Core::CDouble;
using Elastos::Core::CBoolean;
using Elastos::Core::CArrayOf;
using Elastos::Core::EIID_IByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {

const Int32 CContentValues::VAL_NULL;
const Int32 CContentValues::VAL_STRING;
const Int32 CContentValues::VAL_BYTE;
const Int32 CContentValues::VAL_INTEGER16;
const Int32 CContentValues::VAL_INTEGER32;
const Int32 CContentValues::VAL_INTEGER64;
const Int32 CContentValues::VAL_FLOAT;
const Int32 CContentValues::VAL_DOUBLE;
const Int32 CContentValues::VAL_BOOLEAN;
const Int32 CContentValues::VAL_ARRAYOF;
const Int32 CContentValues::VAL_ARRAYLIST;

const String CContentValues::TAG("CContentValues");

CAR_INTERFACE_IMPL(CContentValues, Object, IContentValues)

CAR_OBJECT_IMPL(CContentValues)

CContentValues::CContentValues()
{}

CContentValues::~CContentValues()
{
}

ECode CContentValues::constructor()
{
    // Choosing a default size of 8 based on analysis of typical
    // consumption by applications.
    CHashMap::New(8, (IHashMap**)&mValues);
    return NOERROR;
}

ECode CContentValues::constructor(
    /* [in] */ Int32 size)
{
    if (size < 0) {
        return E_INVALID_ARGUMENT;
    }

    CHashMap::New(size, (IHashMap**)&mValues);
    return NOERROR;
}

ECode CContentValues::constructor(
    /* [in] */ IContentValues* from)
{
    VALIDATE_NOT_NULL(from)
    Int32 size = 0;
    FAIL_RETURN(from->GetSize(&size))
    if (size > 0) {
        CHashMap::New(size, (IHashMap**)&mValues);
    }
    else {
        CHashMap::New(8, (IHashMap**)&mValues);
    }
    FAIL_RETURN(PutAll(from))
    return NOERROR;
}

ECode CContentValues::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (TO_IINTERFACE(this) == object) {
        *result = TRUE;
        return NOERROR;
    }

    if (IContentValues::Probe(object) != NULL) {
        CContentValues* other = (CContentValues*)IContentValues::Probe(object);
        *result = Object::Equals(mValues, other->mValues);
    }

    return NOERROR;
}

ECode CContentValues::GetHashCode(
    /* [out] */ Int32* hashcode)
{
    VALIDATE_NOT_NULL(hashcode)

    *hashcode = Object::GetHashCode(mValues);
    return NOERROR;
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ ICharSequence* value)
{
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    return mValues->Put(TO_IINTERFACE(ko), TO_IINTERFACE(value));
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    AutoPtr<ICharSequence> vo = CoreUtils::Convert(value);
    return mValues->Put(TO_IINTERFACE(ko), TO_IINTERFACE(vo));
}

ECode CContentValues::PutAll(
    /* [in] */ IContentValues* contentvalues)
{
    VALIDATE_NOT_NULL(contentvalues)
    CContentValues* other = (CContentValues*)contentvalues;
    return mValues->PutAll(IMap::Probe(other->mValues));
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ IByte* value)
{
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    return mValues->Put(TO_IINTERFACE(ko), TO_IINTERFACE(value));
}

ECode CContentValues::PutByte(
    /* [in] */ const String& key,
    /* [in] */ Byte value)
{
    return Put(key, CoreUtils::ConvertByte(value));
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ IInteger16* value)
{
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    return mValues->Put(TO_IINTERFACE(ko), TO_IINTERFACE(value));
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ Int16 value)
{
    return Put(key, CoreUtils::Convert(value));
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ IInteger32* value)
{
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    return mValues->Put(TO_IINTERFACE(ko), TO_IINTERFACE(value));
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ Int32 value)
{
    return Put(key, CoreUtils::Convert(value));
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ IInteger64* value)
{
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    return mValues->Put(TO_IINTERFACE(ko), TO_IINTERFACE(value));
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ Int64 value)
{
    return Put(key, CoreUtils::Convert(value));
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ IFloat* value)
{
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    return mValues->Put(TO_IINTERFACE(ko), TO_IINTERFACE(value));
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ Float value)
{
    return Put(key, CoreUtils::Convert(value));
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ IDouble* value)
{
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    return mValues->Put(TO_IINTERFACE(ko), TO_IINTERFACE(value));
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ Double value)
{
    return Put(key, CoreUtils::Convert(value));
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ IBoolean* value)
{
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    return mValues->Put(TO_IINTERFACE(ko), TO_IINTERFACE(value));
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ Boolean value)
{
    return Put(key, CoreUtils::Convert(value));
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ IArrayOf* value)
{
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    return mValues->Put(TO_IINTERFACE(ko), TO_IINTERFACE(value));
}

ECode CContentValues::Put(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Byte>* value)
{
    if (key.IsNull()) return E_INVALID_ARGUMENT;

    if (value != NULL) {
        Int32 length = value->GetLength();
        AutoPtr<IArrayOf> array;
        CArrayOf::New(EIID_IByte, length,(IArrayOf**)&array);
        AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
        return mValues->Put(TO_IINTERFACE(ko), TO_IINTERFACE(array));
    }
    else {
        AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
        return mValues->Put(TO_IINTERFACE(ko), NULL);
    }
}

ECode CContentValues::PutNull(
    /* [in] */ const String& key)
{
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    return mValues->Put(TO_IINTERFACE(ko), NULL);
}

ECode CContentValues::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    return mValues->GetSize(size);
}

ECode CContentValues::Remove(
    /* [in] */ const String& key)
{
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    return mValues->Remove(ko);
}

ECode CContentValues::Clear()
{
    return mValues->Clear();
}

ECode CContentValues::ContainsKey(
    /* [in] */ const String& key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result =  FALSE;
    if (key.IsNull()) return E_INVALID_ARGUMENT;

    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    return mValues->ContainsKey(ko, result);
}

ECode CContentValues::Get(
    /* [in] */ const String& key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;
    if (key.IsNull()) return E_INVALID_ARGUMENT;

    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    return mValues->Get(ko, value);
}

ECode CContentValues::GetAsString(
    /* [in] */ const String& key,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = String(NULL);
    if (key.IsNull()) return E_INVALID_ARGUMENT;

    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    AutoPtr<IInterface> obj;
    mValues->Get(ko, (IInterface**)&obj);

    *value = obj ? Object::ToString(obj) : String(NULL);
    return NOERROR;
}

ECode CContentValues::GetAsInt64(
    /* [in] */ const String& key,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    AutoPtr<IInterface> obj;
    mValues->Get(ko, (IInterface**)&obj);

    if (NULL != obj) {
        if (INumber::Probe(obj) != NULL) {
            FAIL_RETURN(INumber::Probe(obj)->Int64Value(value))
            return NOERROR;
        }
        else if (ICharSequence::Probe(obj) != NULL) {
            String str = Object::ToString(obj);
            *value = StringUtils::ParseInt64(str);
            return NOERROR;
        }
        else {
            // E_CLASS_CAST_EXCEPTION
            Logger::E(TAG, String("Cannot cast value for ") + key + " to a Int64");
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CContentValues::GetAsInt32(
    /* [in] */ const String& key,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    AutoPtr<IInterface> obj;
    mValues->Get(ko, (IInterface**)&obj);

    if (NULL != obj) {
        if (INumber::Probe(obj) != NULL) {
            FAIL_RETURN(INumber::Probe(obj)->Int32Value(value))
            return NOERROR;
        }
        else if (ICharSequence::Probe(obj) != NULL) {
            String str = Object::ToString(obj);
            *value = StringUtils::ParseInt32(str);
            return NOERROR;
        }
        else {
            // E_CLASS_CAST_EXCEPTION
            Logger::E(TAG, String("Cannot cast value for ") + key + " to a Int32");
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CContentValues::GetAsInt16(
    /* [in] */ const String& key,
    /* [out] */ Int16* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    AutoPtr<IInterface> obj;
    mValues->Get(ko, (IInterface**)&obj);

    if (NULL != obj) {
        if (INumber::Probe(obj) != NULL) {
            FAIL_RETURN(INumber::Probe(obj)->Int16Value(value))
            return NOERROR;
        }
        else if (ICharSequence::Probe(obj) != NULL) {
            String str = Object::ToString(obj);
            *value = StringUtils::ParseInt16(str);
            return NOERROR;
        }
        else {
            // E_CLASS_CAST_EXCEPTION
            Logger::E(TAG, String("Cannot cast value for ") + key + " to a Int16");
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CContentValues::GetAsByte(
    /* [in] */ const String& key,
    /* [out] */ Byte* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    AutoPtr<IInterface> obj;
    mValues->Get(ko, (IInterface**)&obj);

    if (NULL != obj) {
        if (INumber::Probe(obj) != NULL) {
            Int16 tmpValue = 0;
            FAIL_RETURN(INumber::Probe(obj)->Int16Value(&tmpValue))
            *value = (Byte)tmpValue;
            return NOERROR;
        }
        else if (ICharSequence::Probe(obj) != NULL) {
            String str = Object::ToString(obj);
            *value = (Byte)StringUtils::ParseInt32(str);
            return NOERROR;
        }
        else {
            // E_CLASS_CAST_EXCEPTION
            Logger::E(TAG, String("Cannot cast value for ") + key + " to a Byte");
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CContentValues::GetAsDouble(
    /* [in] */ const String& key,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0.0;
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    AutoPtr<IInterface> obj;
    mValues->Get(ko, (IInterface**)&obj);

    if (NULL != obj) {
        if (INumber::Probe(obj) != NULL) {
            FAIL_RETURN(INumber::Probe(obj)->DoubleValue(value))
            return NOERROR;
        }
        else if (ICharSequence::Probe(obj) != NULL) {
            String str = Object::ToString(obj);
            *value = StringUtils::ParseDouble(str);
            return NOERROR;
        }
        else {
            // E_CLASS_CAST_EXCEPTION
            Logger::E(TAG, String("Cannot cast value for ") + key + " to a Double");
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CContentValues::GetAsFloat(
    /* [in] */ const String& key,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0.0;
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    AutoPtr<IInterface> obj;
    mValues->Get(ko, (IInterface**)&obj);

    if (NULL != obj) {
        if (INumber::Probe(obj) != NULL) {
            FAIL_RETURN(INumber::Probe(obj)->FloatValue(value))
            return NOERROR;
        }
        else if (ICharSequence::Probe(obj) != NULL) {
            String str = Object::ToString(obj);
            *value = StringUtils::ParseFloat(str);
            return NOERROR;
        }
        else {
            // E_CLASS_CAST_EXCEPTION
            Logger::E(TAG, String("Cannot cast value for ") + key + " to a Float");
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CContentValues::GetAsBoolean(
    /* [in] */ const String& key,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    AutoPtr<IInterface> obj;
    mValues->Get(ko, (IInterface**)&obj);

    if (NULL != obj) {
        if (IBoolean::Probe(obj) != NULL) {
            FAIL_RETURN(((IBoolean*) (IInterface*) obj)->GetValue(value))
            return NOERROR;
        }
        else if (INumber::Probe(obj) != NULL) {
            Int32 tmp = 0;
            FAIL_RETURN(INumber::Probe(obj)->Int32Value(&tmp))
            *value = tmp != 0;
            return NOERROR;
        }
        else if (ICharSequence::Probe(obj) != NULL) {
            String str = Object::ToString(obj);
            *value = str.EqualsIgnoreCase("TRUE");
            return NOERROR;
        }
        else {
            // E_CLASS_CAST_EXCEPTION
            Logger::E(TAG, String("Cannot cast value for ") + key + " to a Boolean");
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CContentValues::GetAsByteArray(
    /* [in] */ const String& key,
    /* [out, callee] */ ArrayOf<Byte>** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    AutoPtr<IInterface> obj;
    mValues->Get(ko, (IInterface**)&obj);

    if (NULL != obj) {
        AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);
        if (array != NULL) {
            Int32 length = 0;
            FAIL_RETURN(array->GetLength(&length))
            AutoPtr<ArrayOf<Byte> > byteArray = ArrayOf<Byte>::Alloc(length);

            Byte b;
            for (Int32 i = 0; i != length; i++) {
                AutoPtr<IInterface> byteObj;
                FAIL_RETURN(array->Get(i, (IInterface**)&byteObj))
                FAIL_RETURN(IByte::Probe(byteObj)->GetValue(&b))
                (*byteArray)[i] = b;
            }
            *value = byteArray;
            REFCOUNT_ADD(*value);
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CContentValues::GetValueSet(
    /* [out] */ ISet** values)
{
    return mValues->GetEntrySet(values);
}

ECode CContentValues::GetKeySet(
    /* [out] */ ISet** values)
{
    return mValues->GetKeySet(values);
}

ECode CContentValues::PutStringArrayList(
    /* [in] */ const String& key,
    /* [in] */ IArrayList* value)
{
    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    return mValues->Put(TO_IINTERFACE(ko), TO_IINTERFACE(value));
}

ECode CContentValues::GetStringArrayList(
    /* [in] */ const String& key,
    /* [out] */ IArrayList** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    if (key.IsNull()) return E_INVALID_ARGUMENT;
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    AutoPtr<IInterface> obj;
    mValues->Get(ko, (IInterface**)&obj);
    *value = IArrayList::Probe(obj);
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode CContentValues::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    AutoPtr<ISet> entrySet;
    mValues->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    IMapEntry* entry;
    StringBuilder sb("{");
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        entry = IMapEntry::Probe(obj);
        AutoPtr<IInterface> ko, vo;
        entry->GetKey((IInterface**)&ko);
        entry->GetValue((IInterface**)&vo);

        sb.Append(Object::ToString(ko));
        sb.Append("=");
        sb.Append(Object::ToString(vo));
        sb.Append(";");
    }

    sb.Append("}");
    *str = sb.ToString();
    return NOERROR;
}

ECode CContentValues::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    mValues->Clear();
    Int32 size = 0;
    source->ReadInt32(&size);
    if (size == -1) {
        return NOERROR;
    }

    String key;
    AutoPtr<IInterface> value;
    AutoPtr<ICharSequence> ko;
    while (size > 0) {
        source->ReadString(&key);
        value = ReadValue(source);
        ko = CoreUtils::Convert(key);
        mValues->Put(TO_IINTERFACE(ko), value);
        size--;
    }

    return NOERROR;
}

AutoPtr<IInterface> CContentValues::ReadValue(
    /* [in] */ IParcel* source)
{
    Int32 type;
    source->ReadInt32(&type);

    switch (type) {
    case VAL_NULL: {
        return NULL;
    }
    case VAL_BYTE: {
        Int32 v;
        source->ReadInt32(&v);
        AutoPtr<IByte> obj;
        CByte::New((Byte)v, (IByte**)&obj);
        return TO_IINTERFACE(obj);
    }
    case VAL_STRING: {
        String v;
        source->ReadString(&v);
        AutoPtr<ICharSequence> obj;
        CString::New(v, (ICharSequence**)&obj);
        return TO_IINTERFACE(obj);
    }
    case VAL_INTEGER16: {
        Int32 v;
        source->ReadInt32(&v);
        Int16 sv = (Int16)v;
        AutoPtr<IInteger16> obj;
        CInteger16::New(sv, (IInteger16**)&obj);
        return TO_IINTERFACE(obj);
    }
    case VAL_INTEGER32: {
        Int32 v;
        source->ReadInt32(&v);
        AutoPtr<IInteger32> obj;
        CInteger32::New(v, (IInteger32**)&obj);
        return TO_IINTERFACE(obj);
    }
    case VAL_INTEGER64: {
        Int64 v;
        source->ReadInt64(&v);
        AutoPtr<IInteger64> obj;
        CInteger64::New(v, (IInteger64**)&obj);
        return TO_IINTERFACE(obj);
    }
    case VAL_FLOAT: {
        Float v;
        source->ReadFloat(&v);
        AutoPtr<IFloat> obj;
        CFloat::New(v, (IFloat**)&obj);
        return TO_IINTERFACE(obj);
    }
    case VAL_DOUBLE: {
        Double v;
        source->ReadDouble(&v);
        AutoPtr<IDouble> obj;
        CDouble::New(v, (IDouble**)&obj);
        return TO_IINTERFACE(obj);
    }
    case VAL_BOOLEAN: {
        Int32 v;
        source->ReadInt32(&v);
        AutoPtr<IBoolean> obj;
        CBoolean::New(v == 1, (IBoolean**)&obj);
        return TO_IINTERFACE(obj);
    }
    case VAL_ARRAYOF: {
        AutoPtr<IArrayOf> array;
        InterfaceID iid;
        source->ReadEMuid(&iid);
        Int32 size = 0;
        source->ReadInt32(&size);
        CArrayOf::New(iid, size, (IArrayOf**)&array);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> elem = ReadValue(source);
            array->Set(i, elem);
        }
        return TO_IINTERFACE(array);
    }
    case VAL_ARRAYLIST: {
        AutoPtr<IArrayList> list;
        CArrayList::New((IArrayList**)&list);
        Int32 size = 0;
        source->ReadInt32(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> elem = ReadValue(source);
            list->Add(i, elem);
        }
        return TO_IINTERFACE(list);
    }
    default:
        Logger::D("CContentValues", "- Unmarshalling unknown type code %d", type);
        assert(0);
    }
    return NULL;
}

ECode CContentValues::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Boolean isEmpty;
    if (mValues->IsEmpty(&isEmpty), isEmpty) {
        dest->WriteInt32(-1);
        return NOERROR;
    }

    Int32 size;
    mValues->GetSize(&size);
    dest->WriteInt32(size);


    AutoPtr<ISet> entrySet;
    mValues->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    IMapEntry* entry;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        entry = IMapEntry::Probe(obj);
        AutoPtr<IInterface> ko, vo;
        entry->GetKey((IInterface**)&ko);
        entry->GetValue((IInterface**)&vo);

        dest->WriteString(Object::ToString(ko));
        WriteValue(dest, vo);
    }

    return NOERROR;
}

ECode CContentValues::WriteValue(
    /* [in] */ IParcel* dest,
    /* [in] */ IInterface* obj)
{
    if (obj == NULL) {
        dest->WriteInt32(VAL_NULL);
    }
    else if (ICharSequence::Probe(obj) != NULL) {
        String v;
        ICharSequence::Probe(obj)->ToString(&v);
        dest->WriteInt32(VAL_STRING);
        dest->WriteString(v);
    }
    else if (IByte::Probe(obj) != NULL) {
        Byte v;
        IByte::Probe(obj)->GetValue(&v);
        dest->WriteInt32(VAL_BYTE);
        dest->WriteInt32((Int32)v);
    }
    else if (IInteger16::Probe(obj) != NULL) {
        Int16 v;
        IInteger16::Probe(obj)->GetValue(&v);
        dest->WriteInt32(VAL_INTEGER16);
        dest->WriteInt32((Int32)v);
    }
    else if (IInteger32::Probe(obj) != NULL) {
        Int32 v;
        IInteger32::Probe(obj)->GetValue(&v);
        dest->WriteInt32(VAL_INTEGER32);
        dest->WriteInt32(v);
    }
    else if (IInteger64::Probe(obj) != NULL) {
        Int64 v;
        IInteger64::Probe(obj)->GetValue(&v);
        dest->WriteInt32(VAL_INTEGER64);
        dest->WriteInt64(v);
    }
    else if (IFloat::Probe(obj) != NULL) {
        Float v;
        IFloat::Probe(obj)->GetValue(&v);
        dest->WriteInt32(VAL_FLOAT);
        dest->WriteFloat(v);
    }
    else if (IDouble::Probe(obj) != NULL) {
        Double v;
        IDouble::Probe(obj)->GetValue(&v);
        dest->WriteInt32(VAL_DOUBLE);
        dest->WriteDouble(v);
    }
    else if (IBoolean::Probe(obj) != NULL) {
        Boolean v;
        IBoolean::Probe(obj)->GetValue(&v);
        dest->WriteInt32(VAL_BOOLEAN);
        dest->WriteInt32(v ? 1 : 0);
    }
    else if (IArrayOf::Probe(obj) != NULL){
        AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);
        Int32 size = 0;
        array->GetLength(&size);
        dest->WriteInt32(VAL_ARRAYOF);
        InterfaceID iid;
        array->GetTypeId(&iid);
        dest->WriteEMuid(iid);
        dest->WriteInt32(size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> elem;
            array->Get(i, (IInterface**)&elem);
            WriteValue(dest, elem);
        }
    }
    else if (IArrayList::Probe(obj) != NULL) {
        AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);
        Int32 size = 0;
        array->GetLength(&size);
        dest->WriteInt32(VAL_ARRAYLIST);
        dest->WriteInt32(size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> elem;
            array->Get(i, (IInterface**)&elem);
            WriteValue(dest, elem);
        }
    }
    else {
        assert(0);
        // throw new RuntimeException("Parcel: unable to marshal value " + v);
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

}
}
}

