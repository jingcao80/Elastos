
#include "V8CarObject.h"

using namespace v8;

namespace Elastos {
namespace Node {

static Persistent<ObjectTemplate> sCarObjectDesc;

Local<Object> V8CarObject::New(
    /* [in] */ Isolate* isolate,
    /* [in] */ const CarValue& carValue)
{
    if (sCarObjectDesc.IsEmpty()) {

    }

    Local<Object> value;
    return value;
}

} // namespace Node
} // namespace Elastos
