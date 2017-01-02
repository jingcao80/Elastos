//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef  __ORG_KXML2_IO_CKXMLPARSER_H__
#define  __ORG_KXML2_IO_CKXMLPARSER_H__

#include "_Org_Kxml2_IO_CKXmlParser.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::IBoolean;
using Elastos::Utility::Etl::HashMap;
using Elastos::IO::ICloseable;
using Elastos::IO::IReader;
using Elastos::IO::IInputStream;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Kxml2::IO::IKXmlParser;

namespace Org {
namespace Kxml2 {
namespace IO {


/**
 * An XML pull parser with limited support for parsing internal DTDs.
 */
CarClass(CKXmlParser)
    , public Object
    , public IKXmlParser
    , public IXmlPullParser
    , public ICloseable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Where a value is found impacts how that value is interpreted. For
     * example, in attributes, "\n" must be replaced with a space character. In
     * text, "]]>" is forbidden. In entity declarations, named references are
     * not resolved.
     */
    enum ValueContext {
        ATTRIBUTE = 100,
        TEXT = 101,
        ENTITY_DECLARATION = 102
    };

private:
    /**
     * A chain of buffers containing XML content. Each content source contains
     * the parser's primary read buffer or the characters of entities actively
     * being parsed.
     *
     * <p>For example, note the buffers needed to parse this document:
     * <pre>   {@code
     *   <!DOCTYPE foo [
     *       <!ENTITY baz "ghi">
     *       <!ENTITY bar "def &baz; jkl">
     *   ]>
     *   <foo>abc &bar; mno</foo>
     * }</pre>
     *
     * <p>Things get interesting when the bar entity is encountered. At that
     * point two buffers are active:
     * <ol>
     * <li>The value for the bar entity, containing {@code "def &baz; jkl"}
     * <li>The parser's primary read buffer, containing {@code " mno</foo>"}
     * </ol>
     * <p>The parser will return the characters {@code "def "} from the bar
     * entity's buffer, and then it will encounter the baz entity. To handle
     * that, three buffers will be active:
     * <ol>
     * <li>The value for the baz entity, containing {@code "ghi"}
     * <li>The remaining value for the bar entity, containing {@code " jkl"}
     * <li>The parser's primary read buffer, containing {@code " mno</foo>"}
     * </ol>
     * <p>The parser will then return the characters {@code ghi jkl mno} in that
     * sequence by reading each buffer in sequence.
     */
    class ContentSource : public Object
    {
        friend class CKXmlParser;

    public:
        ContentSource(
            /* [in] */ ContentSource* next,
            /* [in] */ ArrayOf<Char32>* buffer,
            /* [in] */ Int32 position,
            /* [in] */ Int32 limit);

    private:
        const AutoPtr<ContentSource> mNext;
        const AutoPtr< ArrayOf<Char32> > mBuffer;
        const Int32 mPosition;
        const Int32 mLimit;
    };

public:
    CKXmlParser();

    ~CKXmlParser();

    /**
     * Retains namespace attributes like {@code xmlns="http://foo"} or {@code xmlns:foo="http:foo"}
     * in pulled elements. Most applications will only be interested in the effective namespaces of
     * their elements, so these attributes aren't useful. But for structure preserving wrappers like
     * DOM, it is necessary to keep the namespace data around.
     */
    CARAPI KeepNamespaceAttributes();

    CARAPI Next(
        /* [out] */ Int32* value);

    CARAPI NextToken(
        /* [out] */ Int32* value);

    //  public part starts here...

    CARAPI SetInput(
        /* [in] */ IReader* reader);

    CARAPI SetInput(
        /* [in] */ IInputStream* is,
        /* [in] */ const String& charset);

    CARAPI Close();

    CARAPI GetFeature(
        /* [in] */ const String& feature,
        /* [out] */ Boolean* flag);

    CARAPI GetInputEncoding(
        /* [out] */ String* encoding);

    CARAPI DefineEntityReplacementText(
        /* [in] */ const String& entity,
        /* [in] */ const String& value);

    CARAPI GetProperty(
        /* [in] */ const String& property,
        /* [out] */ IInterface** value);

    /**
     * Returns the root element's name if it was declared in the DTD. This
     * equals the first tag's name for valid documents.
     */
    CARAPI GetRootElementName(
        /* [out] */ String* name);

    /**
     * Returns the document's system ID if it was declared. This is typically a
     * string like {@code http://www.w3.org/TR/html4/strict.dtd}.
     */
    CARAPI GetSystemId(
        /* [out] */ String* id);

    /**
     * Returns the document's public ID if it was declared. This is typically a
     * string like {@code -//W3C//DTD HTML 4.01//EN}.
     */
    CARAPI GetPublicId(
        /* [out] */ String* id);

