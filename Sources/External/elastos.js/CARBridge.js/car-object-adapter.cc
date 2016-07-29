
#include <stdarg.h>
#include <stddef.h>

#include <node.h>

#include <nan.h>

#include <elastos.h>

#include "macros.h"

#include "nan-ext.h"

#include "elastos-ext.h"

#include "car-object-adapter.h"

#include "car-function-adapter.h"
#include "error.h"
#include "js-2-car.h"
#include "new.h"
#include "unique-ptr.h"



using namespace node;

using namespace Nan;

using namespace v8;

_ELASTOS_NAMESPACE_USING

CAR_BRIDGE_NAMESPACE_BEGIN

InterfaceID const EIID_CARObjectAdapter = {
    0X8CC78507,
    0X90A3,
    0X4771,
    {0X91, 0X22, 0XF2, 0X79, 0XD7, 0XD2, 0XD, 0XCA}
};

void CARObjectAdapter::Initialize(void) noexcept
{
    CARInterfaceAdapter::Initialize();
}

CARAPI CARObjectAdapter::New(
        CARObjectAdapter **carObjectAdapter,
        IInterfaceInfo const *interfaceInfo, Local<::v8::Object> object) noexcept
{
    CARObjectAdapter *_carObjectAdapter;

    try {
        _carObjectAdapter = new(NO_THROW) CARObjectAdapter(interfaceInfo, object);
    } catch (ECode ec) {
        return ec;
    }
    if (_carObjectAdapter == nullptr)
        return E_OUT_OF_MEMORY;

    if (carObjectAdapter != nullptr)
        *carObjectAdapter = _carObjectAdapter;

    return NO_ERROR;
}

CARObjectAdapter::CARObjectAdapter(IInterfaceInfo const *interfaceInfo, Local<::v8::Object> object):
    _interfaceInfo(interfaceInfo), _object(object)
{
    ECode ec;

    ClassID classId;

#if 0
    IInterface *callbackConnector;

#endif
    _carInterfaceAdapter = new CARInterfaceAdapter(interfaceInfo, this);
    if (_carInterfaceAdapter == nullptr)
        throw Error(Error::NO_MEMORY, "");
#if 0

    ec = GetClassID(&classId);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    callbackConnector = reinterpret_cast<IInterface *>(_carInterfaceAdapter);

    classId.mClsid.mData1 ^= ECLSID_XOR_CallbackSink.mData1;
    classId.mClsid.mData2 ^= ECLSID_XOR_CallbackSink.mData2;
    classId.mClsid.mData3 ^= ECLSID_XOR_CallbackSink.mData3;
    classId.mClsid.mData4[0] ^= ECLSID_XOR_CallbackSink.mData4[0];
    classId.mClsid.mData4[1] ^= ECLSID_XOR_CallbackSink.mData4[1];
    classId.mClsid.mData4[2] ^= ECLSID_XOR_CallbackSink.mData4[2];
    classId.mClsid.mData4[3] ^= ECLSID_XOR_CallbackSink.mData4[3];
    classId.mClsid.mData4[4] ^= ECLSID_XOR_CallbackSink.mData4[4];
    classId.mClsid.mData4[5] ^= ECLSID_XOR_CallbackSink.mData4[5];
    classId.mClsid.mData4[6] ^= ECLSID_XOR_CallbackSink.mData4[6];
    classId.mClsid.mData4[7] ^= ECLSID_XOR_CallbackSink.mData4[7];

    ec = _CObject_AcquireClassFactory(classId, RGM_SAME_DOMAIN, EIID_CALLBACK_CONNECTOR, &callbackConnector);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    _callbackConnector = static_cast<ICallbackConnector *>(callbackConnector), callbackConnector->Release();
#else
    _callbackConnector = 0;
#endif
}

CARObjectAdapter::~CARObjectAdapter()
{
    _object.Reset();
}

CARAPI_(IInterface *) CARObjectAdapter::Probe(REIID riid)
{
    if (riid == EIID_CARObjectAdapter)
        return static_cast<IObject *>(this);

    if (riid == EIID_CALLBACK_CONNECTOR)
        return static_cast<IInterface *>(_callbackConnector);

    return Core::Object::Probe(riid);
}

CARAPI CARObjectAdapter::GetInterfaceID(IInterface *object, InterfaceID *interfaceId) noexcept
{
}

CARAPI CARObjectAdapter::Aggregate(AggregateType type, IInterface *object) noexcept
{
}

CARAPI CARObjectAdapter::GetDomain(IInterface **object) noexcept
{
    return E_NOT_IMPLEMENTED;
}

CARAPI CARObjectAdapter::GetClassID(ClassID *classId) noexcept
{
    return E_NOT_IMPLEMENTED;
}

CARAPI CARObjectAdapter::ToString(_ELASTOS String *string) noexcept
{
    if (string == 0)
        return NO_ERROR;

    *string = "Class CARObjectAdapter";

    return NO_ERROR;
}

static int const _MAX_NUM_OF_METHODS = 0XE4;

typedef void (*VirtualMethod)(void);

