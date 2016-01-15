
#include "CJSONObjectHelper.h"
#include "JSONObject.h"

namespace Org {
namespace Json {

CAR_SINGLETON_IMPL(CJSONObjectHelper);

CAR_INTERFACE_IMPL(CJSONObjectHelper, Singleton, IJSONObjectHelper);

ECode CJSONObjectHelper::NumberToString(
    /* [in] */ INumber* number,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    return JSONObject::NumberToString(number, str);
}

ECode CJSONObjectHelper::Quote(
    /* [in] */ const String& data,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    return JSONObject::Quote(data, str);
}

ECode CJSONObjectHelper::Wrap(
    /* [in] */ IInterface* o,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    return JSONObject::Wrap(o, obj);
}

} //namespace Json
} //namespace Org
