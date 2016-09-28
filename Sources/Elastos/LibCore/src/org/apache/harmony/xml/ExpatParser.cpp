#include "Elastos.CoreLibrary.External.h"
#include "org/apache/harmony/xml/ExpatParser.h"
#include "elastos/net/CURI.h"
#include "elastos/net/CURL.h"
#include "libcore/io/IoUtils.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/core/StringBuilder.h"
#include "elastos/core/AutoLock.h"
#include "Elastos.CoreLibrary.IO.h"

//import java.io.IOException;
using Elastos::IO::IInputStream;
using Elastos::IO::IReader;
using Elastos::IO::ICloseable;
using Elastos::Net::IURI;
using Elastos::Net::CURI;
using Elastos::Net::IURL;
using Elastos::Net::CURL;
using Elastos::Net::IURLConnection;
using Libcore::IO::IoUtils;
//import org.xml.sax.Attributes;
using Org::Xml::Sax::IContentHandler;
using Org::Xml::Sax::IDTDHandler;
using Org::Xml::Sax::IEntityResolver;
//import org.xml.sax.InputSource;
//import org.xml.sax.Locator;
//import org.xml.sax.SAXException;
//import org.xml.sax.SAXParseException;
using Org::Xml::Sax::IXMLReader;
using Org::Xml::Sax::Ext::ILexicalHandler;
using Org::Xml::Sax::EIID_ILocator;
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {

//*****ExpatParser::CloneAttributes***********

AutoPtr<IAttributes> ExpatParser::ClonedAttributes::EMPTY = new ClonedAttributes(0, 0, 0);

ExpatParser::ClonedAttributes::ClonedAttributes(
    /* [in] */ Int64 parserPointer,
    /* [in] */ Int64 pointer,
    /* [in] */ Int32 length)
{
    this->parserPointer = parserPointer;
    this->pointer = pointer;
    this->length = length;
}

ExpatParser::ClonedAttributes::~ClonedAttributes()
{
    //try {
    if (pointer != 0) {
        FreeAttributes(pointer);
        pointer = 0;
    }
    //} finally {
    //    super.finalize();
    //}

}

ECode ExpatParser::ClonedAttributes::GetParserPointer(
    /* [out] */ Int64* pointer)
{
    VALIDATE_NOT_NULL(pointer);
    *pointer = this->parserPointer;
    return NOERROR;
}

ECode ExpatParser::ClonedAttributes::GetPointer(
    /* [out] */ Int64* _pointer)
{
    VALIDATE_NOT_NULL(_pointer);
    *_pointer = pointer;
    return NOERROR;
}

ECode ExpatParser::ClonedAttributes::GetLength(
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len);
    *len = length;
    return NOERROR;
}

//@Override protected synchronized void finalize() throws Throwable {
//            try {
//                if (pointer != 0) {
//                    freeAttributes(pointer);
//                    pointer = 0;
//                }
//            } finally {
//                super.finalize();
//            }
//        }
//}

//***ExpatParser::ExpatLocator

CAR_INTERFACE_IMPL(ExpatParser::ExpatLocator, Object, ILocator);
ExpatParser::ExpatLocator::ExpatLocator(
    /* [in] */ ExpatParser* host)
    : mHost(host)
{
}

ECode ExpatParser::ExpatLocator::GetPublicId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHost->publicId;
    return NOERROR;
}

ECode ExpatParser::ExpatLocator::GetSystemId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHost->systemId;
    return NOERROR;
}

ECode ExpatParser::ExpatLocator::GetLineNumber(
    /* [out] */ Int32* lineNumber)
{
    VALIDATE_NOT_NULL(lineNumber);
    *lineNumber = mHost->Line();
    return NOERROR;
}

ECode ExpatParser::ExpatLocator::GetColumnNumber(
    /* [out] */ Int32* columnNumber)
{
    VALIDATE_NOT_NULL(columnNumber);
    *columnNumber = mHost->Column();
    return NOERROR;
}

