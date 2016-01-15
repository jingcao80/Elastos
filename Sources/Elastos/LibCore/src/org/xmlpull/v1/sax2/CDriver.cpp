
#include "Elastos.CoreLibrary.IO.h"
#include "sax2/CDriver.h"
#include <elastos/core/StringBuilder.h>
#include "XmlPullParserFactory.h"
#include "CInputSource.h"
#include "CDefaultHandler.h"
#include "CURL.h"

using Elastos::Core::StringBuilder;
using Elastos::IO::IInputStream;
using Elastos::IO::IReader;
using Elastos::Net::IURL;
using Elastos::Net::CURL;
using Org::Xml::Sax::EIID_IXMLReader;
using Org::Xml::Sax::EIID_IContentHandler;
using Org::Xml::Sax::EIID_IErrorHandler;
using Org::Xml::Sax::EIID_ILocator;
using Org::Xml::Sax::EIID_IAttributes;
using Org::Xml::Sax::CInputSource;
using Org::Xml::Sax::Helpers::CDefaultHandler;

namespace Org {
namespace Xmlpull {
namespace V1 {
namespace Sax2 {

const String CDriver::DECLARATION_HANDLER_PROPERTY("http://xml.org/sax/properties/declaration-handler");
const String CDriver::LEXICAL_HANDLER_PROPERTY("http://xml.org/sax/properties/lexical-handler");
const String CDriver::NAMESPACES_FEATURE("http://xml.org/sax/features/namespaces");
const String CDriver::NAMESPACE_PREFIXES_FEATURE("http://xml.org/sax/features/namespace-prefixes");
const String CDriver::VALIDATION_FEATURE("http://xml.org/sax/features/validation");
const String CDriver::APACHE_SCHEMA_VALIDATION_FEATURE("http://apache.org/xml/features/validation/schema");
const String CDriver::APACHE_DYNAMIC_VALIDATION_FEATURE("http://apache.org/xml/features/validation/dynamic");

CAR_INTERFACE_IMPL_3(CDriver, Object, ILocator, IXMLReader, IAttributes)

CAR_OBJECT_IMPL(CDriver)

ECode CDriver::constructor()
{
    AutoPtr<IXmlPullParserFactory> factory;
    FAIL_RETURN(XmlPullParserFactory::NewInstance((IXmlPullParserFactory**)&factory));
    factory->SetNamespaceAware(TRUE);
    factory->NewPullParser((IXmlPullParser**)&mPp);

    AutoPtr<IEntityResolver> er;
    FAIL_RETURN(CDefaultHandler::New((IEntityResolver**)&er));
    mContentHandler = (IContentHandler*)er->Probe(EIID_IContentHandler);

    er = NULL;
    FAIL_RETURN(CDefaultHandler::New((IEntityResolver**)&er));
    mErrorHandler = (IErrorHandler*)er->Probe(EIID_IErrorHandler);
    return NOERROR;
}

ECode CDriver::constructor(
    /* [in] */ IXmlPullParser* pp)
{
    mPp = pp;
    AutoPtr<IEntityResolver> er;
    FAIL_RETURN(CDefaultHandler::New((IEntityResolver**)&er));
    mContentHandler = (IContentHandler*)er->Probe(EIID_IContentHandler);

    er = NULL;
    FAIL_RETURN(CDefaultHandler::New((IEntityResolver**)&er));
    mErrorHandler = (IErrorHandler*)er->Probe(EIID_IErrorHandler);
    return NOERROR;
}

ECode CDriver::GetPublicId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id)

    *id = String(NULL);
    return NOERROR;
}

ECode CDriver::GetSystemId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id)

    *id = mSystemId;
    return NOERROR;
}

ECode CDriver::GetLineNumber(
    /* [out] */ Int32* lineNumber)
{
    VALIDATE_NOT_NULL(lineNumber)

    mPp->GetLineNumber(lineNumber);
    return NOERROR;
}

ECode CDriver::GetColumnNumber(
    /* [out] */ Int32* columnNumber)
{
    VALIDATE_NOT_NULL(columnNumber)

    mPp->GetColumnNumber(columnNumber);
    return NOERROR;
}

