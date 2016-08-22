
#include <cstddef>
#include <cstdlib>

#include <map>
#include <vector>

#include <node.h>

#include <nan.h>

#include <elastos.h>

#include "macros.h"

#include "nan-ext.h"

#include "car-constantoid.h"
#include "car-data-type.h"
#include "car-function.h"
#include "error.h"
#include "js-2-car.h"



using namespace std;

using namespace node;

using namespace Nan;

using namespace v8;

_ELASTOS_NAMESPACE_USING

CAR_BRIDGE_NAMESPACE_BEGIN

static map<AutoPtr<IDataTypeInfo const>, CopyablePersistent<Object>> _mapIntrinsicTypeInfoToCARIntrinsicType;

static Local<Object> _CARIntrinsicType(IDataTypeInfo const *intrinsicTypeInfo)
{
    ::Nan::EscapableHandleScope scope;

    ECode ec;

    CarDataType dataType;

    Local<Object> intrinsicType;

    _ELASTOS String name;

    ec = intrinsicTypeInfo->GetDataType(&dataType);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    switch (dataType) {
    case CarDataType_Int16:
    case CarDataType_Int32:
    case CarDataType_Int64:
    case CarDataType_Byte:
    case CarDataType_Float:
    case CarDataType_Double:
    case CarDataType_Char32:
    case CarDataType_String:
    case CarDataType_Boolean:
    case CarDataType_EMuid:
    case CarDataType_EGuid:
    case CarDataType_ECode:
        break;

    default:
        throw Error(Error::INVALID_ARGUMENT, "");
    }

    auto &_intrinsicType = _mapIntrinsicTypeInfoToCARIntrinsicType[intrinsicTypeInfo];
    if (!_intrinsicType.IsEmpty())
        return scope.Escape(New(_intrinsicType));

    intrinsicType = New<Object>();

    DefineOwnProperty(intrinsicType,
            New("$what").ToLocalChecked(),
            New("CARIntrinsicType").ToLocalChecked(),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = intrinsicTypeInfo->GetName(&name);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    DefineOwnProperty(intrinsicType,
            New("$name").ToLocalChecked(),
            ToValue(name),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    _intrinsicType.Reset(intrinsicType);

    return scope.Escape(intrinsicType);
}

Local<Object> CARDataType(IDataTypeInfo const *dataTypeInfo);

static map<AutoPtr<ILocalPtrInfo const>, CopyablePersistent<Object>> _mapLocalPtrInfoToCARLocalPtr;

Local<Object> CARLocalPtr(ILocalPtrInfo const *localPtrInfo)
{
    ::Nan::EscapableHandleScope scope;

    Local<Object> localPtr;

    ECode ec;

    _ELASTOS String name;

    AutoPtr<IDataTypeInfo const> targetTypeInfo;
    IDataTypeInfo *_targetTypeInfo;

    _ELASTOS Int32 level;

    auto &_localPtr = _mapLocalPtrInfoToCARLocalPtr[localPtrInfo];
    if (!_localPtr.IsEmpty())
        return scope.Escape(New(_localPtr));

    localPtr = New<Object>();

    DefineOwnProperty(localPtr,
            New("$what").ToLocalChecked(),
            New("CARLocalPtr").ToLocalChecked(),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = localPtrInfo->GetName(&name);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    DefineOwnProperty(localPtr,
            New("$name").ToLocalChecked(),
            ToValue(name),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = localPtrInfo->GetTargetTypeInfo(&_targetTypeInfo);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    targetTypeInfo = _targetTypeInfo, _targetTypeInfo->Release();

    DefineOwnProperty(localPtr,
            New("targetType").ToLocalChecked(),
            CARDataType(targetTypeInfo),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));

    ec = localPtrInfo->GetPtrLevel(&level);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    DefineOwnProperty(localPtr,
            New("level").ToLocalChecked(),
            ToValue(level),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));

    _localPtr.Reset(localPtr);

    return scope.Escape(localPtr);
}

static map<AutoPtr<IDataTypeInfo const>, CopyablePersistent<Object>> _mapLocalTypeInfoToCARLocalType;

Local<Object> CARLocalType(IDataTypeInfo const *localTypeInfo)
{
    ::Nan::EscapableHandleScope scope;

    Local<Object> localTypeObject;

    ECode ec;

    _ELASTOS String name;

    auto &_localTypeObject = _mapLocalTypeInfoToCARLocalType[localTypeInfo];
    if (!_localTypeObject.IsEmpty())
        return scope.Escape(New(_localTypeObject));

    localTypeObject = New<Object>();

    DefineOwnProperty(localTypeObject,
            New("$what").ToLocalChecked(),
            New("CARLocalType").ToLocalChecked(),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = localTypeInfo->GetName(&name);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    DefineOwnProperty(localTypeObject,
            New("$name").ToLocalChecked(),
            ToValue(name),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    _localTypeObject.Reset(localTypeObject);

    return scope.Escape(localTypeObject);
}

static map<AutoPtr<IEnumInfo const>, CopyablePersistent<Object>> _mapEnumInfoToCAREnum;

Local<Object> CAREnum(IEnumInfo const *enumInfo)
{
    ::Nan::EscapableHandleScope scope;

    Local<Object> enum_;

    ECode ec;

    _ELASTOS String namespace_;

    Local<Value> _namespace;

    _ELASTOS String name;

    _ELASTOS Int32 nItems;

    AutoPtr<ArrayOf<IEnumItemInfo const *> > itemInfos;

    auto &_enum = _mapEnumInfoToCAREnum[enumInfo];
    if (!_enum.IsEmpty())
        return scope.Escape(New(_enum));

    enum_ = New<Object>();

    DefineOwnProperty(enum_,
            New("$what").ToLocalChecked(),
            New("CAREnum").ToLocalChecked(),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = enumInfo->GetNamespace(&namespace_);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    _namespace = ToValue(namespace_);

    DefineOwnProperty(enum_,
            New("$namespace").ToLocalChecked(),
            _namespace,
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = enumInfo->GetName(&name);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    DefineOwnProperty(enum_,
            New("$name").ToLocalChecked(),
            ToValue(name),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = enumInfo->GetItemCount(&nItems);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    itemInfos = ArrayOf<IEnumItemInfo const *>::Alloc(nItems);
    if (itemInfos == 0)
        throw Error(Error::NO_MEMORY, "");

    ec = enumInfo->GetAllItemInfos(reinterpret_cast<ArrayOf<IEnumItemInfo *> *>(itemInfos.Get()));
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    for (_ELASTOS Int32 i = 0; i < nItems; ++i) {
        ::Nan::HandleScope scope;

        IEnumItemInfo const *itemInfo;

        Local<NumberObject> constantoid;

        _ELASTOS String itemName;

        itemInfo = (*itemInfos)[i];

        constantoid = CARConstantoid(itemInfo, "CAREnumItem");

        DefineOwnProperty(constantoid,
                New("$namespace").ToLocalChecked(),
                _namespace,
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

        ec = itemInfo->GetName(&itemName);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        DefineOwnProperty(enum_,
                ToValue(itemName).As<::v8::String>(),
                constantoid,
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
    }

    _enum.Reset(enum_);

    return scope.Escape(enum_);
}

template<class ArrayInfo>
Local<Object> _Array(ArrayInfo const *arrayInfo)
{
    ::Nan::EscapableHandleScope scope;

    Local<Object> array;

    ECode ec;

    _ELASTOS String name;

    AutoPtr<IDataTypeInfo const> elementTypeInfo;
    IDataTypeInfo *_elementTypeInfo;

    array = New<Object>();

    DefineOwnProperty(array,
            New("$what").ToLocalChecked(),
            New("CARArray").ToLocalChecked(),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = arrayInfo->GetName(&name);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    DefineOwnProperty(array,
            New("$name").ToLocalChecked(),
            ToValue(name),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = arrayInfo->GetElementTypeInfo(&_elementTypeInfo);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    elementTypeInfo = _elementTypeInfo, _elementTypeInfo->Release();

    DefineOwnProperty(array,
            New("elementType").ToLocalChecked(),
            CARDataType(elementTypeInfo),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));

    return scope.Escape(array);
}

static map<AutoPtr<ICarArrayInfo const>, CopyablePersistent<Object>> _mapCARArrayInfoToCARArray;

Local<Object> CARArray(ICarArrayInfo const *carArrayInfo)
{
    Local<Object> carArray;

    auto &_carArray = _mapCARArrayInfoToCARArray[carArrayInfo];
    if (!_carArray.IsEmpty())
        return New(_carArray);

    carArray = _Array(carArrayInfo);

    _carArray.Reset(carArray);

    return carArray;
}

static map<AutoPtr<ICppVectorInfo const>, CopyablePersistent<Object>> _mapCPPVectorInfoToCARCPPVector;

Local<Object> CARCPPVector(ICppVectorInfo const *cppVectorInfo)
{
    ::Nan::EscapableHandleScope scope;

    Local<Object> cppVector;

    ECode ec;

    _ELASTOS Int32 length;

    auto &_cppVector = _mapCPPVectorInfoToCARCPPVector[cppVectorInfo];
    if (!_cppVector.IsEmpty())
        return scope.Escape(New(_cppVector));

    cppVector = _Array(cppVectorInfo);

    ec = cppVectorInfo->GetLength(&length);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    DefineOwnProperty(cppVector,
            New("length").ToLocalChecked(),
            ToValue(length),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));

    _cppVector.Reset(cppVector);

    return scope.Escape(cppVector);
}

static map<AutoPtr<IStructInfo const>, CopyablePersistent<Object>> _mapStructInfoToCARStruct;

Local<Object> CARStruct(IStructInfo const *structInfo)
{
    ::Nan::EscapableHandleScope scope;

    Local<Object> struct_;

    ECode ec;

    _ELASTOS String name;

    _ELASTOS Int32 nFieldInfos;

    AutoPtr<ArrayOf<IFieldInfo const *> > fieldInfos;

    auto &_struct = _mapStructInfoToCARStruct[structInfo];
    if (!_struct.IsEmpty())
        return scope.Escape(New(_struct));

    struct_ = New<Object>();

    DefineOwnProperty(struct_,
            New("$what").ToLocalChecked(),
            New("CARStruct").ToLocalChecked(),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = structInfo->GetName(&name);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    DefineOwnProperty(struct_,
            New("$name").ToLocalChecked(),
            ToValue(name),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = structInfo->GetFieldCount(&nFieldInfos);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    fieldInfos = ArrayOf<IFieldInfo const *>::Alloc(nFieldInfos);
    if (fieldInfos == 0)
        throw Error(Error::NO_MEMORY, "");

    ec = structInfo->GetAllFieldInfos(reinterpret_cast<ArrayOf<IFieldInfo *> *>(fieldInfos.Get()));
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    for (_ELASTOS Int32 i = 0; i < nFieldInfos; ++i) {
        ::Nan::HandleScope scope;

        IFieldInfo const *fieldInfo;

        Local<Object> field;

        _ELASTOS String fieldName;

        AutoPtr<IDataTypeInfo const> typeInfo;
        IDataTypeInfo *_typeInfo;

        fieldInfo = (*fieldInfos)[i];

        field = New<Object>();

        DefineOwnProperty(field,
                New("$what").ToLocalChecked(),
                New("CARStructField").ToLocalChecked(),
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

        ec = fieldInfo->GetName(&fieldName);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        DefineOwnProperty(field,
                New("$name").ToLocalChecked(),
                ToValue(fieldName),
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

        ec = fieldInfo->GetTypeInfo(&_typeInfo);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        typeInfo = _typeInfo, _typeInfo->Release();

        DefineOwnProperty(field,
                New("$type").ToLocalChecked(),
                CARDataType(typeInfo),
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

        DefineOwnProperty(struct_,
                ToValue(fieldName).As<::v8::String>(),
                field,
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
    }

    _struct.Reset(struct_);

    return scope.Escape(struct_);
}

typedef Local<Object> (*NewMethod)(size_t nMethodInfos, IFunctionInfo const *methodInfos[]);

static Local<Object> _CARInterface(IInterfaceInfo const *interfaceInfo, char const *what, NewMethod newMethod)
{
    ::Nan::EscapableHandleScope scope;

    Local<Object> interface_;

    ECode ec;

    _ELASTOS String namespace_;

    _ELASTOS String name;

    InterfaceID id;

    _ELASTOS Boolean local;

    _ELASTOS Boolean hasBase;

    _ELASTOS Int32 nMethods;

    AutoPtr<ArrayOf<IFunctionInfo const *> > methodInfos;

    map<_ELASTOS String, vector<IFunctionInfo const *>> mapNameToMethodInfos;

    interface_ = New<Object>();

    DefineOwnProperty(interface_,
            New("$what").ToLocalChecked(),
            New(what).ToLocalChecked(),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = interfaceInfo->GetNamespace(&namespace_);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    DefineOwnProperty(interface_,
            New("$namespace").ToLocalChecked(),
            ToValue(namespace_),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = interfaceInfo->GetName(&name);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    DefineOwnProperty(interface_,
            New("$name").ToLocalChecked(),
            ToValue(name),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = interfaceInfo->GetId(&id);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    DefineOwnProperty(interface_,
            New("$id").ToLocalChecked(),
            ToValue(&id),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = interfaceInfo->IsLocal(&local);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    DefineOwnProperty(interface_,
            New("$local").ToLocalChecked(),
            ToValueFromBoolean(local),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = interfaceInfo->HasBase(&hasBase);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    if (hasBase != FALSE) {
        AutoPtr<IInterfaceInfo const> baseInfo;
        IInterfaceInfo *_baseInfo;

        ec = interfaceInfo->GetBaseInfo(&_baseInfo);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        baseInfo = _baseInfo, _baseInfo->Release();

        DefineOwnProperty(interface_,
                New("$base").ToLocalChecked(),
                CARInterface(baseInfo),
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
    }

    ec = interfaceInfo->GetMethodCount(&nMethods);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    methodInfos = ArrayOf<IFunctionInfo const *>::Alloc(nMethods);
    if (methodInfos == 0)
        throw Error(Error::NO_MEMORY, "");

    ec = interfaceInfo->GetAllMethodInfos(reinterpret_cast<ArrayOf<IMethodInfo *> *>(methodInfos.Get()));
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    for (_ELASTOS Int32 i = 0; i < nMethods; ++i) {
        IFunctionInfo const *methodInfo;

        _ELASTOS String methodName;

        methodInfo = (*methodInfos)[i];

        ec = methodInfo->GetName(&methodName);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        mapNameToMethodInfos[methodName].push_back(methodInfo);
    }

    for (auto it = mapNameToMethodInfos.begin(), end = mapNameToMethodInfos.end(); it != end; ++it) {
        ::Nan::HandleScope scope;

        DefineOwnProperty(interface_,
                ToValue(it->first).As<::v8::String>(),
                (*newMethod)(it->second.size(), &it->second[0]),
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
    }

    return scope.Escape(interface_);
}

inline Local<Object> _CARMethod(size_t nMethodInfos, IFunctionInfo const *methodInfos[])
{
    return CARMethod(nMethodInfos, reinterpret_cast<IMethodInfo const **>(methodInfos));
}

static map<AutoPtr<IInterfaceInfo const>, CopyablePersistent<Object>> _mapInterfaceInfoToCARInterface;

Local<Object> CARInterface(IInterfaceInfo const *interfaceInfo)
{
    Local<Object> interface_;

    auto &_interface = _mapInterfaceInfoToCARInterface[interfaceInfo];
    if (!_interface.IsEmpty())
        return New(_interface);

    interface_ = _CARInterface(interfaceInfo, "CARInterface", &_CARMethod);

    _interface.Reset(interface_);

    return interface_;
}

inline Local<Object> _CARCallbackMethod(size_t nCallbackMethodInfos, IFunctionInfo const *callbackMethodInfos[])
{
    return CARCallbackMethod(
            nCallbackMethodInfos, reinterpret_cast<ICallbackMethodInfo const **>(callbackMethodInfos));
}

static
map<AutoPtr<ICallbackInterfaceInfo const>, CopyablePersistent<Object>>
_mapCallbackInterfaceInfoToCARCallbackInterface;

Local<Object> CARCallbackInterface(ICallbackInterfaceInfo const *callbackInterfaceInfo)
{
    Local<Object> callbackInterface;

    auto &_callbackInterface = _mapCallbackInterfaceInfoToCARCallbackInterface[callbackInterfaceInfo];
    if (!_callbackInterface.IsEmpty())
        return New(_callbackInterface);

    callbackInterface = _CARInterface(callbackInterfaceInfo, "CARCallbackInterface", &_CARCallbackMethod);

    _callbackInterface.Reset(callbackInterface);

    return callbackInterface;
}

Local<Object> CARDataType(IDataTypeInfo const *dataTypeInfo)
{
    ECode ec;

    CarDataType dataType;

    ec = dataTypeInfo->GetDataType(&dataType);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    switch (dataType) {
    case CarDataType_Int16:
    case CarDataType_Int32:
    case CarDataType_Int64:
    case CarDataType_Byte:
    case CarDataType_Float:
    case CarDataType_Double:
    case CarDataType_Char32:
    case CarDataType_String:
    case CarDataType_Boolean:
    case CarDataType_EMuid:
    case CarDataType_EGuid:
    case CarDataType_ECode:
        return _CARIntrinsicType(dataTypeInfo);

    case CarDataType_LocalPtr:
        return CARLocalPtr(static_cast<ILocalPtrInfo const *>(dataTypeInfo));

    case CarDataType_LocalType:
        return CARLocalType(dataTypeInfo);

    case CarDataType_Enum:
        return CAREnum(static_cast<IEnumInfo const *>(dataTypeInfo));

    case CarDataType_ArrayOf:
        return CARArray(static_cast<ICarArrayInfo const *>(dataTypeInfo));

    case CarDataType_CppVector:
        return CARCPPVector(static_cast<ICppVectorInfo const *>(dataTypeInfo));

    case CarDataType_Struct:
        return CARStruct(static_cast<IStructInfo const *>(dataTypeInfo));

    case CarDataType_Interface:
        return CARInterface(static_cast<IInterfaceInfo const *>(dataTypeInfo));

    default:
        abort();
    }
}

CAR_BRIDGE_NAMESPACE_END

