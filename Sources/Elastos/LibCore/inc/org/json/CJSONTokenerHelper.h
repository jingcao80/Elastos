
#ifndef __ORG_JSON_CJSONTOKENERHELPER_H__
#define __ORG_JSON_CJSONTOKENERHELPER_H__

#include "_Org_Json_CJSONTokenerHelper.h"
#include "Singleton.h"

namespace Org {
namespace Json {

CarClass(CJSONTokenerHelper)
    , public Singleton
    , public IJSONTokenerHelper
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL()

    CARAPI Dehexchar(
        /* [in] */ Char32 hex,
        /* [out] */ Int32* data);
};

} //namespace Json
} //namespace Org

#endif //__ORG_JSON_CJSONTOKENERHELPER_H__
