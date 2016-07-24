#ifndef __CAR_BRIDGE_CAR_ARGUMENTS_CC_H
# define __CAR_BRIDGE_CAR_ARGUMENTS_CC_H

# include <node.h>

# include <nan.h>

# include <elastos.h>

# include "macros.h"

# include "error.h"
# include "new.h"
# include "unique-ptr.h"
# include "weak-external-base.h"



CAR_BRIDGE_NAMESPACE_BEGIN

template<class _CalleeAllocCARArray>
_CalleeAllocCARArray *CalleeAllocCARArray_(ICarArrayInfo const *carArrayInfo, _ELASTOS CarQuintet **carQuintet)
{
    UniquePtr<_CalleeAllocCARArray, &WeakExternalBase::Delete<_CalleeAllocCARArray>> carArray;

    carArray = new(NO_THROW) _CalleeAllocCARArray;
    if (carArray == nullptr)
        throw Error(Error::NO_MEMORY, "");

    carArray->carArrayInfo = carArrayInfo;
    carArray->carQuintet = carQuintet;

    return carArray.Release();
}

template<class _CalleeAllocStruct>
_CalleeAllocStruct *CalleeAllocStruct_(IStructInfo const *structInfo, void **struct_)
{
    UniquePtr<_CalleeAllocStruct, &WeakExternalBase::Delete<_CalleeAllocStruct>> _struct;

    _struct = new(NO_THROW) _CalleeAllocStruct;
    if (_struct == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _struct->structInfo = structInfo;
    _struct->struct_ = struct_;

    return _struct.Release();
}

template<class _CallerAllocInt16>
_CallerAllocInt16 *CallerAllocInt16_(_ELASTOS Int16 *i16)
{
    UniquePtr<_CallerAllocInt16, &WeakExternalBase::Delete<_CallerAllocInt16>> _i16;

    _i16 = new(NO_THROW) _CallerAllocInt16;
    if (_i16 == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _i16->i16 = i16;

    return _i16.Release();
}

template<class _CallerAllocInt32>
_CallerAllocInt32 *CallerAllocInt32_(_ELASTOS Int32 *i32)
{
    UniquePtr<_CallerAllocInt32, &WeakExternalBase::Delete<_CallerAllocInt32>> _i32;

    _i32 = new(NO_THROW) _CallerAllocInt32;
    if (_i32 == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _i32->i32 = i32;

    return _i32.Release();
}

template<class _CallerAllocInt64>
_CallerAllocInt64 *CallerAllocInt64_(_ELASTOS Int64 *i64)
{
    UniquePtr<_CallerAllocInt64, &WeakExternalBase::Delete<_CallerAllocInt64>> _i64;

    _i64 = new(NO_THROW) _CallerAllocInt64;
    if (_i64 == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _i64->i64 = i64;

    return _i64.Release();
}

template<class _CallerAllocByte>
_CallerAllocByte *CallerAllocByte_(_ELASTOS Byte *byte)
{
    UniquePtr<_CallerAllocByte, &WeakExternalBase::Delete<_CallerAllocByte>> _byte;

    _byte = new(NO_THROW) _CallerAllocByte;
    if (_byte == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _byte->byte = byte;

    return _byte.Release();
}

template<class _CallerAllocFloat>
_CallerAllocFloat *CallerAllocFloat_(_ELASTOS Float *f)
{
    UniquePtr<_CallerAllocFloat, &WeakExternalBase::Delete<_CallerAllocFloat>> _f;

    _f = new(NO_THROW) _CallerAllocFloat;
    if (_f == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _f->f = f;

    return _f.Release();
}

template<class _CallerAllocDouble>
_CallerAllocDouble *CallerAllocDouble_(_ELASTOS Double *d)
{
    UniquePtr<_CallerAllocDouble, &WeakExternalBase::Delete<_CallerAllocDouble>> _d;

    _d = new(NO_THROW) _CallerAllocDouble;
    if (_d == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _d->d = d;

    return _d.Release();
}

template<class _CallerAllocChar32>
_CallerAllocChar32 *CallerAllocChar32_(_ELASTOS Char32 *c32)
{
    UniquePtr<_CallerAllocChar32, &WeakExternalBase::Delete<_CallerAllocChar32>> _c32;

    _c32 = new(NO_THROW) _CallerAllocChar32;
    if (_c32 == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _c32->c32 = c32;

    return _c32.Release();
}

template<class _CallerAllocString>
_CallerAllocString *CallerAllocString_(_ELASTOS String *s)
{
    UniquePtr<_CallerAllocString, &WeakExternalBase::Delete<_CallerAllocString>> _s;

    _s = new(NO_THROW) _CallerAllocString;
    if (_s == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _s->s = s;

    return _s.Release();
}

template<class _CallerAllocBoolean>
_CallerAllocBoolean *CallerAllocBoolean_(_ELASTOS Boolean *b)
{
    UniquePtr<_CallerAllocBoolean, &WeakExternalBase::Delete<_CallerAllocBoolean>> _b;

    _b = new(NO_THROW) _CallerAllocBoolean;
    if (_b == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _b->b = b;

    return _b.Release();
}

template<class _CallerAllocEMuid>
_CallerAllocEMuid *CallerAllocEMuid_(_ELASTOS EMuid *id)
{
    UniquePtr<_CallerAllocEMuid, &WeakExternalBase::Delete<_CallerAllocEMuid>> _id;

    _id = new(NO_THROW) _CallerAllocEMuid;
    if (_id == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _id->id = id;

    return _id.Release();
}

template<class _CallerAllocEGuid>
_CallerAllocEGuid *CallerAllocEGuid_(_ELASTOS EGuid *id)
{
    UniquePtr<_CallerAllocEGuid, &WeakExternalBase::Delete<_CallerAllocEGuid>> _id;

    _id = new(NO_THROW) _CallerAllocEGuid;
    if (_id == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _id->id = id;

    return _id.Release();
}

template<class _CallerAllocECode>
_CallerAllocECode *CallerAllocECode_(_ELASTOS ECode *ecode)
{
    UniquePtr<_CallerAllocECode, &WeakExternalBase::Delete<_CallerAllocECode>> _ecode;

    _ecode = new(NO_THROW) _CallerAllocECode;
    if (_ecode == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _ecode->ecode = ecode;

    return _ecode.Release();
}

template<class _CallerAllocLocalPtr>
_CallerAllocLocalPtr *CallerAllocLocalPtr_(void **localPtr)
{
    UniquePtr<_CallerAllocLocalPtr, &WeakExternalBase::Delete<_CallerAllocLocalPtr>> _localPtr;

    _localPtr = new(NO_THROW) _CallerAllocLocalPtr;
    if (_localPtr == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _localPtr->localPtr = localPtr;

    return _localPtr.Release();
}

template<class _CallerAllocLocalType>
_CallerAllocLocalType *CallerAllocLocalType_(IDataTypeInfo const *dataTypeInfo, void *localTypeObject)
{
    UniquePtr<_CallerAllocLocalType, &WeakExternalBase::Delete<_CallerAllocLocalType>> _localTypeObject;

    _localTypeObject = new(NO_THROW) _CallerAllocLocalType;
    if (_localTypeObject == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _localTypeObject->dataTypeInfo = dataTypeInfo;
    _localTypeObject->localTypeObject = localTypeObject;

    return _localTypeObject.Release();
}

template<class _CallerAllocEnum>
_CallerAllocEnum *CallerAllocEnum_(_ELASTOS Int32 *enum_)
{
    UniquePtr<_CallerAllocEnum, &WeakExternalBase::Delete<_CallerAllocEnum>> _enum;

    _enum = new(NO_THROW) _CallerAllocEnum;
    if (_enum == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _enum->enum_ = enum_;

    return _enum.Release();
}

template<class _CallerAllocCARArray>
_CallerAllocCARArray *CallerAllocCARArray_(ICarArrayInfo const *carArrayInfo, IVariableOfCarArray *variableOfCARArray)
{
    UniquePtr<_CallerAllocCARArray, &WeakExternalBase::Delete<_CallerAllocCARArray>> carArray;

    carArray = new(NO_THROW) _CallerAllocCARArray;
    if (carArray == nullptr)
        throw Error(Error::NO_MEMORY, "");

    carArray->carArrayInfo = carArrayInfo;
    carArray->variableOfCARArray = variableOfCARArray;

    return carArray.Release();
}

template<class _CallerAllocStruct>
_CallerAllocStruct *CallerAllocStruct_(IStructInfo const *structInfo, IVariableOfStruct *variableOfStruct)
{
    UniquePtr<_CallerAllocStruct, &WeakExternalBase::Delete<_CallerAllocStruct>> struct_;

    struct_ = new(NO_THROW) _CallerAllocStruct;
    if (struct_ == nullptr)
        throw Error(Error::NO_MEMORY, "");

    struct_->structInfo = structInfo;
    struct_->variableOfStruct = variableOfStruct;

    return struct_.Release();
}

template<class _CallerAllocInterface>
_CallerAllocInterface *CallerAllocInterface_(IInterface **interface_)
{
    UniquePtr<_CallerAllocInterface, &WeakExternalBase::Delete<_CallerAllocInterface>> _interface;

    _interface = new(NO_THROW) _CallerAllocInterface;
    if (_interface == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _interface->interface_ = interface_;

    return _interface.Release();
}

CAR_BRIDGE_NAMESPACE_END

#endif
