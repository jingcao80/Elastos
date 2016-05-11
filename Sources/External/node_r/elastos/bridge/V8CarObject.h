
#ifndef __V8CAROBJECT_H__
#define __V8CAROBJECT_H__

#include "CarValueV8.h"
#include "v8.h"

namespace Elastos {
namespace Node {

class V8CarObject
{
public:
    static v8::Local<v8::Object> New(
        /* [in] */ v8::Isolate* isolate,
        /* [in] */ const CarValue& carValue);
};

} // namespace Node
} // namespace Elastos

#endif //__V8CAROBJECT_H__