    CARAPI GetNamespaceCount(
        /* [in] */ Int32 depth,
        /* [out] */ Int32* count);

    CARAPI GetNamespacePrefix(
        /* [in] */ Int32 pos,
        /* [out] */ String* prefix);

    CARAPI GetNamespaceUri(
        /* [in] */ Int32 pos,
        /* [out] */ String* uri);

    CARAPI GetNamespace(
        /* [in] */ const String& prefix,
        /* [out] */ String* ns);

    CARAPI GetDepth(
        /* [out] */ Int32* depth);

    CARAPI GetPositionDescription(
        /* [out] */ String* des);

    CARAPI GetLineNumber(
        /* [out] */ Int32* num);

    CARAPI GetColumnNumber(
        /* [out] */ Int32* num);

    CARAPI IsWhitespace(
        /* [out] */ Boolean* space);

    CARAPI GetText(
        /* [out] */ String* text);

    CARAPI GetTextCharacters(
        /* [in, out] */ ArrayOf<Int32>* poslen,
        /* [out, callee] */ ArrayOf<Char32>** characters);

    CARAPI GetNamespace(
        /* [out] */ String* ns);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetPrefix(
        /* [out] */ String* prefix);

    CARAPI IsEmptyElementTag(
        /* [out] */ Boolean* isTag);

    CARAPI GetAttributeCount(
        /* [out] */ Int32* count);

    CARAPI GetAttributeType(
        /* [in] */ Int32 index,
        /* [out] */ String* type);

    CARAPI IsAttributeDefault(
        /* [in] */ Int32 index,
        /* [out] */ Boolean* isDef);

    CARAPI GetAttributeNamespace(
        /* [in] */ Int32 index,
        /* [out] */ String* ns);

    CARAPI GetAttributeName(
        /* [in] */ Int32 index,
        /* [out] */ String* name);

    CARAPI GetAttributePrefix(
        /* [in] */ Int32 index,
        /* [out] */ String* prefix);

    CARAPI GetAttributeValue(
        /* [in] */ Int32 index,
        /* [out] */ String* value);

    CARAPI GetAttributeValue(
        /* [in] */ const String& ns,
        /* [in] */ const String& name,
        /* [out] */ String* value);

    CARAPI GetEventType(
        /* [out] */ Int32* type);

    // utility methods to make XML parsing easier ...

    CARAPI NextTag(
        /* [out] */ Int32* tag);

    CARAPI Require(
        /* [in] */ Int32 type,
        /* [in] */ const String& ns,
        /* [in] */ const String& name);

    CARAPI NextText(
        /* [out] */ String* tag);

    CARAPI SetFeature(
        /* [in] */ const String& feature,
        /* [in] */ Boolean value);

    CARAPI SetProperty(
        /* [in] */ const String& property,
        /* [in] */ IInterface* value);

private:
    CARAPI AdjustNsp(
        /* [out] */ Boolean* result);

    CARAPI_(AutoPtr< ArrayOf<String> >) EnsureCapacity(
        /* [in] */ ArrayOf<String>* arr,
        /* [in] */ Int32 required);

    CARAPI CheckRelaxed(
        /* [in] */ const String& errorMessage);

    CARAPI Next(
        /* [in] */ Boolean justOneToken,
        /* [out] */ Int32* token);

    /**
     * Reads text until the specified delimiter is encountered. Consumes the
     * text and the delimiter.
     *
     * @param returnText true to return the read text excluding the delimiter;
     *     false to return null.
     */
    CARAPI ReadUntil(
        /* [in] */ const ArrayOf<Char32>& delimiter,
        /* [in] */ Boolean returnText,
        /* [out] */ String* value);

    /**
     * Returns true if an XML declaration was read.
     */
    CARAPI ReadXmlDeclaration();

    CARAPI ReadComment(
        /* [in] */ Boolean returnText,
        /* [out] */ String* value);

    /**
     * Read the document's DTD. Although this parser is non-validating, the DTD
     * must be parsed to capture entity values and default attribute values.
     */
    CARAPI ReadDoctype(
        /* [in] */ Boolean saveDtdText);

    /**
     * Reads an external ID of one of these two forms:
     *   SYSTEM "quoted system name"
     *   PUBLIC "quoted public id" "quoted system name"
     *
     * If the system name is not required, this also supports lone public IDs of
     * this form:
     *   PUBLIC "quoted public id"
     *
     * Returns true if any ID was read.
     */
    CARAPI ReadExternalId(
        /* [in] */ Boolean requireSystemName,
        /* [in] */ Boolean assignFields,
        /* [out] */ Boolean* result);

