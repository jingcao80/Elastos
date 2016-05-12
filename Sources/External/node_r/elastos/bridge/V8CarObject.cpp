
#include "V8CarObject.h"

namespace Elastos {
namespace Node {

const int V8CarObject::sV8CarObjectIndex = 0;
const int V8CarObject::sV8CarObjectInternalFieldCount = 1;
v8::Persistent<v8::FunctionTemplate> V8CarObject::sCarObjectDesc;

V8CarObject::V8CarObject(
    /* [in] */ IInterface* carObject)
    : mCarObject(carObject)
{}

V8CarObject::~V8CarObject()
{}

v8::Local<v8::Object> V8CarObject::New(
    /* [in] */ v8::Isolate* isolate,
    /* [in] */ const CarValue& carValue)
{
    v8::Local<v8::FunctionTemplate> objectDesc;
    if (sCarObjectDesc.IsEmpty()) {
        objectDesc = v8::FunctionTemplate::New(isolate);
        objectDesc->InstanceTemplate()->SetInternalFieldCount(sV8CarObjectInternalFieldCount);
        objectDesc->InstanceTemplate()->SetNamedPropertyHandler(V8CarObjectNamedPropertyGetter,
                V8CarObjectNamedPropertySetter, V8CarObjectQueryProperty,
                0, V8CarObjectPropertyEnumerator);
        objectDesc->InstanceTemplate()->SetIndexedPropertyHandler(V8CarObjectIndexedPropertyGetter,
                V8CarObjectIndexedPropertySetter, 0,
                0, V8CarObjectIndexedPropertyEnumerator);
        objectDesc->InstanceTemplate()->SetCallAsFunctionHandler(V8CarObjectInvokeDefaultHandler);
        sCarObjectDesc.Reset(isolate, objectDesc);
    }
    else {
        objectDesc = v8::Local<v8::FunctionTemplate>::New(isolate, sCarObjectDesc);
    }

    v8::Local<v8::Object> value = objectDesc->GetFunction()->NewInstance();

    if (value.IsEmpty()) return value;

    assert(value->InternalFieldCount() >= 1);

    V8CarObject* carObject = new V8CarObject(carValue.mObjectValue);
    REFCOUNT_ADD(carObject);
    value->SetAlignedPointerInInternalField(sV8CarObjectIndex, carObject);

    return value;
}

} // namespace Node
} // namespace Elastos
