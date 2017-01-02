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

#include "Elastos.CoreLibrary.External.h"
#include "org/apache/harmony/xml/parsers/DocumentBuilderImpl.h"
#include "org/apache/harmony/xml/dom/CDOMImplementationImplHelper.h"
#include "org/apache/harmony/xml/dom/CDocumentImpl.h"
#include "org/kxml2/io/CKXmlParser.h"
#include "libcore/io/IoUtils.h"
#include "elastos/core/StringUtils.h"
#include <elastos/utility/logging/Logger.h>

using Libcore::IO::IoUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Harmony::Xml::Dom::CDOMImplementationImplHelper;
using Org::Apache::Harmony::Xml::Dom::CDocumentImpl;
using Org::Apache::Harmony::Xml::Dom::IDOMImplementationImplHelper;
using Org::Kxml2::IO::CKXmlParser;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::W3c::Dom::IAttr;
using Org::W3c::Dom::INode;
using Org::W3c::Dom::IComment;
using Org::W3c::Dom::IElement;
using Org::W3c::Dom::IEntityReference;
using Org::W3c::Dom::IProcessingInstruction;
using Org::W3c::Dom::IText;
using Org::W3c::Dom::ICharacterData;

//import java.io.IOException;
//import java.net.URL;
//import java.net.URLConnection;
//import javax.xml.parsers.DocumentBuilder;
//import libcore.io.IoUtils;
//import org.apache.harmony.xml.dom.CDATASectionImpl;
//import org.apache.harmony.xml.dom.DOMImplementationImpl;
//import org.apache.harmony.xml.dom.DocumentImpl;
//import org.apache.harmony.xml.dom.DocumentTypeImpl;
//import org.apache.harmony.xml.dom.TextImpl;
//import org.kxml2.io.KXmlParser;
//import org.w3c.dom.Attr;
//import org.w3c.dom.DOMImplementation;
//import org.w3c.dom.Document;
//import org.w3c.dom.DocumentType;
//import org.w3c.dom.Element;
//import org.w3c.dom.Node;
//import org.w3c.dom.Text;
//import org.xml.sax.EntityResolver;
//import org.xml.sax.ErrorHandler;
//import org.xml.sax.InputSource;
//import org.xml.sax.SAXException;
//import org.xml.sax.SAXParseException;
//import org.xml.sax.helpers.LocatorImpl;
//import org.xmlpull.v1.XmlPullParser;
//import org.xmlpull.v1.XmlPullParserException;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Parsers {

static AutoPtr<IDOMImplementation> InitDom()
{
    AutoPtr<IDOMImplementation> result;
    AutoPtr<IDOMImplementationImplHelper> helper;
    CDOMImplementationImplHelper::AcquireSingleton((IDOMImplementationImplHelper**)&helper);
    helper->GetInstance((IDOMImplementation**)&result);
    return result;
}

AutoPtr<IDOMImplementation> DocumentBuilderImpl::dom = InitDom();

DocumentBuilderImpl::DocumentBuilderImpl()
    : coalescing(FALSE)
    , ignoreComments(FALSE)
    , ignoreElementContentWhitespace(FALSE)
    , namespaceAware(FALSE)
{
}

ECode DocumentBuilderImpl::Reset()
{
    coalescing = FALSE;
    entityResolver = NULL;
    errorHandler = NULL;
    ignoreComments = FALSE;
    ignoreElementContentWhitespace = FALSE;
    namespaceAware = FALSE;
    return NOERROR;
}

ECode DocumentBuilderImpl::GetDOMImplementation(
    /* [out] */ IDOMImplementation** impl)
{
    VALIDATE_NOT_NULL(impl);
    *impl = dom;
    REFCOUNT_ADD(*impl);
    return NOERROR;
}

ECode DocumentBuilderImpl::IsNamespaceAware(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = namespaceAware;
    return NOERROR;
}

