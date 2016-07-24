#ifndef __CAR_BRIDGE_CAR_DATA_TYPE_H
# define __CAR_BRIDGE_CAR_DATA_TYPE_H

# include <node.h>

# include <nan.h>

# include <elastos.h>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

extern ::v8::Local<::v8::Object> CARLocalPtr(ILocalPtrInfo const *localPtrInfo);

extern ::v8::Local<::v8::Object> CARLocalType(IDataTypeInfo const *dataTypeInfo);

extern ::v8::Local<::v8::Object> CAREnum(IEnumInfo const *enumInfo);

extern ::v8::Local<::v8::Object> CARArray(ICarArrayInfo const *carArrayInfo);

extern ::v8::Local<::v8::Object> CARCPPVector(ICppVectorInfo const *cppVectorInfo);

extern ::v8::Local<::v8::Object> CARStruct(IStructInfo const *structInfo);

extern ::v8::Local<::v8::Object> CARInterface(IInterfaceInfo const *interfaceInfo);

typedef IInterfaceInfo ICallbackInterfaceInfo;

extern ::v8::Local<::v8::Object> CARCallbackInterface(ICallbackInterfaceInfo const *callbackInterfaceInfo);

extern ::v8::Local<::v8::Object> CARDataType(IDataTypeInfo const *dataTypeInfo);

CAR_BRIDGE_NAMESPACE_END

#endif
