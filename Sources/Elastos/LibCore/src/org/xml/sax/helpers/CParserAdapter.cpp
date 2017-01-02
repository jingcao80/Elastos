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

#include "Elastos.CoreLibrary.Utility.h"
#include "CParserAdapter.h"
#include "CInputSource.h"
#include "CParserFactory.h"
#include "CAttributesImpl.h"
#include "CNamespaceSupport.h"
#include "CSystem.h"
#include <utils/Log.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

const String CParserAdapter::FEATURES("http://xml.org/sax/features/");
const String CParserAdapter::NAMESPACES("http://xml.org/sax/features/mNamespaces");
const String CParserAdapter::NAMESPACE_PREFIXES("http://xml.org/sax/features/namespace-mPrefixes");
const String CParserAdapter::XMLNS_URIs("http://xml.org/sax/features/xmlns-mUris");

CAR_INTERFACE_IMPL_2(CParserAdapter, Object, IXMLReader, IDocumentHandler)
CAR_OBJECT_IMPL(CParserAdapter)

CParserAdapter::CParserAdapter()
    : mParsing(FALSE)
    , mNamespaces(TRUE)
    , mPrefixes(FALSE)
    , mUris(TRUE)
{
    mNameParts = ArrayOf<String>::Alloc(3);
}

ECode CParserAdapter::constructor()
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    String driver;
    system->GetProperty(String("org.xml.sax.mParser"), &driver);

    // try {
    AutoPtr<IParser> parser;
    FAIL_RETURN(ParserFactory::MakeParser((IParser**)&parser));
    return Setup(parser);
    // } catch (ClassNotFoundException e1) {
    //     throw new
    //     SAXException("Cannot find SAX1 driver class " +
    //              driver, e1);
    // } catch (IllegalAccessException e2) {
    //     throw new
    //     SAXException("SAX1 driver class " +
    //              driver +
    //              " found but cannot be loaded", e2);
    // } catch (InstantiationException e3) {
    //     throw new
    //     SAXException("SAX1 driver class " +
    //              driver +
    //              " loaded but cannot be instantiated", e3);
    // } catch (ClassCastException e4) {
    //     throw new
    //     SAXException("SAX1 driver class " +
    //              driver +
    //              " does not implement org.xml.sax.Parser");
    // } catch (NullPointerException e5) {
    //     throw new
    //     SAXException("System property org.xml.sax.mParser not specified");
    // }
}

ECode CParserAdapter::constructor(
    /* [in] */ IParser * parser)
{
    return Setup(parser);
}

ECode CParserAdapter::GetFeature(
    /* [in] */ const String& name,
    /* [out] */ Boolean * feature)
{
    VALIDATE_NOT_NULL(feature)

    if (name.Equals(NAMESPACES)) {
        *feature = mNamespaces;
    }
    else if (name.Equals(NAMESPACE_PREFIXES)) {
        *feature = mPrefixes;
    }
    else if (name.Equals(XMLNS_URIs)) {
        *feature = mUris;
    }
    else {
        ALOGE("CParserAdapter::GetFeature SAXNotRecognizedException: %s", name.string());
        return E_SAX_NOT_RECOGNIZED_EXCEPTION;
    }
    return NOERROR;
}

ECode CParserAdapter::SetFeature(
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
    if (name.Equals(NAMESPACES)) {
        CheckNotParsing(String("feature"), name);
        mNamespaces = value;
        if (!mNamespaces && !mPrefixes) {
            mPrefixes = TRUE;
        }
    }
    else if (name.Equals(NAMESPACE_PREFIXES)) {
        CheckNotParsing(String("feature"), name);
        mPrefixes = value;
        if (!mPrefixes && !mNamespaces) {
            mNamespaces = TRUE;
        }
    }
    else if (name.Equals(XMLNS_URIs)) {
        CheckNotParsing(String("feature"), name);
        mUris = value;
    } else {
        ALOGE("CParserAdapter::SetFeature SAXNotRecognizedException: %s", name.string());
        return E_SAX_NOT_RECOGNIZED_EXCEPTION;
    }
    return NOERROR;
}

