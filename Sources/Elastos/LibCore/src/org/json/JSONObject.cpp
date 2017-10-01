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

#include "JSONObject.h"
#include "JSON.h"
#include "CJSONArray.h"
#include "CJSONObject.h"
#include "CJSONTokener.h"
#include "CJSONStringer.h"
#include "CoreUtils.h"
#include "StringUtils.h"
#include "Math.h"
#include "utility/CArrayList.h"
#include "utility/CLinkedHashMap.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::INumber;
using Elastos::Core::IString;
using Elastos::Core::StringUtils;
using Elastos::Utility::ICollection;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CLinkedHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Json {

const AutoPtr<IInterface> JSONObject::Object_NULL = (IObject*)new MyObject();

const Double JSONObject::NEGATIVE_ZERO = -0.0l;

//==============================================================================
//          JSONObject::MyObject
//==============================================================================

JSONObject::MyObject::MyObject()
{}

JSONObject::MyObject::~MyObject()
{}

ECode JSONObject::MyObject::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = IInterface::Probe(o) == TO_IINTERFACE(this) || o == NULL; // API specifies this broken equals implementation
    return NOERROR;
}

ECode JSONObject::MyObject::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "NULL";
    return NOERROR;
}

//==============================================================================
//          JSONObject
//==============================================================================

CAR_INTERFACE_IMPL(JSONObject, Object, IJSONObject)

JSONObject::JSONObject()
{}

JSONObject::~JSONObject()
{}

ECode JSONObject::constructor()
{
    CLinkedHashMap::New((ILinkedHashMap**)&mNameValuePairs);
    return NOERROR;
}

ECode JSONObject::constructor(
    /* [in] */ IMap* copyFrom)
{
    constructor();

    AutoPtr<IMap> contentsTyped = copyFrom;
    AutoPtr<ISet> set;
    contentsTyped->GetEntrySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        /*
         * Deviate from the original by checking that keys are non-null and
         * of the proper type. (We still defer validating the values).
         */
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);

        AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
        AutoPtr<IInterface> keyObj;
        entry->GetKey((IInterface**)&keyObj);
        String key;

        if (!ICharSequence::Probe(keyObj)) {
            Logger::E("JSONObject", "key == null");
            return E_NULL_POINTER_EXCEPTION;
        }

        ICharSequence::Probe(keyObj)->ToString(&key);
        if (key.IsNull()) {
            Logger::E("JSONObject", "key == null");
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<IInterface> valueObj;
        entry->GetValue((IInterface**)&valueObj);

        AutoPtr<IInterface> objResult;
        Wrap(valueObj, (IInterface**)&objResult);
        mNameValuePairs->Put(keyObj, objResult);
    }

    return NOERROR;
}

ECode JSONObject::constructor(
    /* [in] */ IJSONTokener* readFrom)
{
    /*
     * Getting the parser to populate this could get tricky. Instead, just
     * parse to temporary JSONObject and then steal the data from that.
     */
    AutoPtr<IInterface> object;
    readFrom->NextValue((IInterface**)&object);
    if (IJSONObject::Probe(object) != NULL) {
        AutoPtr<IJSONObject> jsonObject = IJSONObject::Probe(object);
        AutoPtr<JSONObject> jObject = (JSONObject*)jsonObject.Get();
        mNameValuePairs = jObject->mNameValuePairs;
    }
    else {
        return JSON::TypeMismatch(object, String("JSONObject"));
    }
    return NOERROR;
}

ECode JSONObject::constructor(
    /* [in] */ const String& json)
{
    AutoPtr<IJSONTokener> tokener;
    CJSONTokener::New(json, (IJSONTokener**)&tokener);
    return constructor(tokener);
}

ECode JSONObject::constructor(
    /* [in] */ IJSONObject* copyFrom,
    /* [in] */ ArrayOf<String>* names)
{
    constructor();
    for (Int32 i = 0; i < names->GetLength(); ++i) {
        String name = (*names)[i];
        AutoPtr<IInterface> value;
        copyFrom->Opt(name, (IInterface**)&value);
        if (value != NULL) {
            mNameValuePairs->Put(CoreUtils::Convert(name), value);
        }
    }
    return NOERROR;
}

