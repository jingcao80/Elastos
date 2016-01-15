
#include "CKXmlParser.h"
#include "StringUtils.h"
#include "CString.h"
#include "CInputStreamReader.h"
#include "CBoolean.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::CBoolean;
using Elastos::Core::StringUtils;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::EIID_ICloseable;

namespace Org {
namespace Kxml2 {
namespace IO {

String CKXmlParser::PROPERTY_XMLDECL_VERSION("http://xmlpull.org/v1/doc/properties.html#xmldecl-version");
String CKXmlParser::PROPERTY_XMLDECL_STANDALONE("http://xmlpull.org/v1/doc/properties.html#xmldecl-standalone");
String CKXmlParser::PROPERTY_LOCATION("http://xmlpull.org/v1/doc/properties.html#location");
String CKXmlParser::FEATURE_RELAXED("http://xmlpull.org/v1/doc/features.html#relaxed");

AutoPtr< HashMap<String, String> > Init_DEFAULT_ENTITIES()
{
    AutoPtr< HashMap<String, String> > map = new HashMap<String, String>(7);
    (*map)[String("lt")] = String("<");
    (*map)[String("gt")] = String(">");
    (*map)[String("amp")] = String("&");
    (*map)[String("apos")] = String("'");
    (*map)[String("quot")] = String("\"");

    return map;
}

AutoPtr< HashMap<String, String> > CKXmlParser::DEFAULT_ENTITIES = Init_DEFAULT_ENTITIES();

AutoPtr<ArrayOf<Char32> > Init_CHAR_ARRAY(Int32 size, ...)
{
    va_list ap;
    va_start(ap, size);
    AutoPtr<ArrayOf<Char32> > chars = ArrayOf<Char32>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        (*chars)[i] = va_arg(ap, Char32);
    }
    va_end(ap);
    return chars;
}

const AutoPtr<ArrayOf<Char32> > CKXmlParser::START_COMMENT = Init_CHAR_ARRAY(4, (Char32)'<', (Char32)'!', (Char32)'-', (Char32)'-');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::END_COMMENT = Init_CHAR_ARRAY(3, (Char32)'-', (Char32)'-', (Char32)'>');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::COMMENT_DOUBLE_DASH = Init_CHAR_ARRAY(2, (Char32)'-', (Char32)'-');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::START_CDATA =
        Init_CHAR_ARRAY(9, (Char32)'<', (Char32)'!', (Char32)'[', (Char32)'C', (Char32)'D', (Char32)'A', (Char32)'T', (Char32)'A', (Char32)'[');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::END_CDATA = Init_CHAR_ARRAY(3, (Char32)']', (Char32)']', (Char32)'>');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::START_PROCESSING_INSTRUCTION = Init_CHAR_ARRAY(2, (Char32)'<', (Char32)'?');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::END_PROCESSING_INSTRUCTION = Init_CHAR_ARRAY(2, (Char32)'?', (Char32)'>');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::START_DOCTYPE =
        Init_CHAR_ARRAY(9, (Char32)'<', (Char32)'!', (Char32)'D', (Char32)'O', (Char32)'C', (Char32)'T', (Char32)'Y', (Char32)'P', (Char32)'E');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::SYSTEM =
        Init_CHAR_ARRAY(6, (Char32)'S', (Char32)'Y', (Char32)'S', (Char32)'T', (Char32)'E', (Char32)'M');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::PUBLIC =
        Init_CHAR_ARRAY(6, (Char32)'P', (Char32)'U', (Char32)'B', (Char32)'L', (Char32)'I', (Char32)'C');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::START_ELEMENT =
        Init_CHAR_ARRAY(9, (Char32)'<', (Char32)'!', (Char32)'E', (Char32)'L', (Char32)'E', (Char32)'M', (Char32)'E', (Char32)'N', (Char32)'T');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::START_ATTLIST =
        Init_CHAR_ARRAY(9, (Char32)'<', (Char32)'!', (Char32)'A', (Char32)'T', (Char32)'T', (Char32)'L', (Char32)'I', (Char32)'S', (Char32)'T');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::START_ENTITY =
        Init_CHAR_ARRAY(8, (Char32)'<', (Char32)'!', (Char32)'E', (Char32)'N', (Char32)'T', (Char32)'I', (Char32)'T', (Char32)'Y');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::START_NOTATION =
        Init_CHAR_ARRAY(10, (Char32)'<', (Char32)'!', (Char32)'N', (Char32)'O', (Char32)'T', (Char32)'A', (Char32)'T', (Char32)'I', (Char32)'O', (Char32)'N');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::EMPTY = Init_CHAR_ARRAY(5, (Char32)'E', (Char32)'M', (Char32)'P', (Char32)'T', (Char32)'Y');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::ANY = Init_CHAR_ARRAY(3, (Char32)'A', (Char32)'N', (Char32)'Y');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::NDATA = Init_CHAR_ARRAY(5, (Char32)'N', (Char32)'D', (Char32)'A', (Char32)'T', (Char32)'A');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::NOTATION =
        Init_CHAR_ARRAY(8, (Char32)'N', (Char32)'O', (Char32)'T', (Char32)'A', (Char32)'T', (Char32)'I', (Char32)'O', (Char32)'N');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::REQUIRED =
        Init_CHAR_ARRAY(8, (Char32)'R', (Char32)'E', (Char32)'Q', (Char32)'U', (Char32)'I', (Char32)'R', (Char32)'E', (Char32)'D');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::IMPLIED =
        Init_CHAR_ARRAY(7, (Char32)'I', (Char32)'M', (Char32)'P', (Char32)'L', (Char32)'I', (Char32)'E', (Char32)'D');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::FIXED = Init_CHAR_ARRAY(5, (Char32)'F', (Char32)'I', (Char32)'X', (Char32)'E', (Char32)'D');

const Int32 CKXmlParser::ELEMENTDECL;
const Int32 CKXmlParser::ENTITYDECL;
const Int32 CKXmlParser::ATTLISTDECL;
const Int32 CKXmlParser::NOTATIONDECL;
const Int32 CKXmlParser::PARAMETER_ENTITY_REF;

const String CKXmlParser::UNEXPECTED_EOF("Unexpected EOF");
const String CKXmlParser::ILLEGAL_TYPE("Wrong event type");
const AutoPtr<ArrayOf<Char32> > CKXmlParser::SINGLE_QUOTE = Init_CHAR_ARRAY(1, (Char32)'\'');
const AutoPtr<ArrayOf<Char32> > CKXmlParser::DOUBLE_QUOTE = Init_CHAR_ARRAY(1, (Char32)'"');

const String CKXmlParser::TYPES[] = {
    String("START_DOCUMENT"),
    String("END_DOCUMENT"),
    String("START_TAG"),
    String("END_TAG"),
    String("TEXT"),
    String("CDSECT"),
    String("ENTITY_REF"),
    String("IGNORABLE_WHITESPACE"),
    String("PROCESSING_INSTRUCTION"),
    String("COMMENT"),
    String("DOCDECL")
};

CKXmlParser::ContentSource::ContentSource(
    /* [in] */ ContentSource* Next,
    /* [in] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 position,
    /* [in] */ Int32 limit)
    : mNext(Next)
    , mBuffer(buffer)
    , mPosition(position)
    , mLimit(limit)
{}

CAR_INTERFACE_IMPL_2(CKXmlParser, Object, IKXmlParser, ICloseable)
CAR_OBJECT_IMPL(CKXmlParser)

CKXmlParser::CKXmlParser()
    : mProcessDocDecl(FALSE)
    , mProcessNsp(FALSE)
    , mRelaxed(FALSE)
    , mKeepNamespaceAttributes(FALSE)
    , mDepth(0)
    , mPosition(0)
    , mLimit(0)
    , mBufferStartLine(0)
    , mBufferStartColumn(0)
    , mType(0)
    , mIsWhitespace(FALSE)
    , mDegenerated(FALSE)
    , mAttributeCount(0)
    , mParsedTopLevelStartTag(FALSE)
    , mUnresolved(FALSE)
{
    mElementStack = ArrayOf<String>::Alloc(16);
    mNspStack = ArrayOf<String>::Alloc(8);
    mNspCounts = ArrayOf<Int32>::Alloc(4);
    mBuffer = ArrayOf<Char32>::Alloc(8192);
    mAttributes = ArrayOf<String>::Alloc(16);
}

CKXmlParser::~CKXmlParser()
{
    mDocumentEntities = NULL;
    mDefaultAttributes = NULL;
}

ECode CKXmlParser::KeepNamespaceAttributes()
{
    mKeepNamespaceAttributes = TRUE;
    return NOERROR;
}

ECode CKXmlParser::AdjustNsp(
    /* [out] */ Boolean* result)
{
    Boolean any = FALSE;

    for (Int32 i = 0; i < mAttributeCount << 2; i += 4) {
        String attrName = (*mAttributes)[i + 2];
        Int32 cut = attrName.IndexOf(':');
        String prefix;

        if (cut != -1) {
            prefix = attrName.Substring(0, cut);
            attrName = attrName.Substring(cut + 1);
        }
        else if (attrName.Equals("xmlns")) {
            prefix = attrName;
            attrName = NULL;
        }
        else continue;

        if (!prefix.Equals("xmlns")) {
            any = TRUE;
        }
        else {
            Int32 j = ((*mNspCounts)[mDepth]++) << 1;

            mNspStack = EnsureCapacity(mNspStack, j + 2);
            (*mNspStack)[j] = attrName;
            (*mNspStack)[j + 1] = (*mAttributes)[i + 3];

            if (!attrName.IsNull() && (*mAttributes)[i + 3].IsEmpty()) {
               FAIL_RETURN(CheckRelaxed(String("illegal empty namespace")));
            }

            if (mKeepNamespaceAttributes) {
                // explicitly set the namespace for unprefixed attributes
                // such as xmlns="http://foo"
                (*mAttributes)[i] = "http://www.w3.org/2000/xmlns/";
                any = TRUE;
            }
            else {
                Int32 length = ((--mAttributeCount) << 2) - i;
                for (Int32 j = 0; j < length; j++) {
                    (*mAttributes)[i + j] = (*mAttributes)[i + 4 + j];
                }

                i -= 4;
            }
        }
    }

    if (any) {
        for (Int32 i = (mAttributeCount << 2) - 4; i >= 0; i -= 4) {
            String attrName = (*mAttributes)[i + 2];
            Int32 cut = attrName.IndexOf(':');

            if (cut == 0 && !mRelaxed) {
//                throw new RuntimeException(
//                    "illegal attribute name: " + attrName + " at " + this);
                return E_RUNTIME_EXCEPTION;
            }
            else if (cut != -1) {
                String attrPrefix = attrName.Substring(0, cut);

                attrName = attrName.Substring(cut + 1);

                String attrNs;
                GetNamespace(attrPrefix, &attrNs);

                if (attrNs.IsNull() && !mRelaxed) {
//                    throw new RuntimeException(
//                        "Undefined Prefix: " + attrPrefix + " in " + this);
                    return E_RUNTIME_EXCEPTION;
                }

                (*mAttributes)[i] = attrNs;
                (*mAttributes)[i + 1] = attrPrefix;
                (*mAttributes)[i + 2] = attrName;
            }
        }
    }

    Int32 cut = mName.IndexOf(':');

    if (cut == 0) {
        FAIL_RETURN(CheckRelaxed(String("illegal tag name: ") + mName));
    }

    if (cut != -1) {
        mPrefix = mName.Substring(0, cut);
        mName = mName.Substring(cut + 1);
    }

    GetNamespace(mPrefix, &mNamespace);

    if (mNamespace.IsNull()) {
        if (!mPrefix.IsNull()) {
            FAIL_RETURN(CheckRelaxed(String("undefined prefix: ") + mPrefix));
        }
        mNamespace = NO_NAMESPACE;
    }

    *result = any;
    return NOERROR;
}

AutoPtr< ArrayOf<String> > CKXmlParser::EnsureCapacity(
    /* [in] */ ArrayOf<String>* arr,
    /* [in] */ Int32 required)
{
    if (arr->GetLength() >= required) {
        return arr;
    }
    AutoPtr< ArrayOf<String> > bigger = ArrayOf<String>::Alloc(required + 16);
    for (Int32 i = 0; i < arr->GetLength(); i++) {
        (*bigger)[i] = (*arr)[i];
    }
    return bigger;
}

ECode CKXmlParser::CheckRelaxed(
    /* [in] */ const String& errorMessage)
{
    if (!mRelaxed) {
        //throw new XmlPullParserException(errorMessage, this, NULL);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    if (mError.IsNull()) {
        mError = String("Error: ") + errorMessage;
    }
    return NOERROR;
}

ECode CKXmlParser::Next(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    return Next(FALSE, value);
}

ECode CKXmlParser::NextToken(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    return Next(TRUE, value);
}

ECode CKXmlParser::Next(
    /* [in] */ Boolean justOneToken,
    /* [out] */ Int32* token)
{
    if (mReader == NULL) {
        //throw new XmlPullParserException("setInput() must be called first.", this, NULL);
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    if (mType == END_TAG) {
        mDepth--;
    }

    // degenerated needs to be handled before error because of possible
    // processor expectations(!)

    if (mDegenerated) {
        mDegenerated = FALSE;
        mType = END_TAG;
        *token = mType;
        return NOERROR;
    }

    if (!mError.IsNull()) {
        if (justOneToken) {
            mText = mError;
            mType = COMMENT;
            mError = NULL;
            *token = mType;
            return NOERROR;
        }
        else {
            mError = NULL;
        }
    }

    FAIL_RETURN(PeekType(FALSE, &mType));

    if (mType == XML_DECLARATION) {
        FAIL_RETURN(ReadXmlDeclaration());
        FAIL_RETURN(PeekType(FALSE, &mType));
    }

    mText = NULL;
    mIsWhitespace = TRUE;
    mPrefix = NULL;
    mName = NULL;
    mNamespace = NULL;
    mAttributeCount = -1;
    Boolean throwOnResolveFailure = !justOneToken;

    while (TRUE) {
        switch (mType) {
            /*
             * Return immediately after encountering a start tag, end tag, or
             * the end of the document.
             */
            case START_TAG:
                FAIL_RETURN(ParseStartTag(FALSE, throwOnResolveFailure));
                *token = mType;
                return NOERROR;
            case END_TAG:
                FAIL_RETURN(ReadEndTag());
                *token = mType;
                return NOERROR;
            case END_DOCUMENT:
                *token = mType;
                return NOERROR;

            /*
             * Return after any text token when we're looking for a single
             * token. Otherwise concatenate all text between tags.
             */
            case ENTITY_REF:
                if (justOneToken) {
                    StringBuilder entityTextBuilder;
                    FAIL_RETURN(ReadEntity(entityTextBuilder, TRUE, throwOnResolveFailure, TEXT));
                    entityTextBuilder.ToString(&mText);
                    break;
                }
                // fall-through
            case IXmlPullParser::TEXT:
                FAIL_RETURN(ReadValue('<', !justOneToken, throwOnResolveFailure, TEXT, &mText));
                if (mDepth == 0 && mIsWhitespace) {
                    mType = IGNORABLE_WHITESPACE;
                }
                break;
            case CDSECT:
                FAIL_RETURN(Read(*START_CDATA));
                FAIL_RETURN(ReadUntil(*END_CDATA, TRUE, &mText));
                break;

            /*
             * Comments, processing instructions and declarations are returned
             * when we're looking for a single token. Otherwise they're skipped.
             */
            case COMMENT:
            {
                String commentText;
                FAIL_RETURN(ReadComment(justOneToken, &commentText));
                if (justOneToken) {
                    mText = commentText;
                }
                break;
            }
            case PROCESSING_INSTRUCTION:
            {
                FAIL_RETURN(Read(*START_PROCESSING_INSTRUCTION));
                String processingInstruction;
                FAIL_RETURN(ReadUntil(*END_PROCESSING_INSTRUCTION, justOneToken, &processingInstruction));
                if (justOneToken) {
                    mText = processingInstruction;
                }
                break;
            }
            case DOCDECL:
                FAIL_RETURN(ReadDoctype(justOneToken));
                if (mParsedTopLevelStartTag) {
                    //throw new XmlPullParserException("Unexpected token", this, null);
                    return E_XML_PULL_PARSER_EXCEPTION;
                }
                break;

            default: {
                //throw new XmlPullParserException("Unexpected token", this, NULL);
                return E_XML_PULL_PARSER_EXCEPTION;
            }
        }

        if (mDepth == 0 && (mType == ENTITY_REF || mType == IXmlPullParser::TEXT || mType == CDSECT)) {
            // throw new XmlPullParserException("Unexpected token", this, NULL);
            return E_XML_PULL_PARSER_EXCEPTION;
        }

        if (justOneToken) {
            *token = mType;
            return NOERROR;
        }

        if (mType == IGNORABLE_WHITESPACE) {
            mText = NULL;
        }

        /*
         * We've read all that we can of a non-empty text block. Always
         * report this as text, even if it was a CDATA block or entity
         * reference.
         */
        Int32 peek;
        FAIL_RETURN(PeekType(FALSE, &peek));
        if (!mText.IsNull() && !mText.IsEmpty() && peek < IXmlPullParser::TEXT) {
            mType = IXmlPullParser::TEXT;
            *token = mType;
            return NOERROR;
        }

        mType = peek;
    }
}

/**
 * Reads text until the specified delimiter is encountered. Consumes the
 * text and the delimiter.
 *
 * @param returnText TRUE to return the read text excluding the delimiter;
 *     FALSE to return NULL.
 */
ECode CKXmlParser::ReadUntil(
    /* [in] */ const ArrayOf<Char32>& delimiter,
    /* [in] */ Boolean returnText,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 start = mPosition;
    AutoPtr<StringBuilder> result;

    if (returnText && !mText.IsNull()) {
        result = new StringBuilder();
        result->Append(mText);
    }

    Int32 end;

search:
    while (TRUE) {
        if (mPosition + delimiter.GetLength() > mLimit) {
            if (start < mPosition && returnText) {
                if (result == NULL) {
                    result = new StringBuilder();
                }
                result->Append(*mBuffer, start, mPosition - start);
            }
            Boolean succeeded;
            FAIL_RETURN(FillBuffer(delimiter.GetLength(), &succeeded));
            if (!succeeded) {
                FAIL_RETURN(CheckRelaxed(UNEXPECTED_EOF));
                mType = COMMENT;
                return NOERROR;
            }
            start = mPosition;
        }

        // TODO: replace with Arrays.equals(mBuffer, position, delimiter, 0, delimiter.length)
        // when the VM has better method inlining
        for (Int32 i = 0; i < delimiter.GetLength(); i++) {
            if ((*mBuffer)[mPosition + i] != delimiter[i]) {
                mPosition++;
                goto search;
            }
        }

        break;
    }

    end = mPosition;
    mPosition += delimiter.GetLength();

    if (!returnText) {
        return NOERROR;
    }
    else if (result == NULL) {
        *value = String(*mBuffer, start, end - start);
        return NOERROR;
    }
    else {
        result->Append(*mBuffer, start, end - start);
        return result->ToString(value);
    }
}

/**
 * Returns TRUE if an XML declaration was read.
 */
ECode CKXmlParser::ReadXmlDeclaration()
{
    if (mBufferStartLine != 0 || mBufferStartColumn != 0 || mPosition != 0) {
        FAIL_RETURN(CheckRelaxed(String("processing instructions must not start with xml")));
    }

    FAIL_RETURN(Read(*START_PROCESSING_INSTRUCTION));
    FAIL_RETURN(ParseStartTag(TRUE, TRUE));

    if (mAttributeCount < 1 || !(*mAttributes)[2].Equals("version")) {
        FAIL_RETURN(CheckRelaxed(String("version expected")));
    }

    mVersion = (*mAttributes)[3];

    Int32 pos = 1;

    if (pos < mAttributeCount && (*mAttributes)[2 + 4].Equals("encoding")) {
        mEncoding = (*mAttributes)[3 + 4];
        pos++;
    }

    if (pos < mAttributeCount && (*mAttributes)[4 * pos + 2].Equals("standalone")) {
        String st = (*mAttributes)[3 + 4 * pos];
        if (st.Equals("yes")) {
            mStandalone = NULL;
            CBoolean::New(TRUE, (IBoolean**)&mStandalone);
        }
        else if (st.Equals("no")) {
            mStandalone = NULL;
            CBoolean::New(FALSE, (IBoolean**)&mStandalone);
        }
        else {
            FAIL_RETURN(CheckRelaxed(String("illegal standalone value: ") + st));
        }
        pos++;
    }

    if (pos != mAttributeCount) {
        FAIL_RETURN(CheckRelaxed(String("unexpected attributes in XML declaration")));
    }

    mIsWhitespace = TRUE;
    mText = NULL;
    return NOERROR;
}

ECode CKXmlParser::ReadComment(
    /* [in] */ Boolean returnText,
    /* [out] */ String* value)
{
    assert(value != NULL);
    FAIL_RETURN(Read(*START_COMMENT));

    if (mRelaxed) {
        return ReadUntil(*END_COMMENT, returnText, value);
    }

    FAIL_RETURN(ReadUntil(*COMMENT_DOUBLE_DASH, returnText, value));
    Int32 ch = PeekCharacter();
    if (ch != '>') {
        //throw new XmlPullParserException("Comments may not contain --", this, NULL);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    mPosition++;
    return NOERROR;
}

/**
 * Read the document's DTD. Although this parser is non-validating, the DTD
 * must be parsed to capture entity values and default attribute values.
 */
ECode CKXmlParser::ReadDoctype(
    /* [in] */ Boolean saveDtdText)
{
    FAIL_RETURN(Read(*START_DOCTYPE));

    Int32 startPosition = -1;
    if (saveDtdText) {
        mBufferCapture = new StringBuilder();
        startPosition = mPosition;
    }
    Skip();
    FAIL_RETURN(ReadName(&mRootElementName));
    Boolean result;
    FAIL_RETURN(ReadExternalId(TRUE, TRUE, &result));
    Skip();
    Int32 ch = PeekCharacter();
    if (ch == '[') {
        FAIL_RETURN(ReadInternalSubset());
    }
    Skip();
    if (saveDtdText) {
        mBufferCapture->Append(*mBuffer, 0, mPosition);
        mBufferCapture->Delete(0, startPosition);
        mBufferCapture->ToString(&mText);
        mBufferCapture = NULL;
    }

    return Read('>');
}

/**
 * Reads an external ID of one of these two forms:
 *   SYSTEM "quoted system name"
 *   PUBLIC "quoted public id" "quoted system name"
 *
 * If the system name is not required, this also supports lone public IDs of
 * this form:
 *   PUBLIC "quoted public id"
 *
 * Returns TRUE if any ID was read.
 */
ECode CKXmlParser::ReadExternalId(
    /* [in] */ Boolean requireSystemName,
    /* [in] */ Boolean assignFields,
    /* [out] */ Boolean* result)
{
    Skip();
    Int32 c = PeekCharacter();

    if (c == 'S') {
        FAIL_RETURN(Read(*SYSTEM));
    }
    else if (c == 'P') {
        FAIL_RETURN(Read(*PUBLIC));
        Skip();
        if (assignFields) {
            FAIL_RETURN(ReadQuotedId(TRUE, &mPublicId));
        }
        else {
            String tmp;
            FAIL_RETURN(ReadQuotedId(FALSE, &tmp));
        }
    }
    else {
        *result = FALSE;
        return NOERROR;
    }

    Skip();

    if (!requireSystemName) {
        Int32 delimiter = PeekCharacter();
        if (delimiter != '"' && delimiter != '\'') {
            *result = TRUE; // no system name!
            return NOERROR;
        }
    }

    if (assignFields) {
        FAIL_RETURN(ReadQuotedId(TRUE, &mSystemId));
    }
    else {
        String tmp;
        FAIL_RETURN(ReadQuotedId(FALSE, &tmp));
    }
    *result = TRUE;
    return NOERROR;
}

/**
 * Reads a quoted string, performing no entity escaping of the contents.
 */
ECode CKXmlParser::ReadQuotedId(
    /* [in] */ Boolean returnText,
    /* [out] */ String* value)
{
    assert(value != NULL);

    Int32 quote = PeekCharacter();
    const ArrayOf<Char32>* delimiter;
    if (quote == '"') {
        delimiter = DOUBLE_QUOTE;
    }
    else if (quote == '\'') {
        delimiter = SINGLE_QUOTE;
    }
    else {
        //throw new XmlPullParserException("Expected a quoted string", this, NULL);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    mPosition++;
    return ReadUntil(*delimiter, returnText, value);
}

ECode CKXmlParser::ReadInternalSubset()
{
    FAIL_RETURN(Read('['));

    while (TRUE) {
        Skip();
        Int32 ch = PeekCharacter();
        if (ch == ']') {
            mPosition++;
            return NOERROR;
        }

        Int32 declarationType;
        FAIL_RETURN(PeekType(TRUE, &declarationType));
        switch (declarationType) {
        case ELEMENTDECL:
            FAIL_RETURN(ReadElementDeclaration());
            break;

        case ATTLISTDECL:
            FAIL_RETURN(ReadAttributeListDeclaration());
            break;

        case ENTITYDECL:
            FAIL_RETURN(ReadEntityDeclaration());
            break;

        case NOTATIONDECL:
            FAIL_RETURN(ReadNotationDeclaration());
            break;

        case PROCESSING_INSTRUCTION:
        {
            FAIL_RETURN(Read(*START_PROCESSING_INSTRUCTION));
            String tmp;
            FAIL_RETURN(ReadUntil(*END_PROCESSING_INSTRUCTION, FALSE, &tmp));
            break;
        }
        case COMMENT:
        {
            String comment;
            FAIL_RETURN(ReadComment(FALSE, &comment));
            break;
        }
        case PARAMETER_ENTITY_REF:
            // throw new XmlPullParserException(
            //         "Parameter entity references are not supported", this, NULL);
            return E_XML_PULL_PARSER_EXCEPTION;

        default:
            //throw new XmlPullParserException("Unexpected token", this, NULL);
            return E_XML_PULL_PARSER_EXCEPTION;
        }
    }
}

/**
 * Read an element declaration. This contains a name and a content spec.
 *   <!ELEMENT foo EMPTY >
 *   <!ELEMENT foo (bar?,(baz|quux)) >
 *   <!ELEMENT foo (#PCDATA|bar)* >
 */
ECode CKXmlParser::ReadElementDeclaration()
{
    FAIL_RETURN(Read(*START_ELEMENT));
    Skip();
    String tmp;
    FAIL_RETURN(ReadName(&tmp));
    FAIL_RETURN(ReadContentSpec());
    Skip();
    return Read('>');
}

/**
 * Read an element content spec. This is a regular expression-like pattern
 * of names or other content specs. The following operators are supported:
 *   sequence:    (a,b,c)
 *   choice:      (a|b|c)
 *   optional:    a?
 *   one or more: a+
 *   any number:  a*
 *
 * The special name '#PCDATA' is permitted but only if it is the first
 * element of the first group:
 *   (#PCDATA|a|b)
 *
 * The top-level element must be either a choice, a sequence, or one of the
 * special names EMPTY and ANY.
 */
ECode CKXmlParser::ReadContentSpec()
{
    // this implementation is very lenient; it scans for balanced parens only
    Skip();
    Int32 c = PeekCharacter();
    if (c == '(') {
        Int32 depth = 0;
        do {
            if (c == '(') {
                depth++;
            }
            else if (c == ')') {
                depth--;
            }
            else if (c == -1) {
                // throw new XmlPullParserException("Unterminated element content spec", this, null);
                return E_XML_PULL_PARSER_EXCEPTION;
            }

            mPosition++;
            c = PeekCharacter();
        } while (depth > 0);

        if (c == '*' || c == '?' || c == '+') {
            mPosition++;
        }
        return NOERROR;
    }
    else if ((Char32)c == (*EMPTY)[0]) {
        return Read(*EMPTY);
    }
    else if ((Char32)c == (*ANY)[0]) {
        return Read(*ANY);
    }
    else {
        //throw new XmlPullParserException("Expected element content spec", this, NULL);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
}

/**
 * Reads an attribute list declaration such as the following:
 *   <!ATTLIST foo
 *       bar CDATA #IMPLIED
 *       quux (a|b|c) "c"
 *       baz NOTATION (a|b|c) #FIXED "c">
 *
 * Each attribute has a name, type and default.
 *
 * Types are one of the built-in types (CDATA, ID, IDREF, IDREFS, ENTITY,
 * ENTITIES, NMTOKEN, or NMTOKENS), an enumerated type "(list|of|options)"
 * or NOTATION followed by an enumerated type.
 *
 * The default is either #REQUIRED, #IMPLIED, #FIXED, a quoted value, or
 * #FIXED with a quoted value.
 */
ECode CKXmlParser::ReadAttributeListDeclaration()
{
    FAIL_RETURN(Read(*START_ATTLIST));
    Skip();
    String elementName;
    FAIL_RETURN(ReadName(&elementName));

    while (TRUE) {
        Skip();
        Int32 c = PeekCharacter();
        if (c == '>') {
            mPosition++;
            return NOERROR;
        }

        // attribute name
        String attributeName;
        FAIL_RETURN(ReadName(&attributeName));

        // attribute type
        Skip();
        Boolean succeeded;
        if (mPosition + 1 >= mLimit && (FillBuffer(2, &succeeded), !succeeded)) {
            //throw new XmlPullParserException("Malformed attribute list", this, NULL);
            return E_XML_PULL_PARSER_EXCEPTION;
        }

        if ((*mBuffer)[mPosition] == (*NOTATION)[0] && (*mBuffer)[mPosition + 1] == (*NOTATION)[1]) {
            FAIL_RETURN(Read(*NOTATION));
            Skip();
        }

        c = PeekCharacter();
        if (c == '(') {
            mPosition++;
            while (TRUE) {
                Skip();
                String tmp;
                FAIL_RETURN(ReadName(&tmp));
                Skip();
                c = PeekCharacter();
                if (c == ')') {
                    mPosition++;
                    break;
                }
                else if (c == '|') {
                    mPosition++;
                }
                else {
                    //throw new XmlPullParserException("Malformed attribute type", this, NULL);
                    return E_XML_PULL_PARSER_EXCEPTION;
                }
            }
        }
        else {
            String tmp;
            FAIL_RETURN(ReadName(&tmp));
        }

        // default value
        Skip();
        c = PeekCharacter();
        if (c == '#') {
            mPosition++;
            c = PeekCharacter();
            if (c == 'R') {
                FAIL_RETURN(Read(*REQUIRED));
            }
            else if (c == 'I') {
                FAIL_RETURN(Read(*IMPLIED));
            }
            else if (c == 'F') {
                FAIL_RETURN(Read(*FIXED));
            }
            else {
                //throw new XmlPullParserException("Malformed attribute type", this, NULL);
                return E_XML_PULL_PARSER_EXCEPTION;
            }
            Skip();
            c = PeekCharacter();
        }
        if (c == '"' || c == '\'') {
            mPosition++;
            // TODO: does this do escaping correctly?
            String value;
            FAIL_RETURN(ReadValue((Char32)c, TRUE, TRUE, ATTRIBUTE, &value));
            if (PeekCharacter() == c) {
                mPosition++;
            }
            DefineAttributeDefault(elementName, attributeName, value);
        }
    }
    return NOERROR;
}

void CKXmlParser::DefineAttributeDefault(
    /* [in] */ const String& elementName,
    /* [in] */ const String& attributeName,
    /* [in] */ const String& value)
{
    if (mDefaultAttributes == NULL) {
        mDefaultAttributes = new HashMap<String, AutoPtr<StringMap> >(11);
    }
    AutoPtr<StringMap> elementAttributes = NULL;
    HashMap<String, AutoPtr<StringMap> >::Iterator it = mDefaultAttributes->Find(elementName);
    if (it == mDefaultAttributes->End()) {
        elementAttributes = new StringMap;
        (*mDefaultAttributes)[elementName] = elementAttributes;
    }
    else {
        elementAttributes = it->mSecond;
    }
    (*elementAttributes)[attributeName] = value;
}

/**
 * Read an entity declaration. The value of internal entities are inline:
 *   <!ENTITY foo "bar">
 *
 * The values of external entities must be retrieved by URL or path:
 *   <!ENTITY foo SYSTEM "http://host/file">
 *   <!ENTITY foo PUBLIC "-//Android//Foo//EN" "http://host/file">
 *   <!ENTITY foo SYSTEM "../file.png" NDATA png>
 *
 * Entities may be general or parameterized. Parameterized entities are
 * marked by a percent sign. Such entities may only be used in the DTD:
 *   <!ENTITY % foo "bar">
 */
ECode CKXmlParser::ReadEntityDeclaration()
{
    FAIL_RETURN(Read(*START_ENTITY));
    Boolean generalEntity = TRUE;

    Skip();
    Int32 c = PeekCharacter();
    if (c == '%') {
        generalEntity = FALSE;
        mPosition++;
        Skip();
    }

    String name;
    FAIL_RETURN(ReadName(&name));

    Skip();
    Int32 quote = PeekCharacter();
    String entityValue;
    if (quote == '"' || quote == '\'') {
        mPosition++;
        FAIL_RETURN(ReadValue((Char32)quote, TRUE, FALSE, ENTITY_DECLARATION, &entityValue));
        if (PeekCharacter() == quote) {
            mPosition++;
        }
    }
    else {
        Boolean succeeded;
        FAIL_RETURN(ReadExternalId(TRUE, FALSE, &succeeded));
        if (succeeded) {
            /*
             * Map external entities to the empty string. This is dishonest,
             * but it's consistent with Android's Expat pull parser.
             */
            entityValue = "";
            Skip();
            Int32 ch = PeekCharacter();
            if ((Char32)ch == (*NDATA)[0]) {
                FAIL_RETURN(Read(*NDATA));
                Skip();
                String tmp;
                FAIL_RETURN(ReadName(&tmp));
            }
        }
        else {
            //throw new XmlPullParserException("Expected entity value or external ID", this, NULL);
            return E_XML_PULL_PARSER_EXCEPTION;
        }
    }

    if (generalEntity && mProcessDocDecl) {
        if (mDocumentEntities == NULL) {
            mDocumentEntities = new HashMap<String, AutoPtr< ArrayOf<Char32> > >(11);
        }
        AutoPtr< ArrayOf<Char32> > chars = entityValue.GetChars();
        (*mDocumentEntities)[name] = chars;
    }

    Skip();
    return Read('>');
}

ECode CKXmlParser::ReadNotationDeclaration()
{
    FAIL_RETURN(Read(*START_NOTATION));
    Skip();

    String tmp;
    FAIL_RETURN(ReadName(&tmp));
    Boolean succeeded;
    FAIL_RETURN(ReadExternalId(FALSE, FALSE, &succeeded));
    if (!succeeded) {
        // throw new XmlPullParserException(
        //         "Expected external ID or public ID for notation", this, NULL);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    Skip();
    return Read('>');
}

ECode CKXmlParser::ReadEndTag()
{
    FAIL_RETURN(Read('<'));
    FAIL_RETURN(Read('/'));
    FAIL_RETURN(ReadName(&mName)); // TODO: pass the expected name in as a hint?
    Skip();
    FAIL_RETURN(Read('>'));

    Int32 sp = (mDepth - 1) * 4;

    if (mDepth == 0) {
        FAIL_RETURN(CheckRelaxed(String("read end tag ") + mName + " with no tags open"));
        mType = COMMENT;
        return NOERROR;
    }

    if (mName.Equals((*mElementStack)[sp + 3])) {
        mNamespace = (*mElementStack)[sp];
        mPrefix = (*mElementStack)[sp + 1];
        mName = (*mElementStack)[sp + 2];
    }
    else if (!mRelaxed) {
        // throw new XmlPullParserException(
        //         "expected: /" + (*mElementStack)[sp + 3] + " read: " + mName, this, NULL);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    return NOERROR;
}

/**
 * Returns the type of the Next token.
 */
ECode CKXmlParser::PeekType(
    /* [in] */ Boolean inDeclaration,
    /* [out] */ Int32* token)
{
    Boolean succeeded;
    if (mPosition >= mLimit && (FillBuffer(1, &succeeded), !succeeded)) {
        *token = END_DOCUMENT;
        return NOERROR;
    }

    switch ((*mBuffer)[mPosition]) {
    case '&':
        *token = ENTITY_REF; // &
        return NOERROR;
    case '<':
        if (mPosition + 3 >= mLimit && (FillBuffer(4, &succeeded), !succeeded)) {
            //throw new XmlPullParserException("Dangling <", this, NULL);
            return E_XML_PULL_PARSER_EXCEPTION;
        }

        switch ((*mBuffer)[mPosition + 1]) {
        case '/':
            *token = END_TAG; // </
            return NOERROR;
        case '?':
            // we're looking for "<?xml " with case insensitivity
            if ((mPosition + 5 < mLimit || (FillBuffer(6, &succeeded), succeeded))
                    && ((*mBuffer)[mPosition + 2] == 'x' || (*mBuffer)[mPosition + 2] == 'X')
                    && ((*mBuffer)[mPosition + 3] == 'm' || (*mBuffer)[mPosition + 3] == 'M')
                    && ((*mBuffer)[mPosition + 4] == 'l' || (*mBuffer)[mPosition + 4] == 'L')
                    && ((*mBuffer)[mPosition + 5] == ' ')) {
                *token = XML_DECLARATION; // <?xml
                return NOERROR;
            }
            else {
                *token = PROCESSING_INSTRUCTION; // <?
                return NOERROR;
            }
        case '!':
            switch ((*mBuffer)[mPosition + 2]) {
            case 'D':
                *token = DOCDECL; // <!D
                return NOERROR;
            case '[':
                *token = CDSECT; // <![
                return NOERROR;
            case '-':
                *token = COMMENT; // <!-
                return NOERROR;
            case 'E':
                switch ((*mBuffer)[mPosition + 3]) {
                case 'L':
                    *token = ELEMENTDECL; // <!EL
                    return NOERROR;
                case 'N':
                    *token = ENTITYDECL; // <!EN
                    return NOERROR;
                }
                break;
            case 'A':
                *token = ATTLISTDECL;  // <!A
                return NOERROR;
            case 'N':
                *token = NOTATIONDECL; // <!N
                return NOTATIONDECL;
            }
            //throw new XmlPullParserException("Unexpected <!", this, NULL);
            return E_XML_PULL_PARSER_EXCEPTION;
        default:
            *token = START_TAG; // <
            return NOERROR;
        }
    case '%':
        *token = inDeclaration ? PARAMETER_ENTITY_REF : IXmlPullParser::TEXT;
        return NOERROR;
    default:
        *token = IXmlPullParser::TEXT;
        return NOERROR;
    }
}

/**
 * Sets name and attributes
 */
ECode CKXmlParser::ParseStartTag(
    /* [in] */ Boolean xmldecl,
    /* [in] */ Boolean throwOnResolveFailure)
{
    if (!xmldecl) {
        FAIL_RETURN(Read('<'));
    }
    FAIL_RETURN(ReadName(&mName));
    mAttributeCount = 0;

    String temp;
    Boolean succeeded;
    while (TRUE) {
        Skip();

        if (mPosition >= mLimit && (FillBuffer(1, &succeeded), !succeeded)) {
            return CheckRelaxed(UNEXPECTED_EOF);
        }

        Int32 c = (*mBuffer)[mPosition];

        if (xmldecl) {
            if (c == '?') {
                mPosition++;
                return Read('>');
            }
        }
        else {
            if (c == '/') {
                mDegenerated = TRUE;
                mPosition++;
                Skip();
                FAIL_RETURN(Read('>'));
                break;
            }
            else if (c == '>') {
                mPosition++;
                break;
            }
        }

        String attrName;
        FAIL_RETURN(ReadName(&attrName));

        Int32 i = (mAttributeCount++) * 4;
        mAttributes = EnsureCapacity(mAttributes, i + 4);
        (*mAttributes)[i] = "";
        (*mAttributes)[i + 1] = NULL;
        (*mAttributes)[i + 2] = attrName;

        Skip();
        if (mPosition >= mLimit && (FillBuffer(1, &succeeded), !succeeded)) {
            return CheckRelaxed(UNEXPECTED_EOF);
        }

        if ((*mBuffer)[mPosition] == '=') {
            mPosition++;

            Skip();
            if (mPosition >= mLimit && (FillBuffer(1, &succeeded), !succeeded)) {
                return CheckRelaxed(UNEXPECTED_EOF);
            }
            Char32 delimiter = (*mBuffer)[mPosition];

            if (delimiter == '\'' || delimiter == '"') {
                mPosition++;
            }
            else if (mRelaxed) {
                delimiter = ' ';
            }
            else {
                //throw new XmlPullParserException("attr value delimiter missing!", this, NULL);
                return E_XML_PULL_PARSER_EXCEPTION;
            }

            FAIL_RETURN(ReadValue(delimiter, TRUE, throwOnResolveFailure,
                    ATTRIBUTE, &temp));
            (*mAttributes)[i + 3] = temp;

            if (delimiter != ' ' && PeekCharacter() == delimiter) {
                mPosition++; // end quote
            }
        }
        else if (mRelaxed) {
            (*mAttributes)[i + 3] = attrName;
        }
        else {
            FAIL_RETURN(CheckRelaxed(String("Attr.value missing f. ") + attrName));
            (*mAttributes)[i + 3] = attrName;
        }
    }

    Int32 sp = mDepth++ * 4;
    if (mDepth == 1) {
        mParsedTopLevelStartTag = true;
    }
    mElementStack = EnsureCapacity(mElementStack, sp + 4);
    (*mElementStack)[sp + 3] = mName;

    if (mDepth >= mNspCounts->GetLength()) {
        AutoPtr< ArrayOf<Int32> > bigger = ArrayOf<Int32>::Alloc(mDepth + 4);
        bigger->Copy(mNspCounts);
        mNspCounts = bigger;
    }

    (*mNspCounts)[mDepth] = (*mNspCounts)[mDepth - 1];

    if (mProcessNsp) {
        Boolean result;
        FAIL_RETURN(AdjustNsp(&result));
    }
    else {
        mNamespace = "";
    }

    // For consistency with Expat, add default attributes after fixing namespaces.
    if (mDefaultAttributes != NULL) {
        HashMap<String, AutoPtr<StringMap> >::Iterator it = mDefaultAttributes->Find(mName);
        if (it != mDefaultAttributes->End()) {
            AutoPtr<StringMap> elementDefaultAttributes = it->mSecond;
            typename StringMap::Iterator entryIt = elementDefaultAttributes->Begin();
            for (; entryIt != elementDefaultAttributes->End(); ++entryIt) {
                String value;
                GetAttributeValue(String(NULL), entryIt->mFirst, &value);
                if (!value.IsNull()) {
                    continue; // an explicit value overrides the default
                }

                Int32 i = (mAttributeCount++) * 4;
                mAttributes = EnsureCapacity(mAttributes, i + 4);
                (*mAttributes)[i] = "";
                (*mAttributes)[i + 1] = NULL;
                (*mAttributes)[i + 2] = entryIt->mFirst;
                (*mAttributes)[i + 3] = entryIt->mSecond;
            }
        }
    }

    (*mElementStack)[sp] = mNamespace;
    (*mElementStack)[sp + 1] = mPrefix;
    (*mElementStack)[sp + 2] = mName;
    return NOERROR;
}

/**
 * Reads an entity reference from the buffer, resolves it, and writes the
 * resolved entity to {@code out}. If the entity cannot be read or resolved,
 * {@code out} will contain the partial entity reference.
 */
ECode CKXmlParser::ReadEntity(
    /* [in] */ StringBuilder& out,
    /* [in] */ Boolean isEntityToken,
    /* [in] */ Boolean throwOnResolveFailure,
    /* [in] */ ValueContext valueContext)
{
    Int32 start = out.GetLength();

    if ((*mBuffer)[mPosition++] != '&') {
        // throw new AssertionError();
        return E_ASSERTION_ERROR;
    }

    out.AppendChar('&');

    while (TRUE) {
        Int32 c = PeekCharacter();

        if (c == ';') {
            out.AppendChar(';');
            mPosition++;
            break;
        }
        else if (c >= 128
                || (c >= '0' && c <= '9')
                || (c >= 'a' && c <= 'z')
                || (c >= 'A' && c <= 'Z')
                || c == '_'
                || c == '-'
                || c == '#') {
            mPosition++;
            out.AppendChar((Char32)c);
        }
        else if (mRelaxed) {
            // intentionally leave the partial reference in 'out'
            return NOERROR;
        }
        else {
            //throw new XmlPullParserException("unterminated entity ref", this, NULL);
            return E_XML_PULL_PARSER_EXCEPTION;
        }
    }

    Int32 length;
    out.GetLength(&length);
    String code;
    out.Substring(start + 1, length - 1, &code);

    if (isEntityToken) {
        mName = code;
    }

    if (code.StartWith("#")) {
        // try {
        Int32 c = code.StartWith("#x")
                ? StringUtils::ParseInt32(code.Substring(2), 16)
                : StringUtils::ParseInt32(code.Substring(1));
        out.GetLength(&length);
        out.Delete(start, length);
        out.AppendChar(c);
        mUnresolved = FALSE;
        return NOERROR;
        // } catch (NumberFormatException notANumber) {
        //     throw new XmlPullParserException("Invalid character reference: &" + code);
        // } catch (IllegalArgumentException invalidCodePoint) {
        //     throw new XmlPullParserException("Invalid character reference: &" + code);
        // }
    }

    if (valueContext == ENTITY_DECLARATION) {
        // keep the unresolved &code; in the text to resolve later
        return NOERROR;
    }

    String defaultEntity;
    typename StringMap::Iterator strIt = DEFAULT_ENTITIES->Find(code);
    if (strIt != DEFAULT_ENTITIES->End()) {
        defaultEntity = strIt->mSecond;
    }
    if (!defaultEntity.IsNull()) {
        Int32 length;
        out.GetLength(&length);
        out.Delete(start, length);
        mUnresolved = FALSE;
        out.Append(defaultEntity);
        return NOERROR;
    }

    AutoPtr< ArrayOf<Char32> > resolved;
    if (mDocumentEntities != NULL) {
        HashMap<String, AutoPtr< ArrayOf<Char32> > >::Iterator it = mDocumentEntities->Find(code);
        if (it != mDocumentEntities->End()) {
            resolved = it->mSecond;
        }
        if (resolved != NULL) {
            Int32 length;
            out.GetLength(&length);
            out.Delete(start, length);
            mUnresolved = FALSE;
            if (mProcessDocDecl) {
                PushContentSource(resolved); // parse the entity as XML
            }
            else {
                out.Append(*resolved); // include the entity value as text
            }
            return NOERROR;
        }
    }

    /*
     * The parser skipped an external DTD, and now we've encountered an
     * unknown entity that could have been declared there. Map it to the
     * empty string. This is dishonest, but it's consistent with Android's
     * old ExpatPullParser.
     */
    if (!mSystemId.IsNull()) {
        Int32 length;
        out.GetLength(&length);
        out.Delete(start, length);
        return NOERROR;
    }

    // keep the unresolved entity "&code;" in the text for relaxed clients
    mUnresolved = TRUE;
    if (throwOnResolveFailure) {
        FAIL_RETURN(CheckRelaxed(String("unresolved: &") + code + ";"));
    }
    return NOERROR;
}

/**
 * Returns the current text or attribute value. This also has the side
 * effect of setting isWhitespace to FALSE if a non-whitespace character is
 * encountered.
 *
 * @param delimiter {@code <} for text, {@code "} and {@code '} for quoted
 *     attributes, or a space for unquoted attributes.
 */
ECode CKXmlParser::ReadValue(
    /* [in] */ Char32 delimiter,
    /* [in] */ Boolean resolveEntities,
    /* [in] */ Boolean throwOnResolveFailure,
    /* [in] */ ValueContext valueContext,
    /* [out] */ String* value)
{
    assert(value != NULL);
    /*
     * This method returns all of the characters from the current position
     * through to an appropriate delimiter.
     *
     * If we're lucky (which we usually are), we'll return a single slice of
     * the buffer. This fast path avoids allocating a string builder.
     *
     * There are 6 unlucky characters we could encounter:
     *  - "&":  entities must be resolved.
     *  - "%":  parameter entities are unsupported in entity values.
     *  - "<":  this isn't permitted in attributes unless relaxed.
     *  - "]":  this requires a lookahead to defend against the forbidden
     *          CDATA section delimiter "]]>".
     *  - "\r": If a "\r" is followed by a "\n", we discard the "\r". If it
     *          isn't followed by "\n", we replace "\r" with either a "\n"
     *          in text nodes or a space in attribute values.
     *  - "\n": In attribute values, "\n" must be replaced with a space.
     *
     * We could also get unlucky by needing to refill the buffer midway
     * through the text.
     */

    Int32 start = mPosition;
    AutoPtr<StringBuilder> result;

    // if a text section was already started, prefix the start
    if (valueContext == TEXT && !mText.IsNull()) {
        result = new StringBuilder();
        result->Append(mText);
    }

    while (TRUE) {

        /*
         * Make sure we have at least a single character to read from the
         * buffer. This mutates the buffer, so save the partial result
         * to the slow path string builder first.
         */
        if (mPosition >= mLimit) {
            if (start < mPosition) {
                if (result == NULL) {
                    result = new StringBuilder();
                }
                result->Append(*mBuffer, start, mPosition - start);
            }
            Boolean succeeded;
            if (FillBuffer(1, &succeeded), !succeeded) {
                /* add by aw:lisidong: When delimiter start be '"' and file is end, but did not end with '"'
                 * (the same as '\'')
                 * the value about position and limit will be 0, but it will fail because somewhere make 'position++',
                 * we dont want that partial value
                 */
                // if (delimiter == '\'' || delimiter == '"') {
                //     CheckRelaxed(String("Illegal: \"<\" inside attribute value"));
                // }
                *value = (result != NULL) ? result->ToString() : String("");
                return NOERROR;
            }
            start = mPosition;
        }

        Char32 c = (*mBuffer)[mPosition];

        if (c == delimiter
                || (delimiter == ' ' && (c <= ' ' || c == '>'))
                || (c == '&' && !resolveEntities)) {
            break;
        }

        if (c != '\r'
                && (c != '\n' || valueContext != ATTRIBUTE)
                && c != '&'
                && c != '<'
                && (c != ']' || valueContext != TEXT)
                && (c != '%' || valueContext != ENTITY_DECLARATION)) {
            mIsWhitespace &= (c <= ' ');
            mPosition++;
            continue;
        }

        /*
         * We've encountered an unlucky character! Convert from fast
         * path to slow path if we haven't done so already.
         */
        if (result == NULL) {
            result = new StringBuilder();
        }
        result->Append(*mBuffer, start, mPosition - start);

        if (c == '\r') {
            Boolean succeeded;
            if ((mPosition + 1 < mLimit || (FillBuffer(2, &succeeded), succeeded))
                && (*mBuffer)[mPosition + 1] == '\n') {
                mPosition++;
            }
            c = (valueContext == ATTRIBUTE) ? ' ' : '\n';

        }
        else if (c == '\n') {
            c = ' ';

        }
        else if (c == '&') {
            mIsWhitespace = FALSE; // TODO: what if the entity resolves to whitespace?
            FAIL_RETURN(ReadEntity(*result, FALSE, throwOnResolveFailure, valueContext));
            start = mPosition;
            continue;

        }
        else if (c == '<') {
            if (valueContext == ATTRIBUTE) {
                FAIL_RETURN(CheckRelaxed(String("Illegal: \"<\" inside attribute value")));
            }
            mIsWhitespace = FALSE;

        } else if (c == ']') {
            Boolean succeeded;
            if ((mPosition + 2 < mLimit || (FillBuffer(3, &succeeded), succeeded))
                    && (*mBuffer)[mPosition + 1] == ']' && (*mBuffer)[mPosition + 2] == '>') {
                FAIL_RETURN(CheckRelaxed(String("Illegal: \"]]>\" outside CDATA section")));
            }
            mIsWhitespace = FALSE;

        }
        else if (c == '%') {
            // throw new XmlPullParserException("This parser doesn't support parameter entities",
            //         this, NULL);
            return E_XML_PULL_PARSER_EXCEPTION;
        }
        else {
            //throw new AssertionError();
            return E_ASSERTION_ERROR;
        }

        mPosition++;
        result->AppendChar(c);
        start = mPosition;
    }

    if (result == NULL) {
        String temp(*mBuffer, start, mPosition - start);
        *value = temp;
        return NOERROR;
    }
    else {
        result->Append(*mBuffer, start, mPosition - start);
        result->ToString(value);
        return NOERROR;
    }
}

ECode CKXmlParser::Read(
    /* [in] */ Char32 expected)
{
    Int32 c = PeekCharacter();
    if ((Char32)c != expected) {
        FAIL_RETURN(CheckRelaxed(String("expected: '")/* + expected + "' actual: '" + ((Char32)c) + "'"*/));
        if (c == -1) {
            return NOERROR; // On EOF, don't move position beyond limit
        }
    }
    mPosition++;
    return NOERROR;
}

ECode CKXmlParser::Read(
    /* [in] */ const ArrayOf<Char32>& chars)
{
    Boolean succeeded;
    if (mPosition + chars.GetLength() > mLimit && (FillBuffer(chars.GetLength(), &succeeded), !succeeded)) {
        return CheckRelaxed(String("expected: '") + String(chars, 0, chars.GetLength()) + "' but was EOF");
    }

    // TODO: replace with Arrays.equals(buffer, position, delimiter, 0, delimiter.length)
    // when the VM has better method inlining
    for (Int32 i = 0; i < chars.GetLength(); i++) {
        if ((*mBuffer)[mPosition + i] != chars[i]) {
            FAIL_RETURN(CheckRelaxed(String("expected: \"") + String(chars, 0, chars.GetLength())
                     + "\" but was \"" + String(*mBuffer, mPosition, chars.GetLength()) + "...\""));
        }
    }

    mPosition += chars.GetLength();
    return NOERROR;
}

Int32 CKXmlParser::PeekCharacter()
{
    Boolean succeeded;
    if (mPosition < mLimit || (FillBuffer(1, &succeeded), succeeded)) {
        return (*mBuffer)[mPosition];
    }
    return -1;
}

/**
 * Returns TRUE once {@code limit - position >= minimum}. If the data is
 * exhausted before that many characters are available, this returns
 * FALSE.
 */
ECode CKXmlParser::FillBuffer(
    /* [in] */ Int32 minimum,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    // If we've exhausted the current content source, remove it
    while (mNextContentSource != NULL) {
        if (mPosition < mLimit) {
            //throw new XmlPullParserException("Unbalanced entity!", this, NULL);
            assert(0);
            return E_XML_PULL_PARSER_EXCEPTION;
        }
        PopContentSource();
        if (mLimit - mPosition >= minimum) {
            *result = TRUE;
            return NOERROR;
        }
    }

    // Before clobbering the old characters, update where buffer starts
    for (Int32 i = 0; i < mPosition; i++) {
        if ((*mBuffer)[i] == '\n') {
            mBufferStartLine++;
            mBufferStartColumn = 0;
        }
        else {
            mBufferStartColumn++;
        }
    }

    if (mBufferCapture != NULL) {
        mBufferCapture->Append(*mBuffer, 0, mPosition);
    }

    if (mLimit != mPosition) {
        mLimit -= mPosition;
        memmove(mBuffer->GetPayload(), mBuffer->GetPayload() + mPosition,
                sizeof(Char32) * mLimit);
    }
    else {
        mLimit = 0;
    }

    mPosition = 0;
    Int32 total = 0;
    FAIL_RETURN(mReader->Read(mBuffer, mLimit, mBuffer->GetLength() - mLimit, &total));
    while (total != -1) {
        mLimit += total;
        if (mLimit >= minimum) {
            *result = TRUE;
            return NOERROR;
        }
        FAIL_RETURN(mReader->Read(mBuffer, mLimit, mBuffer->GetLength() - mLimit, &total));
    }

    return NOERROR;
}

/**
 * Returns an element or attribute name. This is always non-empty for
 * non-relaxed parsers.
 */
ECode CKXmlParser::ReadName(
    /* [out] */ String* value)
{
    assert(value != NULL);
    Boolean succeeded;
    if (mPosition >= mLimit && (FillBuffer(1, &succeeded), !succeeded)) {
        FAIL_RETURN(CheckRelaxed(String("name expected")));
        *value = "";
        return NOERROR;
    }

    Int32 start = mPosition;
    AutoPtr<StringBuilder> result;

    // read the first character
    Char32 c = (*mBuffer)[mPosition];
    if ((c >= 'a' && c <= 'z')
            || (c >= 'A' && c <= 'Z')
            || c == '_'
            || c == ':'
            || c >= 0xc0 //'\u00c0' // TODO: check the XML spec
            || mRelaxed) {
        mPosition++;
    }
    else {
        FAIL_RETURN(CheckRelaxed(String("name expected")));
        *value = "";
        return NOERROR;
    }

    while (TRUE) {
        /*
         * Make sure we have at least a single character to read from the
         * buffer. This mutates the buffer, so save the partial result
         * to the slow path string builder first.
         */
        if (mPosition >= mLimit) {
            if (result == NULL) {
                result = new StringBuilder();
            }
            result->Append(*mBuffer, start, mPosition - start);
            Boolean succeeded;
            if (FillBuffer(1, &succeeded), !succeeded) {
                result->ToString(value);
                return NOERROR;
            }
            start = mPosition;
        }

        // read another character
        c = (*mBuffer)[mPosition];
        if ((c >= 'a' && c <= 'z')
                || (c >= 'A' && c <= 'Z')
                || (c >= '0' && c <= '9')
                || c == '_'
                || c == '-'
                || c == ':'
                || c == '.'
                || c >= 0xb7 /*'\u00b7'*/) {  // TODO: check the XML spec
            mPosition++;
            continue;
        }

        // we encountered a non-name character. done!
        if (result == NULL) {
            *value = String(*mBuffer, start, mPosition - start);
            return NOERROR;
        }
        else {
            result->Append(*mBuffer, start, mPosition - start);
            return result->ToString(value);
        }
    }
}

ECode CKXmlParser::Skip()
{
    Boolean succeeded;
    while (mPosition < mLimit || (FillBuffer(1, &succeeded), succeeded)) {
        Int32 c = (*mBuffer)[mPosition];
        if (c > ' ') {
            break;
        }
        mPosition++;
    }
    return NOERROR;
}

//  public part starts here...

ECode CKXmlParser::SetInput(
    /* [in] */ IReader* reader)
{
    mReader = reader;

    mType = START_DOCUMENT;
    mName = NULL;
    mNamespace = NULL;
    mDegenerated = FALSE;
    mAttributeCount = -1;
    mEncoding = NULL;
    mVersion = NULL;
    mStandalone = NULL;

    if (reader == NULL) {
        return NOERROR;
    }

    mPosition = 0;
    mLimit = 0;
    mBufferStartLine = 0;
    mBufferStartColumn = 0;
    mDepth = 0;
    mDocumentEntities = NULL;

    return NOERROR;
}

ECode CKXmlParser::SetInput(
    /* [in] */ IInputStream* is,
    /* [in] */ const String& charsetStr)
{
    mPosition = 0;
    mLimit = 0;
    String charset = charsetStr;
    Boolean detectCharset = charset.IsNull();

    if (is == NULL) {
        //throw new IllegalArgumentException("is == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (detectCharset) {
        // read the four bytes looking for an indication of the encoding in use
        Int32 firstFourBytes = 0;
        while (mLimit < 4) {
            Int32 i = 0;
            is->Read(&i);
            if (i == -1) {
                break;
            }
            firstFourBytes = (firstFourBytes << 8) | i;
            (*mBuffer)[mLimit++] = (Char32) i;
        }

        if (mLimit == 4) {
            switch (firstFourBytes) {
            case 0x00000FEFF: // UTF-32BE BOM
                charset = "UTF-32BE";
                mLimit = 0;
                break;

            case 0x0FFFE0000: // UTF-32LE BOM
                charset = "UTF-32LE";
                mLimit = 0;
                break;

            case 0x0000003c: // '<' in UTF-32BE
                charset = "UTF-32BE";
                (*mBuffer)[0] = '<';
                mLimit = 1;
                break;

            case 0x03c000000: // '<' in UTF-32LE
                charset = "UTF-32LE";
                (*mBuffer)[0] = '<';
                mLimit = 1;
                break;

            case 0x0003c003f: // "<?" in UTF-16BE
                charset = "UTF-16BE";
                (*mBuffer)[0] = '<';
                (*mBuffer)[1] = '?';
                mLimit = 2;
                break;

            case 0x03c003f00: // "<?" in UTF-16LE
                charset = "UTF-16LE";
                (*mBuffer)[0] = '<';
                (*mBuffer)[1] = '?';
                mLimit = 2;
                break;

            case 0x03c3f786d: // "<?xm" in ASCII etc.
                while (TRUE) {
                    Int32 i = 0;
                    is->Read(&i);
                    if (i == -1) {
                        break;
                    }
                    (*mBuffer)[mLimit++] = (Char32)i;
                    if (i == '>') {
                        String s(*mBuffer, 0, mLimit);
                        Int32 i0 = s.IndexOf("encoding");
                        if (i0 != -1) {
                            AutoPtr<ArrayOf<Char32> > charArray = s.GetChars();
                            while ((*charArray)[i0] != '"' && (*charArray)[i0] != '\'') {
                                ++i0;
                            }
                            Char32 deli = (*charArray)[i0++];
                            Int32 i1 = s.IndexOf(deli, i0);
                            charset = s.Substring(i0, i1);
                        }
                        break;
                    }
                }
                break;

            default:
                // handle a byte order mark followed by something other than <?
                if ((firstFourBytes & 0x0ffff0000) == 0x0feff0000) {
                    charset = "UTF-16BE";
                    (*mBuffer)[0] = (Char32)(((*mBuffer)[2] << 8) | (*mBuffer)[3]);
                    mLimit = 1;
                }
                else if ((firstFourBytes & 0x0ffff0000) == 0x0fffe0000) {
                    charset = "UTF-16LE";
                    (*mBuffer)[0] = (Char32)(((*mBuffer)[3] << 8) | (*mBuffer)[2]);
                    mLimit = 1;
                }
                else if ((firstFourBytes & 0x0ffffff00) == 0x0efbbbf00) {
                    charset = "UTF-8";
                    (*mBuffer)[0] = (*mBuffer)[3];
                    mLimit = 1;
                }
            }
        }
    }

    if (charset.IsNull()) {
        charset = "UTF-8";
    }

    Int32 savedLimit = mLimit;
    AutoPtr<IInputStreamReader> reader;
    CInputStreamReader::New(is, charset, (IInputStreamReader**)&reader);
    SetInput((IReader*)reader.Get());
    mEncoding = charset;
    mLimit = savedLimit;

    /*
     * Skip the optional BOM if we didn't above. This decrements limit
     * rather than incrementing position so that <?xml version='1.0'?>
     * is still at character 0.
     */
    if (!detectCharset && PeekCharacter() == 0xfeff) {
        mLimit--;
        memmove(mBuffer->GetPayload(), mBuffer->GetPayload() + 1, mLimit);
    }

    return NOERROR;
}

ECode CKXmlParser::Close()
{
    if (mReader != NULL) {
        assert(ICloseable::Probe(mReader) != NULL);
        return ICloseable::Probe(mReader)->Close();
    }
    return NOERROR;
}

ECode CKXmlParser::GetFeature(
    /* [in] */ const String& feature,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);

    if (FEATURE_PROCESS_NAMESPACES.Equals(feature)) {
        *flag = mProcessNsp;
    }
    else if (FEATURE_RELAXED.Equals(feature)) {
        *flag = mRelaxed;
    }
    else if (FEATURE_PROCESS_DOCDECL.Equals(feature)) {
        *flag = mProcessDocDecl;
    }
    else *flag = FALSE;
    return NOERROR;
}

ECode CKXmlParser::GetInputEncoding(
    /* [out] */ String* encoding)
{
    VALIDATE_NOT_NULL(encoding);
    *encoding = mEncoding;
    return NOERROR;
}

ECode CKXmlParser::DefineEntityReplacementText(
    /* [in] */ const String& entity,
    /* [in] */ const String& value)
{
    if (mProcessDocDecl) {
        // throw new IllegalStateException(
        //         "Entity replacement text may not be defined with DOCTYPE processing enabled.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mReader == NULL) {
        // throw new IllegalStateException(
        //         "Entity replacement text must be defined after setInput()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mDocumentEntities == NULL) {
        mDocumentEntities = new HashMap<String, AutoPtr< ArrayOf<Char32> > >(11);
    }
    AutoPtr< ArrayOf<Char32> > chars = value.GetChars();;
    (*mDocumentEntities)[entity] = chars;
    return NOERROR;
}

ECode CKXmlParser::GetProperty(
    /* [in] */ const String& property,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    if (property.Equals(PROPERTY_XMLDECL_VERSION)) {
        return CString::New(mVersion, (ICharSequence**)value);
    }
    else if (property.Equals(PROPERTY_XMLDECL_STANDALONE)) {
        *value = mStandalone;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }
    else if (property.Equals(PROPERTY_LOCATION)) {
        if (!mLocation.IsNull()) {
            return CString::New(mLocation, (ICharSequence**)value);
        }
        else {
            //TODO:
            assert(0);
            // return mReader->ToString(value);
            return E_NOT_IMPLEMENTED;
        }
    }
    else {
        *value = NULL;
        return NOERROR;
    }
}

/**
 * Returns the root element's name if it was declared in the DTD. This
 * equals the first tag's name for valid documents.
 */
ECode CKXmlParser::GetRootElementName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mRootElementName;
    return NOERROR;
}

/**
 * Returns the document's system ID if it was declared. This is typically a
 * string like {@code http://www.w3.org/TR/html4/strict.dtd}.
 */
ECode CKXmlParser::GetSystemId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mSystemId;
    return NOERROR;
}

/**
 * Returns the document's public ID if it was declared. This is typically a
 * string like {@code -//W3C//DTD HTML 4.01//EN}.
 */
ECode CKXmlParser::GetPublicId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mPublicId;
    return NOERROR;
}

ECode CKXmlParser::GetNamespaceCount(
    /* [in] */ Int32 depth,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    if (depth > mDepth) {
        //throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    *count = (*mNspCounts)[depth];
    return NOERROR;
}

ECode CKXmlParser::GetNamespacePrefix(
    /* [in] */ Int32 pos,
    /* [out] */ String* prefix)
{
    VALIDATE_NOT_NULL(prefix);

    *prefix = (*mNspStack)[pos * 2];
    return NOERROR;
}

ECode CKXmlParser::GetNamespaceUri(
    /* [in] */ Int32 pos,
    /* [out] */ String* uri)
{
    VALIDATE_NOT_NULL(uri);

    *uri = (*mNspStack)[(pos * 2) + 1];
    return NOERROR;
}

ECode CKXmlParser::GetNamespace(
    /* [in] */ const String& prefix,
    /* [out] */ String* ns)
{
    VALIDATE_NOT_NULL(ns);

    if (prefix.Equals("xml")) {
        *ns = "http://www.w3.org/XML/1998/namespace";
        return NOERROR;
    }
    if (prefix.Equals("xmlns")) {
        *ns = "http://www.w3.org/2000/xmlns/";
        return NOERROR;
    }

    Int32 count;
    GetNamespaceCount(mDepth, &count);
    for (Int32 i = (count << 1) - 2; i >= 0; i -= 2) {
        if (prefix.IsNull()) {
            if ((*mNspStack)[i].IsNull()) {
                *ns = (*mNspStack)[i + 1];
                return NOERROR;
            }
        }
        else if (prefix.Equals((*mNspStack)[i])) {
            *ns = (*mNspStack)[i + 1];
            return NOERROR;
        }
    }
    *ns = NULL;
    return NOERROR;
}

ECode CKXmlParser::GetDepth(
    /* [out] */ Int32* depth)
{
    VALIDATE_NOT_NULL(depth);
    *depth = mDepth;
    return NOERROR;
}

ECode CKXmlParser::GetPositionDescription(
    /* [out] */ String* des)
{
    VALIDATE_NOT_NULL(des);

    StringBuilder buf(mType < (Int32)(sizeof(TYPES) / sizeof(String)) ? TYPES[mType] : String("unknown"));
    buf.AppendChar(' ');

    if (mType == START_TAG || mType == END_TAG) {
        if (mDegenerated) {
            buf.Append(String("(empty) "));
        }
        buf.AppendChar('<');
        if (mType == END_TAG) {
            buf.AppendChar('/');
        }

        if (!mPrefix.IsNull()) {
            buf.Append(String("{") + mNamespace + "}" + mPrefix + ":");
        }
        buf.Append(mName);

        Int32 cnt = mAttributeCount * 4;
        for (Int32 i = 0; i < cnt; i += 4) {
            buf.AppendChar(' ');
            if (!(*mAttributes)[i + 1].IsNull()) {
                buf.Append(String("{") + (*mAttributes)[i] + "}" + (*mAttributes)[i + 1] + ":");
            }
            buf.Append((*mAttributes)[i + 2]);
            buf.Append(String("='") + (*mAttributes)[i + 3] + "'");
        }

        buf.AppendChar('>');
    }
    else if (mType == IGNORABLE_WHITESPACE) {}
    else if (mType != IXmlPullParser::TEXT) {
        String text;
        GetText(&text);
        buf.Append(text);
    }
    else if (mIsWhitespace) {
        buf.Append(String("(whitespace)"));
    }
    else {
        String text;
        GetText(&text);
        if (text.GetLength() > 16) {
            text = text.Substring(0, 16) + "...";
        }
        buf.Append(text);
    }

    Int32 lineNum = 0, columnNum = 0;
    GetLineNumber(&lineNum);
    GetColumnNumber(&columnNum);
    buf.AppendChar('@');
    buf.Append(lineNum);
    buf.AppendChar(':');
    buf.Append(columnNum);
    if(!mLocation.IsNull()){
        buf.Append(" in ");
        buf.Append(mLocation);
    }
    else if(mReader != NULL){
        String str("Not Implemented, add later");
        //TODO:
        //assert(0);
        // mReader->ToString(&str);
        buf.Append(" in ");
        buf.Append(str);
    }
    return buf.ToString(des);
}

ECode CKXmlParser::GetLineNumber(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);

    Int32 result = mBufferStartLine;
    for (Int32 i = 0; i < mPosition; i++) {
        if ((*mBuffer)[i] == '\n') {
            result++;
        }
    }
    *num = result + 1; // the first line is '1'
    return NOERROR;
}

ECode CKXmlParser::GetColumnNumber(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);

    Int32 result = mBufferStartColumn;
    for (Int32 i = 0; i < mPosition; i++) {
        if ((*mBuffer)[i] == '\n') {
            result = 0;
        }
        else {
            result++;
        }
    }
    *num = result + 1; // the first column is '1'
    return NOERROR;
}

ECode CKXmlParser::IsWhitespace(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mType != IXmlPullParser::TEXT && mType != IGNORABLE_WHITESPACE && mType != CDSECT) {
        //throw new XmlPullParserException(ILLEGAL_TYPE, this, NULL);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    *result = mIsWhitespace;
    return NOERROR;
}

ECode CKXmlParser::GetText(
    /* [out] */ String* text)
{
    VALIDATE_NOT_NULL(text);
    if (mType < IXmlPullParser::TEXT || (mType == ENTITY_REF && mUnresolved)) {
        *text = NULL;
    }
    else if (mText.IsNull()) {
        *text = String("");
    }
    else {
        *text = mText;
    }
    return NOERROR;
}

ECode CKXmlParser::GetTextCharacters(
    /* [in, out] */ ArrayOf<Int32>* poslen,
    /* [out, callee] */ ArrayOf<Char32>** characters)
{
    VALIDATE_NOT_NULL(poslen);
    VALIDATE_NOT_NULL(characters);

    String text;
    GetText(&text);
    if (text.IsNull()) {
        (*poslen)[0] = -1;
        (*poslen)[1] = -1;
        *characters = NULL;
        return NOERROR;
    }
    AutoPtr< ArrayOf<Char32> > result = text.GetChars();
    (*poslen)[0] = 0;
    (*poslen)[1] = result->GetLength();
    *characters = result;
    REFCOUNT_ADD(*characters)
    return NOERROR;
}

ECode CKXmlParser::GetNamespace(
    /* [out] */ String* ns)
{
    VALIDATE_NOT_NULL(ns);
    *ns = mNamespace;
    return NOERROR;
}

ECode CKXmlParser::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode CKXmlParser::GetPrefix(
    /* [out] */ String* prefix)
{
    VALIDATE_NOT_NULL(prefix);
    *prefix = mPrefix;
    return NOERROR;
}

ECode CKXmlParser::IsEmptyElementTag(
    /* [out] */ Boolean* isTag)
{
    VALIDATE_NOT_NULL(isTag);
    if (mType != START_TAG) {
        // throw new XmlPullParserException(ILLEGAL_TYPE, this, null);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    *isTag = mDegenerated;
    return NOERROR;
}

ECode CKXmlParser::GetAttributeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mAttributeCount;
    return NOERROR;
}

ECode CKXmlParser::GetAttributeType(
    /* [in] */ Int32 index,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);
    *type = "CDATA";
    return NOERROR;
}

ECode CKXmlParser::IsAttributeDefault(
    /* [in] */ Int32 index,
    /* [out] */ Boolean* isDef)
{
    VALIDATE_NOT_NULL(isDef);
    *isDef = FALSE;
    return NOERROR;
}

ECode CKXmlParser::GetAttributeNamespace(
    /* [in] */ Int32 index,
    /* [out] */ String* ns)
{
    VALIDATE_NOT_NULL(ns);

    if (index >= mAttributeCount) {
        //throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    *ns = (*mAttributes)[index * 4];
    return NOERROR;
}

ECode CKXmlParser::GetAttributeName(
    /* [in] */ Int32 index,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    if (index >= mAttributeCount) {
        //throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    *name = (*mAttributes)[(index * 4) + 2];
    return NOERROR;
}

ECode CKXmlParser::GetAttributePrefix(
    /* [in] */ Int32 index,
    /* [out] */ String* prefix)
{
    VALIDATE_NOT_NULL(prefix);
    if (index >= mAttributeCount) {
        //throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    *prefix = (*mAttributes)[(index * 4) + 1];
    return NOERROR;
}

ECode CKXmlParser::GetAttributeValue(
    /* [in] */ Int32 index,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    if (index >= mAttributeCount) {
        //throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    *value = (*mAttributes)[(index * 4) + 3];
    return NOERROR;
}

ECode CKXmlParser::GetAttributeValue(
    /* [in] */ const String& ns,
    /* [in] */ const String& name,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    for (Int32 i = (mAttributeCount * 4) - 4; i >= 0; i -= 4) {
        if ((*mAttributes)[i + 2].Equals(name)
                && (ns.IsNull() || (*mAttributes)[i].Equals(ns))) {
            *value = (*mAttributes)[i + 3];
            return NOERROR;
        }
    }

    *value = NULL;
    return NOERROR;
}

ECode CKXmlParser::GetEventType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    return NOERROR;
}

// utility methods to make XML parsing easier ...

ECode CKXmlParser::NextTag(
    /* [out] */ Int32* tag)
{
    VALIDATE_NOT_NULL(tag);

    Int32 tmp = 0;
    FAIL_RETURN(Next(&tmp));
    if (mType == IXmlPullParser::TEXT && mIsWhitespace) {
        FAIL_RETURN(Next(&tmp));
    }

    if (mType != END_TAG && mType != START_TAG) {
        //throw new XmlPullParserException("unexpected type", this, NULL);
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    *tag = mType;
    return NOERROR;
}

ECode CKXmlParser::Require(
    /* [in] */ Int32 type,
    /* [in] */ const String& ns,
    /* [in] */ const String& name)
{
    String theNs, theNm;
    GetNamespace(&theNs);
    GetName(&theNm);
    if (type != mType
            || (!ns.IsNull() && ns.Equals(theNs))
            || (!name.IsNull() && name.Equals(theNm))) {
        // throw new XmlPullParserException(
        //         "expected: " + TYPES[type] + " {" + namespace + "}" + name, this, NULL);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    return NOERROR;
}

ECode CKXmlParser::NextText(
    /* [out] */ String* text)
{
    VALIDATE_NOT_NULL(text);

    if (mType != START_TAG) {
        //throw new XmlPullParserException("precondition: START_TAG", this, NULL);
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    Int32 tmp = 0;
    FAIL_RETURN(Next(&tmp));

    if (mType == IXmlPullParser::TEXT) {
        FAIL_RETURN(GetText(text));
        FAIL_RETURN(Next(&tmp));
    }
    else {
        *text = "";
    }

    if (mType != END_TAG) {
        //throw new XmlPullParserException("END_TAG expected", this, NULL);
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    return NOERROR;
}

ECode CKXmlParser::SetFeature(
    /* [in] */ const String& feature,
    /* [in] */ Boolean value)
{
    if (FEATURE_PROCESS_NAMESPACES.Equals(feature)) {
        mProcessNsp = value;
    }
    else if (FEATURE_PROCESS_DOCDECL.Equals(feature)) {
        mProcessDocDecl = value;
    }
    else if (FEATURE_RELAXED.Equals(feature)) {
        mRelaxed = value;
    }
    else {
        //throw new XmlPullParserException("unsupported feature: " + feature, this, NULL);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    return NOERROR;
}

ECode CKXmlParser::SetProperty(
    /* [in] */ const String& property,
    /* [in] */ IInterface* value)
{
    if (property.Equals(PROPERTY_LOCATION)) {
        assert(value != NULL && ICharSequence::Probe(value) != NULL);
        ICharSequence::Probe(value)->ToString(&mLocation);
    }
    else {
        //throw new XmlPullParserException("unsupported property: " + property);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    return NOERROR;
}

/**
 * Prepends the characters of {@code newBuffer} to be read before the
 * current buffer.
 */
void CKXmlParser::PushContentSource(
    /* [in] */ ArrayOf<Char32>* newBuffer)
{
    assert(newBuffer != NULL);
    mNextContentSource = new ContentSource(mNextContentSource, mBuffer, mPosition, mLimit);
    mBuffer = newBuffer;
    mPosition = 0;
    mLimit = newBuffer->GetLength();
}

/**
 * Replaces the current exhausted buffer with the Next buffer in the chain.
 */
void CKXmlParser::PopContentSource()
{
    assert(mNextContentSource != NULL);
    mBuffer = mNextContentSource->mBuffer;
    mPosition = mNextContentSource->mPosition;
    mLimit = mNextContentSource->mLimit;
    mNextContentSource = mNextContentSource->mNext;
}

} //namespace IO
} //namespace Kxml2
} //namespace Org
