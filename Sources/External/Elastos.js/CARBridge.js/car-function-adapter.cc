
#include <stdarg.h>
#include <stddef.h>

#include <node.h>

#include <nan.h>

#include <elastos.h>

#include "macros.h"

#include "nan-ext.h"

#include "elastos-ext.h"

#include "car-function-adapter.h"

#include "car-arguments.h"
#include "error.h"
#include "js-2-car.h"
#include "new.h"
#include "unique-ptr.h"



using namespace node;

using namespace Nan;

using namespace v8;

_ELASTOS_NAMESPACE_USING

CAR_BRIDGE_NAMESPACE_BEGIN

template<class T>
inline T &&_To(va_list ap) noexcept
{
    return va_arg(ap, T);
}

template<class T>
inline T &_To(void *ap) noexcept
{
    return *reinterpret_cast<T *>(ap);
}

template<class T>
static void _SetInputArgumentOfInt16(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    Int16 i16;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    i16 = _To<int>(ap);

    argv[index] = ToValue(i16);
}

template<class T>
static void _SetInputArgumentOfInt32(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    _ELASTOS Int32 i32;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    i32 = _To<_ELASTOS Int32>(ap);

    argv[index] = ToValue(i32);
}

template<class T>
static void _SetInputArgumentOfInt64(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    Int64 i64;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    i64 = _To<Int64>(ap);

    argv[index] = ToValue(i64);
}

template<class T>
static void _SetInputArgumentOfByte(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    Byte byte;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    byte = _To<int>(ap);

    argv[index] = ToValue(byte);
}

template<class T>
static void _SetInputArgumentOfFloat(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    Float f;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    f = _To<double>(ap);

    argv[index] = ToValue(f);
}

template<class T>
static void _SetInputArgumentOfDouble(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    Double d;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    d = _To<Double>(ap);

    argv[index] = ToValue(d);
}

template<class T>
static void _SetInputArgumentOfChar32(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    Char32 c32;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    c32 = _To<Char32>(ap);

    argv[index] = ToValue(c32);
}

template<class T>
static void _SetInputArgumentOfString(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    void const *p;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    p = _To<void const *>(ap);

    _ELASTOS String &s = *(_ELASTOS String *)p;

    argv[index] = ToValue(s);
}

template<class T>
static void _SetInputArgumentOfBoolean(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    _ELASTOS Boolean b;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    b = _To<int>(ap);

    argv[index] = ToValueFromBoolean(b);
}

template<class T>
static void _SetInputArgumentOfEMuid(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    EMuid const *id;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    id = _To<EMuid const *>(ap);

    argv[index] = ToValue(id);
}

template<class T>
static void _SetInputArgumentOfEGuid(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    EGuid const *id;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    id = _To<EGuid const *>(ap);

    argv[index] = ToValue(id);
}

template<class T>
static void _SetInputArgumentOfECode(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    ECode ecode;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    ecode = _To<ECode>(ap);

    argv[index] = ToValueFromECode(ecode);
}

template<class T>
static void _SetInputArgumentOfLocalPtr(ILocalPtrInfo const *localPtrInfo,
        size_t argc, Local<Value> argv[], size_t index, T ap)
{
    void *localPtr;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    localPtr = _To<void *>(ap);

    argv[index] = ToValue(localPtr);
}

#if 0
template<class T>
static void _SetInputArgumentOfLocalType(IDataTypeInfo const *dataTypeInfo,
        size_t argc, Local<Value> argv[], size_t index, T ap)
{
    void const *localTypeObject;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    localTypeObject = _To<void const *>(ap);

    argv[index] = ToValue(dataTypeInfo, localTypeObject);
}

#endif
template<class T>
static void _SetInputArgumentOfEnum(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    _ELASTOS Int32 enum_;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    enum_ = _To<_ELASTOS Int32>(ap);

    argv[index] = ToValueFromEnum(enum_);
}

template<class T>
static void _SetInputArgumentOfCARArray(ICarArrayInfo const *carArrayInfo,
        size_t argc, Local<Value> argv[], size_t index, T ap)
{
    CarQuintet const *carQuintet;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    carQuintet = _To<CarQuintet const *>(ap);

    argv[index] = ToValue(carArrayInfo, carQuintet);
}

template<class T>
static void _SetInputArgumentOfStruct(IStructInfo const *structInfo,
        size_t argc, Local<Value> argv[], size_t index, T ap)
{
    void const *struct_;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    struct_ = _To<void const *>(ap);

    argv[index] = ToValue(structInfo, struct_);
}

template<class T>
static void _SetInputArgumentOfInterface(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    IInterface *carObject;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    carObject = _To<IInterface *>(ap);

    argv[index] = ToValue(carObject);
}

template<class T>
static void _SetInputArgumentOf(IDataTypeInfo const *dataTypeInfo,
        size_t argc, Local<Value> argv[], size_t index, T ap)
{
    ECode ec;

    CarDataType dataType;

    ec = const_cast<IDataTypeInfo *>(dataTypeInfo)->GetDataType(&dataType);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    switch (dataType) {
    case CarDataType_Int16:
        _SetInputArgumentOfInt16(argc, argv, index, ap);

        break;

    case CarDataType_Int32:
        _SetInputArgumentOfInt32(argc, argv, index, ap);

        break;

    case CarDataType_Int64:
        _SetInputArgumentOfInt64(argc, argv, index, ap);

        break;

    case CarDataType_Byte:
        _SetInputArgumentOfByte(argc, argv, index, ap);

        break;

    case CarDataType_Float:
        _SetInputArgumentOfFloat(argc, argv, index, ap);

        break;

    case CarDataType_Double:
        _SetInputArgumentOfDouble(argc, argv, index, ap);

        break;

    case CarDataType_Char32:
        _SetInputArgumentOfChar32(argc, argv, index, ap);

        break;

    case CarDataType_String:
        _SetInputArgumentOfString(argc, argv, index, ap);

        break;

    case CarDataType_Boolean:
        _SetInputArgumentOfBoolean(argc, argv, index, ap);

        break;

    case CarDataType_EMuid:
        _SetInputArgumentOfEMuid(argc, argv, index, ap);

        break;

    case CarDataType_EGuid:
        _SetInputArgumentOfEGuid(argc, argv, index, ap);

        break;

    case CarDataType_ECode:
        _SetInputArgumentOfECode(argc, argv, index, ap);

        break;

    case CarDataType_LocalPtr:
        _SetInputArgumentOfLocalPtr(static_cast<ILocalPtrInfo const *>(dataTypeInfo), argc, argv, index, ap);

        break;

#if 0
    case CarDataType_LocalType:
        _SetInputArgumentOfLocalType(dataTypeInfo, argc, argv, index, ap);

        break;

#endif
    case CarDataType_Enum:
        _SetInputArgumentOfEnum(argc, argv, index, ap);

        break;

    case CarDataType_ArrayOf:
        _SetInputArgumentOfCARArray(static_cast<ICarArrayInfo const *>(dataTypeInfo), argc, argv, index, ap);

        break;

    case CarDataType_Struct:
        _SetInputArgumentOfStruct(static_cast<IStructInfo const *>(dataTypeInfo), argc, argv, index, ap);

        break;

    case CarDataType_Interface:
        _SetInputArgumentOfInterface(argc, argv, index, ap);

        break;

    default:
        break;
    }
}

