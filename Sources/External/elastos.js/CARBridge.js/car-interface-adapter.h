#ifndef __CAR_BRIDGE_CAR_INTERFACE_ADAPTER_H
# define __CAR_BRIDGE_CAR_INTERFACE_ADAPTER_H

# include <cstdarg>
# include <cstddef>

# include <node.h>

# include <nan.h>

# include <elastos.h>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

class CARInterfaceAdapter final {
public:
    static void Initialize(void) noexcept;

    static CARAPI New(
            IInterface **interface_,
            IInterfaceInfo const *interfaceInfo, ::v8::Local<::v8::Object> object) noexcept;

    IInterfaceInfo const *interfaceInfo() const noexcept
    {
        return _interfaceInfo;
    }

    ::v8::Local<::v8::Object> object() const
    {
        return ::Nan::New(_object);
    }

private:
    void *_vtptr; // It must be the FIRST non-static data member.

    _ELASTOS Int32 _referenceCount;

    _ELASTOS AutoPtr<IInterfaceInfo const> _interfaceInfo;
    _ELASTOS InterfaceID _interfaceId;
    _ELASTOS AutoPtr<_ELASTOS ArrayOf<IMethodInfo const *> const> _methodInfos;

    ::Nan::Persistent<::v8::Object> _object;

    template<size_t end>
    static void InitializeVTableFrom4To(void) noexcept;

    static CARAPI_(IInterface *) Probe(CARInterfaceAdapter *self, _ELASTOS REIID riid);

    static CARAPI_(_ELASTOS UInt32) AddRef(CARInterfaceAdapter *self);

    static CARAPI_(_ELASTOS UInt32) Release(CARInterfaceAdapter *self);

    static CARAPI GetInterfaceID(CARInterfaceAdapter *self,
            IInterface *object,
            _ELASTOS InterfaceID *interfaceId) noexcept;

    template<size_t index>
    static CARAPI CallOtherMethodIndexed(CARInterfaceAdapter *self, ...) noexcept;

    static CARAPI CallOtherMethodIndexed(size_t index, CARInterfaceAdapter *self, va_list ap) noexcept;

    CARInterfaceAdapter(IInterfaceInfo const *interfaceInfo, ::v8::Local<::v8::Object> object);

    CARInterfaceAdapter(CARInterfaceAdapter const &carInterfaceAdapter) = delete;

    CARInterfaceAdapter(CARInterfaceAdapter &&carInterfaceAdapter) = delete;

    virtual ~CARInterfaceAdapter();

    CARInterfaceAdapter &operator=(CARInterfaceAdapter const &carInterfaceAdapter) = delete;

    CARInterfaceAdapter &operator=(CARInterfaceAdapter &&carInterfaceAdapter) = delete;
};

CAR_BRIDGE_NAMESPACE_END

#endif