static VirtualMethod _vtable[_MAX_NUM_OF_METHODS];

template<size_t end>
void CARObjectAdapter::CARInterfaceAdapter::InitializeVTableFrom4To(void) noexcept
{
    InitializeVTableFrom4To<end - 1>();

    _vtable[end - 1] = reinterpret_cast<VirtualMethod>(&CallOtherMethodIndexed<end - 5>);
}

template<>
void CARObjectAdapter::CARInterfaceAdapter::InitializeVTableFrom4To<5>(void) noexcept
{
    _vtable[4] = reinterpret_cast<VirtualMethod>(&CallOtherMethodIndexed<0>);
}

template<>
void CARObjectAdapter::CARInterfaceAdapter::InitializeVTableFrom4To<4>(void) noexcept
{
}

template<>
void CARObjectAdapter::CARInterfaceAdapter::InitializeVTableFrom4To<3>(void) noexcept
{
}

template<>
void CARObjectAdapter::CARInterfaceAdapter::InitializeVTableFrom4To<2>(void) noexcept
{
}

template<>
void CARObjectAdapter::CARInterfaceAdapter::InitializeVTableFrom4To<1>(void) noexcept
{
}

template<>
void CARObjectAdapter::CARInterfaceAdapter::InitializeVTableFrom4To<0>(void) noexcept
{
}

void CARObjectAdapter::CARInterfaceAdapter::Initialize(void) noexcept
{
    _vtable[0] = &CARInterfaceAdapter::Probe;
    _vtable[1] = &CARInterfaceAdapter::AddRef;
    _vtable[2] = &CARInterfaceAdapter::Release;
    _vtable[3] = &CARInterfaceAdapter::GetInterfaceID;

    InitializeVTableFrom4To<_MAX_NUM_OF_METHODS>();
}

CARAPI_(IInterface *) CARObjectAdapter::CARInterfaceAdapter::Probe(CARInterfaceAdapter *self, REIID riid)
{
    return self->_owner->Probe(riid);
}

CARAPI_(UInt32) CARObjectAdapter::CARInterfaceAdapter::AddRef(CARInterfaceAdapter *self)
{
    return self->_owner->AddRef();
}

CARAPI_(UInt32) CARObjectAdapter::CARInterfaceAdapter::Release(CARInterfaceAdapter *self)
{
    return self->_owner->Release();
}

CARAPI CARObjectAdapter::CARInterfaceAdapter::GetInterfaceID(CARInterfaceAdapter *self,
        IInterface *object,
        InterfaceID *interfaceId) noexcept
{
    return self->_owner->GetInterfaceID(object, interfaceId);
}

template<size_t index>
CARAPI CARObjectAdapter::CARInterfaceAdapter::CallOtherMethodIndexed(CARInterfaceAdapter *self, ...) noexcept
{
    va_list ap;

    ECode ec;

    va_start(ap, self);

    ec = CallOtherMethodIndexed(index, self, ap);

    va_end(ap);

    return ec;
}

CARAPI CARObjectAdapter::CARInterfaceAdapter::CallOtherMethodIndexed(size_t index,
        CARInterfaceAdapter *self,
        va_list ap) noexcept
{
    ::Nan::HandleScope scope;

    IMethodInfo const *methodInfo;

    ECode ec;

    _ELASTOS String methodName;
    Local<::v8::String> _methodName;

    Local<Value> value;

    methodInfo = (*self->_methodInfos)[index];

    ec = methodInfo->GetName(&methodName);
    if (FAILED(ec))
        return ec;

    _methodName = ToValue(methodName).As<::v8::String>();

    if (!Has(::Nan::New(self->_owner->_object), _methodName).FromJust())
        return E_NOT_IMPLEMENTED;

    value = Get(::Nan::New(self->_owner->_object), _methodName).ToLocalChecked();
    if (!value->IsFunction())
        return E_NOT_IMPLEMENTED;

    return CARFunctionAdapter::Call(value.As<Function>(), ::Nan::New(self->_owner->_object), methodInfo, ap);
}

CARObjectAdapter::CARInterfaceAdapter::CARInterfaceAdapter(IInterfaceInfo const *interfaceInfo,
        CARObjectAdapter *owner):
    _interfaceInfo(interfaceInfo), _owner(owner)
{
    ECode ec;

    _ELASTOS Int32 nMethods;

    AutoPtr<ArrayOf<IFunctionInfo const *> > methodInfos;
    ArrayOf<IFunctionInfo const *> *__methodInfos;

    _vtptr = _vtable;

    ec = interfaceInfo->GetMethodCount(&nMethods);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    __methodInfos = ArrayOf<IFunctionInfo const *>::Alloc(nMethods);
    if (__methodInfos == 0)
        throw Error(Error::NO_MEMORY, "");

    methodInfos = __methodInfos, __methodInfos->Release();

    ec = interfaceInfo->GetAllMethodInfos(reinterpret_cast<ArrayOf<IMethodInfo *> *>(methodInfos.Get()));
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    _methodInfos = methodInfos;
}

CAR_BRIDGE_NAMESPACE_END