    /**
     * Reads a quoted string, performing no entity escaping of the contents.
     */
    CARAPI ReadQuotedId(
        /* [in] */ Boolean returnText,
        /* [out] */ String* value);

    CARAPI ReadInternalSubset();

    /**
     * Read an element declaration. This contains a name and a content spec.
     *   <!ELEMENT foo EMPTY >
     *   <!ELEMENT foo (bar?,(baz|quux)) >
     *   <!ELEMENT foo (#PCDATA|bar)* >
     */
    CARAPI ReadElementDeclaration();

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
    CARAPI ReadContentSpec();

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
    CARAPI ReadAttributeListDeclaration();

    CARAPI_(void) DefineAttributeDefault(
        /* [in] */ const String& elementName,
        /* [in] */ const String& attributeName,
        /* [in] */ const String& value);

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
    CARAPI ReadEntityDeclaration();

    CARAPI ReadNotationDeclaration();

    CARAPI ReadEndTag();

    /**
     * Returns the type of the next token.
     */
    CARAPI PeekType(
        /* [in] */ Boolean inDeclaration,
        /* [out] */ Int32* token);

    /**
     * Sets name and attributes
     */
    CARAPI ParseStartTag(
        /* [in] */ Boolean xmldecl,
        /* [in] */ Boolean throwOnResolveFailure);

    /**
     * Reads an entity reference from the buffer, resolves it, and writes the
     * resolved entity to {@code out}. If the entity cannot be read or resolved,
     * {@code out} will contain the partial entity reference.
     */
    CARAPI ReadEntity(
        /* [in] */ StringBuilder& out,
        /* [in] */ Boolean isEntityToken,
        /* [in] */ Boolean throwOnResolveFailure,
        /* [in] */ ValueContext valueContext);

    /**
     * Returns the current text or attribute value. This also has the side
     * effect of setting isWhitespace to false if a non-whitespace character is
     * encountered.
     *
     * @param delimiter {@code <} for text, {@code "} and {@code '} for quoted
     *     attributes, or a space for unquoted attributes.
     */
    CARAPI ReadValue(
        /* [in] */ Char32 delimiter,
        /* [in] */ Boolean resolveEntities,
        /* [in] */ Boolean throwOnResolveFailure,
        /* [in] */ ValueContext valueContext,
        /* [out] */ String* value);

    CARAPI Read(
        /* [in] */ Char32 expected);

    CARAPI Read(
        /* [int] */ const ArrayOf<Char32>& chars);

    Int32 PeekCharacter();

    /**
     * Returns true once {@code limit - position >= minimum}. If the data is
     * exhausted before that many characters are available, this returns
     * false.
     */
    CARAPI FillBuffer(
        /* [in] */ Int32 minimum,
        /* [out] */ Boolean* result);

    /**
     * Returns an element or attribute name. This is always non-empty for
     * non-relaxed parsers.
     */
    CARAPI ReadName(
        /* [out] */ String* value);

    CARAPI Skip();

    /**
     * Prepends the characters of {@code newBuffer} to be read before the
     * current buffer.
     */
    CARAPI_(void) PushContentSource(
        /* [in] */ ArrayOf<Char32>* newBuffer);

    /**
     * Replaces the current exhausted buffer with the next buffer in the chain.
     */
    CARAPI_(void) PopContentSource();

private:
    typedef HashMap<String, String> StringMap;

    static String PROPERTY_XMLDECL_VERSION;
    static String PROPERTY_XMLDECL_STANDALONE;
    static String PROPERTY_LOCATION;
    static String FEATURE_RELAXED;

    static AutoPtr< StringMap > DEFAULT_ENTITIES;

    static const Int32 ELEMENTDECL = 11;
    static const Int32 ENTITYDECL = 12;
    static const Int32 ATTLISTDECL = 13;
    static const Int32 NOTATIONDECL = 14;
    static const Int32 PARAMETER_ENTITY_REF = 15;

