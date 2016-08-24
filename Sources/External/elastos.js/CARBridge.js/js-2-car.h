#ifndef __CAR_BRIDGE_JS_2_CAR_H
# define __CAR_BRIDGE_JS_2_CAR_H

# include <node.h>

# include <nan.h>

# include <elastos.h>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

extern bool IsInt16(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsInt16(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Int16 ToInt16(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS Int16 i16);

extern bool IsInt32(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsInt32(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Int32 ToInt32(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS Int32 i32);

extern bool IsInt64(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsInt64(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Int64 ToInt64(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS Int64 i64);

extern bool IsByte(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsByte(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Byte ToByte(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS Byte byte);

extern bool IsFloat(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsFloat(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Float ToFloat(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS Float f);

extern bool IsDouble(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsDouble(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Double ToDouble(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS Double d);

extern bool IsChar32(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsChar32(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Char32 ToChar32(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS Char32 c32);

extern bool IsString(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsString(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern void ToString(_ELASTOS String &s, ::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS String const &s);

extern bool IsBoolean(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsBoolean(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Boolean ToBoolean(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValueFromBoolean(_ELASTOS Boolean b);

extern bool IsEMuid(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsEMuid(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern void ToEMuid(_ELASTOS EMuid *id, ::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS EMuid const *id);

extern bool IsEGuid(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsEGuid(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern void ToEGuid(_ELASTOS EGuid *id, ::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(_ELASTOS EGuid const *id);

extern bool IsECode(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsECode(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS ECode ToECode(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValueFromECode(_ELASTOS ECode ecode);

extern bool IsLocalPtr(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsLocalPtr(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern void *ToLocalPtr(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(void *localPtr);

# if 0
extern bool IsLocalType(IDataTypeInfo const *dataTypeInfo, ::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsLocalType(IDataTypeInfo const *dataTypeInfo,
        ::v8::Local<::v8::Value> value,
        int *priority = nullptr);

extern void ToLocalType(IDataTypeInfo const *dataTypeInfo, void *localTypeObject, ::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(IDataTypeInfo const *dataTypeInfo, void const *localTypeObject);

# endif
extern bool IsEnum(::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsEnum(::v8::Local<::v8::Value> value, int *priority = nullptr);

extern _ELASTOS Int32 ToEnum(::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValueFromEnum(_ELASTOS Int32 enum_);

extern bool IsCARArray(ICarArrayInfo const *carArrayInfo, ::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsCARArray(ICarArrayInfo const *carArrayInfo,
        ::v8::Local<::v8::Value> value,
        int *priority = nullptr);

extern void ToCARArray(ICarArrayInfo const *carArrayInfo,
        ICarArraySetter *carArraySetter,
        ::v8::Local<::v8::Value> value);

extern void ToCARArray(ICarArrayInfo const *carArrayInfo,
        _ELASTOS CarQuintet *carQuintet,
        ::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(ICarArrayInfo const *carArrayInfo, ICarArrayGetter const *carArrayGetter);

extern ::v8::Local<::v8::Value> ToValue(ICarArrayInfo const *carArrayInfo, _ELASTOS CarQuintet const *carQuintet);

extern bool IsCPPVector(ICppVectorInfo const *cppVectorInfo, ::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsCPPVector(ICppVectorInfo const *cppVectorInfo,
        ::v8::Local<::v8::Value> value,
        int *priority = nullptr);

extern void ToCPPVector(ICppVectorInfo const *cppVectorInfo,
        ICppVectorSetter *cppVectorSetter,
        ::v8::Local<::v8::Value> value);

# if 0
extern void ToCPPVector(ICppVectorInfo const *cppVectorInfo, void *cppVector, ::v8::Local<::v8::Value> value);

# endif
extern ::v8::Local<::v8::Value> ToValue(ICppVectorInfo const *cppVectorInfo, ICppVectorGetter const *cppVectorGetter);

# if 0
extern ::v8::Local<::v8::Value> ToValue(ICppVectorInfo const *cppVectorInfo, void const *cppVector);

# endif
extern bool IsStruct(IStructInfo const *structInfo, ::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsStruct(IStructInfo const *structInfo, ::v8::Local<::v8::Value> value, int *priority = nullptr);

extern void ToStruct(IStructInfo const *structInfo, IStructSetter *structSetter, ::v8::Local<::v8::Value> value);

extern void ToStruct(IStructInfo const *structInfo, void *struct_, ::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(IStructInfo const *structInfo, IStructGetter const *structGetter);

extern ::v8::Local<::v8::Value> ToValue(IStructInfo const *structInfo, void const *struct_);

extern bool IsInterface(IInterfaceInfo const *interfaceInfo, ::v8::Local<::v8::Value> value);

extern bool CanBeUsedAsInterface(IInterfaceInfo const *interfaceInfo,
        ::v8::Local<::v8::Value> value,
        int *priority = nullptr);

extern _ELASTOS AutoPtr<IInterface> ToInterface(IInterfaceInfo const *interfaceInfo, ::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(IInterface *interface_);

extern bool Is(IDataTypeInfo const *dataTypeInfo, ::v8::Local<::v8::Value> value);

extern bool CanBeUsedAs(IDataTypeInfo const *dataTypeInfo, ::v8::Local<::v8::Value> value, int *priority = nullptr);

extern bool IsCARObject(::v8::Local<::v8::Value> value);

extern IInterface *AsCARObject(::v8::Local<::v8::Value> value);

extern bool IsGeneric(::v8::Local<::v8::Value> value);

extern bool IsRegime(::v8::Local<::v8::Value> value);

extern IRegime *AsRegime(::v8::Local<::v8::Value> value);

extern bool IsAspect(::v8::Local<::v8::Value> value);

extern IAspect *AsAspect(::v8::Local<::v8::Value> value);

CAR_BRIDGE_NAMESPACE_END

#endif
