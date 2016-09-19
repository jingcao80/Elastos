
#include <errno.h>
#include <stdio.h>

#include <json.h>

#include <elastos.h>

#include "macros.h"



_ELASTOS_NAMESPACE_USING

CAR_BRIDGE_TEST_NAMESPACE_BEGIN

int Parse(FILE *file, struct json_object **value)
{
    char buf[JSON_FILE_BUF_SIZE];

    struct json_object *_value;

    do {
        n = fread(buf, JSON_FILE_BUF_SIZE, 1, file);
        if (errno)
            return -1;
    } while (n == JSON_FILE_BUF_SIZE);

    _value = json_tokener_parse(s);
    if (_value == NULL)
        return -1;

    if (value != 0)
        *value = json_object_get(_value);

    json_object_put(_value);

    return 0;
}

bool IsInt16(struct json_object const *value)
{
    int64_t i64;

    if (!json_object_is_type(value, json_type_int))
        return false;

    i64 = json_object_get_int64(value);

    return i64 >= INT16_MIN && i64 <= INT16_MAX;
}

Int16 GetInt16(struct json_object const *value)
{
    return json_object_get_int(value);
}

bool IsInt32(struct json_object const *value)
{
    int64_t i64;

    if (!json_object_is_type(value, json_type_int))
        return false;

    i64 = json_object_get_int64(value);

    return i64 >= INT32_MIN && i64 <= INT32_MAX;
}

Int32 GetInt32(struct json_object const *value)
{
    return json_object_get_int(value);
}

bool IsInt64(struct json_object const *value)
{
    return !!json_object_is_type(value, json_type_int);
}

Int64 GetInt64(struct json_object const *value)
{
    return json_object_get_int64(value);
}

bool IsByte(struct json_object const *value)
{
    int64_t i64;

    if (!json_object_is_type(value, json_type_int))
        return false;

    i64 = json_object_get_int64(value);

    return i64 >= INT8_MIN && i64 <= INT8_MAX;
}

Byte GetByte(struct json_object const *value)
{
    return json_object_get_int(value);
}

bool IsFloat(struct json_object const *value)
{
    return !!json_object_is_type(value, json_type_double);
}

Float GetFloat(struct json_object const *value)
{
    return json_object_get_double(value);
}

bool IsDouble(struct json_object const *value)
{
    return !!json_object_is_type(value, json_type_double);
}

Double GetDouble(struct json_object const *value)
{
    return json_object_get_double(value);
}

bool IsChar32(struct json_object const *value)
{
    if (!json_object_is_type(value, json_type_string))
        return false;

    return json_object_get_string_len(value) == 4;
}

Char32 GetChar32(struct json_object const *value)
{
    return json_object_get_string(value)[0];
}

bool IsString(struct json_object const *value)
{
    return !!json_object_is_type(value, json_type_string);
}

String GetString(struct json_object const *value)
{
    return json_object_get_string(value);
}

bool IsBoolean(struct json_object const *value)
{
    return !!json_object_is_type(value, json_type_boolean);
}

Boolean GetBoolean(struct json_object const *value)
{
    return json_object_get_boolean(value) ? TRUE : FALSE;
}

static bool _IsUInt32(struct json_object const *value)
{
    int64_t i64;

    if (!json_object_is_type(value, json_type_int))
        return false;

    i64 = json_object_get_int64(value);

    return i64 >= 0 && i64 <= UINT32_MAX;
}

static bool _IsUInt16(struct json_object const *value)
{
    int64_t i64;

    if (!json_object_is_type(value, json_type_int))
        return false;

    i64 = json_object_get_int64(value);

    return i64 >= 0 && i64 <= UINT16_MAX;
}

static bool _IsUInt8(struct json_object const *value)
{
    int64_t i64;

    if (!json_object_is_type(value, json_type_int))
        return false;

    i64 = json_object_get_int64(value);

    return i64 >= 0 && i64 <= UINT8_MAX;
}

