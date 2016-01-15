
#ifndef __ORG_XML_SAX_HELPERS_CPARSERFACTORY_H__
#define __ORG_XML_SAX_HELPERS_CPARSERFACTORY_H__

#include "_Org_Xml_Sax_Helpers_CParserFactory.h"
#include <elastos/core/Singleton.h>

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

class ParserFactory {
public:
    static CARAPI MakeParser(
        /* [out] */ IParser** parser);

    static CARAPI MakeParser(
        /* [in] */ const String& className,
        /* [out] */ IParser** parser);

private:
    ParserFactory();
    ParserFactory(const ParserFactory&);
};

CarClass(CParserFactory)
    , public Singleton
    , public IParserFactory
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Create a new SAX parser using the `org.xml.sax.parser' system property.
     *
     * <p>The named class must exist and must implement the
     * {@link org.xml.sax.Parser Parser} interface.</p>
     *
     * @return the newly created parser.
     *
     * @exception java.lang.NullPointerException There is no value
     *            for the `org.xml.sax.parser' system property.
     * @exception java.lang.ClassNotFoundException The SAX parser
     *            class was not found (check your CLASSPATH).
     * @exception IllegalAccessException The SAX parser class was
     *            found, but you do not have permission to load
     *            it.
     * @exception InstantiationException The SAX parser class was
     *            found but could not be instantiated.
     * @exception java.lang.ClassCastException The SAX parser class
     *            was found and instantiated, but does not implement
     *            org.xml.sax.Parser.
     * @see #makeParser(java.lang.String)
     * @see org.xml.sax.Parser
     */
    CARAPI MakeParser(
        /* [out] */ IParser** parser);

    /**
     * Create a new SAX parser object using the class name provided.
     *
     * <p>The named class must exist and must implement the
     * {@link org.xml.sax.Parser Parser} interface.</p>
     *
     * @return the newly created parser.
     *
     * @param className A string containing the name of the
     *                  SAX parser class.
     * @exception java.lang.ClassNotFoundException The SAX parser
     *            class was not found (check your CLASSPATH).
     * @exception IllegalAccessException The SAX parser class was
     *            found, but you do not have permission to load
     *            it.
     * @exception InstantiationException The SAX parser class was
     *            found but could not be instantiated.
     * @exception java.lang.ClassCastException The SAX parser class
     *            was found and instantiated, but does not implement
     *            org.xml.sax.Parser.
     * @see #makeParser()
     * @see org.xml.sax.Parser
     */
    CARAPI MakeParser(
        /* [in] */ const String& className,
        /* [out] */ IParser** parser);

private:
    // TODO: Add your private member variables here.
};


} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org

#endif // __ORG_XML_SAX_HELPERS_CPARSERFACTORY_H__