ECode CDriver::GetFeature(
    /* [in] */ const String& name,
    /* [out] */ Boolean* feature)
{
    VALIDATE_NOT_NULL(feature)

    if(NAMESPACES_FEATURE.Equals(name)) {
        return mPp->GetFeature(IXmlPullParser::FEATURE_PROCESS_NAMESPACES, feature);
    }
    else if(NAMESPACE_PREFIXES_FEATURE.Equals(name)) {
        return mPp->GetFeature(IXmlPullParser::FEATURE_REPORT_NAMESPACE_ATTRIBUTES, feature);
    }
    else if(VALIDATION_FEATURE.Equals(name)) {
        return mPp->GetFeature(IXmlPullParser::FEATURE_VALIDATION, feature);
        //        } else if(APACHE_SCHEMA_VALIDATION_FEATURE.equals(name)) {
        //            return false;  //TODO
        //        } else if(APACHE_DYNAMIC_VALIDATION_FEATURE.equals(name)) {
        //            return false; //TODO
    }
    else {
        return mPp->GetFeature(name, feature);
        //throw new SAXNotRecognizedException("unrecognized feature "+name);
    }
    return NOERROR;
}

ECode CDriver::SetFeature(
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
    // try {
    if(NAMESPACES_FEATURE.Equals(name)) {
        mPp->SetFeature(IXmlPullParser::FEATURE_PROCESS_NAMESPACES, value);
    }
    else if(NAMESPACE_PREFIXES_FEATURE.Equals(name)) {
        Boolean flag = FALSE;
        mPp->GetFeature(IXmlPullParser::FEATURE_REPORT_NAMESPACE_ATTRIBUTES, &flag);
        if(flag != value) {
            mPp->SetFeature(IXmlPullParser::FEATURE_REPORT_NAMESPACE_ATTRIBUTES, value);
        }
    }
    else if(VALIDATION_FEATURE.Equals(name)) {
        mPp->SetFeature(IXmlPullParser::FEATURE_VALIDATION, value);
        //          } else if(APACHE_SCHEMA_VALIDATION_FEATURE.equals(name)) {
        //              // can ignore as validation must be false ...
        //              //              if(true == value) {
        //              //                  throw new SAXNotSupportedException("schema validation is not supported");
        //              //              }
        //          } else if(APACHE_DYNAMIC_VALIDATION_FEATURE.equals(name)) {
        //              if(true == value) {
        //                  throw new SAXNotSupportedException("dynamic validation is not supported");
        //              }
    }
    else {
        mPp->SetFeature(name, value);
        //throw new SAXNotRecognizedException("unrecognized feature "+name);
    }
    // } catch(XmlPullParserException ex) {
    //    // throw new SAXNotSupportedException("problem with setting feature "+name+": "+ex);
    // }
    return NOERROR;
}

ECode CDriver::GetProperty(
    /* [in] */ const String& name,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)

    if(DECLARATION_HANDLER_PROPERTY.Equals(name)) {
        *value = NULL;
    }
    else if(LEXICAL_HANDLER_PROPERTY.Equals(name)) {
        *value = NULL;
    }
    else {
        return mPp->GetProperty(name, value);
        //throw new SAXNotRecognizedException("not recognized get property "+name);
    }
    return NOERROR;
}

ECode CDriver::SetProperty(
    /* [in] */ const String& name,
    /* [in] */ IInterface* value)
{
    if(DECLARATION_HANDLER_PROPERTY.Equals(name)) {
        return E_SAX_NOT_SUPPORTED_EXCEPTION;
        // throw new SAXNotSupportedException("not supported setting property "+name);//+" to "+value);
    }
    else if(LEXICAL_HANDLER_PROPERTY.Equals(name)) {
        return E_SAX_NOT_SUPPORTED_EXCEPTION;
        // throw new SAXNotSupportedException("not supported setting property "+name);//+" to "+value);
    }
    else {
        // try {
        mPp->SetProperty(name, value);
        // } catch(XmlPullParserException ex) {
        //     throw new SAXNotSupportedException("not supported set property "+name+": "+ ex);
        // }
        // //throw new SAXNotRecognizedException("not recognized set property "+name);
    }
    return NOERROR;
}

ECode CDriver::SetEntityResolver(
    /* [in] */ IEntityResolver* resolver)
{
    return NOERROR;
}

ECode CDriver::GetEntityResolver(
    /* [out] */ IEntityResolver** resolver)
{
    VALIDATE_NOT_NULL(resolver)

    *resolver = NULL;
    return NOERROR;
}

