
#include "V8CarObject.h"
#include "CarUtility.h"

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

bool V8CarObject::IsInstance(
    /* [in] */ v8::Local<v8::Object> object)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::FunctionTemplate> objectDesc =
            v8::Local<v8::FunctionTemplate>::New(isolate, sCarObjectDesc);
    return objectDesc->HasInstance(object);
}

AutoPtr<V8CarObject> V8CarObject::AsV8CarObject(
    /* [in] */ v8::Local<v8::Object> object)
{
    assert(object->InternalFieldCount() >= 1);
    V8CarObject* carObject = (V8CarObject*)object->GetAlignedPointerFromInternalField(sV8CarObjectIndex);
    return carObject;
}

v8::Handle<v8::Value> V8CarObject::V8CarObjectGetProperty(
    /* [in] */ v8::Local<v8::Object> self,
    /* [in] */ const String& identifier,
    /* [in] */ v8::Local<v8::Value> key)
{
    AutoPtr<V8CarObject> carObject = AsV8CarObject(self);

    AutoPtr<IClassInfo> klazz;
    CObject::ReflectClassInfo(carObject->mCarObject, (IClassInfo**)&klazz);

    Int32 methodCount;
    klazz->GetMethodCount(&methodCount);
    AutoPtr< ArrayOf<IMethodInfo*> > methods = ArrayOf<IMethodInfo*>::Alloc(methodCount);
    klazz->GetAllMethodInfos(methods);
    for (Int32 i = 0; i < methodCount; i++) {
        IMethodInfo* method = (*methods)[i];
        String name;
        method->GetName(&name);
        if (name.Equals(identifier)) {
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            v8::Local<v8::FunctionTemplate> functionTemplate = v8::FunctionTemplate::New(isolate);
            functionTemplate->SetCallHandler(V8CarObjectMethodHandler, key);

            // FunctionTemplate caches function for each context.
            v8::Local<v8::Function> v8Function = functionTemplate->GetFunction();
            v8Function->SetName(v8::Handle<v8::String>::Cast(key));
            return v8Function;
        }
        else if (name.Equals(String("Get") + identifier)) {
            CarValue value = GetProperty(carObject->mCarObject, method);
            v8::Handle<v8::Value> returnValue = CarUtility::ConvertCarValueToV8Object(value);
            return returnValue;
        }
    }

    return v8::Handle<v8::Value>();
}

void V8CarObject::V8CarObjectNamedPropertyGetter(
    /* [in] */ v8::Local<v8::String> property,
    /* [in] */ const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::String::Utf8Value utfStr(property);
    String identifier(*utfStr);
    v8::Handle<v8::Value> value = V8CarObjectGetProperty(info.Holder(), identifier, property);
    info.GetReturnValue().Set(value);
}


} // namespace Node
} // namespace Elastos