static Local<Value> _OutputArgumentOf(_ELASTOS String const &typeName,
        GetterCallback getter, SetterCallback setter, Local<Value> data)
{
    ::Nan::EscapableHandleScope scope;

    Local<Object> object;

    object = New<Object>();

    SetAccessor(object, New("data").ToLocalChecked(), getter, setter, data);

    ::Nan::Set(object, New(".type").ToLocalChecked(), ToValue(typeName));

    return scope.Escape(object);
}

static Local<Value> _OutputArgumentOf(IDataTypeInfo const *dataTypeInfo,
        GetterCallback getter, SetterCallback setter, Local<Value> data)
{
    ECode ec;

    _ELASTOS String name;

    ec = dataTypeInfo->GetName(&name);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    return _OutputArgumentOf(name, getter, setter, data);
}

static Local<Value> _CalleeAllocOutputArgumentOf(IDataTypeInfo const *dataTypeInfo,
        GetterCallback getter, SetterCallback setter, Local<Value> data)
{
    ::Nan::EscapableHandleScope scope;

    Local<Object> object;

    object = _OutputArgumentOf(dataTypeInfo, getter, setter, data).As<Object>();

    ::Nan::Set(object, New(".io").ToLocalChecked(), New("CalleeAllocOutput").ToLocalChecked());

    return scope.Escape(object);
}

