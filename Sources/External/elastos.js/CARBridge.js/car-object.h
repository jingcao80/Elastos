#ifndef __CAR_BRIDGE_CAR_OBJECT_H
# define __CAR_BRIDGE_CAR_OBJECT_H

# include <cstddef>
# include <cstdint>

# include <functional>
# include <map>
# include <memory>
# include <set>

# include <node.h>

# include <nan.h>

# include <elastos.h>

# include "macros.h"

# include "nan-ext.h"

# include "car-function-adapter.h"



CAR_BRIDGE_NAMESPACE_BEGIN

class CARObject: public ::Nan::ObjectWrap {
public:
    static void Initialize(void);

    static ::v8::Local<::v8::FunctionTemplate> NewClassTemplate(IClassInfo const *classInfo);

    static ::v8::Local<::v8::Object> New(IInterface *carObject);

    static bool HasInstance(::v8::Local<::v8::Object> object);

    IClassInfo const *classInfo() const noexcept
    {
        return _classInfo;
    }

    IInterface *carObject() const noexcept
    {
        return _carObject;
    }

private:
    template<class T>
    struct Less: ::std::binary_function<CopyablePersistent<T>, CopyablePersistent<T>, bool> {
        bool operator()(CopyablePersistent<T> const &x, CopyablePersistent<T> const &y) const
        {
            ::v8::internal::Object **_x;
            ::v8::internal::Object **_y;

            _x = reinterpret_cast<::v8::internal::Object **>(*::Nan::New(x));
            _y = reinterpret_cast<::v8::internal::Object **>(*::Nan::New(y));

            if (_x == nullptr)
                return true;
            if (_y == nullptr)
                return false;

            return *_x < *_y;
        }
    };

    typedef ::std::map<CopyablePersistent<::v8::Function>,
            ::std::unique_ptr<CARFunctionAdapter>,
            Less<::v8::Function>> MF2CARFA;
    typedef ::std::map<CopyablePersistent<::v8::String>, MF2CARFA, Less<::v8::String>> MS2MF2CARFA;

    typedef CARObject *(*Constructor)(size_t argc, ::v8::Local<::v8::Value> argv[], ::v8::Local<::v8::Value> data);

    static ::Nan::Persistent<::v8::FunctionTemplate> _classBaseTemplate;

    _ELASTOS AutoPtr<IClassInfo const> _classInfo;

    _ELASTOS AutoPtr<IInterface> _carObject;

    MS2MF2CARFA _mapNameToMapListenerToCARFunctionAdapter;

    ::std::set<uintptr_t> _connectionIds;

    static NAN_METHOD(On);

    static void Off(CARObject *carObject,
            CARFunctionAdapter *carEventDelegate,
            ::v8::Local<::v8::String> name,
            ::v8::Local<::v8::Function> listener);

    static void Off(CARObject *carObject, CARFunctionAdapter *carEventDelegate);

    static void Off(::v8::Local<::v8::Object> object, ::v8::Local<::v8::Number> connectionId);

    static void Off(::v8::Local<::v8::Object> object,
            ::v8::Local<::v8::String> name,
            ::v8::Local<::v8::Function> listener);

    static void Off(::v8::Local<::v8::Object> object, ::v8::Local<::v8::String> name);

    static NAN_METHOD(Off);

    static NAN_METHOD(OffAll);

    static NAN_METHOD(EnterRegime);

    static NAN_METHOD(LeaveRegime);

    static NAN_METHOD(Equal);

    static NAN_METHOD(InvokeMethod);

    static NAN_METHOD(Attach);

    static NAN_METHOD(Detach);

    static NAN_METHOD(Probe);

    static CARObject *NewInRegimeConstructor(size_t argc, ::v8::Local<::v8::Value> argv[],
            ::v8::Local<::v8::Value> data);

    static NAN_METHOD(NewInRegimeConstructor);

    static NAN_METHOD(InRegime);

    static ::v8::Local<::v8::FunctionTemplate> NewClassTemplate(IClassInfo const *classInfo,
            _ELASTOS ArrayOf<IConstructorInfo const *> const &constructorInfos,
            ::Nan::FunctionCallback constructor, ::v8::Local<::v8::Value> data);

    static ::v8::Local<::v8::FunctionTemplate> NewClassTemplate(IClassInfo const *classInfo,
            ::Nan::FunctionCallback constructor, ::v8::Local<::v8::Value> data);

    static ::Nan::NAN_METHOD_RETURN_TYPE ClassConstructor(::Nan::NAN_METHOD_ARGS_TYPE info, Constructor constructor);

    static CARObject *NewConstructor(size_t argc, ::v8::Local<::v8::Value> argv[], ::v8::Local<::v8::Value> data);

    static NAN_METHOD(NewConstructor);

    static CARObject *WrapConstructor(size_t argc, ::v8::Local<::v8::Value> argv[], ::v8::Local<::v8::Value> data);

    static NAN_METHOD(WrapConstructor);

    CARObject(IClassInfo const *classInfo, _ELASTOS ArrayOf<IConstructorInfo const *> const &constructorInfos,
            IRegime *regime,
            size_t argc, ::v8::Local<::v8::Value> argv[]);

    CARObject(IClassInfo const *classInfo, _ELASTOS ArrayOf<IConstructorInfo const *> const &constructorInfos,
            size_t argc, ::v8::Local<::v8::Value> argv[]);

    CARObject(IClassInfo const *classInfo, IInterface *carObject) noexcept;

    CARObject(CARObject const &carObject) = delete;

    CARObject(CARObject &&carObject) = delete;

    CARObject &operator=(CARObject const &carObject) = delete;

    CARObject &operator=(CARObject &&carObject) = delete;
};

CAR_BRIDGE_NAMESPACE_END

#endif
