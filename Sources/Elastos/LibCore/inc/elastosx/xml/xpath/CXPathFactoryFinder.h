
#ifndef __ELASTOSX_XML_XPATH_CXPATHFACTORYFINDER_H__
#define __ELASTOSX_XML_XPATH_CXPATHFACTORYFINDER_H__

#include "_Elastosx_Xml_XPath_CXPathFactoryFinder.h"
#include "Object.h"

using Elastos::Utility::IProperties;
using Elastos::Core::IClassLoader;
using Elastos::IO::IInputStream;

namespace Elastosx {
namespace Xml {
namespace XPath {

/**
 * Wrapper for a text attribute value which represents an annotation. An
 * annotation has two special aspects:
 * <ol>
 * <li>it is connected to a range of main text; if this range or the main text
 * is changed then the annotation becomes invalid,</li>
 * <li>it can not be joined with adjacent annotations even if the text attribute
 * value is the same.</li>
 * </ol>
 * <p>
 * By wrapping text attribute values into an {@code Annotation}, these aspects
 * will be taken into account when handling annotation text and the
 * corresponding main text.
 * <p>
 * Note: There is no semantic connection between this annotation class and the
 * {@code java.lang.annotation} package.
 */
CarClass(CXPathFactoryFinder)
    , public Object
    , public IXPathFactoryFinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IClassLoader* loader);

    CARAPI NewFactory(
        /* [in] */ const String& uri,
        /* [out] */ IXPathFactory** nf);

    CARAPI CreateInstance(
        /* [in] */ const String& className,
        /* [out] */ IXPathFactory** instance);

private:
    static CARAPI_(void) DebugPrintln(
        /* [in] */ const String& msg);

    CARAPI_(void) DebugDisplayClassLoader();

    CARAPI _newFactory(
        /* [in] */ const String& uri,
        /* [out] */ IXPathFactory** xpf);

    CARAPI LoadFromServicesFile(
        /* [in] */ const String& uri,
        /* [in] */ const String& resourceName,
        /* [in] */ IInputStream* in,
        /* [out] */ IXPathFactory** xpf);

     //Iterable<URL> createServiceFileIterator()
     //CARAPI CreateServiceFileIterator(
     //   /* [out] */ IIterable** sfi);

    static CARAPI_(String) Which(
        /* [in] */ IClassInfo* clazz );

    static CARAPI_(String) Which(
        /* [in] */ const String& classname,
        /* [in] */ IClassLoader* loader);
private:
    static Boolean debug;
    const static Int32 DEFAULT_LINE_LENGTH = 80;
    static AutoPtr<IProperties> cacheProps;
    static Boolean firstTime;
    AutoPtr<IClassLoader> classLoader;

    //private static final Class SERVICE_CLASS = XPathFactory.class;
    //private static final String SERVICE_ID = "META-INF/services/" + SERVICE_CLASS.getName();

};

} // namespace XPath
} // namespace Xml
} // namespace Elastos

#endif // __ELASTOSX_XML_XPATH_CXPATHFACTORYFINDER_H__