ECode JSONObject::GetLength(
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len);
    return mNameValuePairs->GetSize(len);
}

ECode JSONObject::Put(
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
    FAIL_RETURN(CheckName(name));
    return mNameValuePairs->Put(CoreUtils::Convert(name), CoreUtils::Convert(value));
}

ECode JSONObject::Put(
    /* [in] */ const String& name,
    /* [in] */ Double value)
{
    FAIL_RETURN(CheckName(name));
    FAIL_RETURN(JSON::CheckDouble(value));
    return mNameValuePairs->Put(CoreUtils::Convert(name), CoreUtils::Convert(value));
}

ECode JSONObject::Put(
    /* [in] */ const String& name,
    /* [in] */ Int32 value)
{
    FAIL_RETURN(CheckName(name));
    return mNameValuePairs->Put(CoreUtils::Convert(name), CoreUtils::Convert(value));
}

ECode JSONObject::Put(
    /* [in] */ const String& name,
    /* [in] */ Int64 value)
{
    FAIL_RETURN(CheckName(name));
    return mNameValuePairs->Put(CoreUtils::Convert(name), CoreUtils::Convert(value));
}

ECode JSONObject::Put(
    /* [in] */ const String& name,
    /* [in] */ IInterface* value)
{
    if (value == NULL) {
        mNameValuePairs->Remove(CoreUtils::Convert(name));
        return NOERROR;
    }
    if (INumber::Probe(value) != NULL) {
        // deviate from the original by checking all Numbers, not just floats & doubles
        Double data;
        INumber::Probe(value)->DoubleValue(&data);
        FAIL_RETURN(JSON::CheckDouble(data));
    }

    FAIL_RETURN(CheckName(name));
    mNameValuePairs->Put(CoreUtils::Convert(name), value);
    return NOERROR;
}

ECode JSONObject::PutOpt(
    /* [in] */ const String& name,
    /* [in] */ IInterface* value)
{
    if (name.IsNull() || value == NULL) {
        return NOERROR;
    }
    return Put(name, value);
}

ECode JSONObject::Accumulate(
    /* [in] */ const String& name,
    /* [in] */ IInterface* value)
{
    FAIL_RETURN(CheckName(name));
    AutoPtr<IInterface> current;
    mNameValuePairs->Get(CoreUtils::Convert(name), (IInterface**)&current);
    if (current == NULL) {
        return Put(name, value);
    }

    if (IJSONArray::Probe(current) != NULL) {
        AutoPtr<IJSONArray> obj = IJSONArray::Probe(current);
        JSONArray* array = (JSONArray*)obj.Get();
        return array->CheckedPut(value);
    }
    else {
        AutoPtr<IJSONArray> obj;
        CJSONArray::New((IJSONArray**)&obj);
        JSONArray* array = (JSONArray*)obj.Get();
        FAIL_RETURN(array->CheckedPut(current));
        FAIL_RETURN(array->CheckedPut(value));
        mNameValuePairs->Put(CoreUtils::Convert(name), obj);
    }
    return NOERROR;
}

ECode JSONObject::Append(
    /* [in] */ const String& name,
    /* [in] */ IInterface* value)
{
    FAIL_RETURN(CheckName(name));
    AutoPtr<IInterface> current;
    mNameValuePairs->Get(CoreUtils::Convert(name), (IInterface**)&current);

    AutoPtr<IJSONArray> array;
    if (IJSONArray::Probe(current) != NULL) {
        array = IJSONArray::Probe(current);
    }
    else if (current == NULL) {
        AutoPtr<IJSONArray> newArray;
        CJSONArray::New((IJSONArray**)&newArray);
        mNameValuePairs->Put(CoreUtils::Convert(name), newArray);
        array = newArray;
    }
    else {
        Logger::E("JSONObject", "Key %s is not a JSONArray", name.string());
        return E_JSON_EXCEPTION;
        // throw new JSONException("Key " + name + " is not a JSONArray");
    }

    return ((JSONArray*)array.Get())->CheckedPut(value);
}

ECode JSONObject::CheckName(
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        Logger::E("JSONObject", "Names must be non-null");
        return E_JSON_EXCEPTION;
        // throw new JSONException("Names must be non-null");
    }
    return NOERROR;
}

