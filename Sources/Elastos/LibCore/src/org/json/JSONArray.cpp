
#include "JSONArray.h"
#include "JSON.h"
#include "JSONObject.h"
#include "CJSONObject.h"
#include "CJSONTokener.h"
#include "CJSONStringer.h"
#include "CoreUtils.h"
#include "utility/CArrayList.h"
#include "Math.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::INumber;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Org::Json::JSONStringerScope;

namespace Org {
namespace Json {

CAR_INTERFACE_IMPL(JSONArray, Object, IJSONArray)

JSONArray::JSONArray()
{}

JSONArray::~JSONArray()
{}

ECode JSONArray::constructor()
{
    CArrayList::New((IList**)&mValues);
    return NOERROR;
}

ECode JSONArray::constructor(
    /* [in] */ ICollection* copyFrom)
{
    constructor();
    if (copyFrom != NULL) {
        AutoPtr<IIterator> it;
        copyFrom->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while(it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> object;
            it->GetNext((IInterface**)&object);
            AutoPtr<IInterface> otherObj;
            JSONObject::Wrap(object, (IInterface**)&otherObj);
            Put(otherObj);
        }
    }
    return NOERROR;
}

ECode JSONArray::constructor(
    /* [in] */ IJSONTokener* readFrom)
{
    /*
     * Getting the parser to populate this could get tricky. Instead, just
     * parse to temporary JSONArray and then steal the data from that.
     */
    AutoPtr<IInterface> object;
    readFrom->NextValue((IInterface**)&object);
    if (IJSONArray::Probe(object) != NULL) {
        AutoPtr<IJSONArray> obj = IJSONArray::Probe(object);
        mValues = ((JSONArray*)obj.Get())->mValues;
    }
    else {
        return JSON::TypeMismatch(object, String("JSONArray"));
        // throw JSON.typeMismatch(object, "JSONArray");
    }
    return NOERROR;
}

ECode JSONArray::constructor(
    /* [in] */ const String& json)
{
    AutoPtr<IJSONTokener> tokener;
    CJSONTokener::New(json, (IJSONTokener**)&tokener);

    return constructor(tokener);
}

ECode JSONArray::constructor(
    /* [in] */ IInterface* array)
{
    if (IArrayOf::Probe(array) == NULL) {
        Logger::E("JSONArray", "Not a primitive array: %p", array);
        return E_JSON_EXCEPTION;
        // throw new JSONException("Not a primitive array: " + array.getClass());
    }

    AutoPtr<IArrayOf> arrayOf = IArrayOf::Probe(array);
    Int32 length;
    arrayOf->GetLength(&length);
    CArrayList::New(length, (IList**)&mValues);
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IInterface> object;
        arrayOf->Get(i, (IInterface**)&object);
        AutoPtr<IInterface> otherObj;
        JSONObject::Wrap(object, (IInterface**)&otherObj);
        Put(otherObj);
    }
    return NOERROR;
}

ECode JSONArray::GetLength(
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len);
    return mValues->GetSize(len);
}

ECode JSONArray::Put(
    /* [in] */ Boolean value)
{
    mValues->Add(CoreUtils::Convert(value));
    return NOERROR;
}

ECode JSONArray::Put(
    /* [in] */ Double value)
{
    FAIL_RETURN(JSON::CheckDouble(value));
    mValues->Add(CoreUtils::Convert(value));
    return NOERROR;
}

ECode JSONArray::Put(
    /* [in] */ Int32 value)
{
    mValues->Add(CoreUtils::Convert(value));
    return NOERROR;
}

ECode JSONArray::Put(
    /* [in] */ Int64 value)
{
    mValues->Add(CoreUtils::Convert(value));
    return NOERROR;
}

ECode JSONArray::Put(
    /* [in] */ IInterface* value)
{
    mValues->Add(value);
    return NOERROR;
}

ECode JSONArray::CheckedPut(
    /* [in] */ IInterface* value)
{
    if (INumber::Probe(value)) {
        Double result;
        INumber::Probe(value)->DoubleValue(&result);
        FAIL_RETURN(JSON::CheckDouble(result));
    }

    return Put(value);
}

ECode JSONArray::Put(
    /* [in] */ Int32 index,
    /* [in] */ Boolean value)
{
    return Put(index, CoreUtils::Convert(value));
}

ECode JSONArray::Put(
    /* [in] */ Int32 index,
    /* [in] */ Double value)
{
    return Put(index, CoreUtils::Convert(value));
}

ECode JSONArray::Put(
    /* [in] */ Int32 index,
    /* [in] */ Int32 value)
{
    return Put(index, CoreUtils::Convert(value));
}

