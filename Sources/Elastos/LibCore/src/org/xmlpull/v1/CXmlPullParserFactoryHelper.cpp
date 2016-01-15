
#include "CXmlPullParserFactoryHelper.h"
#include "XmlPullParserFactory.h"

namespace Org {
namespace Xmlpull {
namespace V1 {

CAR_SINGLETON_IMPL(CXmlPullParserFactoryHelper)

CAR_INTERFACE_IMPL(CXmlPullParserFactoryHelper, Singleton, IXmlPullParserFactoryHelper)

ECode CXmlPullParserFactoryHelper::NewInstance(
    /* [out] */ IXmlPullParserFactory** xmlPullParserFac)
{
    return XmlPullParserFactory::NewInstance(xmlPullParserFac);
}

ECode CXmlPullParserFactoryHelper::NewInstance(
    /* [in] */ const String& classNames,
 // /* [in] */  Class context,
    /* [out] */ IXmlPullParserFactory** xmlPullParserFac)
{
    return XmlPullParserFactory::NewInstance(classNames, xmlPullParserFac);
}

} // namespace V1
} // namespace Xmlpull
} // namespace Org
