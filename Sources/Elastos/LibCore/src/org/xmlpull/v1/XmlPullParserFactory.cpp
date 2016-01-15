
#include "XmlPullParserFactory.h"
#include "CXmlPullParserFactory.h"

namespace Org {
namespace Xmlpull {
namespace V1 {

const String XmlPullParserFactory::PROPERTY_NAME("org.xmlpull.v1.XmlPullParserFactory");
const String XmlPullParserFactory::RESOURCE_NAME("/META-INF/services/org.xmlpull.v1.XmlPullParserFactory");

CAR_INTERFACE_IMPL(XmlPullParserFactory, Object, IXmlPullParserFactory)

ECode XmlPullParserFactory::SetFeature(
    /* [in] */ const String& name,
    /* [in] */ Boolean state)
{
    mFeatures[name] = state;
    return NOERROR;
}

ECode XmlPullParserFactory::GetFeature(
    /* [in] */ const String& name,
    /* [out] */ Boolean* isGot)
{
    VALIDATE_NOT_NULL(isGot)

    HashMap<String, Boolean>::Iterator iter = mFeatures.Find(name);
    *isGot = iter != mFeatures.End() ? iter->mSecond : FALSE;
    return NOERROR;
}

ECode XmlPullParserFactory::SetNamespaceAware(
    /* [in] */ Boolean awareness)
{
    mFeatures[IXmlPullParser::FEATURE_PROCESS_NAMESPACES] = awareness;
    return NOERROR;
}

ECode XmlPullParserFactory::IsNamespaceAware(
    /* [out] */ Boolean* isNspAware)
{
    return GetFeature(IXmlPullParser::FEATURE_PROCESS_NAMESPACES, isNspAware);
}

ECode XmlPullParserFactory::SetValidating(
    /* [in] */ Boolean validating)
{
    mFeatures[IXmlPullParser::FEATURE_VALIDATION] = validating;
    return NOERROR;
}

ECode XmlPullParserFactory::IsValidating(
    /* [out] */ Boolean* isValidating)
{
    VALIDATE_NOT_NULL(isValidating)

    return GetFeature(IXmlPullParser::FEATURE_VALIDATION, isValidating);
}

ECode XmlPullParserFactory::NewPullParser(
    /* [out] */ IXmlPullParser** pullParser)
{
    AutoPtr<IXmlPullParser> pp;
    FAIL_RETURN(GetParserInstance((IXmlPullParser**)&pp))
    if (pp) {
        HashMap<String, Boolean>::Iterator it;
        for (it = mFeatures.Begin(); it != mFeatures.End(); ++it) {
            // NOTE: This test is needed for compatibility reasons. We guarantee
            // that we only set a feature on a parser if its value is true.
            if (it->mSecond) {
                pp->SetFeature(it->mFirst, it->mSecond);
            }
        }
    }

    *pullParser = pp;
    REFCOUNT_ADD(*pullParser)
    return NOERROR;
}

ECode XmlPullParserFactory::NewSerializer(
    /* [out] */ IXmlSerializer** serial)
{
    return GetSerializerInstance(serial);
}

ECode XmlPullParserFactory::GetParserInstance(
    /* [out] */ IXmlPullParser** parser)
{
    VALIDATE_NOT_NULL(parser)
    *parser = NULL;

    assert(0 && "TODO");
    // ArrayList<Exception> exceptions = null;

    // if (parserClasses != null && !parserClasses.isEmpty()) {
    //     exceptions = new ArrayList<Exception>();
    //     for (Object o : parserClasses) {
    //         try {
    //             if (o != null) {
    //                 Class<?> parserClass = (Class<?>) o;
    //                 return (XmlPullParser) parserClass.newInstance();
    //             }
    //         } catch (InstantiationException e) {
    //             exceptions.add(e);
    //         } catch (IllegalAccessException e) {
    //             exceptions.add(e);
    //         } catch (ClassCastException e) {
    //             exceptions.add(e);
    //         }
    //     }
    // }

    // throw newInstantiationException("Invalid parser class list", exceptions);
    return NOERROR;
}

ECode XmlPullParserFactory::GetSerializerInstance(
    /* [out] */ IXmlSerializer** serializer)
{
    VALIDATE_NOT_NULL(serializer)
    *serializer = NULL;

    assert(0 && "TODO");
    // ArrayList<Exception> exceptions = null;

    // if (serializerClasses != null && !serializerClasses.isEmpty()) {
    //     exceptions = new ArrayList<Exception>();
    //     for (Object o : serializerClasses) {
    //         try {
    //             if (o != null) {
    //                 Class<?> serializerClass = (Class<?>) o;
    //                 return (XmlSerializer) serializerClass.newInstance();
    //             }
    //         } catch (InstantiationException e) {
    //             exceptions.add(e);
    //         } catch (IllegalAccessException e) {
    //             exceptions.add(e);
    //         } catch (ClassCastException e) {
    //             exceptions.add(e);
    //         }
    //     }
    // }

    // throw newInstantiationException("Invalid serializer class list", exceptions);
    return NOERROR;
}

// private static XmlPullParserException newInstantiationException(String message,
//         ArrayList<Exception> exceptions) {
//     if (exceptions == null || exceptions.isEmpty()) {
//         return new XmlPullParserException(message);
//     } else {
//         XmlPullParserException exception = new XmlPullParserException(message);
//         for (Exception ex : exceptions) {
//             exception.addSuppressed(ex);
//         }

//         return exception;
//     }
// }

ECode XmlPullParserFactory::NewInstance(
    /* [out] */ IXmlPullParserFactory** xmlPullParserFac)
{
    VALIDATE_NOT_NULL(xmlPullParserFac)

    AutoPtr<CXmlPullParserFactory> factory;
    CXmlPullParserFactory::NewByFriend((CXmlPullParserFactory**)&factory);
    *xmlPullParserFac = (IXmlPullParserFactory*)factory.Get();
    REFCOUNT_ADD(*xmlPullParserFac);
    return NOERROR;
}

ECode XmlPullParserFactory::NewInstance(
    /* [in] */ const String& classNames,
 // /* [in] */  Class context,
    /* [out] */ IXmlPullParserFactory** xmlPullParserFac)
{
    return NewInstance(xmlPullParserFac);
}

} // namespace V1
} // namespace Xmlpull
} // namespace Org
