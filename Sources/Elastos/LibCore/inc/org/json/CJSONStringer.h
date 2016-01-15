
#ifndef __ORG_JSON_CJSONSTRINGER_H__
#define __ORG_JSON_CJSONSTRINGER_H__

#include "_Org_Json_CJSONStringer.h"
#include "JSONStringer.h"

namespace Org {
namespace Json {

CarClass(CJSONStringer)
    , public JSONStringer
{
public:
    CAR_OBJECT_DECL();
};

} //namespace Json
} //namespace Org

#endif //__ORG_JSON_CJSONSTRINGER_H__
