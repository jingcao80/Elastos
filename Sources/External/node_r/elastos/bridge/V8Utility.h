
#ifndef __V8UTILITY_H__
#define __V8UTILITY_H__

#include <v8.h>
#include "eldef.h"

namespace Elastos {
namespace Node {

class V8Utility
{
public:
    static v8::Local<v8::Value> GetProperty(
        /* [in] */ v8::Local<v8::Object> v8Object,
        /* [in] */ String& propertyName);
};

} // namespace Node
} // namespace Elastos

#endif //__V8UTILITY_H__