ECode JSONArray::Put(
    /* [in] */ Int32 index,
    /* [in] */ Int64 value)
{
    return Put(index, CoreUtils::Convert(value));
}

ECode JSONArray::Put(
    /* [in] */ Int32 index,
    /* [in] */ IInterface* value)
{
    if (INumber::Probe(value)) {
        // deviate from the original by checking all Numbers, not just floats & doubles
        Double result;
        INumber::Probe(value)->DoubleValue(&result);
        FAIL_RETURN(JSON::CheckDouble(result));
    }
    Int32 size;
    while (mValues->GetSize(&size), size <= index) {
        mValues->Add(NULL);
    }
    mValues->Set(index, value);
    return NOERROR;
}

ECode JSONArray::IsNull(
    /* [in] */ Int32 index,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    AutoPtr<IInterface> value;
    Opt(index, (IInterface**)&value);
    *res = value == NULL || value == JSONObject::Object_NULL;
    return NOERROR;
}

ECode JSONArray::Get(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;

    // try {
    AutoPtr<IInterface> value;
    ECode ec = mValues->Get(index, (IInterface**)&value);
    if (SUCCEEDED(ec)) {
        if (value == NULL) {
            Logger::E("JSONArray", "Value at %d is null", index);
            return E_JSON_EXCEPTION;
            // throw new JSONException("Value at " + index + " is null.");
        }
        *obj = value;
        REFCOUNT_ADD(*obj);
        return NOERROR;
    }
    Int32 size;
    mValues->GetSize(&size);
    Logger::E("JSONArray", "Index %d  out of range [0..%d]", index, size);
    return E_JSON_EXCEPTION;
    // } catch (IndexOutOfBoundsException e) {
    //     throw new JSONException("Index " + index + " out of range [0.." + values.size() + ")");
    // }
}

ECode JSONArray::Opt(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;

    Int32 size;
    mValues->GetSize(&size);
    if (index < 0 || index >= size) {
        return NOERROR;
    }
    return mValues->Get(index, obj);
}

ECode JSONArray::Remove(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;

    Int32 size;
    mValues->GetSize(&size);
    if (index < 0 || index >= size) {
        return NOERROR;
    }
    return mValues->Remove(index, obj);
}

ECode JSONArray::GetBoolean(
    /* [in] */ Int32 index,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    AutoPtr<IInterface> object;
    Get(index, (IInterface**)&object);
    AutoPtr<IBoolean> result;
    JSON::ToBoolean(object, (IBoolean**)&result);
    if (result == NULL) {
        return JSON::TypeMismatch(CoreUtils::Convert(index), object, String("boolean"));
        // throw JSON.typeMismatch(index, object, "boolean");
    }
    return result->GetValue(res);
}

ECode JSONArray::OptBoolean(
    /* [in] */ Int32 index,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return OptBoolean(index, FALSE, res);
}

ECode JSONArray::OptBoolean(
    /* [in] */ Int32 index,
    /* [in] */ Boolean fallback,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    AutoPtr<IInterface> object;
    Opt(index, (IInterface**)&object);
    AutoPtr<IBoolean> result;
    JSON::ToBoolean(object, (IBoolean**)&result);
    if (result != NULL) {
        return result->GetValue(res);
    }
    *res = fallback;
    return NOERROR;
}

ECode JSONArray::GetDouble(
    /* [in] */ Int32 index,
    /* [out] */ Double* data)
{
    VALIDATE_NOT_NULL(data);
    *data = 0;

    AutoPtr<IInterface> object;
    Get(index, (IInterface**)&object);
    AutoPtr<IDouble> result;
    JSON::ToDouble(object, (IDouble**)&result);
    if (result == NULL) {
        return JSON::TypeMismatch(CoreUtils::Convert(index), object, String("double"));
        // throw JSON.typeMismatch(index, object, "double");
    }
    return result->GetValue(data);
}

ECode JSONArray::OptDouble(
    /* [in] */ Int32 index,
    /* [out] */ Double* data)
{
    VALIDATE_NOT_NULL(data);
    return OptDouble(index, Elastos::Core::Math::DOUBLE_NAN, data);
}

ECode JSONArray::OptDouble(
    /* [in] */ Int32 index,
    /* [in] */ Double fallback,
    /* [out] */ Double* data)
{
    VALIDATE_NOT_NULL(data);

    AutoPtr<IInterface> object;
    Opt(index, (IInterface**)&object);
    AutoPtr<IDouble> result;
    JSON::ToDouble(object, (IDouble**)&result);
    if (result != NULL) {
        return result->GetValue(data);
    }
    *data = fallback;
    return NOERROR;
}