    static const AutoPtr<ArrayOf<Char32> > START_COMMENT;
    static const AutoPtr<ArrayOf<Char32> > END_COMMENT;
    static const AutoPtr<ArrayOf<Char32> > COMMENT_DOUBLE_DASH;
    static const AutoPtr<ArrayOf<Char32> > START_CDATA;
    static const AutoPtr<ArrayOf<Char32> > END_CDATA;
    static const AutoPtr<ArrayOf<Char32> > START_PROCESSING_INSTRUCTION;
    static const AutoPtr<ArrayOf<Char32> > END_PROCESSING_INSTRUCTION;
    static const AutoPtr<ArrayOf<Char32> > START_DOCTYPE;
    static const AutoPtr<ArrayOf<Char32> > SYSTEM;
    static const AutoPtr<ArrayOf<Char32> > PUBLIC;
    static const AutoPtr<ArrayOf<Char32> > START_ELEMENT;
    static const AutoPtr<ArrayOf<Char32> > START_ATTLIST;
    static const AutoPtr<ArrayOf<Char32> > START_ENTITY;
    static const AutoPtr<ArrayOf<Char32> > START_NOTATION;
    static const AutoPtr<ArrayOf<Char32> > EMPTY;
    static const AutoPtr<ArrayOf<Char32> > ANY;
    static const AutoPtr<ArrayOf<Char32> > NDATA;
    static const AutoPtr<ArrayOf<Char32> > NOTATION;
    static const AutoPtr<ArrayOf<Char32> > REQUIRED;
    static const AutoPtr<ArrayOf<Char32> > IMPLIED;
    static const AutoPtr<ArrayOf<Char32> > FIXED;

    static const String UNEXPECTED_EOF;
    static const String ILLEGAL_TYPE;
    static const Int32 XML_DECLARATION = 998;

    // general
    String mLocation;

    String mVersion;
    AutoPtr<IBoolean> mStandalone;
    String mRootElementName;
    String mSystemId;
    String mPublicId;

    /**
     * True if the {@code <!DOCTYPE>} contents are handled. The DTD defines
     * entity values and default attribute values. These values are parsed at
     * inclusion time and may contain both tags and entity references.
     *
     * <p>If this is false, the user must {@link #defineEntityReplacementText
     * define entity values manually}. Such entity values are literal strings
     * and will not be parsed. There is no API to define default attributes
     * manually.
     */
    Boolean mProcessDocDecl;
    Boolean mProcessNsp;
    Boolean mRelaxed;
    Boolean mKeepNamespaceAttributes;

    /**
     * If non-null, the contents of the read buffer must be copied into this
     * string builder before the read buffer is overwritten. This is used to
     * capture the raw DTD text while parsing the DTD.
     */
    AutoPtr<StringBuilder> mBufferCapture;

    /**
     * Entities defined in or for this document. This map is created lazily.
     */
    AutoPtr< HashMap<String, AutoPtr< ArrayOf<Char32> > > > mDocumentEntities;

    /**
     * Default attributes in this document. The outer map's key is the element
     * name; the inner map's key is the attribute name. Both keys should be
     * without namespace adjustments. This map is created lazily.
     */
    AutoPtr< HashMap<String, AutoPtr<StringMap> > > mDefaultAttributes;


    Int32 mDepth;
    AutoPtr< ArrayOf<String> > mElementStack;
    AutoPtr< ArrayOf<String> > mNspStack;
    AutoPtr< ArrayOf<Int32> > mNspCounts;

    // source

    AutoPtr<IReader> mReader;
    String mEncoding;
    AutoPtr<ContentSource> mNextContentSource;
    AutoPtr< ArrayOf<Char32> > mBuffer;
    Int32 mPosition;
    Int32 mLimit;

    /*
     * Track the number of newlines and columns preceding the current buffer. To
     * compute the line and column of a position in the buffer, compute the line
     * and column in the buffer and add the preceding values.
     */
    Int32 mBufferStartLine;
    Int32 mBufferStartColumn;

    // the current token

    Int32 mType;
    Boolean mIsWhitespace;
    String mNamespace;
    String mPrefix;
    String mName;
    String mText;

    Boolean mDegenerated;
    Int32 mAttributeCount;

    // true iff. we've encountered the START_TAG of an XML element at depth == 0;
    Boolean mParsedTopLevelStartTag;

    /*
     * The current element's attributes arranged in groups of 4:
     * i + 0 = attribute namespace URI
     * i + 1 = attribute namespace prefix
     * i + 2 = attribute qualified name (may contain ":", as in "html:h1")
     * i + 3 = attribute value
     */
    AutoPtr< ArrayOf<String> > mAttributes;

    String mError;

    Boolean mUnresolved;

    static const AutoPtr<ArrayOf<Char32> > SINGLE_QUOTE;
    static const AutoPtr<ArrayOf<Char32> > DOUBLE_QUOTE;

    /**
     * This array can be used to convert the event type integer constants
     * such as START_TAG or TEXT to
     * to a string. For example, the value of TYPES[START_TAG] is
     * the string "START_TAG".
     *
     * This array is intended for diagnostic output only. Relying
     * on the contents of the array may be dangerous since malicious
     * applications may alter the array, although it is final, due
     * to limitations of the Java language.
     */
    // copy from XmlPullParser.java
    static const String TYPES[];
};

} //namespace IO
} //namespace Kxml2
} //namespace Org

#endif //__ORG_KXML2_IO_CKXMLPARSER_H__