ECode JSONObject::Remove(
    /* [in] */ const String& name,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    return mNameValuePairs->Remove(CoreUtils::Convert(name), value);
}

ECode JSONObject::IsNull(
    /* [in] */ const String& name,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    AutoPtr<IInterface> value;
    mNameValuePairs->Get(CoreUtils::Convert(name), (IInterface**)&value);
    *res = value == NULL || IInterface::Probe(value) == IInterface::Probe(Object_NULL);
    return NOERROR;
}

ECode JSONObject::Has(
    /* [in] */ const String& name,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return mNameValuePairs->ContainsKey(CoreUtils::Convert(name), res);
}

ECode JSONObject::Get(
    /* [in] */ const String& name,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;

    AutoPtr<IInterface> result;
    mNameValuePairs->Get(CoreUtils::Convert(name), (IInterface**)&result);
    if (result == NULL) {
        Logger::E("JSONObject", "No value for %s", name.string());
        return E_JSON_EXCEPTION;
    }
    *obj = result;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode JSONObject::Opt(
    /* [in] */ const String& name,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    return mNameValuePairs->Get(CoreUtils::Convert(name), obj);
}

ECode JSONObject::GetBoolean(
    /* [in] */ const String& name,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    AutoPtr<IInterface> object;
    Get(name, (IInterface**)&object);
    AutoPtr<IBoolean> result;
    JSON::ToBoolean(object, (IBoolean**)&result);
    if (result == NULL) {
        return JSON::TypeMismatch(CoreUtils::Convert(name), object, String("boolean"));
    }

    return result->GetValue(res);
}

ECode JSONObject::OptBoolean(
    /* [in] */ const String& name,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return OptBoolean(name, FALSE, res);
}

ECode JSONObject::OptBoolean(
    /* [in] */ const String& name,
    /* [in] */ Boolean fallback,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IInterface> object;
    Opt(name, (IInterface**)&object);
    AutoPtr<IBoolean> result;
    JSON::ToBoolean(object, (IBoolean**)&result);
    if (result != NULL) {
        return result->GetValue(res);
    }
    *res = fallback;
    return NOERROR;
}

ECode JSONObject::GetDouble(
    /* [in] */ const String& name,
    /* [out] */ Double* data)
{
    VALIDATE_NOT_NULL(data);
    *data = 0;

    AutoPtr<IInterface> object;
    Get(name, (IInterface**)&object);
    AutoPtr<IDouble> result;
    JSON::ToDouble(object, (IDouble**)&result);
    if (result == NULL) {
        return JSON::TypeMismatch(CoreUtils::Convert(name), object, String("double"));
    }
    return result->GetValue(data);
}

ECode JSONObject::OptDouble(
    /* [in] */ const String& name,
    /* [out] */ Double* data)
{
    VALIDATE_NOT_NULL(data);
    return OptDouble(name, Elastos::Core::Math::DOUBLE_NAN, data);
}

ECode JSONObject::OptDouble(
    /* [in] */ const String& name,
    /* [in] */ Double fallback,
    /* [out] */ Double* data)
{
    VALIDATE_NOT_NULL(data);

    AutoPtr<IInterface> object;
    Opt(name, (IInterface**)&object);
    AutoPtr<IDouble> result;
    JSON::ToDouble(object, (IDouble**)&result);
    if (result != NULL) {
        return result->GetValue(data);
    }
    *data = fallback;
    return NOERROR;
}

ECode JSONObject::GetInt32(
    /* [in] */ const String& name,
    /* [out] */ Int32* data)
{
    VALIDATE_NOT_NULL(data);
    *data = 0;

    AutoPtr<IInterface> object;
    Get(name, (IInterface**)&object);
    AutoPtr<IInteger32> result;
    JSON::ToInteger32(object, (IInteger32**)&result);
    if (result == NULL) {
        return JSON::TypeMismatch(CoreUtils::Convert(name), object, String("int"));
    }
    return result->GetValue(data);
}

ECode JSONObject::OptInt32(
    /* [in] */ const String& name,
    /* [out] */ Int32* data)
{
    VALIDATE_NOT_NULL(data);
    return OptInt32(name, 0, data);
}

ECode JSONObject::OptInt32(
    /* [in] */ const String& name,
    /* [in] */ Int32 fallback,
    /* [out] */ Int32* data)
{
    VALIDATE_NOT_NULL(data);

    AutoPtr<IInterface> object;
    Opt(name, (IInterface**)&object);
    AutoPtr<IInteger32> result;
    JSON::ToInteger32(object, (IInteger32**)&result);
    if (result != NULL) {
        return result->GetValue(data);
    }
    *data = fallback;
    return NOERROR;
}

ECode JSONObject::GetInt64(
    /* [in] */ const String& name,
    /* [out] */ Int64* data)
{
    VALIDATE_NOT_NULL(data);
    *data = 0LL;

    AutoPtr<IInterface> object;
    Get(name, (IInterface**)&object);
    AutoPtr<IInteger64> result;
    JSON::ToInteger64(object, (IInteger64**)&result);
    if (result == NULL) {
        return JSON::TypeMismatch(CoreUtils::Convert(name), object, String("long"));
    }
    return result->GetValue(data);
}

ECode JSONObject::OptInt64(
    /* [in] */ const String& name,
    /* [out] */ Int64* data)
{
    VALIDATE_NOT_NULL(data);
    return OptInt64(name, 0LL, data);
}

ECode JSONObject::OptInt64(
    /* [in] */ const String& name,
    /* [in] */ Int64 fallback,
    /* [out] */ Int64* data)
{
    VALIDATE_NOT_NULL(data);

    AutoPtr<IInterface> object;
    Opt(name, (IInterface**)&object);
    AutoPtr<IInteger64> result;
    JSON::ToInteger64(object, (IInteger64**)&result);
    if (result != NULL) {
        return result->GetValue(data);
    }
    *data = fallback;
    return NOERROR;
}

ECode JSONObject::GetString(
    /* [in] */ const String& name,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);

    AutoPtr<IInterface> object;
    Get(name, (IInterface**)&object);
    String result;
    JSON::ToString(object, &result);
    if (result.IsNull()) {
        return JSON::TypeMismatch(CoreUtils::Convert(name), object, String("String"));
    }
    *str = result;
    return NOERROR;
}