bool IsEMuid(struct json_object const *value)
{
    struct json_object *data1;
    struct json_object *data2;
    struct json_object *data3;
    struct json_object *data4;

    if (!json_object_object_get_ex(value, "mData1", &data1))
        return false;

    if (!_IsUInt32(data1))
        return false;

    if (!json_object_object_get_ex(value, "mData2", &data2))
        return false;

    if (!_IsUInt16(data2))
        return false;

    if (!json_object_object_get_ex(value, "mData3", &data3))
        return false;

    if (!_IsUInt16(data3))
        return false;

    if (!json_object_object_get_ex(value, "mData4", &data4))
        return false;

    if (!json_object_is_type(data4, json_type_array))
        return false;

    for (size_t i = 0; i < 8; ++i)
        if (!_IsUInt8(json_object_array_get_idx(data4, i)))
            return false;

    return true;
}

void GetEMuid(EMuid *id, struct json_object const *value)
{
    struct json_object const *data4;

    id->mData1 = json_object_get_int64(json_object_object_get(value, "mData1"));
    id->mData2 = json_object_get_int(json_object_object_get(value, "mData2"));
    id->mData3 = json_object_get_int(json_object_object_get(value, "mData3"));

    data4 = json_object_object_get(value, "mData4");
    for (size_t i = 0; i < 8; ++i)
        id->mData4[i] = json_object_get_int(json_object_array_get_idx(data4, i));
}

bool IsEGuid(struct json_object const *value)
{
    struct json_object *clsid;
    struct json_object *uunm;
    struct json_object *carcode;

    if (!json_object_object_get_ex(value, "mClsid", &clsid))
        return false;

    if (!IsEMuid(clsid))
        return false;

    if (!json_object_object_get_ex(value, "mUunm", &uunm))
        return false;

    if (!json_object_is_type(uunm, json_type_string))
        return false;

    if (!json_object_object_get_ex(value, "mCarcode", &carcode))
        return false;

    if (!_IsUInt32(carcode))
        return false;

    return true;
}

void GetEGuid(EGuid *id, struct json_object const *value)
{
    GetEMuid(id->mClsid, json_object_object_get(value, "mClsid"));
    id->mUunm = json_object_get_string(json_object_object_get(value, "mUunm"));
    id->mCarcode = json_object_get_int64(json_object_object_get(value, "mCarcode"));
}

bool IsECode(struct json_object const *value)
{
    int64_t i64;

    if (!json_object_is_type(value, json_type_int))
        return false;

    i64 = json_object_get_int64(value);

    return i64 >= INT32_MIN && i64 <= INT32_MAX;
}

ECode GetECode(struct json_object const *value)
{
    return json_object_get_int(value);
}

bool IsLocalPtr(struct json_object const *value)
{
    int64_t i64;

    if (!json_object_is_type(value, json_type_int))
        return false;

    i64 = json_object_get_int64(value);

    return i64 >= INTPTR_MIN && i64 <= INTPTR_MAX;
}

PVoid GetLocalPtr(struct json_object const *value)
{
    return reinterpret_cast<PVoid>(json_object_get_int64(value));
}

bool IsInterface(struct json_object const *value)
{
    int64_t i64;

    if (!json_object_is_type(value, json_type_int))
        return false;

    i64 = json_object_get_int64(value);

    return i64 >= INTPTR_MIN && i64 <= INTPTR_MAX;
}

IInterface *GetInterface(struct json_object const *value)
{
    return reinterpret_cast<PVoid>(json_object_get_int64(value));
}

struct json_object *New(Int16 i16)
{
    return json_object_new_int(i16);
}

int Print(FILE *file, Int16 i16)
{
    return fprintf(file, json_object_to_json_string(New(i16)));
}

struct json_object *New(Int32 i32)
{
    return json_object_new_int(i32);
}

int Print(FILE *file, Int32 i32)
{
    return fprintf(file, json_object_to_json_string(New(i32)));
}

struct json_object *New(Int64 i64)
{
    return json_object_new_int64(i64);
}

int Print(FILE *file, Int64 i64)
{
    return fprintf(file, json_object_to_json_string(New(i64)));
}

struct json_object *New(Byte byte)
{
    return json_object_new_int(byte);
}

int Print(FILE *file, Byte byte)
{
    return fprintf(file, json_object_to_json_string(New(byte)));
}

struct json_object *New(Float f)
{
    return json_object_new_double(f);
}

int Print(FILE *file, Float f)
{
    return fprintf(file, json_object_to_json_string(New(f)));
}

struct json_object *New(Double d)
{
    return json_object_new_double(d);
}

