
#ifndef __ORG_JSON_CJSONTOKENER_H__
#define __ORG_JSON_CJSONTOKENER_H__

#include "_Org_Json_CJSONTokener.h"
#include "JSONTokener.h"

namespace Org {
namespace Json {

CarClass(CJSONTokener)
    , public JSONTokener
{
public:
    CAR_OBJECT_DECL();
};

} //namespace Json
} //namespace Org

#endif //__ORG_JSON_CJSONTOKENER_H__