ECode JSONObject::OptString(
    /* [in] */ const String& name,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    return OptString(name, String(""), str);
}

ECode JSONObject::OptString(
    /* [in] */ const String& name,
    /* [in] */ const String& fallback,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<IInterface> object;
    Opt(name, (IInterface**)&object);
    String result;
    JSON::ToString(object, &result);
    if (!result.IsNull()) {
        *str = result;
        return NOERROR;
    }
    *str = fallback;
    return NOERROR;
}

ECode JSONObject::GetJSONArray(
    /* [in] */ const String& name,
    /* [out] */ IJSONArray** jsonArray)
{
    VALIDATE_NOT_NULL(jsonArray);
    *jsonArray = NULL;

    AutoPtr<IInterface> object;
    Get(name, (IInterface**)&object);
    if (IJSONArray::Probe(object)) {
        *jsonArray = IJSONArray::Probe(object);
        REFCOUNT_ADD(*jsonArray);
        return NOERROR;
    }
    else {
        return JSON::TypeMismatch(CoreUtils::Convert(name), object, String("JSONArray"));
    }
}

ECode JSONObject::OptJSONArray(
    /* [in] */ const String& name,
    /* [out] */ IJSONArray** jsonArray)
{
    VALIDATE_NOT_NULL(jsonArray);
    *jsonArray = NULL;

    AutoPtr<IInterface> object;
    Opt(name, (IInterface**)&object);
    if (IJSONArray::Probe(object) != NULL) {
        *jsonArray = IJSONArray::Probe(object);
        REFCOUNT_ADD(*jsonArray);
    }
    return NOERROR;
}