ECode CDriver::SetDTDHandler(
    /* [in] */ IDTDHandler* handler)
{
    return NOERROR;
}

ECode CDriver::GetDTDHandler(
    /* [out] */ IDTDHandler** handler)
{
    VALIDATE_NOT_NULL(handler)

    *handler = NULL;
    return NOERROR;
}

ECode CDriver::SetContentHandler(
    /* [in] */ IContentHandler* handler)
{
    mContentHandler = handler;
    return NOERROR;
}

ECode CDriver::GetContentHandler(
    /* [out] */ IContentHandler** handler)
{
    VALIDATE_NOT_NULL(handler)

    *handler = mContentHandler;
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode CDriver::SetErrorHandler(
    /* [in] */ IErrorHandler* handler)
{
    mErrorHandler = handler;
    return NOERROR;
}

ECode CDriver::GetErrorHandler(
    /* [out] */ IErrorHandler** handler)
{
    VALIDATE_NOT_NULL(handler)

    *handler = mErrorHandler;
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode CDriver::Parse(
    /* [in] */ IInputSource* source)
{
    source->GetSystemId(&mSystemId);
    mContentHandler->SetDocumentLocator((ILocator*)this->Probe(EIID_ILocator));

    AutoPtr<IReader> reader;
    source->GetCharacterStream((IReader**)&reader);
    // try {
    if (reader == NULL) {
        AutoPtr<IInputStream> stream;
        source->GetByteStream((IInputStream**)&stream);
        String encoding;
        source->GetEncoding(&encoding);

        if (stream == NULL) {
            source->GetSystemId(&mSystemId);
            if(mSystemId.IsNull()) {
                // SAXParseException saxException = new SAXParseException("null source systemId" , this);
                mErrorHandler->FatalError(E_SAX_PARSE_EXCEPTION);
                return E_SAX_PARSE_EXCEPTION;
            }
            // NOTE: replace with Connection to run in J2ME environment
            // try {
            AutoPtr<IURL> url;
            CURL::New(mSystemId, (IURL**)&url);
            url->OpenStream((IInputStream**)&stream);
            // } catch (MalformedURLException nue) {
            //     try {
            //         stream = new FileInputStream(systemId);
            //     } catch (FileNotFoundException fnfe) {
            //         final SAXParseException saxException = new SAXParseException(
            //             "could not open file with systemId "+systemId, this, fnfe);
            //         errorHandler.fatalError(saxException);
            //         return;
            //     }
            // }
        }
        mPp->SetInput(stream, encoding);
    }
    else {
        mPp->SetInput(reader);
    }
    // } catch (XmlPullParserException ex)  {
    //     final SAXParseException saxException = new SAXParseException(
    //         "parsing initialization error: "+ex, this, ex);
    //     //if(DEBUG) ex.printStackTrace();
    //     errorHandler.fatalError(saxException);
    //     return;
    // }

    // start parsing - move to first start tag
    // try {
    mContentHandler->StartDocument();
    // get first event
    Int32 type = 0;
    mPp->Next(&type);
    // it should be start tag...
    type = 0;
    mPp->GetEventType(&type);
    if(type != IXmlPullParser::START_TAG) {
        // final SAXParseException saxException = new SAXParseException("expected start tag not"+pp.getPositionDescription(), this);
        //throw saxException;
        mErrorHandler->FatalError(E_SAX_PARSE_EXCEPTION);
        return E_SAX_PARSE_EXCEPTION;
    }
    // } catch (XmlPullParserException ex)  {
    //     final SAXParseException saxException = new SAXParseException(
    //         "parsing initialization error: "+ex, this, ex);
    //     //ex.printStackTrace();
    //     errorHandler.fatalError(saxException);
    //     return;
    // }

    // now real parsing can start!

    ParseSubTree(mPp);

    // and finished ...

    mContentHandler->EndDocument();
    return NOERROR;
}

ECode CDriver::Parse(
    /* [in] */ const String& systemId)
{
    AutoPtr<IInputSource> is;
    FAIL_RETURN(CInputSource::New(mSystemId, (IInputSource**)&is));
    return Parse(is);
}

ECode CDriver::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)

    return mPp->GetAttributeCount(length);
}