ECode JSONArray::GetInt32(
    /* [in] */ Int32 index,
    /* [out] */ Int32* data)
{
    VALIDATE_NOT_NULL(data);
    *data = 0;

    AutoPtr<IInterface> object;
    Get(index, (IInterface**)&object);
    AutoPtr<IInteger32> result;
    JSON::ToInteger32(object, (IInteger32**)&result);
    if (result == NULL) {
        return JSON::TypeMismatch(CoreUtils::Convert(index), object, String("int"));
        // throw JSON.typeMismatch(index, object, "int");
    }
    return result->GetValue(data);
}

ECode JSONArray::OptInt32(
    /* [in] */ Int32 index,
    /* [out] */ Int32* data)
{
    VALIDATE_NOT_NULL(data);
    return OptInt32(index, 0, data);
}

ECode JSONArray::OptInt32(
    /* [in] */ Int32 index,
    /* [in] */ Int32 fallback,
    /* [out] */ Int32* data)
{
    VALIDATE_NOT_NULL(data);

    AutoPtr<IInterface> object;
    Opt(index, (IInterface**)&object);
    AutoPtr<IInteger32> result;
    JSON::ToInteger32(object, (IInteger32**)&result);
    if (result != NULL) {
        return result->GetValue(data);
    }
    *data = fallback;
    return NOERROR;
}

ECode JSONArray::GetInt64(
    /* [in] */ Int32 index,
    /* [out] */ Int64* data)
{
    VALIDATE_NOT_NULL(data);
    *data = 0LL;

    AutoPtr<IInterface> object;
    Get(index, (IInterface**)&object);
    AutoPtr<IInteger64> result;
    JSON::ToInteger64(object, (IInteger64**)&result);
    if (result == NULL) {
        return JSON::TypeMismatch(CoreUtils::Convert(index), object, String("long"));
        // throw JSON.typeMismatch(index, object, "long");
    }
    return result->GetValue(data);
}

ECode JSONArray::OptInt64(
    /* [in] */ Int32 index,
    /* [out] */ Int64* data)
{
    VALIDATE_NOT_NULL(data);
    return OptInt64(index, 0LL, data);
}

ECode JSONArray::OptInt64(
    /* [in] */ Int32 index,
    /* [in] */ Int64 fallback,
    /* [out] */ Int64* data)
{
    VALIDATE_NOT_NULL(data);

    AutoPtr<IInterface> object;
    Opt(index, (IInterface**)&object);
    AutoPtr<IInteger64> result;
    JSON::ToInteger64(object, (IInteger64**)&result);
    if (result != NULL) {
        return result->GetValue(data);
    }
    *data = fallback;
    return NOERROR;
}

ECode JSONArray::GetString(
    /* [in] */ Int32 index,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);

    AutoPtr<IInterface> object;
    Get(index, (IInterface**)&object);
    String result;
    JSON::ToString(object, &result);
    if (result.IsNull()) {
        return JSON::TypeMismatch(CoreUtils::Convert(index), object, String("String"));
        // throw JSON.typeMismatch(index, object, "String");
    }
    *str = result;
    return NOERROR;
}

ECode JSONArray::OptString(
    /* [in] */ Int32 index,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    return OptString(index, String(""), str);
}

ECode JSONArray::OptString(
    /* [in] */ Int32 index,
    /* [in] */ const String& fallback,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<IInterface> object;
    Opt(index, (IInterface**)&object);
    String result;
    JSON::ToString(object, &result);
    if (!result.IsNull()) {
        *str = result;
        return NOERROR;
    }
    *str = fallback;
    return NOERROR;
}

ECode JSONArray::GetJSONArray(
    /* [in] */ Int32 index,
    /* [out] */ IJSONArray** jsonArray)
{
    VALIDATE_NOT_NULL(jsonArray);
    *jsonArray = NULL;

    AutoPtr<IInterface> object;
    Get(index, (IInterface**)&object);
    if (IJSONArray::Probe(object)) {
        *jsonArray = IJSONArray::Probe(object);
        REFCOUNT_ADD(*jsonArray);
        return NOERROR;
    }
    else {
        return JSON::TypeMismatch(CoreUtils::Convert(index), object, String("JSONArray"));
        // throw JSON.typeMismatch(index, object, "JSONArray");
    }
}

