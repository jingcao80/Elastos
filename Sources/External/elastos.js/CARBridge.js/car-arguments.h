#ifndef __CAR_BRIDGE_CAR_ARGUMENTS_H
# define __CAR_BRIDGE_CAR_ARGUMENTS_H

# include <node.h>

# include <nan.h>

# include <elastos.h>

# include "macros.h"

# include "weak-external-base.h"



CAR_BRIDGE_NAMESPACE_BEGIN

struct CARArgumentBase: WeakExternalBase {
    ::Nan::Persistent<::v8::Value> value;

protected:
    ~CARArgumentBase()
    {
        value.Reset();
    }
};

struct CalleeAllocCARArray: CARArgumentBase {
    _ELASTOS AutoPtr<ICarArrayInfo const> carArrayInfo;

    _ELASTOS CarQuintet **carQuintet = nullptr;

protected:
    ~CalleeAllocCARArray() = default;
};

template<class _CalleeAllocCARArray = struct CalleeAllocCARArray>
_CalleeAllocCARArray *CalleeAllocCARArray_(ICarArrayInfo const *carArrayInfo, _ELASTOS CarQuintet **carQuintet);

struct CalleeAllocStruct: CARArgumentBase {
    _ELASTOS AutoPtr<IStructInfo const> structInfo;

    void **struct_ = nullptr;

protected:
    ~CalleeAllocStruct() = default;
};

template<class _CalleeAllocStruct = struct CalleeAllocStruct>
_CalleeAllocStruct *CalleeAllocStruct_(IStructInfo const *structInfo, void **struct_);

struct CallerAllocInt16: CARArgumentBase {
    _ELASTOS Int16 *i16 = nullptr;

protected:
    ~CallerAllocInt16() = default;
};

template<class _CallerAllocInt16 = struct CallerAllocInt16>
_CallerAllocInt16 *CallerAllocInt16_(_ELASTOS Int16 *i16);

struct CallerAllocInt32: CARArgumentBase {
    _ELASTOS Int32 *i32 = nullptr;

protected:
    ~CallerAllocInt32() = default;
};

template<class _CallerAllocInt32 = struct CallerAllocInt32>
_CallerAllocInt32 *CallerAllocInt32_(_ELASTOS Int32 *i32);

struct CallerAllocInt64: CARArgumentBase {
    _ELASTOS Int64 *i64 = nullptr;

protected:
    ~CallerAllocInt64() = default;
};

template<class _CallerAllocInt64 = struct CallerAllocInt64>
_CallerAllocInt64 *CallerAllocInt64_(_ELASTOS Int64 *i64);

struct CallerAllocByte: CARArgumentBase {
    _ELASTOS Byte *byte = nullptr;

protected:
    ~CallerAllocByte() = default;
};

template<class _CallerAllocByte = struct CallerAllocByte>
_CallerAllocByte *CallerAllocByte_(_ELASTOS Byte *byte);

struct CallerAllocFloat: CARArgumentBase {
    _ELASTOS Float *f = nullptr;

protected:
    ~CallerAllocFloat() = default;
};

template<class _CallerAllocFloat = struct CallerAllocFloat>
_CallerAllocFloat *CallerAllocFloat_(_ELASTOS Float *f);

struct CallerAllocDouble: CARArgumentBase {
    _ELASTOS Double *d = nullptr;

protected:
    ~CallerAllocDouble() = default;
};

template<class _CallerAllocDouble = struct CallerAllocDouble>
_CallerAllocDouble *CallerAllocDouble_(_ELASTOS Double *d);

struct CallerAllocChar32: CARArgumentBase {
    _ELASTOS Char32 *c32 = nullptr;

protected:
    ~CallerAllocChar32() = default;
};

template<class _CallerAllocChar32 = struct CallerAllocChar32>
_CallerAllocChar32 *CallerAllocChar32_(_ELASTOS Char32 *c32);

struct CallerAllocString: CARArgumentBase {
    _ELASTOS String *s = nullptr;

protected:
    ~CallerAllocString() = default;
};