ECode CDriver::GetURI(
    /* [in] */ Int32 index,
    /* [out] */ String* URI)
{
    VALIDATE_NOT_NULL(URI)

    return mPp->GetAttributeNamespace(index, URI);
}

ECode CDriver::GetLocalName(
    /* [in] */ Int32 index,
    /* [out] */ String* localName)
{
    VALIDATE_NOT_NULL(localName)

    return mPp->GetAttributeName(index, localName);
}

ECode CDriver::GetQName(
    /* [in] */ Int32 index,
    /* [out] */ String* qName)
{
    VALIDATE_NOT_NULL(qName)

    String prefix;
    mPp->GetAttributePrefix(index, &prefix);
    if(!prefix.IsNull()) {
        String str;
        mPp->GetAttributeName(index, &str);
        *qName = prefix + String(":") + str;
    }
    else {
        return mPp->GetAttributeName(index, qName);
    }
    return NOERROR;
}

ECode CDriver::GetType(
    /* [in] */ Int32 index,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type)

    return mPp->GetAttributeType(index, type);
}

ECode CDriver::GetValue(
    /* [in] */ Int32 index,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)

    return mPp->GetAttributeValue(index, value);
}

ECode CDriver::GetIndex(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    Int32 num = 0;
    mPp->GetAttributeCount(&num);
    for (Int32 i = 0; i < num; i++) {
        String strspace;
        mPp->GetAttributeNamespace(i, &strspace);
        String strname;
        mPp->GetAttributeName(i, &strname);
        if(strspace.Equals(uri) && strname.Equals(localName)) {
            *index = i;
            return NOERROR;
        }

    }
    *index = -1;
    return NOERROR;
}

ECode CDriver::GetIndex(
    /* [in] */ const String& qName,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    Int32 num = 0;
    mPp->GetAttributeCount(&num);
    for (Int32 i = 0; i < num; i++) {
        String strname;
        mPp->GetAttributeName(i, &strname);
        if(strname.Equals(qName)) {
            *index = i;
            return NOERROR;
        }

    }
    *index = -1;
    return NOERROR;
}

ECode CDriver::GetType(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type)

    Int32 num = 0;
    mPp->GetAttributeCount(&num);
    for (Int32 i = 0; i < num; i++) {
        String strspace;
        mPp->GetAttributeNamespace(i, &strspace);
        String strname;
        mPp->GetAttributeName(i, &strname);
        if(strspace.Equals(uri) && strname.Equals(localName)) {
            return mPp->GetAttributeType(i, type);
        }

    }
    *type = String(NULL);
    return NOERROR;
}

ECode CDriver::GetType(
    /* [in] */ const String& qName,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type)

    Int32 num = 0;
    mPp->GetAttributeCount(&num);
    for (Int32 i = 0; i < num; i++) {
        String strname;
        mPp->GetAttributeName(i, &strname);
        if(strname.Equals(qName)) {
            return mPp->GetAttributeType(i, type);
        }

    }
    *type = String(NULL);
    return NOERROR;
}

ECode CDriver::GetValue(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)

    return mPp->GetAttributeValue(uri, localName, value);
}

ECode CDriver::GetValue(
    /* [in] */ const String& qName,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)

    return mPp->GetAttributeValue(String(NULL), qName, value);
}