ECode ExpatParser::ExpatLocator::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder sb;
    sb.Append("Locator[publicId: ");
    sb.Append(mHost->publicId);
    sb.Append(", systemId: ");
    sb.Append(mHost->systemId);
    sb.Append(", line: ");
    Int32 lineNumber;
    GetLineNumber(&lineNumber);
    sb.Append(lineNumber);
    sb.Append(", column: ");
    Int32 columnNumber;
    GetColumnNumber(&columnNumber);
    sb.Append(columnNumber);
    sb.Append("]");
    *result = sb.ToString();
    return NOERROR;
}

//*****ExpatParser::CurrentAttributes
ExpatParser::CurrentAttributes::CurrentAttributes(
    /* [in] */ ExpatParser* host)
    : mHost(host)
{
}

ECode ExpatParser::CurrentAttributes::GetParserPointer(
    /* [out] */ Int64* pointer)
{
    VALIDATE_NOT_NULL(pointer);
    *pointer = mHost->pointer;
    return NOERROR;
}

ECode ExpatParser::CurrentAttributes::GetPointer(
    /* [out] */ Int64* pointer)
{
    VALIDATE_NOT_NULL(pointer);
    if (!mHost->inStartElement) {
        //throw new IllegalStateException(OUTSIDE_START_ELEMENT);
        assert(0);
        Logger::E("ExpatParser::CurrentAttributes" , OUTSIDE_START_ELEMENT);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *pointer = mHost->attributePointer;
    return NOERROR;
}

ECode ExpatParser::CurrentAttributes::GetLength(
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len);
    if (!mHost->inStartElement) {
        //throw new IllegalStateException(OUTSIDE_START_ELEMENT);
        assert(0);
        Logger::E("ExpatParser::CurrentAttributes" , OUTSIDE_START_ELEMENT);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *len = mHost->attributeCount;
    return NOERROR;
}

//private static class ParseException extends SAXParseException {
//
//        private ParseException(String message, Locator locator) {
//            super(makeMessage(message, locator), locator);
//        }
//
//        private static String makeMessage(String message, Locator locator) {
//            return makeMessage(message, locator.getLineNumber(),
//                    locator.getColumnNumber());
//        }
//
//        private static String makeMessage(
//                String message, int line, int column) {
//            return "At line " + line + ", column "
//                    + column + ": " + message;
//        }
//}

//*****ExpatParser::EntityParser

EntityParser::EntityParser(
    /* [in] */ const String& encoding,
    /* [in] */ IExpatReader* xmlReader,
    /* [in] */ Int64 pointer,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId)
    : ExpatParser(encoding, xmlReader, pointer, publicId, systemId)
    , depth(0)
{
}

void EntityParser::StartElement(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName,
    /* [in] */ Int64 attributePointer,
    /* [in] */ Int32 attributeCount)
{
    /*
     * Skip topmost element generated by our workaround in
     * {@link #handleExternalEntity}.
     */
    if (depth++ > 0) {
        ExpatParser::StartElement(uri, localName, qName, attributePointer, attributeCount);
    }
}

void EntityParser::EndElement(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName)
{
    if (--depth > 0) {
        ExpatParser::EndElement(uri, localName, qName);
    }
}

//@Override
//@SuppressWarnings("FinalizeDoesntCallSuperFinalize")
//protected synchronized void finalize() throws Throwable {
//    /*
//     * Don't release our native resources. We do so explicitly in
//     * {@link #handleExternalEntity} and we don't want to release the
//     * parsing context--our parent is using it.
//     */
//}


const String ExpatParser::CHARACTER_ENCODING("UTF-16");
const Int32 ExpatParser::BUFFER_SIZE;// = 8096; // in bytes
const String ExpatParser::OUTSIDE_START_ELEMENT("Attributes can only be used within the scope of startElement().");
const String ExpatParser::DEFAULT_ENCODING("UTF-8");
const Int32 ExpatParser::TIMEOUT;


//TODO Int32 ExpatParser::dummyCode = ExpatParser::StaticInitialize(String(""));

ExpatParser::ExpatParser(
    /* [in] */ const String& encoding,
    /* [in] */ IExpatReader* xmlReader,
    /* [in] */ Boolean processNamespaces,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId)
    : inStartElement(FALSE)
    , attributeCount(-1)
    , attributePointer(0)
{
    locator = new ExpatLocator(this);
    attributes = new CurrentAttributes(this);

    this->publicId = publicId;
    this->systemId = systemId;

    this->xmlReader = xmlReader;

    /*
     * TODO: Let Expat try to guess the encoding instead of defaulting.
     * Unfortunately, I don't know how to tell which encoding Expat picked,
     * so I won't know how to encode "<externalEntity>" below. The solution
     * I think is to fix Expat to not require the "<externalEntity>"
     * workaround.
     */
    this->encoding = encoding.IsNull() ? DEFAULT_ENCODING : encoding;
    this->pointer = Initialize(
        this->encoding,
        processNamespaces
    );
}

/**
 * Used by {@link EntityParser}.
 */
ExpatParser::ExpatParser(
    /* [in] */ const String& encoding,
    /* [in] */ IExpatReader* _xmlReader,
    /* [in] */ Int64 pointer,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId)
    : inStartElement(FALSE)
    , attributeCount(-1)
    , attributePointer(0)
{
    locator = new ExpatLocator(this);
    attributes = new CurrentAttributes(this);

    this->encoding = encoding;
    this->xmlReader = _xmlReader;
    this->pointer = pointer;
    this->systemId = systemId;
    this->publicId = publicId;
}
ExpatParser::~ExpatParser()
{
    AutoLock lock(mLock);// ??
    //try {
    if (this->pointer != 0) {
        ReleasePointer(this->pointer);
        this->pointer = 0;
    }
    //} finally {
    //    super.finalize();
    //}
}

Int64 ExpatParser::Initialize(
    /* [in] */ const String& encoding,
    /* [in] */ Boolean namespacesEnabled)
{
    assert(0);
    return 0;
}

void ExpatParser::StartElement(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName,
    /* [in] */ Int64 attributePointer,
    /* [in] */ Int32 attributeCount)
{
    AutoPtr<IContentHandler> contentHandler;
    IXMLReader::Probe(xmlReader)->GetContentHandler((IContentHandler**)&contentHandler);
    if (contentHandler == NULL) {
        return;
    }

    //try {
    inStartElement = TRUE;
    this->attributePointer = attributePointer;
    this->attributeCount = attributeCount;

    contentHandler->StartElement(uri, localName, qName, this->attributes);
    //} finally {
    inStartElement = FALSE;
    this->attributeCount = -1;
    this->attributePointer = 0;
    //}
}

void ExpatParser::EndElement(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName)
{
    AutoPtr<IContentHandler> contentHandler;
    IXMLReader::Probe(xmlReader)->GetContentHandler((IContentHandler**)&contentHandler);
    if (contentHandler != NULL) {
        contentHandler->EndElement(uri, localName, qName);
    }
}

void ExpatParser::Text(
    /* [in] */ ArrayOf<Char32>* text, // char[]
    /* [in] */ Int32 length)
{
    AutoPtr<IContentHandler> contentHandler;
    IXMLReader::Probe(xmlReader)->GetContentHandler((IContentHandler**)&contentHandler);
    if (contentHandler != NULL) {
        contentHandler->Characters(text, 0, length);
    }
}

void ExpatParser::Comment(
    /* [in] */ ArrayOf<Char32>* text, // char[]
    /* [in] */ Int32 length)
{
    AutoPtr<ILexicalHandler> lexicalHandler;
    xmlReader->GetLexicalHandler((ILexicalHandler**)&lexicalHandler);
    if (lexicalHandler != NULL) {
        lexicalHandler->Comment(text, 0, length);
    }
}

void ExpatParser::StartCdata()
{
    AutoPtr<ILexicalHandler> lexicalHandler;
    xmlReader->GetLexicalHandler((ILexicalHandler**)&lexicalHandler);
    if (lexicalHandler != NULL) {
        lexicalHandler->StartCDATA();
    }
}

void ExpatParser::EndCdata()
{
    AutoPtr<ILexicalHandler> lexicalHandler;
    xmlReader->GetLexicalHandler((ILexicalHandler**)&lexicalHandler);
    if (lexicalHandler != NULL) {
        lexicalHandler->EndCDATA();
    }
}

void ExpatParser::StartNamespace(
    /* [in] */ const String& prefix,
    /* [in] */ const String& uri)
{
    AutoPtr<IContentHandler> contentHandler;
    IXMLReader::Probe(xmlReader)->GetContentHandler((IContentHandler**)&contentHandler);
    if (contentHandler != NULL) {
        contentHandler->StartPrefixMapping(prefix, uri);
    }
}

void ExpatParser::EndNamespace(
    /* [in] */ const String& prefix)
{
    AutoPtr<IContentHandler> contentHandler;
    IXMLReader::Probe(xmlReader)->GetContentHandler((IContentHandler**)&contentHandler);
    if (contentHandler != NULL) {
        contentHandler->EndPrefixMapping(prefix);
    }
}

void ExpatParser::StartDtd(
    /* [in] */ const String& name,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId)
{
    AutoPtr<ILexicalHandler> lexicalHandler;
    xmlReader->GetLexicalHandler((ILexicalHandler**)&lexicalHandler);
    if (lexicalHandler != NULL) {
        lexicalHandler->StartDTD(name, publicId, systemId);
    }
}

void ExpatParser::EndDtd()
{
    AutoPtr<ILexicalHandler> lexicalHandler;
    xmlReader->GetLexicalHandler((ILexicalHandler**)&lexicalHandler);
    if (lexicalHandler != NULL) {
        lexicalHandler->EndDTD();
    }
}

void ExpatParser::ProcessingInstruction(
    /* [in] */ const String& target,
    /* [in] */ const String& data)
{
    AutoPtr<IContentHandler> contentHandler;
    IXMLReader::Probe(xmlReader)->GetContentHandler((IContentHandler**)&contentHandler);
    if (contentHandler != NULL) {
        contentHandler->ProcessingInstruction(target, data);
    }
}

void ExpatParser::NotationDecl(
    /* [in] */ const String& name,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId)
{
    AutoPtr<IDTDHandler> dtdHandler;
    IXMLReader::Probe(xmlReader)->GetDTDHandler((IDTDHandler**)&dtdHandler);;
    if (dtdHandler != NULL) {
        dtdHandler->NotationDecl(name, publicId, systemId);
    }
}

void ExpatParser::UnparsedEntityDecl(
    /* [in] */ const String& name,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId,
    /* [in] */ const String& notationName)
{
    AutoPtr<IDTDHandler> dtdHandler;
    IXMLReader::Probe(xmlReader)->GetDTDHandler((IDTDHandler**)&dtdHandler);;
    if (dtdHandler != NULL) {
        dtdHandler->UnparsedEntityDecl(name, publicId, systemId, notationName);
    }
}

void ExpatParser::HandleExternalEntity(
    /* [in] */ const String& context,
    /* [in] */ const String& _publicId,
    /* [in] */ const String& _systemId)
{
    String systemId = _systemId;
    String publicId = _publicId;
    AutoPtr<IEntityResolver> entityResolver;
    IXMLReader::Probe(xmlReader)->GetEntityResolver((IEntityResolver**)&entityResolver);
    if (entityResolver == NULL) {
        return;
    }

    /*
     * The spec. is terribly under-specified here. It says that if the
     * systemId is a URL, we should try to resolve it, but it doesn't
     * specify how to tell whether or not the systemId is a URL let alone
     * how to resolve it.
     *
     * Other implementations do various insane things. We try to keep it
     * simple: if the systemId parses as a URI and it's relative, we try to
     * resolve it against the parent document's systemId. If anything goes
     * wrong, we go with the original systemId. If crazybob had designed
     * the API, he would have left all resolving to the EntityResolver.
     */
    if (!this->systemId.IsNull()) {
        //try {
        AutoPtr<IURI> systemUri;
        CURI::New(systemId, (IURI**)&systemUri);
        Boolean isAbsolute, isOpaque;
        if (!(systemUri->IsAbsolute(&isAbsolute), isAbsolute) && !(systemUri->IsOpaque(&isOpaque), isOpaque)) {
            // It could be relative (or it may not be a URI at all!)
            AutoPtr<IURI> baseUri;
            CURI::New(this->systemId, (IURI**)&baseUri);
            AutoPtr<IURI> tmp;
            baseUri->Resolve(systemUri, (IURI**)&tmp);
            systemUri = tmp;

            // Replace systemId w/ resolved URI
            IObject::Probe(systemUri)->ToString(&systemId);
        }
        //} catch (Exception e) {
        //    System.logI("Could not resolve '" + systemId + "' relative to"
        //            + " '" + this.systemId + "' at " + locator, e);
        //}
    }

    AutoPtr<IInputSource> inputSource;
    entityResolver->ResolveEntity(publicId, systemId, (IInputSource**)&inputSource);
    if (inputSource == NULL) {
        /*
         * The spec. actually says that we should try to treat systemId
         * as a URL and download and parse its contents here, but an
         * entity resolver can easily accomplish the same by returning
         * new InputSource(systemId).
         *
         * Downloading external entities by default would result in several
         * unwanted DTD downloads, not to mention pose a security risk
         * when parsing untrusted XML -- see for example
         * http://archive.cert.uni-stuttgart.de/bugtraq/2002/10/msg00421.html --
         * so we just do nothing instead. This also enables the user to
         * opt out of entity parsing when using
         * {@link org.xml.sax.helpers.DefaultHandler}, something that
         * wouldn't be possible otherwise.
         */
        return;
    }

    String encoding = PickEncoding(inputSource);
    Int64 pointer = CreateEntityParser(this->pointer, context);
    //try {
    String publicId2;
    String systemId2;
    inputSource->GetPublicId(&publicId2);
    inputSource->GetSystemId(&systemId2);
    AutoPtr<EntityParser> entityParser = new EntityParser(encoding, xmlReader,
            pointer, publicId2, systemId);

    ParseExternalEntity(entityParser, inputSource);
    //} finally {
    ReleaseParser(pointer);
    //}
}

String ExpatParser::PickEncoding(
    /* [in] */ IInputSource* inputSource)
{
    AutoPtr<IReader> reader;
    inputSource->GetCharacterStream((IReader**)&reader);
    if (reader != NULL) {
        return CHARACTER_ENCODING;
    }

    String encoding;
    inputSource->GetEncoding(&encoding);
    if (encoding.IsNull())
        return DEFAULT_ENCODING;
    return encoding;
}

void ExpatParser::ParseExternalEntity(
    /* [in] */ ExpatParser* entityParser,
    /* [in] */ IInputSource* inputSource)
{
    /*
     * Expat complains if the external entity isn't wrapped with a root
     * element so we add one and ignore it later on during parsing.
     */

    // Try the character stream.
    AutoPtr<IReader> reader;
    inputSource->GetCharacterStream((IReader**)&reader);
    if (reader != NULL) {
        //try {
        entityParser->Append(String("<externalEntity>"));
        entityParser->ParseFragment(reader);
        entityParser->Append(String("</externalEntity>"));
        //} finally {
        IoUtils::CloseQuietly(ICloseable::Probe(reader));
        //}
        return;
    }

    // Try the byte stream.
    AutoPtr<IInputStream> in;
    inputSource->GetByteStream((IInputStream**)&in);
    if (in != NULL) {
        //try {
        entityParser->Append(String("<externalEntity>").GetBytes(/*entityParser.encoding*/));
        entityParser->ParseFragment(in);
        entityParser->Append(String("</externalEntity>").GetBytes(/*entityParser.encoding*/));
        //} finally {
        IoUtils::CloseQuietly(ICloseable::Probe(in));
        //}
        return;
    }

    // Make sure we use the user-provided systemId.
    String systemId;
    inputSource->GetSystemId(&systemId);
    if (systemId.IsNull()) {
        // TODO: We could just try our systemId here.
        //throw new ParseException("No input specified.", locator);
        Logger::E("ExpatParser", "No input specified.");
        assert(0);
        return;
    }

    // Try the system id.
    in = OpenUrl(systemId);
    //try {
    entityParser->Append(String("<externalEntity>").GetBytes(/*entityParser.encoding*/));
    entityParser->ParseFragment(in);
    entityParser->Append(String("</externalEntity>").GetBytes(/*entityParser.encoding*/));
    //} finally {
    IoUtils::CloseQuietly(ICloseable::Probe(in));
    //}
}

Int64 ExpatParser::CreateEntityParser(
    /* [in] */ Int64 parentPointer,
    /* [in] */ const String& context)
{
    assert(0);
    return 0;
}

void ExpatParser::Append(
    /* [in] */ const String& xml)
{
    //try {
    AppendString(this->pointer, xml, FALSE);
    //} catch (ExpatException e) {
    //    throw new ParseException(e.getMessage(), this.locator);
    //}
}

void ExpatParser::AppendString(
    /* [in] */ Int64 pointer,
    /* [in] */ const String& xml,
    /* [in] */ Boolean isFinal)
{
    assert(0);
    return;
}

void ExpatParser::Append(
    /* [in] */ ArrayOf<Char32>* xml, //char[]
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    //try {
    AppendChars(this->pointer, xml, offset, length);
    //} catch (ExpatException e) {
    //    throw new ParseException(e.getMessage(), this.locator);
    //}
}

void ExpatParser::AppendChars(
    /* [in] */ Int64 pointer,
    /* [in] */ ArrayOf<Char32>* xml, // char[]
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    assert(0);
    return;
}

void ExpatParser::Append(
    /* [in] */ ArrayOf<Byte>* xml)
{
    Append(xml, 0, xml->GetLength());
}

void ExpatParser::Append(
    /* [in] */ ArrayOf<Byte>* xml,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    //try {
    AppendBytes(this->pointer, xml, offset, length);
    //} catch (ExpatException e) {
    //    throw new ParseException(e.getMessage(), this.locator);
    //}
}

void ExpatParser::AppendBytes(
    /* [in] */ Int64 pointer,
    /* [in] */ ArrayOf<Byte>* xml,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    assert(0);
    return;
}

void ExpatParser::ParseDocument(
    /* [in] */ IInputStream* in)
{
    StartDocument();
    ParseFragment(in);
    Finish();
    EndDocument();
}

void ExpatParser::ParseDocument(
    /* [in] */ IReader* in)
{
    StartDocument();
    ParseFragment(in);
    Finish();
    EndDocument();
}

void ExpatParser::ParseFragment(
    /* [in] */ IReader* in)
{
    //char[] buffer = new char[BUFFER_SIZE / 2];
    AutoPtr<ArrayOf<Char32> > buffer = ArrayOf<Char32>::Alloc(BUFFER_SIZE / 2);
    Int32 length;
    while ((in->Read(buffer, &length), length) != -1) {
        //try {
            AppendChars(this->pointer, buffer, 0, length);
        //} catch (ExpatException e) {
        //    throw new ParseException(e.getMessage(), locator);
        //}
    }
}

void ExpatParser::ParseFragment(
    /* [in] */ IInputStream* in)
{
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(BUFFER_SIZE);
    Int32 length;
    while ((in->Read(buffer, &length), length) != -1) {
        //try {
        AppendBytes(this->pointer, buffer, 0, length);
        //} catch (ExpatException e) {
        //    throw new ParseException(e.getMessage(), this.locator);
        //}
    }
}

void ExpatParser::StartDocument()
{
    AutoPtr<IContentHandler> contentHandler;
    IXMLReader::Probe(xmlReader)->GetContentHandler((IContentHandler**)&contentHandler);
    if (contentHandler != NULL) {
        contentHandler->SetDocumentLocator(this->locator);
        contentHandler->StartDocument();
    }
}

void ExpatParser::EndDocument()
{
    AutoPtr<IContentHandler> contentHandler;
    IXMLReader::Probe(xmlReader)->GetContentHandler((IContentHandler**)&contentHandler);
    if (contentHandler != NULL) {
        contentHandler->EndDocument();
    }
}

void ExpatParser::Finish()
{
    //try {
    AppendString(this->pointer, String(""), TRUE);
    //} catch (ExpatException e) {
    //    throw new ParseException(e.getMessage(), this.locator);
    //}
}

//@Override protected synchronized void finalize() throws Throwable {
//    try {
//        if (this.pointer != 0) {
//            release(this.pointer);
//            this.pointer = 0;
//        }
//    } finally {
//        super.finalize();
//    }
//}

void ExpatParser::ReleasePointer(
    /* [in] */ Int64 pointer)
{
    assert(0);
}

void ExpatParser::ReleaseParser(
    /* [in] */ Int64 pointer)
{
    assert(0);
}

Int32 ExpatParser::StaticInitialize(
    /* [in] */ const String& emptyString)
{
    assert(0);
    return 0;
}

Int32 ExpatParser::Line()
{
    return Line(this->pointer);
}

Int32 ExpatParser::Line(
    /* [in] */ Int64 pointer)
{
    assert(0);
    return 0;
}

Int32 ExpatParser::Column()
{
    return Column(this->pointer);
}

Int32 ExpatParser::Column(
    /* [in] */ Int64 pointer)
{
    assert(0);
    return 0;
}

ECode ExpatParser::CloneAttributes(
    /* [out] */ IAttributes** result)
{
    VALIDATE_NOT_NULL(result);
    if (!inStartElement) {
        //throw new IllegalStateException(OUTSIDE_START_ELEMENT);
        Logger::E("ExpatParser", OUTSIDE_START_ELEMENT);
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (attributeCount == 0) {
        *result = ClonedAttributes::EMPTY;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    Int64 clonePointer = CloneAttributes(this->attributePointer, this->attributeCount);
    AutoPtr<IAttributes> attri = new ClonedAttributes(pointer, clonePointer, attributeCount);
    *result = attri;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

Int64 ExpatParser::CloneAttributes(
    /* [in] */ Int64 pointer,
    /* [in] */ Int32 attributeCount)
{
    assert(0);
    return 0;
}

AutoPtr<IInputStream> ExpatParser::OpenUrl(
    /* [in] */ const String& url)
{
    //try {
    AutoPtr<IURLConnection> urlConnection;
    AutoPtr<IURL> iurl;
    CURL::New(url, (IURL**)&iurl);
    iurl->OpenConnection((IURLConnection**)&urlConnection);
    urlConnection->SetConnectTimeout(TIMEOUT);
    urlConnection->SetReadTimeout(TIMEOUT);
    urlConnection->SetDoInput(TRUE);
    urlConnection->SetDoOutput(FALSE);
    AutoPtr<IInputStream> inputStream;
    urlConnection->GetInputStream((IInputStream**)&inputStream);
    return inputStream;
    //} catch (Exception e) {
    //    IOException ioe = new IOException("Couldn't open " + url);
    //    ioe.initCause(e);
    //    throw ioe;
    //}
}

} // namespace Xml
} // namespace Harmony
} // namespace Apache
} // namespace Org