template<class _CallerAllocString = struct CallerAllocString>
_CallerAllocString *CallerAllocString_(_ELASTOS String *s);

struct CallerAllocBoolean: CARArgumentBase {
    _ELASTOS Boolean *b = nullptr;

protected:
    ~CallerAllocBoolean() = default;
};

template<class _CallerAllocBoolean = struct CallerAllocBoolean>
_CallerAllocBoolean *CallerAllocBoolean_(_ELASTOS Boolean *b);

struct CallerAllocEMuid: CARArgumentBase {
    _ELASTOS EMuid *id = nullptr;

protected:
    ~CallerAllocEMuid() = default;
};

template<class _CallerAllocEMuid = struct CallerAllocEMuid>
_CallerAllocEMuid *CallerAllocEMuid_(_ELASTOS EMuid *id);

struct CallerAllocEGuid: CARArgumentBase {
    _ELASTOS EGuid *id = nullptr;

protected:
    ~CallerAllocEGuid() = default;
};

template<class _CallerAllocEGuid = struct CallerAllocEGuid>
_CallerAllocEGuid *CallerAllocEGuid_(_ELASTOS EGuid *id);

struct CallerAllocECode: CARArgumentBase {
    _ELASTOS ECode *ecode = nullptr;

protected:
    ~CallerAllocECode() = default;
};

template<class _CallerAllocECode = struct CallerAllocECode>
_CallerAllocECode *CallerAllocECode_(_ELASTOS ECode *ecode);

struct CallerAllocLocalPtr: CARArgumentBase {
    void **localPtr = nullptr;

protected:
    ~CallerAllocLocalPtr() = default;
};

template<class _CallerAllocLocalPtr = struct CallerAllocLocalPtr>
_CallerAllocLocalPtr *CallerAllocLocalPtr_(void **localPtr);

struct CallerAllocLocalType: CARArgumentBase {
    _ELASTOS AutoPtr<IDataTypeInfo const> dataTypeInfo;

    void *localTypeObject = nullptr;

protected:
    ~CallerAllocLocalType() = default;
};

template<class _CallerAllocLocalType = struct CallerAllocLocalType>
_CallerAllocLocalType *CallerAllocLocalType_(IDataTypeInfo const *dataTypeInfo, void *localTypeObject);

struct CallerAllocEnum: CARArgumentBase {
    _ELASTOS Int32 *enum_ = nullptr;

protected:
    ~CallerAllocEnum() = default;
};

template<class _CallerAllocEnum = struct CallerAllocEnum>
_CallerAllocEnum *CallerAllocEnum_(_ELASTOS Int32 *enum_);

struct CallerAllocCARArray: CARArgumentBase {
    _ELASTOS AutoPtr<ICarArrayInfo const> carArrayInfo;

    _ELASTOS AutoPtr<IVariableOfCarArray> variableOfCARArray;

protected:
    ~CallerAllocCARArray() = default;
};

template<class _CallerAllocCARArray = struct CallerAllocCARArray>
_CallerAllocCARArray *CallerAllocCARArray_(ICarArrayInfo const *carArrayInfo, IVariableOfCarArray *variableOfCARArray);

struct CallerAllocStruct: CARArgumentBase {
    _ELASTOS AutoPtr<IStructInfo const> structInfo;

    _ELASTOS AutoPtr<IVariableOfStruct> variableOfStruct;

protected:
    ~CallerAllocStruct() = default;
};

template<class _CallerAllocStruct = struct CallerAllocStruct>
_CallerAllocStruct *CallerAllocStruct_(IStructInfo const *structInfo, IVariableOfStruct *variableOfStruct);

struct CallerAllocInterface: CARArgumentBase {
    _ELASTOS AutoPtr<IInterfaceInfo const> interfaceInfo;

    IInterface **interface_ = nullptr;

protected:
    ~CallerAllocInterface() = default;
};

template<class _CallerAllocInterface = struct CallerAllocInterface>
_CallerAllocInterface *CallerAllocInterface_(IInterfaceInfo const *interfaceInfo, IInterface **interface_);

CAR_BRIDGE_NAMESPACE_END

# include "car-arguments-cc.h"

#endif
