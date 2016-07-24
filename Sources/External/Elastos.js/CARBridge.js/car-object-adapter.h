#ifndef __CAR_BRIDGE_CAR_OBJECT_ADAPTER_H
# define __CAR_BRIDGE_CAR_OBJECT_ADAPTER_H

# include <stdarg.h>
# include <stddef.h>

# include <node.h>

# include <nan.h>

# include <elastos.h>
# include <elastos/core/Object.h>

# include "macros.h"

# include "car-function-adapter.h"
# include "unique-ptr.h"



CAR_BRIDGE_NAMESPACE_BEGIN

class CARObjectAdapter: public _ELASTOS Core::Object {
public:
    static void Initialize(void) noexcept;

    static CARAPI New(
            CARObjectAdapter **carObjectAdapter,
            IInterfaceInfo const *interfaceInfo, ::v8::Local<::v8::Object> object) noexcept;

    IInterfaceInfo const *interfaceInfo() const noexcept
    {
        return _interfaceInfo;
    }

    ::v8::Local<::v8::Object> object() const
    {
        return ::Nan::New(_object);
    }

    CARAPI_(IInterface *) Probe(_ELASTOS REIID riid);

    CARAPI GetInterfaceID(IInterface *object, _ELASTOS InterfaceID *interfaceId) noexcept;

    CARAPI Aggregate(AggregateType type, IInterface *object) noexcept;

    CARAPI GetDomain(IInterface **object) noexcept;

    CARAPI GetClassID(_ELASTOS ClassID *classId) noexcept;

    CARAPI ToString(_ELASTOS String *string) noexcept;

protected:
    CARObjectAdapter(IInterfaceInfo const *interfaceInfo, ::v8::Local<::v8::Object> object);

    virtual ~CARObjectAdapter();

private:
    class CARInterfaceAdapter final {
    public:
        static void Initialize(void) noexcept;

        CARInterfaceAdapter(IInterfaceInfo const *interfaceInfo, CARObjectAdapter *owner);

        IInterfaceInfo const *interfaceInfo() const noexcept
        {
            return _interfaceInfo;
        }

        CARObjectAdapter *owner() const noexcept
        {
            return _owner;
        }

    private:
        void *_vtptr; // It must be the first non-static data member.

        _ELASTOS AutoPtr<IInterfaceInfo const> _interfaceInfo;
        _ELASTOS AutoPtr<_ELASTOS ArrayOf<IFunctionInfo const *> const> _methodInfos;

        _ELASTOS AutoPtr<CARObjectAdapter> _owner;

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

        CARInterfaceAdapter(CARInterfaceAdapter const &carInterfaceAdapter) = delete;

        CARInterfaceAdapter(CARInterfaceAdapter &&carInterfaceAdapter) = delete;

        CARInterfaceAdapter &operator=(CARInterfaceAdapter const &carInterfaceAdapter) = delete;

        CARInterfaceAdapter &operator=(CARInterfaceAdapter &&carInterfaceAdapter) = delete;
    };

    _ELASTOS AutoPtr<IInterfaceInfo const> _interfaceInfo;

    ::Nan::Persistent<::v8::Object> _object;

    UniquePtr<CARInterfaceAdapter> _carInterfaceAdapter;

    _ELASTOS AutoPtr<ICallbackConnector> _callbackConnector;

    CARObjectAdapter(CARObjectAdapter const &carObjectAdapter) = delete;

    CARObjectAdapter(CARObjectAdapter &&carObjectAdapter) = delete;

    CARObjectAdapter &operator=(CARObjectAdapter const &carObjectAdapter) = delete;

    CARObjectAdapter &operator=(CARObjectAdapter &&carObjectAdapter) = delete;
};

CAR_BRIDGE_NAMESPACE_END

#endif