ECode JSONObject::GetJSONObject(
    /* [in] */ const String& name,
    /* [out] */ IJSONObject** jsonObject)
{
    VALIDATE_NOT_NULL(jsonObject);
    *jsonObject = NULL;

    AutoPtr<IInterface> object;
    Get(name, (IInterface**)&object);
    if (IJSONObject::Probe(object)) {
        *jsonObject = IJSONObject::Probe(object);
        REFCOUNT_ADD(*jsonObject);
        return NOERROR;
    }
    else {
        return JSON::TypeMismatch(CoreUtils::Convert(name), object, String("JSONObject"));
    }
}

ECode JSONObject::OptJSONObject(
    /* [in] */ const String& name,
    /* [out] */ IJSONObject** jsonObject)
{
    VALIDATE_NOT_NULL(jsonObject);
    *jsonObject = NULL;

    AutoPtr<IInterface> object;
    Opt(name, (IInterface**)&object);
    if (IJSONObject::Probe(object) != NULL) {
        *jsonObject = IJSONObject::Probe(object);
        REFCOUNT_ADD(*jsonObject);
    }
    return NOERROR;
}

ECode JSONObject::ToJSONArray(
    /* [in] */ IJSONArray* names,
    /* [out] */ IJSONArray** jsonArray)
{
    VALIDATE_NOT_NULL(jsonArray);
    *jsonArray = NULL;

    if (names == NULL) {
        return NOERROR;
    }

    Int32 length;
    names->GetLength(&length);
    if (length == 0) {
        return NOERROR;
    }

    AutoPtr<IJSONArray> result;
    CJSONArray::New((IJSONArray**)&result);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> obj;
        names->Opt(i, (IInterface**)&obj);
        String name;
        JSON::ToString(obj, &name);
        AutoPtr<IInterface> object;
        Opt(name, (IInterface**)&object);
        result->Put(object);
    }
    *jsonArray = result;
    REFCOUNT_ADD(*jsonArray);
    return NOERROR;
}

ECode JSONObject::GetKeys(
    /* [out] */ IIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator);

    AutoPtr<ISet> set;
    mNameValuePairs->GetKeySet((ISet**)&set);
    return set->GetIterator(iterator);
}

ECode JSONObject::GetKeySet(
    /* [out] */ ISet** set)
{
    VALIDATE_NOT_NULL(set);

    return mNameValuePairs->GetKeySet(set);
}

ECode JSONObject::GetNames(
    /* [out] */ IJSONArray** jsonArray)
{
    VALIDATE_NOT_NULL(jsonArray);
    *jsonArray = NULL;

    Boolean res;
    mNameValuePairs->IsEmpty(&res);
    if (!res) {
        AutoPtr<ISet> set;
        mNameValuePairs->GetKeySet((ISet**)&set);
        AutoPtr<ICollection> coll;
        CArrayList::New(ICollection::Probe(set), (ICollection**)&coll);
        return CJSONArray::New(coll, jsonArray);
    }
    return NOERROR;
}

ECode JSONObject::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    // try {
    AutoPtr<IJSONStringer> stringer;
    CJSONStringer::New((IJSONStringer**)&stringer);
    ECode ec = WriteTo(stringer);
    if (ec == (ECode) E_JSON_EXCEPTION) {
        *str = NULL;
        return NOERROR;
    }
    return IObject::Probe(stringer)->ToString(str);
    // } catch (JSONException e) {
    //     return null;
    // }
}

ECode JSONObject::ToString(
    /* [in] */ Int32 indentSpaces,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<IJSONStringer> stringer;
    CJSONStringer::New(indentSpaces, (IJSONStringer**)&stringer);
    WriteTo(stringer);
    return IObject::Probe(stringer)->ToString(str);
}

ECode JSONObject::WriteTo(
    /* [in] */ IJSONStringer* stringer)
{
    stringer->Object();

    AutoPtr<ISet> set;
    mNameValuePairs->GetEntrySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
        AutoPtr<IInterface> keyObj;
        entry->GetKey((IInterface**)&keyObj);
        AutoPtr<IInterface> valueObj;
        entry->GetValue((IInterface**)&valueObj);
        String key;
        ICharSequence::Probe(keyObj)->ToString(&key);
        stringer->Key(key);
        FAIL_RETURN(stringer->Value(valueObj));
    }

    stringer->EndObject();
    return NOERROR;
}

