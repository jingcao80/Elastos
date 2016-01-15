
#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/CDefaultDataHandler.h"
#include "elastos/droid/content/CContentValues.h"
//***#include "elastos/droid/net/StringUri.h"

using Elastos::Droid::Net::IUriBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Org::Xml::Sax::EIID_IContentHandler;

namespace Elastos {
namespace Droid {
namespace Content {

const String CDefaultDataHandler::ROW("row");
const String CDefaultDataHandler::COL("col");
const String CDefaultDataHandler::URI_STR("uri");
const String CDefaultDataHandler::POSTFIX("postfix");
const String CDefaultDataHandler::DEL("del");
const String CDefaultDataHandler::SELECT("select");
const String CDefaultDataHandler::ARG("arg");

CAR_INTERFACE_IMPL_2(CDefaultDataHandler, Object, IContentInsertHandler, IContentHandler)

CAR_OBJECT_IMPL(CDefaultDataHandler)

CDefaultDataHandler::CDefaultDataHandler()
{
}

CDefaultDataHandler::~CDefaultDataHandler()
{
}

ECode CDefaultDataHandler::SetDocumentLocator(
    /* [in] */ ILocator* locator)
{
    // TODO Auto-generated method stub
    return NOERROR;
}

ECode CDefaultDataHandler::StartDocument()
{
    // TODO Auto-generated method stub
    return NOERROR;
}

ECode CDefaultDataHandler::EndDocument()
{
    // TODO Auto-generated method stub
    return NOERROR;
}

ECode CDefaultDataHandler::StartPrefixMapping(
    /* [in] */ const String& prefix,
    /* [in] */ const String& uri)
{
    // TODO Auto-generated method stub
    return NOERROR;
}

ECode CDefaultDataHandler::EndPrefixMapping(
    /* [in] */ const String& prefix)
{
    // TODO Auto-generated method stub
    return NOERROR;
}

ECode CDefaultDataHandler::StartElement(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName,
    /* [in] */ IAttributes* atts)
{
    VALIDATE_NOT_NULL(atts)

    if (ROW.Equals(localName)) {
        if (NULL != mValues) {
            // case 2, <Col> before <Row> insert last uri
            if (mUris.IsEmpty()) {
                //throw new SAXException("uri is empty");
                return E_SAX_EXCEPTION;
            }
            AutoPtr<IUri> nextUri;
            FAIL_RETURN(InsertRow((IUri**)&nextUri))
            if (NULL == nextUri) {
                //throw new SAXException("insert to uri " + mUris.lastElement().toString() + " failure");
                return E_SAX_EXCEPTION;
            }
            else {
                // make sure the stack lastElement save uri for more than one row
                mUris.Pop();
                mUris.Push(nextUri);
                FAIL_RETURN(ParseRow(atts))
            }
        }
        else {
            Int32 attrLen = 0;
            FAIL_RETURN(atts->GetLength(&attrLen))
            if (attrLen == 0) {
                // case 3, share same uri as last level
                AutoPtr<IUri> lastUri = mUris.GetTop(); // mUris.lastElement()
                mUris.Push(lastUri);
            }
            else {
                FAIL_RETURN(ParseRow(atts))
            }
        }
    }
    else if (COL.Equals(localName)) {
        Int32 attrLen = 0;
        FAIL_RETURN(atts->GetLength(&attrLen))
        if (attrLen != 2) {
            //throw new SAXException("illegal attributes number " + attrLen);
            return E_SAX_EXCEPTION;
        }
        String key;
        String value;
        FAIL_RETURN(atts->GetValue(0, &key))
        FAIL_RETURN(atts->GetValue(1, &value))
        if (!key.IsNullOrEmpty() && !value.IsNullOrEmpty()) {
            if (NULL == mValues) {
                FAIL_RETURN(CContentValues::New((IContentValues**)&mValues))
            }
            FAIL_RETURN(mValues->Put(key, value))
        }
        else {
            //throw new SAXException("illegal attributes value");
            return E_SAX_EXCEPTION;
        }
    }
    else if (DEL.Equals(localName)) {
        String uriString;
        AutoPtr<IUri> u;
        FAIL_RETURN(atts->GetValue(URI_STR, &uriString))
//***        FAIL_RETURN(StringUri::New(uriString, (IUri**)&u))

        if (NULL == u) {
            //throw new SAXException("attribute " + atts.getValue(URI_STR) + " parsing failure");
            return E_SAX_EXCEPTION;
        }

        Int32 attrLen = 0;
        FAIL_RETURN(atts->GetLength(&attrLen))
        attrLen -= 2;
        String tmpStr;
        Int32 rowsAffected = 0;
        if (attrLen > 0) {
            AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(attrLen);

            for (Int32 i = 0; i < attrLen; i++) {
                FAIL_RETURN(atts->GetValue(i+2, &(*selectionArgs)[i]))
            }
            FAIL_RETURN(atts->GetValue(1, &tmpStr))
            FAIL_RETURN(mContentResolver->Delete(u, tmpStr, selectionArgs, &rowsAffected))
        }
        else if (attrLen == 0) {
            FAIL_RETURN(atts->GetValue(1, &tmpStr))
            FAIL_RETURN(mContentResolver->Delete(u, tmpStr, NULL, &rowsAffected))
        }
        else {
            FAIL_RETURN(mContentResolver->Delete(u, tmpStr, NULL, &rowsAffected))
        }
    }
    else {
        //throw new SAXException("unknown element: " + localName);
        return E_SAX_EXCEPTION;
    }
    return NOERROR;
}

ECode CDefaultDataHandler::EndElement(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& name)
{
    if (ROW.Equals(localName)) {
        if (mUris.IsEmpty()) {
            //throw new SAXException("uri mismatch");
            return E_SAX_EXCEPTION;
        }
        if (NULL != mValues) {
            AutoPtr<IUri> tmp;
            FAIL_RETURN(InsertRow((IUri**)&tmp))
        }
        mUris.Pop();
    }
    return NOERROR;
}

ECode CDefaultDataHandler::Characters(
    /* [out] */ ArrayOf<Char32>* ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    // TODO Auto-generated method stub
    return NOERROR;
}

ECode CDefaultDataHandler::IgnorableWhitespace(
    /* [out] */ ArrayOf<Char32>* ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    // TODO Auto-generated method stub
    return NOERROR;
}

ECode CDefaultDataHandler::ProcessingInstruction(
    /* [in] */ const String& target,
    /* [in] */ const String& data)
{
    // TODO Auto-generated method stub
    return NOERROR;
}

ECode CDefaultDataHandler::SkippedEntity(
    /* [in] */ const String& name)
{
    // TODO Auto-generated method stub
    return NOERROR;
}

ECode CDefaultDataHandler::Insert(
    /* [in] */ IContentResolver* contentResolver,
    /* [in] */ IInputStream* inStream)
{
    mContentResolver = contentResolver;
//    Xml.parse(in, Xml.Encoding.UTF_8, this);
    return E_NOT_IMPLEMENTED;
}

ECode CDefaultDataHandler::Insert(
    /* [in] */ IContentResolver* contentResolver,
    /* [in] */ const String& inStr)
{
    mContentResolver = contentResolver;
//    Xml.parse(in, this);
    return E_NOT_IMPLEMENTED;
}

ECode CDefaultDataHandler::ParseRow(
    /* [in] */ IAttributes* atts)
{
    assert(0 && "TODO");
    String uriStr;
    AutoPtr<IUri> uri;
    FAIL_RETURN(atts->GetValue(URI_STR, &uriStr))

    if (!uriStr.IsNull()) {
        // case 1
//***        FAIL_RETURN(StringUri::New(uriStr, (IUri**)&uri))
        if (NULL == uri) {
            //throw new SAXException("attribute " + atts.getValue(URI_STR) + " parsing failure");
            return E_SAX_EXCEPTION;
        }
    }
    else if (mUris.IsEmpty() == FALSE) {
        // case 2
        String postfix;
        FAIL_RETURN(atts->GetValue(POSTFIX, &postfix))
        if (!postfix.IsNull()) {
            AutoPtr<IUri> lastUri = mUris.GetTop();
            AutoPtr<IUriBuilder> builder;
            FAIL_RETURN(lastUri->BuildUpon((IUriBuilder**)&builder))
            FAIL_RETURN(builder->AppendEncodedPath(postfix))
            FAIL_RETURN(builder->Build((IUri**)&uri))
        }
        else {
            // uri = mUris.At(mUris.GetSize()-1);
        }
    }
    else {
        //throw new SAXException("attribute parsing failure");
        return E_SAX_EXCEPTION;
    }

    mUris.Push(uri);
    return NOERROR;
}

ECode CDefaultDataHandler::InsertRow(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    AutoPtr<IUri> lastUri = mUris.GetTop();
    FAIL_RETURN(mContentResolver->Insert(lastUri, mValues, uri))
    mValues = NULL;
    return NOERROR;
}

}
}
}

