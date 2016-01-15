
#ifndef __ORG_JSON_CJSONARRAY_H__
#define __ORG_JSON_CJSONARRAY_H__

#include "_Org_Json_CJSONArray.h"
#include "JSONArray.h"

namespace Org {
namespace Json {

CarClass(CJSONArray)
    , public JSONArray
{
public:
    CAR_OBJECT_DECL();
};

} //namespace Json
} //namespace Org

#endif //__ORG_JSON_CJSONARRAY_H__