ECode CParserAdapter::GetProperty(
    /* [in] */ const String& name,
    /* [out] */ IInterface ** value)
{
    ALOGE("CParserAdapter::GetProperty SAXNotRecognizedException: %s", name.string());
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

ECode CParserAdapter::SetProperty(
    /* [in] */ const String& name,
    /* [in] */ IInterface * value)
{
    ALOGE("CParserAdapter::SetProperty SAXNotRecognizedException: %s", name.string());
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

ECode CParserAdapter::SetEntityResolver(
    /* [in] */ IEntityResolver * resolver)
{
    mEntityResolver = resolver;
    return NOERROR;
}

ECode CParserAdapter::GetEntityResolver(
    /* [out] */ IEntityResolver ** resolver)
{
    VALIDATE_NOT_NULL(resolver)

    *resolver = mEntityResolver;
    REFCOUNT_ADD(*resolver)

    return NOERROR;
}

ECode CParserAdapter::SetDTDHandler(
    /* [in] */ IDTDHandler * handler)
{
    mDtdHandler = handler;
    return NOERROR;
}

ECode CParserAdapter::GetDTDHandler(
    /* [out] */ IDTDHandler ** handler)
{
    VALIDATE_NOT_NULL(handler)

    *handler = mDtdHandler;
    REFCOUNT_ADD(*handler)

    return NOERROR;
}

ECode CParserAdapter::SetContentHandler(
    /* [in] */ IContentHandler * handler)
{
    mContentHandler = handler;
    return NOERROR;
}

ECode CParserAdapter::GetContentHandler(
    /* [out] */ IContentHandler ** handler)
{
    VALIDATE_NOT_NULL(handler)

    *handler = mContentHandler;
    REFCOUNT_ADD(*handler)

    return NOERROR;
}

ECode CParserAdapter::SetErrorHandler(
    /* [in] */ IErrorHandler * handler)
{
    mErrorHandler = handler;
    return NOERROR;
}

ECode CParserAdapter::GetErrorHandler(
    /* [out] */ IErrorHandler ** handler)
{
    VALIDATE_NOT_NULL(handler)

    *handler = mErrorHandler;
    REFCOUNT_ADD(*handler)

    return NOERROR;
}

ECode CParserAdapter::Parse(
    /* [in] */ IInputSource * input)
{
    if (mParsing) {
        ALOGE("CParserAdapter::Parse: SAXException: Parser is already in use");
        return E_SAX_EXCEPTION;
    }

    SetupParser();
    mParsing = TRUE;
    mParser->Parse(input);
    mParsing = FALSE;
    mParsing = FALSE;
    return NOERROR;
}

ECode CParserAdapter::Parse(
    /* [in] */ const String& systemId)
{
    AutoPtr<IInputSource> input;
    FAIL_RETURN(CInputSource::New(systemId, (IInputSource**)&input));
    return Parse(input);
}

ECode CParserAdapter::SetDocumentLocator(
    /* [in] */ ILocator * locator)
{
    mLocator = locator;
    if (mContentHandler != NULL) {
        mContentHandler->SetDocumentLocator(mLocator);
    }
    return NOERROR;
}

ECode CParserAdapter::StartDocument()
{
    if (mContentHandler != NULL) {
        mContentHandler->StartDocument();
    }
    return NOERROR;
}

ECode CParserAdapter::EndDocument()
{
    if (mContentHandler != NULL) {
        mContentHandler->EndDocument();
    }
    return NOERROR;
}

ECode CParserAdapter::StartElement(
    /* [in] */ const String& qName,
    /* [in] */ IAttributeList * qAtts)
{
    // These are exceptions from the
    // first pass; they should be
    // ignored if there's a second pass,
    // but reported otherwise.
    AutoPtr< ArrayOf<ECode> > exceptions;

    // If we're not doing Namespace
    // processing, dispatch this quickly.
    if (!mNamespaces) {
        if (mContentHandler != NULL) {
            mAttAdapter->SetAttributeList(qAtts);
            mContentHandler->StartElement(String(""), String(""), qName /*qName.intern()*/, mAttAdapter);
        }
        return NOERROR;
    }

    // OK, we're doing Namespace processing.
    mNsSupport->PushContext();
    Int32 length = 0;
    qAtts->GetLength(&length);

    // First pass:  handle NS decls
    for (Int32 i = 0; i < length; i++) {
        String attQName;
        qAtts->GetName(i, &attQName);

        if (!attQName.StartWith("xmlns"))
            continue;
        // Could be a declaration...
        String prefix;
        Int32 n = attQName.IndexOf(':');

        // xmlns=...
        if (n == -1 && attQName.GetLength () == 5) {
            prefix = "";
        }
        else if (n != 5) {
            // XML mNamespaces spec doesn't discuss "xmlnsf:oo"
            // (and similarly named) attributes ... at most, warn
            continue;
        }
        else // xmlns:foo=...
            prefix = attQName.Substring(n+1);

        String value;
        qAtts->GetValue(i, &value);
        Boolean isflag = FALSE;
        mNsSupport->DeclarePrefix(prefix, value, &isflag);
        if (!isflag) {
            ReportError(String("Illegal Namespace prefix: ") + prefix);
            continue;
        }
        if (mContentHandler != NULL)
            mContentHandler->StartPrefixMapping(prefix, value);
    }

    // Second pass: copy all relevant
    // attributes into the SAX2 AttributeList
    // using updated prefix bindings
    mAtts->Clear();
    for (Int32 i = 0; i < length; i++) {
        String attQName;
        qAtts->GetName(i, &attQName);
        String type;
        qAtts->GetType(i, &type);
        String value;
        qAtts->GetValue(i, &value);

        // Declaration?
        if (attQName.StartWith("xmlns")) {
            String prefix;
            Int32 n = attQName.IndexOf(':');

            if (n == -1 && attQName.GetLength () == 5) {
                prefix = "";
            } else if (n != 5) {
                // XML mNamespaces spec doesn't discuss "xmlnsf:oo"
                // (and similarly named) attributes ... ignore
                prefix = NULL;
            } else {
                prefix = attQName.Substring(6);
            }
            // Yes, decl:  report or prune
            if (!prefix.IsNull()) {
                if (mPrefixes) {
                    if (mUris)
                        // note funky case:  localname can be null
                        // when declaring the default prefix, and
                        // yet the uri isn't null.
                        mAtts->AddAttribute(INamespaceSupport::XMLNS, prefix, attQName/* attQName.intern() */, type, value);
                    else
                        mAtts->AddAttribute (String(""), String(""), attQName/* attQName.intern() */, type, value);
                }
                continue;
            }
        }

        // Not a declaration -- report
        // try {
        AutoPtr< ArrayOf<String> > attName = ProcessName(attQName, TRUE, TRUE);
        mAtts->AddAttribute((*attName)[0], (*attName)[1], (*attName)[2], type, value);
        // } catch (SAXException e) {
        //     if (exceptions == null) {
        //         exceptions = new ArrayList<SAXParseException>();
        //     }
        //     exceptions.add((SAXParseException) e);
        //     mAtts.addAttribute("", attQName, attQName, type, value);
        // }
    }

    // now handle the deferred exception reports
    if (exceptions != NULL && mErrorHandler != NULL) {
        for (Int32 i = 0; i < exceptions->GetLength(); ++i) {
            mErrorHandler->Error((*exceptions)[i]);
        }
    }

    // OK, finally report the event.
    if (mContentHandler != NULL) {
        AutoPtr< ArrayOf<String> > name = ProcessName(qName, FALSE, FALSE);
        mContentHandler->StartElement((*name)[0], (*name)[1], (*name)[2], IAttributes::Probe(mAtts));
    }
    return NOERROR;
}

ECode CParserAdapter::EndElement(
    /* [in] */ const String& qName)
{
    // If we're not doing Namespace
    // processing, dispatch this quickly.
    if (!mNamespaces) {
        if (mContentHandler != NULL) {
            mContentHandler->EndElement(String(""), String(""), qName/* qName.intern() */);
        }
        return NOERROR;
    }

    // Split the name.
    AutoPtr< ArrayOf<String> > names = ProcessName(qName, FALSE, FALSE);
    if (mContentHandler != NULL) {
        mContentHandler->EndElement((*names)[0], (*names)[1], (*names)[2]);
        AutoPtr<IEnumeration> prefixes;
        mNsSupport->GetDeclaredPrefixes((IEnumeration**)&prefixes);
        if (prefixes != NULL) {
            String prefix;
            Boolean isflag = FALSE;
            while (prefixes->HasMoreElements(&isflag), isflag) {
                AutoPtr<IInterface> obj;
                prefixes->GetNextElement((IInterface**)&obj);
                ICharSequence* csq = ICharSequence::Probe(obj);
                csq->ToString(&prefix);
                mContentHandler->EndPrefixMapping(prefix);
            }
        }
    }

    return mNsSupport->PopContext();
}

ECode CParserAdapter::Characters(
    /* [in] */ ArrayOf<Char32>* ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    if (mContentHandler != NULL) {
        mContentHandler->Characters(ch, start, length);
    }
    return NOERROR;
}

ECode CParserAdapter::IgnorableWhitespace(
    /* [in] */ ArrayOf<Char32>* ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    if (mContentHandler != NULL) {
        mContentHandler->IgnorableWhitespace(ch, start, length);
    }
    return NOERROR;
}

ECode CParserAdapter::ProcessingInstruction(
    /* [in] */ const String& target,
    /* [in] */ const String& data)
{
    if (mContentHandler != NULL) {
        mContentHandler->ProcessingInstruction(target, data);
    }
    return NOERROR;
}

ECode CParserAdapter::Setup(
    /* [in] */ IParser* parser)
{
    if (parser == NULL) {
        // throw new NullPointerException("Parser argument must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mParser = parser;
    FAIL_RETURN(CAttributesImpl::New((IAttributesImpl**)&mAtts));
    FAIL_RETURN(CNamespaceSupport::New((INamespaceSupport**)&mNsSupport));
    mAttAdapter = new AttributeListAdapter(this);
    return NOERROR;
}

ECode CParserAdapter::SetupParser()
{
    // catch an illegal "nonsense" state.
    if (!mPrefixes && !mNamespaces)
        // throw new IllegalStateException ();
        return E_ILLEGAL_STATE_EXCEPTION;

    mNsSupport->Reset();
    if (mUris)
        mNsSupport->SetNamespaceDeclUris(TRUE);

    if (mEntityResolver != NULL) {
        mParser->SetEntityResolver(mEntityResolver);
    }
    if (mDtdHandler != NULL) {
        mParser->SetDTDHandler(mDtdHandler);
    }
    if (mErrorHandler != NULL) {
        mParser->SetErrorHandler(mErrorHandler);
    }
    mParser->SetDocumentHandler(this);
    mLocator = NULL;
    return NOERROR;
}

AutoPtr<ArrayOf<String> > CParserAdapter::ProcessName(
    /* [in] */ const String& qName,
    /* [in] */ Boolean isAttribute,
    /* [in] */ Boolean useException)
{
    AutoPtr< ArrayOf<String> > parts;
    mNsSupport->ProcessName(qName, mNameParts, isAttribute, (ArrayOf<String>**)&parts);
    if (parts == NULL) {
        if (useException)
            MakeException(String("Undeclared prefix: ") + qName);
        ReportError(String("Undeclared prefix: ") + qName);
        parts = ArrayOf<String>::Alloc(3);
        (*parts)[0] = "";
        (*parts)[1] = "";
        (*parts)[2] = qName; // qName.intern();
    }
    return parts;
}

void CParserAdapter::ReportError(
    /* [in] */ const String& message)
{
    if (mErrorHandler != NULL)
        mErrorHandler->Error(MakeException(message));
}

ECode CParserAdapter::MakeException(
    /* [in] */ const String& message)
{
    // if (mLocator != NULL) {
    //     return new SAXParseException(message, mLocator);
    // } else {
    //     return new SAXParseException(message, null, null, -1, -1);
    // }
    return E_SAX_PARSE_EXCEPTION;
}

void CParserAdapter::CheckNotParsing(
    /* [in] */ const String& type,
    /* [in] */ const String& name)
{
    if (mParsing) {
        // throw new SAXNotSupportedException("Cannot change " + type + ' ' + name + " while mParsing");
        assert(0 && "TODO");
    }
}

CAR_INTERFACE_IMPL(CParserAdapter::AttributeListAdapter, Object, IAttributes)

CParserAdapter::AttributeListAdapter::AttributeListAdapter(
    /* [in] */ CParserAdapter* host)
{
    IWeakReferenceSource* wrs = (IWeakReferenceSource*)host->Probe(EIID_IWeakReferenceSource);
    wrs->GetWeakReference((IWeakReference**)&mWeakHost);
}

ECode CParserAdapter::AttributeListAdapter::SetAttributeList(
    /* [in] */ IAttributeList* qAtts)
{
    mQAtts = qAtts;
    return NOERROR;
}

ECode CParserAdapter::AttributeListAdapter::GetLength(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    return mQAtts->GetLength(value);
}

ECode CParserAdapter::AttributeListAdapter::GetURI(
    /* [in] */ Int32 i,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    *str = String("");
    return NOERROR;
}

ECode CParserAdapter::AttributeListAdapter::GetLocalName(
    /* [in] */ Int32 i,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    *str = String("");
    return NOERROR;
}

ECode CParserAdapter::AttributeListAdapter::GetQName(
    /* [in] */ Int32 i,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    return mQAtts->GetName(i, str); // qAtts.getName(i).intern();
}

ECode CParserAdapter::AttributeListAdapter::GetType(
    /* [in] */ Int32 i,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    return mQAtts->GetType(i, str); // qAtts.getType(i).intern();
}

ECode CParserAdapter::AttributeListAdapter::GetValue(
    /* [in] */ Int32 i,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    return mQAtts->GetValue(i, str); // qAtts.getValue(i);
}

ECode CParserAdapter::AttributeListAdapter::GetIndex(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = -1;
    return NOERROR;
}

ECode CParserAdapter::AttributeListAdapter::GetIndex(
    /* [in] */ const String& qName,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = -1;

    AutoPtr<IXMLReader> reader;
    mWeakHost->Resolve(EIID_IXMLReader, (IInterface**)&reader);
    if (reader) {
        CParserAdapter* host = (CParserAdapter*)reader.Get();
        Int32 max = 0;
        IAttributes::Probe(host->mAtts)->GetLength(&max);
        for (Int32 i = 0; i < max; i++) {
            String name;
            mQAtts->GetName(i, &name);
            if (name.Equals(qName)) {
                *value = i;
                return NOERROR;
            }
        }
    }

    return NOERROR;
}

ECode CParserAdapter::AttributeListAdapter::GetType(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    *str = String("");
    return NOERROR;
}

ECode CParserAdapter::AttributeListAdapter::GetType(
    /* [in] */ const String& qName,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    return mQAtts->GetType(qName, str); // qAtts.getType(qName).intern();
}

ECode CParserAdapter::AttributeListAdapter::GetValue(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    *str = String("");
    return NOERROR;
}

ECode CParserAdapter::AttributeListAdapter::GetValue(
    /* [in] */ const String& qName,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    return mQAtts->GetValue(qName, str);
}


} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org