static NAN_GETTER(_GetCalleeAllocOutputArgument)
{
    try {
        ::Nan::HandleScope scope;

        struct CARArgumentBase *carArgument;

        carArgument = (struct CARArgumentBase *)info.Data().As<External>()->Value();

        if (carArgument->value.IsEmpty())
            carArgument->value.Reset(Undefined());

        NAN_GETTER_RETURN_VALUE(New(carArgument->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

#define _GetCalleeAllocOutputArgumentOfCARArray _GetCalleeAllocOutputArgument

static NAN_SETTER(_SetCalleeAllocOutputArgumentOfCARArray)
{
    try {
        struct CalleeAllocCARArray *carArray;

        carArray = (struct CalleeAllocCARArray *)info.Data().As<External>()->Value();

        if (CanBeUsedAsCARArray(carArray->carArrayInfo, value)) {
            ECode ec;

            AutoPtr<IVariableOfCarArray> variableOfCARArray;
            IVariableOfCarArray *_variableOfCARArray;

            AutoPtr<ICarArraySetter> carArraySetter;
            ICarArraySetter *_carArraySetter;

            CarQuintet *carQuintet;

            ec = carArray->carArrayInfo->CreateVariable(value.As<Array>()->Length(), &_variableOfCARArray);
            if (FAILED(ec))
                throw Error(Error::TYPE_ELASTOS, ec, "");

            variableOfCARArray = _variableOfCARArray, _variableOfCARArray->Release();

            ec = variableOfCARArray->GetSetter(&_carArraySetter);
            if (FAILED(ec))
                throw Error(Error::TYPE_ELASTOS, ec, "");

            carArraySetter = _carArraySetter, _carArraySetter->Release();

            ToCARArray(carArray->carArrayInfo, carArraySetter, value);

            ec = variableOfCARArray->GetPayload((void **)&carQuintet);
            if (FAILED(ec))
                throw Error(Error::TYPE_ELASTOS, ec, "");

            *carArray->carQuintet = _CarQuintet_Clone(carQuintet);
        }

        carArray->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CalleeAllocOutputArgumentOfCARArray(ICarArrayInfo const *carArrayInfo, CarQuintet **carQuintet)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CalleeAllocCARArray, &CalleeAllocCARArray::Delete<struct CalleeAllocCARArray>> carArray;

    Local<Value> escapedArgument;

    carArray = CalleeAllocCARArray_(carArrayInfo, carQuintet);

    escapedArgument = scope.Escape(
            _CalleeAllocOutputArgumentOf(static_cast<IDataTypeInfo const *>(carArrayInfo),
                _GetCalleeAllocOutputArgumentOfCARArray,
                _SetCalleeAllocOutputArgumentOfCARArray,
                carArray->self())
            );

    carArray.Release();

    return escapedArgument;
}

template<class T>
static void _SetCalleeAllocOutputArgumentOfCARArray(ICarArrayInfo const *carArrayInfo,
        size_t argc, Local<Value> argv[], size_t index, T ap)
{
    CarQuintet **carQuintet;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    carQuintet = _To<CarQuintet **>(ap);

    argv[index] = _CalleeAllocOutputArgumentOfCARArray(carArrayInfo, carQuintet);
}

#define _GetCalleeAllocOutputArgumentOfStruct _GetCalleeAllocOutputArgument

static NAN_SETTER(_SetCalleeAllocOutputArgumentOfStruct)
{
    try {
        struct CalleeAllocStruct *struct_;

        struct_ = (struct CalleeAllocStruct *)info.Data().As<External>()->Value();

        if (CanBeUsedAsStruct(struct_->structInfo, value)) {
            ECode ec;

            MemorySize size;

            UniquePtr<void> _struct;

            ec = struct_->structInfo->GetSize(&size);
            if (FAILED(ec))
                throw Error(Error::TYPE_ELASTOS, ec, "");

            _struct = operator new(size, NO_THROW);
            if (_struct == nullptr)
                throw Error(Error::NO_MEMORY, "");

            ToStruct(struct_->structInfo, _struct, value);

            *struct_->struct_ = _struct;
        }

        struct_->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CalleeAllocOutputArgumentOfStruct(IStructInfo const *structInfo, void **struct_)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CalleeAllocStruct, &CalleeAllocStruct::Delete<struct CalleeAllocStruct>> _struct;

    Local<Value> escapedArgument;

    _struct = CalleeAllocStruct_(structInfo, struct_);

    escapedArgument = scope.Escape(
            _CalleeAllocOutputArgumentOf(static_cast<IDataTypeInfo const *>(structInfo),
                _GetCalleeAllocOutputArgumentOfStruct,
                _SetCalleeAllocOutputArgumentOfStruct,
                _struct->self())
            );

    _struct.Release();

    return escapedArgument;
}

template<class T>
static void _SetCalleeAllocOutputArgumentOfStruct(IStructInfo const *structInfo,
        size_t argc, Local<Value> argv[], size_t index, T ap)
{
    void **struct_;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    struct_ = _To<void **>(ap);

    argv[index] = _CalleeAllocOutputArgumentOfStruct(structInfo, struct_);
}

template<class T>
static void _SetCalleeAllocOutputArgumentOf(IDataTypeInfo const *dataTypeInfo,
        size_t argc, Local<Value> argv[], size_t index, T ap)
{
    ECode ec;

    CarDataType dataType;

    ec = const_cast<IDataTypeInfo *>(dataTypeInfo)->GetDataType(&dataType);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    switch (dataType) {
    case CarDataType_ArrayOf:
        _SetCalleeAllocOutputArgumentOfCARArray(static_cast<ICarArrayInfo const *>(dataTypeInfo),
                argc, argv, index, ap);

        break;

    case CarDataType_Struct:
        _SetCalleeAllocOutputArgumentOfStruct(static_cast<IStructInfo const *>(dataTypeInfo), argc, argv, index, ap);

        break;

    default:
        break;
    }
}

static Local<Value> _CallerAllocOutputArgumentOf(_ELASTOS String const &typeName,
        GetterCallback getter, SetterCallback setter, Local<Value> data)
{
    ::Nan::EscapableHandleScope scope;

    Local<Object> object;

    object = _OutputArgumentOf(typeName, getter, setter, data).As<Object>();

    ::Nan::Set(object, New(".io").ToLocalChecked(), New("CallerAllocOutput").ToLocalChecked());

    return scope.Escape(object);
}

static Local<Value> _CallerAllocOutputArgumentOf(IDataTypeInfo const *dataTypeInfo,
        GetterCallback getter, SetterCallback setter, Local<Value> data)
{
    ECode ec;

    _ELASTOS String name;

    ec = dataTypeInfo->GetName(&name);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    return _CallerAllocOutputArgumentOf(name, getter, setter, data);
}

static NAN_GETTER(_GetCallerAllocOutputArgumentOfInt16)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocInt16 *i16;

        i16 = (struct CallerAllocInt16 *)info.Data().As<External>()->Value();

        if (i16->value.IsEmpty())
            i16->value.Reset(ToValue(*i16->i16));

        NAN_GETTER_RETURN_VALUE(New(i16->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfInt16)
{
    try {
        struct CallerAllocInt16 *i16;

        i16 = (struct CallerAllocInt16 *)info.Data().As<External>()->Value();

        if (CanBeUsedAsInt16(value))
            *i16->i16 = ToInt16(value);

        i16->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfInt16(Int16 *i16)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CallerAllocInt16, &CallerAllocInt16::Delete<struct CallerAllocInt16>> _i16;

    Local<Value> escapedArgument;

    _i16 = CallerAllocInt16_(i16);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(_ELASTOS String("Int16"),
                _GetCallerAllocOutputArgumentOfInt16,
                _SetCallerAllocOutputArgumentOfInt16,
                _i16->self())
            );

    _i16.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfInt16(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    Int16 *i16;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    i16 = _To<Int16 *>(ap);

    argv[index] = _CallerAllocOutputArgumentOfInt16(i16);
}

static NAN_GETTER(_GetCallerAllocOutputArgumentOfInt32)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocInt32 *i32;

        i32 = (struct CallerAllocInt32 *)info.Data().As<External>()->Value();

        if (i32->value.IsEmpty())
            i32->value.Reset(ToValue(*i32->i32));

        NAN_GETTER_RETURN_VALUE(New(i32->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfInt32)
{
    try {
        struct CallerAllocInt32 *i32;

        i32 = (struct CallerAllocInt32 *)info.Data().As<External>()->Value();

        if (CanBeUsedAsInt32(value))
            *i32->i32 = ToInt32(value);

        i32->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfInt32(_ELASTOS Int32 *i32)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CallerAllocInt32, &CallerAllocInt32::Delete<struct CallerAllocInt32>> _i32;

    Local<Value> escapedArgument;

    _i32 = CallerAllocInt32_(i32);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(_ELASTOS String("Int32"),
                _GetCallerAllocOutputArgumentOfInt32,
                _SetCallerAllocOutputArgumentOfInt32,
                _i32->self())
            );

    _i32.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfInt32(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    _ELASTOS Int32 *i32;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    i32 = _To<_ELASTOS Int32 *>(ap);

    argv[index] = _CallerAllocOutputArgumentOfInt32(i32);
}

static NAN_GETTER(_GetCallerAllocOutputArgumentOfInt64)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocInt64 *i64;

        i64 = (struct CallerAllocInt64 *)info.Data().As<External>()->Value();

        if (i64->value.IsEmpty())
            i64->value.Reset(ToValue(*i64->i64));

        NAN_GETTER_RETURN_VALUE(New(i64->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfInt64)
{
    try {
        struct CallerAllocInt64 *i64;

        i64 = (struct CallerAllocInt64 *)info.Data().As<External>()->Value();

        if (CanBeUsedAsInt64(value))
            *i64->i64 = ToInt64(value);

        i64->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfInt64(Int64 *i64)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CallerAllocInt64, &CallerAllocInt64::Delete<struct CallerAllocInt64>> _i64;

    Local<Value> escapedArgument;

    _i64 = CallerAllocInt64_(i64);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(_ELASTOS String("Int64"),
                _GetCallerAllocOutputArgumentOfInt64,
                _SetCallerAllocOutputArgumentOfInt64,
                _i64->self())
            );

    _i64.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfInt64(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    Int64 *i64;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    i64 = _To<Int64 *>(ap);

    argv[index] = _CallerAllocOutputArgumentOfInt64(i64);
}

static NAN_GETTER(_GetCallerAllocOutputArgumentOfByte)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocByte *byte;

        byte = (struct CallerAllocByte *)info.Data().As<External>()->Value();

        if (byte->value.IsEmpty())
            byte->value.Reset(ToValue(*byte->byte));

        NAN_GETTER_RETURN_VALUE(New(byte->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfByte)
{
    try {
        struct CallerAllocByte *byte;

        byte = (struct CallerAllocByte *)info.Data().As<External>()->Value();

        if (CanBeUsedAsByte(value))
            *byte->byte = ToByte(value);

        byte->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfByte(Byte *byte)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CallerAllocByte, &CallerAllocByte::Delete<struct CallerAllocByte>> _byte;

    Local<Value> escapedArgument;

    _byte = CallerAllocByte_(byte);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(_ELASTOS String("Byte"),
                _GetCallerAllocOutputArgumentOfByte,
                _SetCallerAllocOutputArgumentOfByte,
                _byte->self())
            );

    _byte.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfByte(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    Byte *byte;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    byte = _To<Byte *>(ap);

    argv[index] = _CallerAllocOutputArgumentOfByte(byte);
}

static NAN_GETTER(_GetCallerAllocOutputArgumentOfFloat)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocFloat *f;

        f = (struct CallerAllocFloat *)info.Data().As<External>()->Value();

        if (f->value.IsEmpty())
            f->value.Reset(ToValue(*f->f));

        NAN_GETTER_RETURN_VALUE(New(f->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfFloat)
{
    try {
        struct CallerAllocFloat *f;

        f = (struct CallerAllocFloat *)info.Data().As<External>()->Value();

        if (CanBeUsedAsFloat(value))
            *f->f = ToFloat(value);

        f->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfFloat(Float *f)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CallerAllocFloat, &CallerAllocFloat::Delete<struct CallerAllocFloat>> _f;

    Local<Value> escapedArgument;

    _f = CallerAllocFloat_(f);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(_ELASTOS String("Float"),
                _GetCallerAllocOutputArgumentOfFloat,
                _SetCallerAllocOutputArgumentOfFloat,
                _f->self())
            );

    _f.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfFloat(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    Float *f;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    f = _To<Float *>(ap);

    argv[index] = _CallerAllocOutputArgumentOfFloat(f);
}

static NAN_GETTER(_GetCallerAllocOutputArgumentOfDouble)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocDouble *d;

        d = (struct CallerAllocDouble *)info.Data().As<External>()->Value();

        if (d->value.IsEmpty())
            d->value.Reset(ToValue(*d->d));

        NAN_GETTER_RETURN_VALUE(New(d->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfDouble)
{
    try {
        struct CallerAllocDouble *d;

        d = (struct CallerAllocDouble *)info.Data().As<External>()->Value();

        if (CanBeUsedAsDouble(value))
            *d->d = ToDouble(value);

        d->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfDouble(Double *d)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CallerAllocDouble, &CallerAllocDouble::Delete<struct CallerAllocDouble>> _d;

    Local<Value> escapedArgument;

    _d = CallerAllocDouble_(d);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(_ELASTOS String("Double"),
                _GetCallerAllocOutputArgumentOfDouble,
                _SetCallerAllocOutputArgumentOfDouble,
                _d->self())
            );

    _d.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfDouble(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    Double *d;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    d = _To<Double *>(ap);

    argv[index] = _CallerAllocOutputArgumentOfDouble(d);
}

static NAN_GETTER(_GetCallerAllocOutputArgumentOfChar32)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocChar32 *c32;

        c32 = (struct CallerAllocChar32 *)info.Data().As<External>()->Value();

        if (c32->value.IsEmpty())
            c32->value.Reset(ToValue(*c32->c32));

        NAN_GETTER_RETURN_VALUE(New(c32->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfChar32)
{
    try {
        struct CallerAllocChar32 *c32;

        c32 = (struct CallerAllocChar32 *)info.Data().As<External>()->Value();

        if (CanBeUsedAsChar32(value))
            *c32->c32 = ToChar32(value);

        c32->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfChar32(Char32 *c32)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CallerAllocChar32, &CallerAllocChar32::Delete<struct CallerAllocChar32>> _c32;

    Local<Value> escapedArgument;

    _c32 = CallerAllocChar32_(c32);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(_ELASTOS String("Char32"),
                _GetCallerAllocOutputArgumentOfChar32,
                _SetCallerAllocOutputArgumentOfChar32,
                _c32->self())
            );

    _c32.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfChar32(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    Char32 *c32;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    c32 = _To<Char32 *>(ap);

    argv[index] = _CallerAllocOutputArgumentOfChar32(c32);
}

static NAN_GETTER(_GetCallerAllocOutputArgumentOfString)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocString *s;

        s = (struct CallerAllocString *)info.Data().As<External>()->Value();

        if (s->value.IsEmpty())
            s->value.Reset(ToValue(*s->s));

        NAN_GETTER_RETURN_VALUE(New(s->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfString)
{
    try {
        struct CallerAllocString *s;

        s = (struct CallerAllocString *)info.Data().As<External>()->Value();

        if (CanBeUsedAsString(value))
            ToString(*s->s, value);

        s->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfString(_ELASTOS String *s)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CallerAllocString, &CallerAllocString::Delete<struct CallerAllocString>> _s;

    Local<Value> escapedArgument;

    _s = CallerAllocString_(s);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(_ELASTOS String("String"),
                _GetCallerAllocOutputArgumentOfString,
                _SetCallerAllocOutputArgumentOfString,
                _s->self())
            );

    _s.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfString(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    _ELASTOS String *s;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    s = _To<_ELASTOS String *>(ap);

    argv[index] = _CallerAllocOutputArgumentOfString(s);
}

static NAN_GETTER(_GetCallerAllocOutputArgumentOfBoolean)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocBoolean *b;

        b = (struct CallerAllocBoolean *)info.Data().As<External>()->Value();

        if (b->value.IsEmpty())
            b->value.Reset(ToValueFromBoolean(*b->b));

        NAN_GETTER_RETURN_VALUE(New(b->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfBoolean)
{
    try {
        struct CallerAllocBoolean *b;

        b = (struct CallerAllocBoolean *)info.Data().As<External>()->Value();

        if (CanBeUsedAsBoolean(value))
            *b->b = ToBoolean(value);

        b->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfBoolean(_ELASTOS Boolean *b)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CallerAllocBoolean, &CallerAllocBoolean::Delete<struct CallerAllocBoolean>> _b;

    Local<Value> escapedArgument;

    _b = CallerAllocBoolean_(b);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(_ELASTOS String("Boolean"),
                _GetCallerAllocOutputArgumentOfBoolean,
                _SetCallerAllocOutputArgumentOfBoolean,
                _b->self())
            );

    _b.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfBoolean(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    _ELASTOS Boolean *b;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    b = _To<_ELASTOS Boolean *>(ap);

    argv[index] = _CallerAllocOutputArgumentOfBoolean(b);
}

static NAN_GETTER(_GetCallerAllocOutputArgumentOfEMuid)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocEMuid *id;

        id = (struct CallerAllocEMuid *)info.Data().As<External>()->Value();

        if (id->value.IsEmpty())
            id->value.Reset(ToValue(id->id));

        NAN_GETTER_RETURN_VALUE(New(id->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfEMuid)
{
    try {
        struct CallerAllocEMuid *id;

        id = (struct CallerAllocEMuid *)info.Data().As<External>()->Value();

        if (CanBeUsedAsEMuid(value))
            ToEMuid(id->id, value);

        id->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfEMuid(EMuid *id)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CallerAllocEMuid, &CallerAllocEMuid::Delete<struct CallerAllocEMuid>> _id;

    Local<Value> escapedArgument;

    _id = CallerAllocEMuid_(id);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(_ELASTOS String("EMuid"),
                _GetCallerAllocOutputArgumentOfEMuid,
                _SetCallerAllocOutputArgumentOfEMuid,
                _id->self())
            );

    _id.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfEMuid(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    EMuid *id;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    id = _To<EMuid *>(ap);

    argv[index] = _CallerAllocOutputArgumentOfEMuid(id);
}

static NAN_GETTER(_GetCallerAllocOutputArgumentOfEGuid)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocEGuid *id;

        id = (struct CallerAllocEGuid *)info.Data().As<External>()->Value();

        if (id->value.IsEmpty())
            id->value.Reset(ToValue(id->id));

        NAN_GETTER_RETURN_VALUE(New(id->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfEGuid)
{
    try {
        struct CallerAllocEGuid *id;

        id = (struct CallerAllocEGuid *)info.Data().As<External>()->Value();

        if (CanBeUsedAsEGuid(value))
            ToEGuid(id->id, value);

        id->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfEGuid(EGuid *id)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CallerAllocEGuid, &CallerAllocEGuid::Delete<struct CallerAllocEGuid>> _id;

    Local<Value> escapedArgument;

    _id = CallerAllocEGuid_(id);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(_ELASTOS String("EGuid"),
                _GetCallerAllocOutputArgumentOfEGuid,
                _SetCallerAllocOutputArgumentOfEGuid,
                _id->self())
            );

    _id.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfEGuid(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    EGuid *id;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    id = _To<EGuid *>(ap);

    argv[index] = _CallerAllocOutputArgumentOfEGuid(id);
}

static NAN_GETTER(_GetCallerAllocOutputArgumentOfECode)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocECode *ecode;

        ecode = (struct CallerAllocECode *)info.Data().As<External>()->Value();

        if (ecode->value.IsEmpty())
            ecode->value.Reset(ToValueFromECode(*ecode->ecode));

        NAN_GETTER_RETURN_VALUE(New(ecode->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfECode)
{
    try {
        struct CallerAllocECode *ecode;

        ecode = (struct CallerAllocECode *)info.Data().As<External>()->Value();

        if (CanBeUsedAsECode(value))
            *ecode->ecode = ToECode(value);

        ecode->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfECode(ECode *ecode)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CallerAllocECode, &CallerAllocECode::Delete<struct CallerAllocECode>> _ecode;

    Local<Value> escapedArgument;

    _ecode = CallerAllocECode_(ecode);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(_ELASTOS String("ECode"),
                _GetCallerAllocOutputArgumentOfECode,
                _SetCallerAllocOutputArgumentOfECode,
                _ecode->self())
            );

    _ecode.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfECode(size_t argc, Local<Value> argv[], size_t index, T ap)
{
    ECode *ecode;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    ecode = _To<ECode *>(ap);

    argv[index] = _CallerAllocOutputArgumentOfECode(ecode);
}

static NAN_GETTER(_GetCallerAllocOutputArgumentOfLocalPtr)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocLocalPtr *localPtr;

        localPtr = (struct CallerAllocLocalPtr *)info.Data().As<External>()->Value();

        if (localPtr->value.IsEmpty())
            localPtr->value.Reset(ToValue(*localPtr->localPtr));

        NAN_GETTER_RETURN_VALUE(New(localPtr->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfLocalPtr)
{
    try {
        struct CallerAllocLocalPtr *localPtr;

        localPtr = (struct CallerAllocLocalPtr *)info.Data().As<External>()->Value();

        if (CanBeUsedAsLocalPtr(value))
            *localPtr->localPtr = ToLocalPtr(value);

        localPtr->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfLocalPtr(ILocalPtrInfo const *localPtrInfo, void **localPtr)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CallerAllocLocalPtr, &CallerAllocLocalPtr::Delete<struct CallerAllocLocalPtr>> _localPtr;

    Local<Value> escapedArgument;

    _localPtr = CallerAllocLocalPtr_(localPtr);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(static_cast<IDataTypeInfo const *>(localPtrInfo),
                _GetCallerAllocOutputArgumentOfLocalPtr,
                _SetCallerAllocOutputArgumentOfLocalPtr,
                _localPtr->self())
            );

    _localPtr.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfLocalPtr(ILocalPtrInfo const *localPtrInfo,
        size_t argc, Local<Value> argv[], size_t index, T ap)
{
    void **localPtr;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    localPtr = _To<void **>(ap);

    argv[index] = _CallerAllocOutputArgumentOfLocalPtr(localPtrInfo, localPtr);
}

#if 0
static NAN_GETTER(_GetCallerAllocOutputArgumentOfLocalType)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocLocalType *localTypeObject;

        localTypeObject = (struct CallerAllocLocalType *)info.Data().As<External>()->Value();

        if (localTypeObject->value.IsEmpty())
            localTypeObject->value.Reset(ToValue(localTypeObject->localTypeObject));

        NAN_GETTER_RETURN_VALUE(localTypeObject->value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfLocalType)
{
    try {
        struct CallerAllocLocalType *localTypeObject;

        localTypeObject = (struct CallerAllocLocalType *)info.Data().As<External>()->Value();

        if (CanBeUsedAsLocalType(value))
            ToLocalType(localTypeObject->localTypeObject, value);

        localTypeObject->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfLocalType(IDataTypeInfo const *dataTypeInfo, void *localTypeObject)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CallerAllocLocalType, &CallerAllocLocalType::Delete<struct CallerAllocLocalType>>
        _localTypeObject;

    Local<Value> escapedArgument;

    _localTypeObject = CallerAllocLocalType_(dataTypeInfo, localTypeObject);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(dataTypeInfo,
                _GetCallerAllocOutputArgumentOfLocalType,
                _SetCallerAllocOutputArgumentOfLocalType,
                _localTypeObject->self())
            );

    _localTypeObject.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfLocalType(IDataTypeInfo const *dataTypeInfo,
        size_t argc, Local<Value> argv[], size_t index, T ap)
{
    void *localTypeObject;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    localTypeObject = _To<void *>(ap);

    argv[index] = _CallerAllocOutputArgumentOfLocalType(dataTypeInfo, localTypeObject);
}

#endif
static NAN_GETTER(_GetCallerAllocOutputArgumentOfEnum)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocEnum *enum_;

        enum_ = (struct CallerAllocEnum *)info.Data().As<External>()->Value();

        if (enum_->value.IsEmpty())
            enum_->value.Reset(ToValueFromEnum(*enum_->enum_));

        NAN_GETTER_RETURN_VALUE(New(enum_->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfEnum)
{
    try {
        struct CallerAllocEnum *enum_;

        enum_ = (struct CallerAllocEnum *)info.Data().As<External>()->Value();

        if (CanBeUsedAsEnum(value))
            *enum_->enum_ = ToEnum(value);

        enum_->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfEnum(IEnumInfo const *enumInfo, _ELASTOS Int32 *enum_)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CallerAllocEnum, &CallerAllocEnum::Delete<struct CallerAllocEnum>> _enum;

    Local<Value> escapedArgument;

    _enum = CallerAllocEnum_(enum_);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(static_cast<IDataTypeInfo const *>(enumInfo),
                _GetCallerAllocOutputArgumentOfEnum,
                _SetCallerAllocOutputArgumentOfEnum,
                _enum->self())
            );

    _enum.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfEnum(IEnumInfo const *enumInfo,
        size_t argc, Local<Value> argv[], size_t index, T ap)
{
    _ELASTOS Int32 *enum_;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    enum_ = _To<_ELASTOS Int32 *>(ap);

    argv[index] = _CallerAllocOutputArgumentOfEnum(enumInfo, enum_);
}

static NAN_GETTER(_GetCallerAllocOutputArgumentOfCARArray)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocCARArray *carArray;

        carArray = (struct CallerAllocCARArray *)info.Data().As<External>()->Value();

        if (carArray->value.IsEmpty()) {
            ECode ec;

            AutoPtr<ICarArrayGetter const> carArrayGetter;
            ICarArrayGetter *_carArrayGetter;

            ec = carArray->variableOfCARArray->GetGetter(&_carArrayGetter);
            if (FAILED(ec))
                throw Error(Error::TYPE_ELASTOS, ec, "");

            carArrayGetter = _carArrayGetter, _carArrayGetter->Release();

            carArray->value.Reset(ToValue(carArray->carArrayInfo, carArrayGetter));
        }

        NAN_GETTER_RETURN_VALUE(New(carArray->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfCARArray)
{
    try {
        struct CallerAllocCARArray *carArray;

        carArray = (struct CallerAllocCARArray *)info.Data().As<External>()->Value();

        if (CanBeUsedAsCARArray(carArray->carArrayInfo, value)) {
            ECode ec;

            AutoPtr<ICarArraySetter> carArraySetter;
            ICarArraySetter *_carArraySetter;

            ec = carArray->variableOfCARArray->GetSetter(&_carArraySetter);
            if (FAILED(ec))
                throw Error(Error::TYPE_ELASTOS, ec, "");

            carArraySetter = _carArraySetter, _carArraySetter->Release();

            ToCARArray(carArray->carArrayInfo, carArraySetter, value);
        }

        carArray->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfCARArray(ICarArrayInfo const *carArrayInfo, CarQuintet *carQuintet)
{
    ::Nan::EscapableHandleScope scope;

    ECode ec;

    AutoPtr<IVariableOfCarArray> variableOfCARArray;
    IVariableOfCarArray *_variableOfCARArray;

    UniquePtr<struct CallerAllocCARArray, &CallerAllocCARArray::Delete<struct CallerAllocCARArray>> carArray;

    Local<Value> escapedArgument;

    ec = carArrayInfo->CreateVariableBox(carQuintet, &_variableOfCARArray);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    variableOfCARArray = _variableOfCARArray, _variableOfCARArray->Release();

    carArray = CallerAllocCARArray_(carArrayInfo, variableOfCARArray);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(static_cast<IDataTypeInfo const *>(carArrayInfo),
                _GetCallerAllocOutputArgumentOfCARArray,
                _SetCallerAllocOutputArgumentOfCARArray,
                carArray->self())
            );

    carArray.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfCARArray(ICarArrayInfo const *carArrayInfo,
        size_t argc, Local<Value> argv[], size_t index, T ap)
{
    CarQuintet *carQuintet;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    carQuintet = _To<CarQuintet *>(ap);

    argv[index] = _CallerAllocOutputArgumentOfCARArray(carArrayInfo, carQuintet);
}

static NAN_GETTER(_GetCallerAllocOutputArgumentOfStruct)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocStruct *struct_;

        struct_ = (struct CallerAllocStruct *)info.Data().As<External>()->Value();

        if (struct_->value.IsEmpty()) {
            ECode ec;

            AutoPtr<IStructGetter const> structGetter;
            IStructGetter *_structGetter;

            ec = struct_->variableOfStruct->GetGetter(&_structGetter);
            if (FAILED(ec))
                throw Error(Error::TYPE_ELASTOS, ec, "");

            structGetter = _structGetter, _structGetter->Release();

            struct_->value.Reset(ToValue(struct_->structInfo, structGetter));
        }

        NAN_GETTER_RETURN_VALUE(New(struct_->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfStruct)
{
    try {
        struct CallerAllocStruct *struct_;

        struct_ = (struct CallerAllocStruct *)info.Data().As<External>()->Value();

        if (CanBeUsedAsStruct(struct_->structInfo, value)) {
            ECode ec;

            AutoPtr<IStructSetter> structSetter;
            IStructSetter *_structSetter;

            ec = struct_->variableOfStruct->GetSetter(&_structSetter);
            if (FAILED(ec))
                throw Error(Error::TYPE_ELASTOS, ec, "");

            structSetter = _structSetter, _structSetter->Release();

            ToStruct(struct_->structInfo, structSetter, value);
        }

        struct_->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfStruct(IStructInfo const *structInfo, void *struct_)
{
    ::Nan::EscapableHandleScope scope;

    ECode ec;

    AutoPtr<IVariableOfStruct> variableOfStruct;
    IVariableOfStruct *_variableOfStruct;

    UniquePtr<struct CallerAllocStruct, &CallerAllocStruct::Delete<struct CallerAllocStruct>> _struct;

    Local<Value> escapedArgument;

    ec = structInfo->CreateVariableBox(struct_, &_variableOfStruct);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    variableOfStruct = _variableOfStruct, _variableOfStruct->Release();

    _struct = CallerAllocStruct_(structInfo, variableOfStruct);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(static_cast<IDataTypeInfo const *>(structInfo),
                _GetCallerAllocOutputArgumentOfStruct,
                _SetCallerAllocOutputArgumentOfStruct,
                _struct->self())
            );

    _struct.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfStruct(IStructInfo const *structInfo,
        size_t argc, Local<Value> argv[], size_t index, T ap)
{
    void *struct_;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    struct_ = _To<void *>(ap);

    argv[index] = _CallerAllocOutputArgumentOfStruct(structInfo, struct_);
}

static NAN_GETTER(_GetCallerAllocOutputArgumentOfInterface)
{
    try {
        ::Nan::HandleScope scope;

        struct CallerAllocInterface *interface_;

        interface_ = (struct CallerAllocInterface *)info.Data().As<External>()->Value();

        if (interface_->value.IsEmpty())
            interface_->value.Reset(ToValue(*interface_->interface_));

        NAN_GETTER_RETURN_VALUE(New(interface_->value));
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_SETTER(_SetCallerAllocOutputArgumentOfInterface)
{
    try {
        struct CallerAllocInterface *interface_;

        interface_ = (struct CallerAllocInterface *)info.Data().As<External>()->Value();

        if (CanBeUsedAsInterface(interface_->interfaceInfo, value))
            *interface_->interface_ = ToInterface(interface_->interfaceInfo, value);

        interface_->value.Reset(value);
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static Local<Value> _CallerAllocOutputArgumentOfInterface(IInterfaceInfo const *interfaceInfo, IInterface **interface_)
{
    ::Nan::EscapableHandleScope scope;

    UniquePtr<struct CallerAllocInterface, &CallerAllocInterface::Delete<struct CallerAllocInterface>> _interface;

    Local<Value> escapedArgument;

    _interface = CallerAllocInterface_(interfaceInfo, interface_);

    escapedArgument = scope.Escape(
            _CallerAllocOutputArgumentOf(static_cast<IDataTypeInfo const *>(interfaceInfo),
                _GetCallerAllocOutputArgumentOfInterface,
                _SetCallerAllocOutputArgumentOfInterface,
                _interface->self())
            );

    _interface.Release();

    return escapedArgument;
}

template<class T>
static void _SetCallerAllocOutputArgumentOfInterface(IInterfaceInfo const *interfaceInfo,
        size_t argc, Local<Value> argv[], size_t index, T ap)
{
    IInterface **interface_;

    if (index >= argc)
        throw Error(Error::INVALID_ARGUMENT, "");

    interface_ = _To<IInterface **>(ap);

    argv[index] = _CallerAllocOutputArgumentOfInterface(interfaceInfo, interface_);
}

template<class T>
static void _SetCallerAllocOutputArgumentOf(IDataTypeInfo const *dataTypeInfo,
        size_t argc, Local<Value> argv[], size_t index, T ap)
{
    ECode ec;

    CarDataType dataType;

    ec = const_cast<IDataTypeInfo *>(dataTypeInfo)->GetDataType(&dataType);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    switch (dataType) {
    case CarDataType_Int16:
        _SetCallerAllocOutputArgumentOfInt16(argc, argv, index, ap);

        break;

    case CarDataType_Int32:
        _SetCallerAllocOutputArgumentOfInt32(argc, argv, index, ap);

        break;

    case CarDataType_Int64:
        _SetCallerAllocOutputArgumentOfInt64(argc, argv, index, ap);

        break;

    case CarDataType_Byte:
        _SetCallerAllocOutputArgumentOfByte(argc, argv, index, ap);

        break;

    case CarDataType_Float:
        _SetCallerAllocOutputArgumentOfFloat(argc, argv, index, ap);

        break;

    case CarDataType_Double:
        _SetCallerAllocOutputArgumentOfDouble(argc, argv, index, ap);

        break;

    case CarDataType_Char32:
        _SetCallerAllocOutputArgumentOfChar32(argc, argv, index, ap);

        break;

    case CarDataType_String:
        _SetCallerAllocOutputArgumentOfString(argc, argv, index, ap);

        break;

    case CarDataType_Boolean:
        _SetCallerAllocOutputArgumentOfBoolean(argc, argv, index, ap);

        break;

    case CarDataType_EMuid:
        _SetCallerAllocOutputArgumentOfEMuid(argc, argv, index, ap);

        break;

    case CarDataType_EGuid:
        _SetCallerAllocOutputArgumentOfEGuid(argc, argv, index, ap);

        break;

    case CarDataType_ECode:
        _SetCallerAllocOutputArgumentOfECode(argc, argv, index, ap);

        break;

    case CarDataType_LocalPtr:
        _SetCallerAllocOutputArgumentOfLocalPtr(static_cast<ILocalPtrInfo const *>(dataTypeInfo),
                argc, argv, index, ap);

        break;

#if 0
    case CarDataType_LocalType:
        _SetCallerAllocOutputArgumentOfLocalType(dataTypeInfo, argc, argv, index, ap);

        break;

#endif
    case CarDataType_Enum:
        _SetCallerAllocOutputArgumentOfEnum(static_cast<IEnumInfo const *>(dataTypeInfo), argc, argv, index, ap);

        break;

    case CarDataType_ArrayOf:
        _SetCallerAllocOutputArgumentOfCARArray(static_cast<ICarArrayInfo const *>(dataTypeInfo),
                argc, argv, index, ap);

        break;

    case CarDataType_Struct:
        _SetCallerAllocOutputArgumentOfStruct(static_cast<IStructInfo const *>(dataTypeInfo), argc, argv, index, ap);

        break;

    case CarDataType_Interface:
        _SetCallerAllocOutputArgumentOfInterface(static_cast<IInterfaceInfo const *>(dataTypeInfo),
                argc, argv, index, ap);

        break;

    default:
        break;
    }
}

template<class T>
static void _SetArgumentOf(IParamInfo const *paramInfo, size_t argc, Local<Value> argv[], size_t index, T ap)
{
    ECode ec;

    ParamIOAttribute io;

    AutoPtr<IDataTypeInfo const> dataTypeInfo;
    IDataTypeInfo *_dataTypeInfo;

    ec = const_cast<IParamInfo *>(paramInfo)->GetIOAttribute(&io);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    ec = const_cast<IParamInfo *>(paramInfo)->GetTypeInfo(&_dataTypeInfo);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    dataTypeInfo = _dataTypeInfo, _dataTypeInfo->Release();

    if (io == ParamIOAttribute_In)
        _SetInputArgumentOf(dataTypeInfo, argc, argv, index, ap);
    else if (io == ParamIOAttribute_CalleeAllocOut)
        _SetCalleeAllocOutputArgumentOf(dataTypeInfo, argc, argv, index, ap);
    else if (io == ParamIOAttribute_CallerAllocOut)
        _SetCallerAllocOutputArgumentOf(dataTypeInfo, argc, argv, index, ap);
}

static void _SetArgumentOf(IParamInfo const *paramInfo, size_t argc, Local<Value> argv[], size_t index, va_list ap)
{
    _SetArgumentOf<va_list>(paramInfo, argc, argv, index, ap);
}

static void _SetArgumentOf(IParamInfo const *paramInfo, size_t argc, Local<Value> argv[], size_t index, void *ap)
{
    _SetArgumentOf<void *>(paramInfo, argc, argv, index, ap);
}

static void _ToError(Error *error, Local<Value> value)
{
    if (CanBeUsedAsError(value))
        ToError(error, value);
    else
        error->Set(Error::FAILED, "");
}

static Local<Value> _CallFunction(Local<Function> function, Local<Value> receiver, size_t argc, Local<Value> argv[])
{
    ::Nan::TryCatch tryCatch;

    Local<Value> returnValue;

    returnValue = ::Nan::MakeCallback(receiver, function, argc, argv);

    if (tryCatch.HasCaught()) {
        Error error;

        _ToError(&error, tryCatch.Exception());

        throw error;
    }

    return returnValue;
}

ECode CARFunctionAdapter::Call(Local<Function> function, Local<Value> receiver,
        IFunctionInfo const *functionInfo, ...) noexcept
{
    va_list ap;

    ECode ec;

    va_start(ap, functionInfo);

    ec = Call(function, receiver, functionInfo, ap);

    va_end(ap);

    return ec;
}

ECode CARFunctionAdapter::Call(Local<Function> function, Local<Value> receiver,
        IFunctionInfo const *functionInfo, va_list ap) noexcept
{
    try {
        ::Nan::HandleScope scope;

        ECode ec;

        _ELASTOS Int32 nParams;

        AutoPtr<ArrayOf<IParamInfo const *> > paramInfos;
        ArrayOf<IParamInfo const *> *_paramInfos;

        size_t argc;
        UniquePtr<Local<Value> []> argv;

        ec = functionInfo->GetParamCount(&nParams);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        _paramInfos = ArrayOf<IParamInfo const *>::Alloc(nParams);
        if (_paramInfos == 0)
            throw Error(Error::NO_MEMORY, "");

        paramInfos = _paramInfos, _paramInfos->Release();

        ec = functionInfo->GetAllParamInfos(reinterpret_cast<ArrayOf<IParamInfo *> *>(paramInfos.Get()));
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        argc = nParams;
        argv = new(NO_THROW) Local<Value>[argc];
        if (argv == nullptr)
            throw Error(Error::NO_MEMORY, "");

        for (size_t i = 0; i < argc; ++i)
            _SetArgumentOf((*paramInfos)[i], argc, argv, i, ap);

        _CallFunction(function, receiver, argc, argv);
    } catch (Error const &error) {
        return ToECode(error);
    } catch (...) {
        return E_FAILED;
    }

    return NO_ERROR;
}

CARFunctionAdapter::CARFunctionAdapter(IFunctionInfo const *functionInfo,
        Local<Function> function, Local<Value> receiver):
    _functionInfo(functionInfo), _function(function), _receiver(receiver)
{
    ECode ec;

    _ELASTOS Int32 nParams;

    AutoPtr<ArrayOf<IParamInfo const *> > paramInfos;
    ArrayOf<IParamInfo const *> *__paramInfos;

    ec = functionInfo->GetParamCount(&nParams);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    __paramInfos = ArrayOf<IParamInfo const *>::Alloc(nParams);
    if (__paramInfos == 0)
        throw Error(Error::NO_MEMORY, "");

    paramInfos = __paramInfos, __paramInfos->Release();

    ec = functionInfo->GetAllParamInfos(reinterpret_cast<ArrayOf<IParamInfo *> *>(paramInfos.Get()));
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    _paramInfos = paramInfos;
}

CARFunctionAdapter::~CARFunctionAdapter()
{
    _receiver.Reset();
}

ECode CARFunctionAdapter::operator()(void) noexcept
{
    return Call();
}

template<class T>
ECode CARFunctionAdapter::operator()(T firstArgument, ...) noexcept
{
    va_list ap;

    ECode ec;

    va_start(ap, firstArgument);

    ec = Call(firstArgument, ap);

    va_end(ap);

    return ec;
}

Local<Value> CARFunctionAdapter::CallFunction(size_t argc, Local<Value> argv[])
{
    ::Nan::EscapableHandleScope scope;

    ::Nan::TryCatch tryCatch;

    Local<Value> returnValue;

    returnValue = _function(New(_receiver), argc, argv);

    if (tryCatch.HasCaught()) {
        Error error;

        _ToError(&error, tryCatch.Exception());

        throw error;
    }

    return scope.Escape(returnValue);
}

ECode CARFunctionAdapter::Call(void) noexcept
{
    try {
        CallFunction(0, nullptr);
    } catch (Error const &error) {
        return ToECode(error);
    } catch (...) {
        return E_FAILED;
    }

    return NO_ERROR;
}

template<class T>
ECode CARFunctionAdapter::Call(T firstArgument, ...) noexcept
{
    va_list ap;

    ECode ec;

    va_start(ap, firstArgument);

    ec = Call(firstArgument, ap);

    va_end(ap);

    return ec;
}

template<class T>
ECode CARFunctionAdapter::Call(T firstArgument, va_list ap) noexcept
{
    try {
        ::Nan::HandleScope scope;

        size_t argc;
        UniquePtr<Local<Value> []> argv;

        argc = _paramInfos->GetLength();
        argv = new(NO_THROW) Local<Value>[argc];
        if (argv == nullptr)
            throw Error(Error::NO_MEMORY, "");

        _SetArgumentOf((*_paramInfos)[0], argc, argv, 0, &firstArgument);
        for (size_t i = 1; i < argc; ++i)
            _SetArgumentOf((*_paramInfos)[i], argc, argv, i, ap);

        CallFunction(argc, argv);
    } catch (Error const &error) {
        return ToECode(error);
    } catch (...) {
        return E_FAILED;
    }

    return NO_ERROR;
}

ECode CARFunctionAdapter::Call(va_list ap) noexcept
{
    try {
        ::Nan::HandleScope scope;

        size_t argc;
        UniquePtr<Local<Value> []> argv;

        argc = _paramInfos->GetLength();
        argv = new(NO_THROW) Local<Value>[argc];
        if (argv == nullptr)
            throw Error(Error::NO_MEMORY, "");

        for (size_t i = 0; i < argc; ++i)
            _SetArgumentOf((*_paramInfos)[i], argc, argv, i, ap);

        CallFunction(argc, argv);
    } catch (Error const &error) {
        return ToECode(error);
    } catch (...) {
        return E_FAILED;
    }

    return NO_ERROR;
}

CAR_BRIDGE_NAMESPACE_END

