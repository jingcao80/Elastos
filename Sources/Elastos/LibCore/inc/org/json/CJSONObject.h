
#ifndef __ORG_JSON_CJSONOBJECT_H__
#define __ORG_JSON_CJSONOBJECT_H__

#include "_Org_Json_CJSONObject.h"
#include "JSONObject.h"

namespace Org {
namespace Json {

CarClass(CJSONObject)
    , public JSONObject
{
public:
    CAR_OBJECT_DECL();
};

} //namespace Json
} //namespace Org

#endif //__ORG_JSON_CJSONOBJECT_H__
