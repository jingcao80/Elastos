
#ifndef __V8CAROBJECT_H__
#define __V8CAROBJECT_H__

#include "CarValueV8.h"
#include "v8.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Node {

class V8CarObject : public Elastos::Core::Object
{
public:
    static v8::Local<v8::Object> New(
        /* [in] */ v8::Isolate* isolate,
        /* [in] */ const CarValue& carValue);

    static bool IsInstance(
        /* [in] */ v8::Local<v8::Object> object);

    static AutoPtr<V8CarObject> AsV8CarObject(
        /* [in] */ v8::Local<v8::Object> object);

    AutoPtr<IInterface> GetCarObject() { return mCarObject; }

private:
    V8CarObject(
        /* [in] */ IInterface* carObject);

    ~V8CarObject();

    static void V8CarObjectNamedPropertyGetter(
        /* [in] */ v8::Local<v8::String> property,
        /* [in] */ const v8::PropertyCallbackInfo<v8::Value>& info);

    static void V8CarObjectNamedPropertySetter(
        /* [in] */ v8::Local<v8::String> property,
        /* [in] */ v8::Local<v8::Value> value,
        /* [in] */ const v8::PropertyCallbackInfo<v8::Value>& info);

    static void V8CarObjectQueryProperty(
        /* [in] */ v8::Local<v8::String> property,
        /* [in] */ const v8::PropertyCallbackInfo<v8::Integer>& info);

    static void V8CarObjectPropertyEnumerator(
        /* [in] */ const v8::PropertyCallbackInfo<v8::Array>& info);

    static void V8CarObjectIndexedPropertyGetter(
        /* [in] */ uint32_t index,
        /* [in] */ const v8::PropertyCallbackInfo<v8::Value>& info);

    static void V8CarObjectIndexedPropertySetter(
        /* [in] */ uint32_t index,
        /* [in] */ v8::Local<v8::Value> value,
        /* [in] */ const v8::PropertyCallbackInfo<v8::Value>& info);

    static void V8CarObjectIndexedPropertyEnumerator(
        /* [in] */ const v8::PropertyCallbackInfo<v8::Array>& info);

    static void V8CarObjectInvokeDefaultHandler(
        /* [in] */ const v8::FunctionCallbackInfo<v8::Value>& info);

private:
    static const int sV8CarObjectIndex;
    static const int sV8CarObjectInternalFieldCount;
    static v8::Persistent<v8::FunctionTemplate> sCarObjectDesc;

    AutoPtr<IInterface> mCarObject;
};

} // namespace Node
} // namespace Elastos

#endif //__V8CAROBJECT_H__
