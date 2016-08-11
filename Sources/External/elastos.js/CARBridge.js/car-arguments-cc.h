#ifndef __CAR_BRIDGE_CAR_ARGUMENTS_CC_H
# define __CAR_BRIDGE_CAR_ARGUMENTS_CC_H

# include <memory>
# include <new>

# include <node.h>

# include <nan.h>

# include <elastos.h>

# include "macros.h"

# include "error.h"
# include "weak-external-base.h"



CAR_BRIDGE_NAMESPACE_BEGIN

template<class _CalleeAllocCARArray>
_CalleeAllocCARArray *CalleeAllocCARArray_(ICarArrayInfo const *carArrayInfo, _ELASTOS CarQuintet **carQuintet)
{
    ::std::unique_ptr<_CalleeAllocCARArray, _CalleeAllocCARArray::Deleter> carArray(
            new(::std::nothrow) _CalleeAllocCARArray
            );
    if (carArray == nullptr)
        throw Error(Error::NO_MEMORY, "");

    carArray->carArrayInfo = carArrayInfo;
    carArray->carQuintet = carQuintet;

    return carArray.release();
}

template<class _CalleeAllocStruct>
_CalleeAllocStruct *CalleeAllocStruct_(IStructInfo const *structInfo, void **struct_)
{
    ::std::unique_ptr<_CalleeAllocStruct, _CalleeAllocStruct::Deleter> _struct(new(::std::nothrow) _CalleeAllocStruct);
    if (_struct == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _struct->structInfo = structInfo;
    _struct->struct_ = struct_;

    return _struct.release();
}

template<class _CallerAllocInt16>
_CallerAllocInt16 *CallerAllocInt16_(_ELASTOS Int16 *i16)
{
    ::std::unique_ptr<_CallerAllocInt16, _CallerAllocInt16::Deleter> _i16(new(::std::nothrow) _CallerAllocInt16);
    if (_i16 == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _i16->i16 = i16;

    return _i16.release();
}

template<class _CallerAllocInt32>
_CallerAllocInt32 *CallerAllocInt32_(_ELASTOS Int32 *i32)
{
    ::std::unique_ptr<_CallerAllocInt32, _CallerAllocInt32::Deleter> _i32(new(::std::nothrow) _CallerAllocInt32);
    if (_i32 == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _i32->i32 = i32;

    return _i32.release();
}

template<class _CallerAllocInt64>
_CallerAllocInt64 *CallerAllocInt64_(_ELASTOS Int64 *i64)
{
    ::std::unique_ptr<_CallerAllocInt64, _CallerAllocInt64::Deleter> _i64(new(::std::nothrow) _CallerAllocInt64);
    if (_i64 == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _i64->i64 = i64;

    return _i64.release();
}

template<class _CallerAllocByte>
_CallerAllocByte *CallerAllocByte_(_ELASTOS Byte *byte)
{
    ::std::unique_ptr<_CallerAllocByte, _CallerAllocByte::Deleter> _byte(new(::std::nothrow) _CallerAllocByte);
    if (_byte == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _byte->byte = byte;

    return _byte.release();
}

template<class _CallerAllocFloat>
_CallerAllocFloat *CallerAllocFloat_(_ELASTOS Float *f)
{
    ::std::unique_ptr<_CallerAllocFloat, _CallerAllocFloat::Deleter> _f(new(::std::nothrow) _CallerAllocFloat);
    if (_f == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _f->f = f;

    return _f.release();
}

template<class _CallerAllocDouble>
_CallerAllocDouble *CallerAllocDouble_(_ELASTOS Double *d)
{
    ::std::unique_ptr<_CallerAllocDouble, _CallerAllocDouble::Deleter> _d(new(::std::nothrow) _CallerAllocDouble);
    if (_d == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _d->d = d;

    return _d.release();
}

template<class _CallerAllocChar32>
_CallerAllocChar32 *CallerAllocChar32_(_ELASTOS Char32 *c32)
{
    ::std::unique_ptr<_CallerAllocChar32, _CallerAllocChar32::Deleter> _c32(new(::std::nothrow) _CallerAllocChar32);
    if (_c32 == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _c32->c32 = c32;

    return _c32.release();
}

template<class _CallerAllocString>
_CallerAllocString *CallerAllocString_(_ELASTOS String *s)
{
    ::std::unique_ptr<_CallerAllocString, _CallerAllocString::Deleter> _s(new(::std::nothrow) _CallerAllocString);
    if (_s == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _s->s = s;

    return _s.release();
}

template<class _CallerAllocBoolean>
_CallerAllocBoolean *CallerAllocBoolean_(_ELASTOS Boolean *b)
{
    ::std::unique_ptr<_CallerAllocBoolean, _CallerAllocBoolean::Deleter> _b(new(::std::nothrow) _CallerAllocBoolean);
    if (_b == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _b->b = b;

    return _b.release();
}

template<class _CallerAllocEMuid>
_CallerAllocEMuid *CallerAllocEMuid_(_ELASTOS EMuid *id)
{
    ::std::unique_ptr<_CallerAllocEMuid, _CallerAllocEMuid::Deleter> _id(new(::std::nothrow) _CallerAllocEMuid);
    if (_id == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _id->id = id;

    return _id.release();
}

template<class _CallerAllocEGuid>
_CallerAllocEGuid *CallerAllocEGuid_(_ELASTOS EGuid *id)
{
    ::std::unique_ptr<_CallerAllocEGuid, _CallerAllocEGuid::Deleter> _id(new(::std::nothrow) _CallerAllocEGuid);
    if (_id == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _id->id = id;

    return _id.release();
}

template<class _CallerAllocECode>
_CallerAllocECode *CallerAllocECode_(_ELASTOS ECode *ecode)
{
    ::std::unique_ptr<_CallerAllocECode, _CallerAllocECode::Deleter> _ecode(new(::std::nothrow) _CallerAllocECode);
    if (_ecode == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _ecode->ecode = ecode;

    return _ecode.release();
}

template<class _CallerAllocLocalPtr>
_CallerAllocLocalPtr *CallerAllocLocalPtr_(void **localPtr)
{
    ::std::unique_ptr<_CallerAllocLocalPtr, _CallerAllocLocalPtr::Deleter> _localPtr(
            new(::std::nothrow) _CallerAllocLocalPtr
            );
    if (_localPtr == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _localPtr->localPtr = localPtr;

    return _localPtr.release();
}

template<class _CallerAllocLocalType>
_CallerAllocLocalType *CallerAllocLocalType_(IDataTypeInfo const *dataTypeInfo, void *localTypeObject)
{
    ::std::unique_ptr<_CallerAllocLocalType, _CallerAllocLocalType::Deleter> _localTypeObject(
            new(::std::nothrow) _CallerAllocLocalType
            );
    if (_localTypeObject == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _localTypeObject->dataTypeInfo = dataTypeInfo;
    _localTypeObject->localTypeObject = localTypeObject;

    return _localTypeObject.release();
}

template<class _CallerAllocEnum>
_CallerAllocEnum *CallerAllocEnum_(_ELASTOS Int32 *enum_)
{
    ::std::unique_ptr<_CallerAllocEnum, _CallerAllocEnum::Deleter> _enum(new(::std::nothrow) _CallerAllocEnum);
    if (_enum == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _enum->enum_ = enum_;

    return _enum.release();
}

template<class _CallerAllocCARArray>
_CallerAllocCARArray *CallerAllocCARArray_(ICarArrayInfo const *carArrayInfo, IVariableOfCarArray *variableOfCARArray)
{
    ::std::unique_ptr<_CallerAllocCARArray, _CallerAllocCARArray::Deleter> carArray(
            new(::std::nothrow) _CallerAllocCARArray
            );
    if (carArray == nullptr)
        throw Error(Error::NO_MEMORY, "");

    carArray->carArrayInfo = carArrayInfo;
    carArray->variableOfCARArray = variableOfCARArray;

    return carArray.release();
}

template<class _CallerAllocStruct>
_CallerAllocStruct *CallerAllocStruct_(IStructInfo const *structInfo, IVariableOfStruct *variableOfStruct)
{
    ::std::unique_ptr<_CallerAllocStruct, _CallerAllocStruct::Deleter> struct_(new(::std::nothrow) _CallerAllocStruct);
    if (struct_ == nullptr)
        throw Error(Error::NO_MEMORY, "");

    struct_->structInfo = structInfo;
    struct_->variableOfStruct = variableOfStruct;

    return struct_.release();
}

template<class _CallerAllocInterface>
_CallerAllocInterface *CallerAllocInterface_(IInterface **interface_)
{
    ::std::unique_ptr<_CallerAllocInterface, _CallerAllocInterface::Deleter> _interface(
            new(::std::nothrow) _CallerAllocInterface
            );
    if (_interface == nullptr)
        throw Error(Error::NO_MEMORY, "");

    _interface->interface_ = interface_;

    return _interface.release();
}

CAR_BRIDGE_NAMESPACE_END

#endif
