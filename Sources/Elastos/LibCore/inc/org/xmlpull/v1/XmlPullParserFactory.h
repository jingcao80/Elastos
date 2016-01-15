
#ifndef __ORG_XMLPULL_V1_XMLPULLPARSERFACTORY_H__
#define __ORG_XMLPULL_V1_XMLPULLPARSERFACTORY_H__

#include "Elastos.CoreLibrary.External.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Org::Xmlpull::V1::IXmlPullParserFactory;

namespace Org {
namespace Xmlpull {
namespace V1 {

/**
 * This class is used to create implementations of XML Pull Parser defined in XMPULL V1 API.
 *
 * @see XmlPullParser
 *
 * @author <a href="http://www.extreme.indiana.edu/~aslom/">Aleksander Slominski</a>
 * @author Stefan Haustein
 */
class XmlPullParserFactory
    : public Object
    , public IXmlPullParserFactory
{
public:
    CAR_INTERFACE_DECL()

    CARAPI SetFeature(
        /* [in] */ const String& name,
        /* [in] */ Boolean state);

    CARAPI GetFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean* isGot);

    CARAPI SetNamespaceAware(
        /* [in] */ Boolean awareness);

    CARAPI IsNamespaceAware(
        /* [out] */ Boolean* isNspAware);

    CARAPI SetValidating(
        /* [in] */ Boolean validating);

    CARAPI IsValidating(
        /* [out] */ Boolean* isValidating);

    /**
     * Creates a new instance of a XML Pull Parser
     * using the currently configured factory features.
     *
     * @return A new instance of a XML Pull Parser.
     */
    CARAPI NewPullParser(
        /* [out] */ IXmlPullParser** pullParser);

    CARAPI NewSerializer(
        /* [out] */ IXmlSerializer** serial);

    static CARAPI NewInstance(
        /* [out] */ IXmlPullParserFactory** xmlPullParserFac);

    static CARAPI NewInstance(
        /* [in] */ const String& classNames,
     // /* [in] */  Class context,
        /* [out] */ IXmlPullParserFactory** xmlPullParserFac);

private:
    ECode GetParserInstance(
        /* [out] */ IXmlPullParser** parser);

    ECode GetSerializerInstance(
        /* [out] */ IXmlSerializer** serializer);

public:
    static const String PROPERTY_NAME; // = "org.xmlpull.v1.XmlPullParserFactory";

protected:
    String mClassNamesLocation;

    // ArrayList mParserClasses;
    // ArrayList mSerializerClasses;

    // features are kept there
    HashMap<String, Boolean> mFeatures;
private:
    // /** used as default class to server as context class in newInstance() */
    // final static Class referenceContextClass;

    // static {
    //     XmlPullParserFactory f = new XmlPullParserFactory();
    //     referenceContextClass = f.getClass();
    // }

    static const String RESOURCE_NAME; // = "/META-INF/services/" + PROPERTY_NAME;
};

} // namespace V1
} // namespace Xmlpull
} // namespace Org

#endif // __ORG_XMLPULL_V1_XMLPULLPARSERFACTORY_H__