ECode JSONArray::OptJSONArray(
    /* [in] */ Int32 index,
    /* [out] */ IJSONArray** jsonArray)
{
    VALIDATE_NOT_NULL(jsonArray);
    *jsonArray = NULL;

    AutoPtr<IInterface> object;
    Opt(index, (IInterface**)&object);
    if (IJSONArray::Probe(object) != NULL) {
        *jsonArray = IJSONArray::Probe(object);
        REFCOUNT_ADD(*jsonArray);
    }
    return NOERROR;
}

ECode JSONArray::GetJSONObject(
    /* [in] */ Int32 index,
    /* [out] */ IJSONObject** jsonObject)
{
    VALIDATE_NOT_NULL(jsonObject);
    *jsonObject = NULL;

    AutoPtr<IInterface> object;
    Get(index, (IInterface**)&object);
    if (IJSONObject::Probe(object)) {
        *jsonObject = IJSONObject::Probe(object);
        REFCOUNT_ADD(*jsonObject);
        return NOERROR;
    }
    else {
        return JSON::TypeMismatch(CoreUtils::Convert(index), object, String("JSONObject"));
        // throw JSON.typeMismatch(index, object, "JSONObject");
    }
}

ECode JSONArray::OptJSONObject(
    /* [in] */ Int32 index,
    /* [out] */ IJSONObject** jsonObject)
{
    VALIDATE_NOT_NULL(jsonObject);
    *jsonObject = NULL;

    AutoPtr<IInterface> object;
    Opt(index, (IInterface**)&object);
    if (IJSONObject::Probe(object) != NULL) {
        *jsonObject = IJSONObject::Probe(object);
        REFCOUNT_ADD(*jsonObject);
    }
    return NOERROR;
}

ECode JSONArray::ToJSONObject(
    /* [in] */ IJSONArray* names,
    /* [out] */ IJSONObject** jsonObject)
{
    VALIDATE_NOT_NULL(jsonObject);
    *jsonObject = NULL;

    AutoPtr<IJSONObject> result;
    CJSONObject::New((IJSONObject**)&result);
    Int32 len;
    names->GetLength(&len);
    Int32 size;
    mValues->GetSize(&size);
    using Elastos::Core::Math;
    Int32 length = Math::Min(len, size);
    if (length == 0) {
        return NOERROR;
    }
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> obj;
        names->Opt(i, (IInterface**)&obj);
        String name;
        JSON::ToString(obj, &name);
        AutoPtr<IInterface> object;
        Opt(i, (IInterface**)&object);
        result->Put(name, object);
    }
    *jsonObject = result;
    REFCOUNT_ADD(*jsonObject);
    return NOERROR;
}

ECode JSONArray::Join(
    /* [in] */ const String& separator,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    AutoPtr<CJSONStringer> stringer;
    CJSONStringer::NewByFriend((CJSONStringer**)&stringer);
    stringer->Open(JSONStringerScope_NULL, String(""));
    Int32 size;
    mValues->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        if (i > 0) {
            stringer->mOut->Append(separator);
        }
        AutoPtr<IInterface> object;
        mValues->Get(i, (IInterface**)&object);
        stringer->Value(object);
    }
    stringer->Close(JSONStringerScope_NULL, JSONStringerScope_NULL, String(""));
    *res = stringer->mOut->ToString();
    return NOERROR;
}

ECode JSONArray::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);

    // try {
    AutoPtr<IJSONStringer> stringer;
    CJSONStringer::New((IJSONStringer**)&stringer);
    FAIL_RETURN(WriteTo(stringer));
    return IObject::Probe(stringer)->ToString(str);
    // } catch (JSONException e) {
    //     return null;
    // }
}

ECode JSONArray::ToString(
    /* [in] */ Int32 indentSpaces,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<IJSONStringer> stringer;
    CJSONStringer::New(indentSpaces, (IJSONStringer**)&stringer);
    FAIL_RETURN(WriteTo(stringer));
    return IObject::Probe(stringer)->ToString(str);
}

ECode JSONArray::WriteTo(
    /* [in] */ IJSONStringer* stringer)
{
    FAIL_RETURN(stringer->Array());
    Int32 size;
    mValues->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> value;
        mValues->Get(i, (IInterface**)&value);
        FAIL_RETURN(stringer->Value(value));
    }
    return stringer->EndArray();
}

ECode JSONArray::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    if (IJSONArray::Probe(o) != NULL) {
        AutoPtr<IJSONArray> object = IJSONArray::Probe(o);
        return ((JSONArray*)object.Get())->mValues->Equals(mValues, res);
    }
    return NOERROR;
}

ECode JSONArray::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);
    // diverge from the original, which doesn't implement hashCode
    return mValues->GetHashCode(code);
}

} //namespace Json
} //namespace Org
