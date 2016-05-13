
#include "V8Utility.h"

namespace Elastos {
namespace Node {

v8::Local<v8::Value> V8Utility::GetProperty(
    /* [in] */ v8::Local<v8::Object> v8Object,
    /* [in] */ String& propertyName)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope handleScope(isolate);

    v8::Local<v8::Value> v8result;

    if (v8Object.IsEmpty()) {
        return v8result;
    }

    v8result = v8Object->Get(v8::String::NewFromUtf8(isolate, propertyName.string()));
    return v8result;
}

} // namespace Node
} // namespace Elastos