ECode DocumentBuilderImpl::IsValidating(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode DocumentBuilderImpl::NewDocument(
    /* [out] */ IDocument** doc)
{
    return dom->CreateDocument(String(NULL), String(NULL), NULL, doc);
}

ECode DocumentBuilderImpl::Parse(
    /* [in] */ IInputSource* source,
    /* [out] */ IDocument** doc)
{
    VALIDATE_NOT_NULL(doc);
    *doc = NULL;

    if (source == NULL) {
        //throw new IllegalArgumentException("source == null");
        Logger::E("DocumentBuilderImpl", "Parse IllegalArgumentException source == null");
        assert(0);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String namespaceURI;
    String qualifiedName;
    AutoPtr<IDocumentType> doctype;
    String inputEncoding;
    source->GetEncoding(&inputEncoding);
    String systemId;
    source->GetSystemId(&systemId);
    AutoPtr<IDocument> document;
    CDocumentImpl::New(dom, namespaceURI, qualifiedName, doctype, inputEncoding, (IDocument**)&document);
    document->SetDocumentURI(systemId);

    AutoPtr<IKXmlParser> parser;
    CKXmlParser::New((IKXmlParser**)&parser);
    //TODO
    assert(0);
    //try {
        //parser.keepNamespaceAttributes();
        //parser.setFeature(XmlPullParser.FEATURE_PROCESS_NAMESPACES, namespaceAware);

        //if (source.getByteStream() != null) {
        //    parser.setInput(source.getByteStream(), inputEncoding);
        //} else if (source.getCharacterStream() != null) {
        //    parser.setInput(source.getCharacterStream());
        //} else if (systemId != null) {
        //    URL url = new URL(systemId);
        //    URLConnection urlConnection = url.openConnection();
        //    urlConnection.connect();
        //    // TODO: if null, extract the inputEncoding from the Content-Type header?
        //    parser.setInput(urlConnection.getInputStream(), inputEncoding);
        //} else {
        //    throw new SAXParseException("InputSource needs a stream, reader or URI", null);
        //}

        //if (parser.nextToken() == XmlPullParser.END_DOCUMENT) {
        //    throw new SAXParseException("Unexpected end of document", null);
        //}

        //parse(parser, document, document, XmlPullParser.END_DOCUMENT);

        //parser.require(XmlPullParser.END_DOCUMENT, null, null);
    //} catch (XmlPullParserException ex) {
        //if (ex.getDetail() instanceof IOException) {
        //    throw (IOException) ex.getDetail();
        //}
        //if (ex.getDetail() instanceof RuntimeException) {
        //    throw (RuntimeException) ex.getDetail();
        //}

        //LocatorImpl locator = new LocatorImpl();

        //locator.setPublicId(source.getPublicId());
        //locator.setSystemId(systemId);
        //locator.setLineNumber(ex.getLineNumber());
        //locator.setColumnNumber(ex.getColumnNumber());

        //SAXParseException newEx = new SAXParseException(ex.getMessage(), locator);

        //if (errorHandler != null) {
        //    errorHandler.error(newEx);
        //}

        //throw newEx;
    //} finally {
    IoUtils::CloseQuietly(ICloseable::Probe(parser));
    //}
    *doc = document;
    REFCOUNT_ADD(*doc);
    return NOERROR;
}

void DocumentBuilderImpl::Parse(
    /* [in] */ IKXmlParser* parser,
    /* [in] */ IDocument* document,
    /* [in] */ INode* node,
    /* [in] */ Int32 endToken)
{
    Int32 token;
    IXmlPullParser::Probe(parser)->GetEventType(&token);

    /*
     * The main parsing loop. The precondition is that we are already on the
     * token to be processed. This holds for each iteration of the loop, so
     * the inner statements have to ensure that (in particular the recursive
     * call).
     */
    while (token != endToken && token != IXmlPullParser::END_DOCUMENT) {
        if (token == IXmlPullParser::PROCESSING_INSTRUCTION) {
            /*
             * Found a processing instructions. We need to split the token
             * text at the first whitespace character.
             */
            String text;
            IXmlPullParser::Probe(parser)->GetText(&text);

            Int32 dot = text.IndexOf(' ');

            String target = (dot != -1 ? text.Substring(0, dot) : text);
            String data = (dot != -1 ? text.Substring(dot + 1) : String(""));

            AutoPtr<IProcessingInstruction> pi;
            document->CreateProcessingInstruction(target, data, (IProcessingInstruction**)&pi);
            AutoPtr<INode> temp;
            node->AppendChild(INode::Probe(pi), (INode**)&temp);
        } else if (token == IXmlPullParser::DOCDECL) {
            String name;
            parser->GetRootElementName(&name);
            String publicId;
            parser->GetPublicId(&publicId);
            String systemId;
            parser->GetSystemId(&systemId);
            //TODO document->AppendChild(new DocumentTypeImpl(document, name, publicId, systemId));
            assert(0);

        } else if (token == IXmlPullParser::COMMENT) {
            /*
             * Found a comment. We simply take the token text, but we only
             * create a node if the client wants to see comments at all.
             */
            if (!ignoreComments) {
                String text;
                IXmlPullParser::Probe(parser)->GetText(&text);
                AutoPtr<IComment> comment;
                document->CreateComment(text, (IComment**)&comment);
                AutoPtr<INode> tmp;
                node->AppendChild(INode::Probe(comment), (INode**)&tmp);
            }
        } else if (token == IXmlPullParser::IGNORABLE_WHITESPACE) {
            /*
             * Found some ignorable whitespace. We only add it if the client
             * wants to see whitespace. Whitespace before and after the
             * document element is always ignored.
             */
            if (!ignoreElementContentWhitespace && INode::Probe(document) != node) {
                String text;
                IXmlPullParser::Probe(parser)->GetText(&text);
                AppendText(document, node, token, text);
            }
        } else if (token == IXmlPullParser::TEXT || token == IXmlPullParser::CDSECT) {
            /*
             * Found a piece of text (possibly encoded as a CDATA section).
             * That's the easiest case. We simply take it and create a new text node,
             * or merge with an adjacent text node.
             */
            String text;
            IXmlPullParser::Probe(parser)->GetText(&text);
            AppendText(document, node, token, text);
        } else if (token == IXmlPullParser::ENTITY_REF) {
            /*
             * Found an entity reference. If an entity resolver is
             * installed, we replace it by text (if possible). Otherwise we
             * add an entity reference node.
             */
            String entity;
            IXmlPullParser::Probe(parser)->GetName(&entity);

            if (entityResolver != NULL) {
                // TODO Implement this...
            }

            String resolved = ResolvePredefinedOrCharacterEntity(entity);
            if (resolved != NULL) {
                AppendText(document, node, token, resolved);
            } else {
                AutoPtr<IEntityReference> entityReference;
                document->CreateEntityReference(entity, (IEntityReference**)&entityReference);
                AutoPtr<INode> tmp;
                node->AppendChild(INode::Probe(entityReference), (INode**)&tmp);
            }
        } else if (token == IXmlPullParser::START_TAG) {
            /*
             * Found an element start tag. We create an element node with
             * the proper info and attributes. We then invoke parse()
             * recursively to handle the next level of nesting. When we
             * return from this call, we check that we are on the proper
             * element end tag. The whole handling differs somewhat
             * depending on whether the parser is namespace-aware or not.
             */
            if (namespaceAware) {
                // Collect info for element node
                String ns;
                IXmlPullParser::Probe(parser)->GetNamespace(&ns);
                String name;
                IXmlPullParser::Probe(parser)->GetName(&name);
                String prefix;
                IXmlPullParser::Probe(parser)->GetPrefix(&prefix);

                if (ns.IsEmpty()) {
                    ns = String(NULL);
                }

                // Create element node and wire it correctly
                AutoPtr<IElement> element;
                document->CreateElementNS(ns, name, (IElement**)&element);
                INode::Probe(element)->SetPrefix(prefix);
                AutoPtr<INode> tmp;
                node->AppendChild(INode::Probe(element), (INode**)&tmp);

                Int32 acount;
                IXmlPullParser::Probe(parser)->GetAttributeCount(&acount);
                for (Int32 i = 0; i < acount; i++) {
                    // Collect info for a single attribute node
                    String attrNamespace;
                    IXmlPullParser::Probe(parser)->GetAttributeNamespace(i, &attrNamespace);
                    String attrPrefix;
                    IXmlPullParser::Probe(parser)->GetAttributePrefix(i, &attrPrefix);
                    String attrName;
                    IXmlPullParser::Probe(parser)->GetAttributeName(i, &attrName);
                    String attrValue;
                    IXmlPullParser::Probe(parser)->GetAttributeValue(i, &attrValue);

                    if (attrNamespace.IsEmpty()) {
                        attrNamespace = String(NULL);
                    }

                    // Create attribute node and wire it correctly
                    AutoPtr<IAttr> attr;
                    document->CreateAttributeNS(attrNamespace, attrName, (IAttr**)&attr);
                    INode::Probe(attr)->SetPrefix(attrPrefix);
                    attr->SetValue(attrValue);
                    AutoPtr<IAttr> tmp;
                    element->SetAttributeNodeNS(attr, (IAttr**)&tmp);
                }

                // Recursive descent
                IXmlPullParser::Probe(parser)->NextToken(&token);
                Parse(parser, document, INode::Probe(element), IXmlPullParser::END_TAG);

                // Expect the element's end tag here
                IXmlPullParser::Probe(parser)->Require(IXmlPullParser::END_TAG, ns, name);

            } else {
                // Collect info for element node
                String name;
                IXmlPullParser::Probe(parser)->GetName(&name);

                // Create element node and wire it correctly
                AutoPtr<IElement> element;
                document->CreateElement(name, (IElement**)&element);
                AutoPtr<INode> tmp;
                node->AppendChild(INode::Probe(element), (INode**)&tmp);

                Int32 acount;
                IXmlPullParser::Probe(parser)->GetAttributeCount(&acount);
                for (Int32 i = 0; i < acount; i++) {
                    // Collect info for a single attribute node
                    String attrName;
                    IXmlPullParser::Probe(parser)->GetAttributeName(i, &attrName);
                    String attrValue;
                    IXmlPullParser::Probe(parser)->GetAttributeValue(i, &attrValue);

                    // Create attribute node and wire it correctly
                    AutoPtr<IAttr> attr;
                    document->CreateAttribute(attrName, (IAttr**)&attr);
                    attr->SetValue(attrValue);
                    AutoPtr<IAttr> tmp;
                    element->SetAttributeNode(attr, (IAttr**)&tmp);
                }

                // Recursive descent
                IXmlPullParser::Probe(parser)->NextToken(&token);
                Parse(parser, document, INode::Probe(element), IXmlPullParser::END_TAG);

                // Expect the element's end tag here
                IXmlPullParser::Probe(parser)->Require(IXmlPullParser::END_TAG, String(""), name);
            }
        }

        IXmlPullParser::Probe(parser)->NextToken(&token);
    }
}

void DocumentBuilderImpl::AppendText(
    /* [in] */ IDocument* document,
    /* [in] */ INode* parent,
    /* [in] */ Int32 token,
    /* [in] */ const String& text)
{
    // Ignore empty runs.
    if (text.IsEmpty()) {
        return;
    }
    // Merge with any previous text node if possible.
    if (coalescing || token != IXmlPullParser::CDSECT) {
        AutoPtr<INode> lastChild;
        parent->GetLastChild((INode**)&lastChild);
        Int16 nodeType;
        if (lastChild != NULL && (lastChild->GetNodeType(&nodeType), nodeType) == INode::TEXT_NODE) {
            IText* textNode = IText::Probe(lastChild);
            ICharacterData::Probe(textNode)->AppendData(text);
            return;
        }
    }
    // Okay, we really do need a new text node
    //TODO
    assert(0);
    //parent->AppendChild(token == IXmlPullParser::CDSECT
    //        ? new CDATASectionImpl(document, text)
    //        : new TextImpl(document, text));
}

ECode DocumentBuilderImpl::SetEntityResolver(
    /* [in] */ IEntityResolver* resolver)
{
    entityResolver = resolver;
    return NOERROR;
}

ECode DocumentBuilderImpl::SetErrorHandler(
    /* [in] */ IErrorHandler* handler)
{
    errorHandler = handler;
    return NOERROR;
}

ECode DocumentBuilderImpl::SetIgnoreComments(
    /* [in] */ Boolean value)
{
    ignoreComments = value;
    return NOERROR;
}

ECode DocumentBuilderImpl::SetCoalescing(
    /* [in] */ Boolean value)
{
    coalescing = value;
    return NOERROR;
}

ECode DocumentBuilderImpl::SetIgnoreElementContentWhitespace(
    /* [in] */ Boolean value)
{
    ignoreElementContentWhitespace = value;
    return NOERROR;
}

ECode DocumentBuilderImpl::SetNamespaceAware(
    /* [in] */ Boolean value)
{
    namespaceAware = value;
    return NOERROR;
}

String DocumentBuilderImpl::ResolvePredefinedOrCharacterEntity(
    /* [in] */ const String& entityName)
{
    // Character references, section 4.1 of the XML specification.
    if (entityName.StartWith("#x")) {
        return ResolveCharacterReference(entityName.Substring(2), 16);
    } else if (entityName.StartWith("#")) {
        return ResolveCharacterReference(entityName.Substring(1), 10);
    }
    // Predefined entities, section 4.6 of the XML specification.
    if (String("lt").Equals(entityName)) {
        return String("<");
    } else if (String("gt").Equals(entityName)) {
        return String(">");
    } else if (String("amp").Equals(entityName)) {
        return String("&");
    } else if (String("apos").Equals(entityName)) {
        return String("'");
    } else if (String("quot").Equals(entityName)) {
        return String("\"");
    } else {
        return String(NULL);
    }
}

String DocumentBuilderImpl::ResolveCharacterReference(
    /* [in] */ const String& value,
    /* [in] */ Int32 base)
{
    //TODO
    assert(0);
    //try {
    Int32 ch = StringUtils::ParseInt32(value, base);
    assert(ch); // TODO remove
    //TODO
    //if (ch < 0 || ch > Character.MAX_VALUE) {
    //    return String(NULL);
    //}
    //TODO return String.valueOf((char) ch);
    return String(NULL);
    //} catch (NumberFormatException ex) {
    //    return null;
    //}
}

} // Parsers
} // Xml
} // Harmony
} // Apache
} // Org