ECode JSONObject::NumberToString(
    /* [in] */ INumber* number,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);

    if (number == NULL) {
        Logger::E("JSONObject", "Number must be non-null");
        return E_JSON_EXCEPTION;
        // throw new JSONException("Number must be non-null");
    }

    Double doubleValue;
    number->DoubleValue(&doubleValue);
    FAIL_RETURN(JSON::CheckDouble(doubleValue));

    // the original returns "-0" instead of "-0.0" for negative zero
    Boolean res;
    if (IObject::Probe(number)->Equals(CoreUtils::Convert(NEGATIVE_ZERO), &res), res) {
        *str = "-0";
        return NOERROR;
    }

    Int64 longValue;
    number->Int64Value(&longValue);
    if (doubleValue == (Double) longValue) {
        *str = StringUtils::ToString(longValue);
        return NOERROR;
    }

    return IObject::Probe(number)->ToString(str);
}

ECode JSONObject::Quote(
    /* [in] */ const String& data,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    if (data.IsNull()) {
        *str = "\"\"";
        return NOERROR;
    }
    // try {
    AutoPtr<IJSONStringer> stringer;
    CJSONStringer::New((IJSONStringer**)&stringer);
    stringer->Open(JSONStringerScope_NULL, String(""));
    stringer->Value(CoreUtils::Convert(data));
    stringer->Close(JSONStringerScope_NULL, JSONStringerScope_NULL, String(""));
    return IObject::Probe(stringer)->ToString(str);
    // } catch (JSONException e) {
    //     throw new AssertionError();
    // }
}

ECode JSONObject::Wrap(
    /* [in] */ IInterface* o,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;

    if (o == NULL) {
        *obj = Object_NULL;
        REFCOUNT_ADD(*obj);
        return NOERROR;
    }
    if (IJSONArray::Probe(o) != NULL || IJSONObject::Probe(o) != NULL) {
        *obj = o;
        REFCOUNT_ADD(*obj);
        return NOERROR;
    }
    Boolean res;
    if (IObject::Probe(o)->Equals(Object_NULL, &res), res) {
        *obj = o;
        REFCOUNT_ADD(*obj);
        return NOERROR;
    }
    // try {
    if (ICollection::Probe(o) != NULL) {
        AutoPtr<IJSONArray> jsonArray;
        CJSONArray::New(ICollection::Probe(o), (IJSONArray**)&jsonArray);
        *obj = jsonArray;
        REFCOUNT_ADD(*obj);
        return NOERROR;
    }
    else if (IArrayOf::Probe(o) != NULL) {
        AutoPtr<IJSONArray> jsonArray;
        CJSONArray::New(o, (IJSONArray**)&jsonArray);
        *obj = jsonArray;
        REFCOUNT_ADD(*obj);
        return NOERROR;
    }
    if (IMap::Probe(o) != NULL) {
        AutoPtr<IJSONObject> jsonObject;
        CJSONObject::New(IMap::Probe(o), (IJSONObject**)&jsonObject);
        *obj = jsonObject;
        REFCOUNT_ADD(*obj);
        return NOERROR;
    }
    if (IBoolean::Probe(o) != NULL ||
        IByte::Probe(o) != NULL ||
        IChar32::Probe(o) != NULL ||
        IDouble::Probe(o) != NULL ||
        IFloat::Probe(o) != NULL ||
        IInteger32::Probe(o) != NULL ||
        IInteger64::Probe(o) != NULL ||
        IInteger16::Probe(o) != NULL ||
        IString::Probe(o) != NULL ) {
        *obj = o;
        REFCOUNT_ADD(*obj);
        return NOERROR;
    }

    AutoPtr<IClassInfo> classInfo;
    CObject::ReflectClassInfo(o, (IClassInfo**)&classInfo);
    String name;;
    classInfo->GetNamespace(&name);
    if (name.StartWith("Elastos.")) {
        String str;
        IObject::Probe(o)->ToString(&str);
        AutoPtr<ICharSequence> cs = CoreUtils::Convert(str);
        *obj = cs;
        REFCOUNT_ADD(*obj);
        return NOERROR;
    }
    // } catch (Exception ignored) {
    // }
    return NOERROR;
}

} //namespace Json
} //namespace Org
