
#ifndef __ORG_XML_SAX_EXT_CLOCATOR2IMPL_H__
#define __ORG_XML_SAX_EXT_CLOCATOR2IMPL_H__

#include "_Org_Xml_Sax_Ext_CLocator2Impl.h"
#include "LocatorImpl.h"

using Org::Xml::Sax::Helpers::LocatorImpl;

namespace Org {
namespace Xml {
namespace Sax {
namespace Ext {

CarClass(CLocator2Impl)
    , public LocatorImpl
    , public ILocator2
    , public ILocator2Impl
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Construct a new, empty Locator2Impl object.
     * This will not normally be useful, since the main purpose
     * of this class is to make a snapshot of an existing Locator.
     */
    CARAPI constructor();

    /**
     * Copy an existing Locator or Locator2 object.
     * If the object implements Locator2, values of the
     * <em>encoding</em> and <em>version</em>strings are copied,
     * otherwise they set to <em>null</em>.
     *
     * @param locator The existing Locator object.
     */
    CARAPI constructor(
        /* [in] */ ILocator* locator);

    /**
     * Assigns the current value of the version property.
     *
     * @param version the new "version" value
     * @see #getXMLVersion
     */
    CARAPI SetXMLVersion(
        /* [in] */ const String& xmlVersion);

    /**
     * Assigns the current value of the encoding property.
     *
     * @param encoding the new "encoding" value
     * @see #getEncoding
     */
    CARAPI SetEncoding(
        /* [in] */ const String& encoding);

    /**
     * Returns the current value of the version property.
     *
     * @return the current value of the version property.
     *
     * @see #setXMLVersion
     */
    CARAPI GetXMLVersion(
        /* [out] */ String* xmlVersion);

    /**
     * Returns the current value of the encoding property.
     *
     * @return the current value of the encoding property.
     *
     * @see #setEncoding
     */
    CARAPI GetEncoding(
        /* [out] */ String* encoding);

private:
    String mEncoding;
    String mVersion;
};

} // namespace Ext
} // namespace Sax
} // namespace Xml
} // namespace Org

#endif // __ORG_XML_SAX_EXT_CLOCATOR2IMPL_H__