int Print(FILE *file, Double d)
{
    return fprintf(file, json_object_to_json_string(New(d)));
}

struct json_object *New(Char32 c32)
{
    return json_object_new_string_len(reinterpret_cast<char *>(&c32), 4);
}

int Print(FILE *file, Char32 c32)
{
    return fprintf(file, json_object_to_json_string(New(c32)));
}

struct json_object *New(String const &s)
{
    return json_object_new_string(s);
}

int Print(FILE *file, String const &s)
{
    return fprintf(file, json_object_to_json_string(New(s)));
}

struct json_object *NewBoolean(Boolean b)
{
    return json_object_new_boolean(b);
}

int Print(FILE *file, Boolean b)
{
    return fprintf(file, json_object_to_json_string(NewBoolean(b)));
}

struct json_object *New(EMuid const *id)
{
    struct json_object *_id;

    struct json_object *data1;
    struct json_object *data2;
    struct json_object *data3;
    struct json_object *data4;

    _id = json_object_new_object();
    if (_id == NULL)
        return 0;

    data1 = json_object_new_int64(id->mData1);
    if (data1 == NULL)
        goto abort;

    if (json_object_object_add(_id, "mData1", data1)) {
        json_object_put(data1);

        goto abort;
    }

    data2 = json_object_new_int(id->mData2);
    if (data2 == NULL)
        goto abort;

    if (json_object_object_add(_id, "mData2", data2)) {
        json_object_put(data2);

        goto abort;
    }

    data3 = json_object_new_int(id->mData3);
    if (data3 == NULL)
        goto abort;

    if (json_object_object_add(_id, "mData3", data3)) {
        json_object_put(data3);

        goto abort;
    }

    data4 = json_object_new_array();
    if (data4 == NULL)
        goto abort;

    for (size_t i = 0; i < 8; ++i) {
        struct json_object *ui8;

        ui8 = json_object_new_int(id->mData4[i]);
        if (ui8 == NULL)
            goto abort;

        if (json_object_array_add(data4, ui8)) {
            json_object_put(ui8);

            goto abort;
        }
    }

    if (json_object_object_add(_id, "mData4", data4)) {
        json_object_put(data4);

        goto abort;
    }

    return _id;

abort:
    json_object_put(_id);

    return 0;
}

int Print(FILE *file, EMuid const *id)
{
    return fprintf(file, json_object_to_json_string(New(id)));
}

struct json_object *New(EGuid const *id)
{
    struct json_object *_id;

    struct json_object *clsid;
    struct json_object *uunm;
    struct json_object *carcode;

    _id = json_object_new_object();
    if (_id == NULL)
        return 0;

    clsid = New(id->mClsid);
    if (clsid == NULL)
        goto abort;

    if (json_object_object_add(_id, "mClsid", clsid)) {
        json_object_put(clsid);

        goto abort;
    }

    uunm = json_object_new_string(id->mUunm);
    if (uunm == NULL)
        goto abort;

    if (json_object_object_add(_id, "mUunm", uunm)) {
        json_object_put(uunm);

        goto abort;
    }

    carcode = json_object_new_int(id->mCarcode);
    if (carcode == NULL)
        goto abort;

    if (json_object_object_add(_id, "mCarcode", carcode)) {
        json_object_put(carcode);

        goto abort;
    }

    return _id;

abort:
    json_object_put(_id);

    return 0;
}

int Print(FILE *file, EGuid const *id)
{
    return fprintf(file, json_object_to_json_string(New(id)));
}

struct json_object *NewECode(ECode ecode)
{
    return json_object_new_int(ecode);
}

int Print(FILE *file, ECode ecode)
{
    return fprintf(file, json_object_to_json_string(NewECode(ecode)));
}

struct json_object *New(PVoid localPtr)
{
    return json_object_new_int64(reinterpret_cast<int64_t>(localPtr));
}

int Print(FILE *file, PVoid localPtr)
{
    return fprintf(file, json_object_to_json_string(New(localPtr)));
}

struct json_object *New(IInterface *interface_)
{
    return json_object_new_int64(reinterpret_cast<int64_t>(interface_));
}

int Print(FILE *file, IInterface *interface_)
{
    return fprintf(file, json_object_to_json_string(New(interface_)));
}

CAR_BRIDGE_TEST_NAMESPACE_END

