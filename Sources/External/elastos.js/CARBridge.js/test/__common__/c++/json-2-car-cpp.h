#ifndef __CAR_BRIDGE_TEST_JSON_2_CAR_CPP_H
# define __CAR_BRIDGE_TEST_JSON_2_CAR_CPP_H

# include <stddef.h>
# include <stdio.h>

# include <json.h>

# include <elastos.h>

# include "macros.h"



CAR_BRIDGE_TEST_NAMESPACE_BEGIN

# if 0
template<class LocalType>
bool IsLocalType(struct json_object const *value)
{
    return false;
}

template<class LocalType>
void GetLocalType(LocalType *localTypeObject, struct json_object const *value)
{
}

# endif
template<class T>
bool IsArrayOf(struct json_object const *value)
{
}

template<>
bool IsArrayOf<_ELASTOS Int16>(struct json_object const *value)
{
}

template<>
bool IsArrayOf<_ELASTOS Int32>(struct json_object const *value)
{
}

template<>
bool IsArrayOf<_ELASTOS Int64>(struct json_object const *value)
{
}

template<>
bool IsArrayOf<_ELASTOS Byte>(struct json_object const *value)
{
}

template<>
bool IsArrayOf<_ELASTOS Float>(struct json_object const *value)
{
}

template<>
bool IsArrayOf<_ELASTOS Double>(struct json_object const *value)
{
}

template<>
bool IsArrayOf<_ELASTOS Char32>(struct json_object const *value)
{
}

template<>
bool IsArrayOf<_ELASTOS String>(struct json_object const *value)
{
}

template<>
bool IsArrayOf<_ELASTOS Boolean>(struct json_object const *value)
{
}

template<>
bool IsArrayOf<_ELASTOS EMuid>(struct json_object const *value)
{
}

template<>
bool IsArrayOf<_ELASTOS EGuid>(struct json_object const *value)
{
}

template<>
bool IsArrayOf<_ELASTOS ECode>(struct json_object const *value)
{
}

template<>
bool IsArrayOf<_ELASTOS PVoid>(struct json_object const *value)
{
}

template<>
bool IsArrayOf<IInterface *>(struct json_object const *value)
{
}

template<class T>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<T> > GetArrayOf(struct json_object const *value)
{
}

template<>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<_ELASTOS Int16> > GetArrayOf<_ELASTOS Int16>(struct json_object const *value)
{
}

template<>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<_ELASTOS Int32> > GetArrayOf<_ELASTOS Int32>(struct json_object const *value)
{
}

template<>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<_ELASTOS Int64> > GetArrayOf<_ELASTOS Int64>(struct json_object const *value)
{
}

template<>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<_ELASTOS Byte> > GetArrayOf<_ELASTOS Byte>(struct json_object const *value)
{
}

template<>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<_ELASTOS Float> > GetArrayOf<_ELASTOS Float>(struct json_object const *value)
{
}

template<>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<_ELASTOS Double> > GetArrayOf<_ELASTOS Double>(struct json_object const *value)
{
}

template<>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<_ELASTOS Char32> > GetArrayOf<_ELASTOS Char32>(struct json_object const *value)
{
}

template<>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<_ELASTOS String> > GetArrayOf<_ELASTOS String>(struct json_object const *value)
{
}

template<>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<_ELASTOS Boolean> > GetArrayOf<_ELASTOS Boolean>(struct json_object const *value)
{
}

template<>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<_ELASTOS EMuid> > GetArrayOf<_ELASTOS EMuid>(struct json_object const *value)
{
}

template<>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<_ELASTOS EGuid> > GetArrayOf<_ELASTOS EGuid>(struct json_object const *value)
{
}

template<>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<_ELASTOS ECode> > GetArrayOf<_ELASTOS ECode>(struct json_object const *value)
{
}

template<>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<_ELASTOS PVoid> > GetArrayOf<_ELASTOS PVoid>(struct json_object const *value)
{
}

template<>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<IInterface *> > GetArrayOf<IInterface *>(struct json_object const *value)
{
}

template<class T, size_t L>
bool IsCArrayOf(struct json_object const *value)
{
}

template<class T, size_t L>
T (*GetCArrayOf(struct json_object const *value))[L]
{
}

# if 0
template<class LocalType>
struct json_object *New(LocalType const *localTypeObject)
{
    return 0;
}

template<class LocalType>
int Print(FILE *file, LocalType const *localTypeObject)
{
    return 0;
}

# endif
template<class T>
struct json_object *New(_ELASTOS ArrayOf<T> const &array)
{
}

template<class T>
int Print(FILE *file, _ELASTOS ArrayOf<T> const &array)
{
}

template<class T, size_t L>
struct json_object *New(T (const &array)[L])
{
}

template<class T, size_t L>
int Print(FILE *file, T (const &array)[L])
{
}

CAR_BRIDGE_TEST_NAMESPACE_END

#endif
