
#ifndef __ELASTOS_DROID_CONTENT_CDEFAULTDATAHANDLER_H__
#define __ELASTOS_DROID_CONTENT_CDEFAULTDATAHANDLER_H__

#include "Elastos.Droid.Net.h"
#define HASH_FOR_NET
#include "elastos/droid/ext/frameworkhash.h"
#include "_Elastos_Droid_Content_CDefaultDataHandler.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/Stack.h>

using Elastos::Utility::Etl::Stack;
using Elastos::Droid::Net::IUri;
using Elastos::IO::IInputStream;
using Org::Xml::Sax::IAttributes;
using Org::Xml::Sax::ILocator;
using Org::Xml::Sax::IContentHandler;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * Inserts default data from InputStream, should be in XML format.
 * If the provider syncs data to the server, the imported data will be synced to the server.
 * <p>Samples:</p>
 * <br/>
 *  Insert one row:
 * <pre>
 * &lt;row uri="content://contacts/people">
 *  &lt;Col column = "name" value = "foo feebe "/>
 *  &lt;Col column = "addr" value = "Tx"/>
 * &lt;/row></pre>
 * <br/>
 * Delete, it must be in order of uri, select, and arg:
 * <pre>
 * &lt;del uri="content://contacts/people" select="name=? and addr=?"
 *  arg1 = "foo feebe" arg2 ="Tx"/></pre>
 * <br/>
 *  Use first row's uri to insert into another table,
 *  content://contacts/people/1/phones:
 * <pre>
 * &lt;row uri="content://contacts/people">
 *  &lt;col column = "name" value = "foo feebe"/>
 *  &lt;col column = "addr" value = "Tx"/>
 *  &lt;row postfix="phones">
 *    &lt;col column="number" value="512-514-6535"/>
 *  &lt;/row>
 *  &lt;row postfix="phones">
 *    &lt;col column="cell" value="512-514-6535"/>
 *  &lt;/row>
 * &lt;/row></pre>
 * <br/>
 *  Insert multiple rows in to same table and same attributes:
 * <pre>
 * &lt;row uri="content://contacts/people" >
 *  &lt;row>
 *   &lt;col column= "name" value = "foo feebe"/>
 *   &lt;col column= "addr" value = "Tx"/>
 *  &lt;/row>
 *  &lt;row>
 *  &lt;/row>
 * &lt;/row></pre>
 *
 * @hide
 */
CarClass(CDefaultDataHandler)
    , public Object
    , public IContentInsertHandler
    , public IContentHandler
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDefaultDataHandler();

    virtual ~CDefaultDataHandler();

    CARAPI SetDocumentLocator(
        /* [in] */ ILocator* locator);

    CARAPI StartDocument();

    CARAPI EndDocument();

    CARAPI StartPrefixMapping(
        /* [in] */ const String& prefix,
        /* [in] */ const String& uri);

    CARAPI EndPrefixMapping(
        /* [in] */ const String& prefix);

    CARAPI StartElement(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [in] */ const String& qName,
        /* [in] */ IAttributes* atts);

    CARAPI EndElement(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [in] */ const String& qName);

    CARAPI Characters(
        /* [in] */ ArrayOf<Char32>* ch,
        /* [in] */ Int32 start,
        /* [in] */ Int32 length);

    CARAPI IgnorableWhitespace(
        /* [in] */ ArrayOf<Char32>* ch,
        /* [in] */ Int32 start,
        /* [in] */ Int32 length);

    CARAPI ProcessingInstruction(
        /* [in] */ const String& target,
        /* [in] */ const String& data);

    CARAPI SkippedEntity(
        /* [in] */ const String& name);

    CARAPI Insert(
        /* [in] */ IContentResolver* contentResolver,
        /* [in] */ IInputStream* inStream);

    CARAPI Insert(
        /* [in] */ IContentResolver* contentResolver,
        /* [in] */ const String& inStr);

private:
    CARAPI ParseRow(
        /* [in] */ IAttributes* atts);

    CARAPI InsertRow(
        /* [out] */ IUri** uri);

private:
    static const String ROW;
    static const String COL;
    static const String URI_STR;
    static const String POSTFIX;
    static const String DEL;
    static const String SELECT;
    static const String ARG;

private:
    Stack< AutoPtr<IUri> > mUris;
    AutoPtr<IContentValues> mValues;
    AutoPtr<IContentResolver> mContentResolver;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CDEFAULTDATAHANDLER_H__
