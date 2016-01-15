
#ifndef __ORG_XMLPULL_V1_CXMLPULLPARSERFACTORYHELPER_H__
#define __ORG_XMLPULL_V1_CXMLPULLPARSERFACTORYHELPER_H__

#include "_Org_Xmlpull_V1_CXmlPullParserFactoryHelper.h"
#include <elastos/core/Singleton.h>

namespace Org {
namespace Xmlpull {
namespace V1 {

CarClass(CXmlPullParserFactoryHelper)
    , public Singleton
    , public IXmlPullParserFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewInstance(
        /* [out] */ IXmlPullParserFactory** xmlPullParserFac);

    CARAPI NewInstance(
        /* [in] */ const String& classNames,
     // /* [in] */  Class context,
        /* [out] */ IXmlPullParserFactory** xmlPullParserFac);
};

} // namespace V1
} // namespace Xmlpull
} // namespace Org

#endif // __ORG_XMLPULL_V1_CXMLPULLPARSERFACTORYHELPER_H__