void CDriver::ParseSubTree(
        /* [in] */ IXmlPullParser* pp)
{
    mPp = pp;
    Boolean namespaceAware = FALSE;
    mPp->GetFeature(IXmlPullParser::FEATURE_PROCESS_NAMESPACES, &namespaceAware);
    // try {
    Int32 num = 0;
    mPp->GetEventType(&num);
    if(num != IXmlPullParser::START_TAG) {
        // throw new SAXException("start tag must be read before skiping subtree"+pp.getPositionDescription());
        return;
    }
    AutoPtr< ArrayOf<Int32> > holderForStartAndLength = ArrayOf<Int32>::Alloc(2);
    StringBuilder rawName;
    String prefix(NULL);
    String name(NULL);
    Int32 level = 0;
    mPp->GetDepth(&level);
    level -= 1;
    Int32 type = IXmlPullParser::START_TAG;
    Int32 depthlen = 0;

    // LOOP:
    do {
        switch(type) {
            case IXmlPullParser::START_TAG:
                if(namespaceAware) {
                    Int32 depth = 0;
                    mPp->GetDepth(&depth);
                    depth -= 1;
                    Int32 spacecount = 0;
                    mPp->GetNamespaceCount(depth, &spacecount);
                    Int32 countPrev = (level > depth) ? spacecount : 0;
                    //int countPrev = pp.getNamespaceCount(pp.getDepth() - 1);
                    Int32 count = 0;
                    mPp->GetNamespaceCount(depth + 1, &count);
                    for (Int32 i = countPrev; i < count; i++) {
                        String spaceprefix;
                        mPp->GetNamespacePrefix(i, &spaceprefix);
                        String spaceuri;
                        mPp->GetNamespaceUri(i, &spaceuri);
                        mContentHandler->StartPrefixMapping(spaceprefix, spaceuri);
                    }
                    mPp->GetName(&name);
                    mPp->GetPrefix(&prefix);
                    if(!prefix.IsNull()) {
                        // rawName.setLength(0);
                        rawName.Reset();
                        rawName.Append(prefix);
                        rawName.AppendChar(':');
                        rawName.Append(name);
                    }
                    String strspace;
                    mPp->GetNamespace(&strspace);
                    StartElement(strspace,
                                 name,
                                 // TODO Fixed this. Was "not equals".
                                 prefix.IsNull() ? name : rawName.ToString());
                }
                else {
                    String strspace;
                    mPp->GetNamespace(&strspace);
                    String strname;
                    mPp->GetName(&strname);
                    StartElement(strspace, strname, strname);
                }
                //++level;

                break;
            case IXmlPullParser::TEXT:
                {
                    AutoPtr< ArrayOf<Char32> > chars;
                    mPp->GetTextCharacters(holderForStartAndLength, (ArrayOf<Char32>**)&chars);
                    mContentHandler->Characters(chars,
                                                (*holderForStartAndLength)[0], //start
                                                (*holderForStartAndLength)[1] //len
                                                );
                }
                break;
            case IXmlPullParser::END_TAG:
                //--level;
                if(namespaceAware) {
                    mPp->GetName(&name);
                    mPp->GetPrefix(&prefix);
                    if(!prefix.IsNull()) {
                        // rawName.setLength(0);
                        rawName.Reset();
                        rawName.Append(prefix);
                        rawName.AppendChar(':');
                        rawName.Append(name);
                    }
                    String strspace;
                    mPp->GetNamespace(&strspace);
                    mContentHandler->EndElement(strspace,
                                                name,
                                                !prefix.IsNull() ? name : rawName.ToString()
                                                );
                    // when entering show prefixes for all levels!!!!
                    Int32 depth = 0;
                    mPp->GetDepth(&depth);
                    Int32 spacecount = 0;
                    mPp->GetNamespaceCount(depth, &spacecount);
                    Int32 countPrev = (level > depth) ? spacecount : 0;
                    Int32 count = 0;
                    mPp->GetNamespaceCount(depth - 1, &count);
                    // undeclare them in reverse order
                    for (Int32 i = count - 1; i >= countPrev; i--) {
                        String strpre;
                        mPp->GetNamespacePrefix(i, &strpre);
                        mContentHandler->EndPrefixMapping(strpre);
                    }
                }
                else {
                    String strspace;
                    mPp->GetNamespace(&strspace);
                    String strname;
                    mPp->GetName(&strname);
                    mContentHandler->EndElement(strspace, strname, strname);
                }
                break;
            case IXmlPullParser::END_DOCUMENT:
                goto LOOP;
        }
        mPp->Next(&type);
        mPp->GetDepth(&depthlen);
    } while(depthlen > level);
    // } catch (XmlPullParserException ex)  {
    //     final SAXParseException saxException = new SAXParseException("parsing error: "+ex, this, ex);
    //     ex.printStackTrace();
    //     errorHandler.fatalError(saxException);
    // }
    LOOP:
    ;
}

void CDriver::StartElement(
        /* [in] */ const String& namesp,
        /* [in] */ const String& localName,
        /* [in] */ const String& qName)
{
    mContentHandler->StartElement(namesp, localName, qName, (IAttributes*)this->Probe(EIID_IAttributes));
}

} // namespace Sax2
} // namespace V1
} // namespace Xmlpull
} // namespace Org
