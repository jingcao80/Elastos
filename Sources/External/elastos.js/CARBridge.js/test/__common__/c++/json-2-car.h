#ifndef __CAR_BRIDGE_TEST_JSON_2_CAR_H
# define __CAR_BRIDGE_TEST_JSON_2_CAR_H

# include <stddef.h>
# include <stdio.h>

# include <json.h>

# include <elastos.h>

# include "macros.h"



CAR_BRIDGE_TEST_NAMESPACE_BEGIN

extern int Parse(FILE *file, struct json_object **value);

extern bool IsInt16(struct json_object const *value);

extern _ELASTOS Int16 GetInt16(struct json_object const *value);

extern bool IsInt32(struct json_object const *value);

extern _ELASTOS Int32 GetInt32(struct json_object const *value);

extern bool IsInt64(struct json_object const *value);

extern _ELASTOS Int64 GetInt64(struct json_object const *value);

extern bool IsByte(struct json_object const *value);

extern _ELASTOS Byte GetByte(struct json_object const *value);

extern bool IsFloat(struct json_object const *value);

extern _ELASTOS Float GetFloat(struct json_object const *value);

extern bool IsDouble(struct json_object const *value);

extern _ELASTOS Double GetDouble(struct json_object const *value);

extern bool IsChar32(struct json_object const *value);

extern _ELASTOS Char32 GetChar32(struct json_object const *value);

extern bool IsString(struct json_object const *value);

extern _ELASTOS String GetString(struct json_object const *value);

extern bool IsBoolean(struct json_object const *value);

extern _ELASTOS Boolean GetBoolean(struct json_object const *value);

extern bool IsEMuid(struct json_object const *value);

extern void GetEMuid(_ELASTOS EMuid *id, struct json_object const *value);

extern bool IsEGuid(struct json_object const *value);

extern void GetEGuid(_ELASTOS EGuid *id, struct json_object const *value);

extern bool IsECode(struct json_object const *value);

extern _ELASTOS ECode GetECode(struct json_object const *value);

extern bool IsLocalPtr(struct json_object const *value);

extern _ELASTOS PVoid GetLocalPtr(struct json_object const *value);

# if 0
template<class LocalType>
bool IsLocalType(struct json_object const *value);

template<class LocalType>
void GetLocalType(LocalType *localTypeObject, struct json_object const *value);

# endif
template<class T>
extern bool IsArrayOf(struct json_object const *value);

template<class T>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<T> > GetArrayOf(struct json_object const *value);

template<class T, size_t L>
extern bool IsCArrayOf(struct json_object const *value);

template<class T, size_t L>
T (*GetCArrayOf(struct json_object const *value))[L];

extern bool IsInterface(struct json_object const *value);

extern IInterface *GetInterface(struct json_object const *value);

extern struct json_object *New(_ELASTOS Int16 i16);

extern int Print(FILE *file, _ELASTOS Int16 i16);

extern struct json_object *New(_ELASTOS Int32 i32);

extern int Print(FILE *file, _ELASTOS Int32 i32);

extern struct json_object *New(_ELASTOS Int64 i64);

extern int Print(FILE *file, _ELASTOS Int64 i64);

extern struct json_object *New(_ELASTOS Byte byte);

extern int Print(FILE *file, _ELASTOS Byte byte);

extern struct json_object *New(_ELASTOS Float f);

extern int Print(FILE *file, _ELASTOS Float f);

extern struct json_object *New(_ELASTOS Double d);

extern int Print(FILE *file, _ELASTOS Double d);

extern struct json_object *New(_ELASTOS Char32 c32);

extern int Print(FILE *file, _ELASTOS Char32 c32);

extern struct json_object *New(_ELASTOS String const &s);

extern int Print(FILE *file, _ELASTOS String const &s);

extern struct json_object *NewBoolean(_ELASTOS Boolean b);

extern int Print(FILE *file, _ELASTOS Boolean b);

extern struct json_object *New(_ELASTOS EMuid const *id);

extern int Print(FILE *file, _ELASTOS EMuid const *id);

extern struct json_object *New(_ELASTOS EGuid const *id);

extern int Print(FILE *file, _ELASTOS EGuid const *id);

extern struct json_object *NewECode(_ELASTOS ECode ecode);

extern int Print(FILE *file, _ELASTOS ECode ecode);

extern struct json_object *New(_ELASTOS PVoid localPtr);

extern int Print(FILE *file, _ELASTOS PVoid localPtr);

# if 0
template<class LocalType>
struct json_object *New(LocalType const *localTypeObject);

template<class LocalType>
int Print(FILE *file, LocalType const *localTypeObject);

# endif
template<class T>
struct json_object *New(_ELASTOS ArrayOf<T> const &array);

template<class T>
int Print(FILE *file, _ELASTOS ArrayOf<T> const &array);

template<class T, size_t L>
struct json_object *New(T (const &array)[L]);

template<class T, size_t L>
int Print(FILE *file, T (const &array)[L]);

extern struct json_object *New(IInterface *interface_);

extern int Print(FILE *file, IInterface *interface_);

CAR_BRIDGE_TEST_NAMESPACE_END

# include "json-2-car-cpp.h"

#endif
