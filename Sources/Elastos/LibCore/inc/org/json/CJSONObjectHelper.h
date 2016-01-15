
#ifndef __ORG_JSON_CJSONOBJECTHELPER_H__
#define __ORG_JSON_CJSONOBJECTHELPER_H__

#include "_Org_Json_CJSONObjectHelper.h"
#include "Singleton.h"

using Elastos::Core::INumber;

namespace Org {
namespace Json {

CarClass(CJSONObjectHelper)
    , public Singleton
    , public IJSONObjectHelper
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL();

    CARAPI NumberToString(
        /* [in] */ INumber* number,
        /* [out] */ String* str);

    CARAPI Quote(
        /* [in] */ const String& data,
        /* [out] */ String* str);

    CARAPI Wrap(
        /* [in] */ IInterface* o,
        /* [out] */ IInterface** obj);
};

} //namespace Json
} //namespace Org

#endif //__ORG_JSON_CJSONOBJECTHELPER_H__
