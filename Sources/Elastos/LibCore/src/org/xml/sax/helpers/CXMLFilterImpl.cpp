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

#include "CXMLFilterImpl.h"
#include "CInputSource.h"

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

CAR_INTERFACE_IMPL_6(CXMLFilterImpl, Object, IXMLReader, IXMLFilter, IEntityResolver, IDTDHandler, IContentHandler, IErrorHandler)
CAR_OBJECT_IMPL(CXMLFilterImpl)

ECode CXMLFilterImpl::GetFeature(
    /* [in] */ const String& name,
    /* [out] */ Boolean* feature)
{
    VALIDATE_NOT_NULL(feature);

    if (mParent != NULL) {
        return mParent->GetFeature(name, feature);
    } else {
//        throw new SAXNotRecognizedException("Feature: " + name);
        return E_SAX_NOT_RECOGNIZED_EXCEPTION;
    }
}

ECode CXMLFilterImpl::SetFeature(
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
    if (mParent != NULL) {
        return mParent->SetFeature(name, value);
    } else {
//        throw new SAXNotRecognizedException("Feature: " + name);
        return E_SAX_NOT_RECOGNIZED_EXCEPTION;
    }
}

ECode CXMLFilterImpl::GetProperty(
    /* [in] */ const String& name,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    if (mParent != NULL) {
        return mParent->GetProperty(name, value);
    } else {
//        throw new SAXNotRecognizedException("Property: " + name);
        return E_SAX_NOT_RECOGNIZED_EXCEPTION;
    }
}

ECode CXMLFilterImpl::SetProperty(
    /* [in] */ const String& name,
    /* [in] */ IInterface* value)
{
    if (mParent != NULL) {
        return mParent->SetProperty(name, value);
    } else {
//        throw new SAXNotRecognizedException("Property: " + name);
        return E_SAX_NOT_RECOGNIZED_EXCEPTION;
    }
}

ECode CXMLFilterImpl::SetEntityResolver(
    /* [in] */ IEntityResolver* resolver)
{
    mEntityResolver = resolver;

    return NOERROR;
}

ECode CXMLFilterImpl::GetEntityResolver(
    /* [out] */ IEntityResolver** resolver)
{
    VALIDATE_NOT_NULL(resolver);

    *resolver = mEntityResolver;
    REFCOUNT_ADD(*resolver);
    return NOERROR;
}

ECode CXMLFilterImpl::SetDTDHandler(
    /* [in] */ IDTDHandler* handler)
{
    mDtdHandler = handler;

    return NOERROR;
}

ECode CXMLFilterImpl::GetDTDHandler(
    /* [out] */ IDTDHandler** handler)
{
    VALIDATE_NOT_NULL(handler);

    *handler = mDtdHandler;
    REFCOUNT_ADD(*handler);
    return NOERROR;
}

ECode CXMLFilterImpl::SetContentHandler(
    /* [in] */ IContentHandler* handler)
{
    mContentHandler = handler;

    return NOERROR;
}

ECode CXMLFilterImpl::GetContentHandler(
    /* [out] */ IContentHandler** handler)
{
    VALIDATE_NOT_NULL(handler);

    *handler = mContentHandler;
    REFCOUNT_ADD(*handler);
    return NOERROR;
}

ECode CXMLFilterImpl::SetErrorHandler(
    /* [in] */ IErrorHandler* handler)
{
    mErrorHandler = handler;

    return NOERROR;
}

ECode CXMLFilterImpl::GetErrorHandler(
    /* [out] */ IErrorHandler** handler)
{
    VALIDATE_NOT_NULL(handler);

    *handler = mErrorHandler;
    REFCOUNT_ADD(*handler);
    return NOERROR;
}

ECode CXMLFilterImpl::Parse(
    /* [in] */ IInputSource* input)
{
    SetupParse();

    return mParent->Parse(input);
}

ECode CXMLFilterImpl::Parse(
    /* [in] */ const String& systemId)
{
    AutoPtr<IInputSource> inputSource;

    CInputSource::New(systemId, (IInputSource**)&inputSource);

    return Parse(inputSource);
}

ECode CXMLFilterImpl::SetParent(
    /* [in] */ IXMLReader* parent)
{
    mParent = parent;

    return NOERROR;
}

ECode CXMLFilterImpl::GetParent(
    /* [out] */ IXMLReader** parent)
{
    VALIDATE_NOT_NULL(parent);

    *parent = mParent;
    REFCOUNT_ADD(*parent);
    return NOERROR;
}

ECode CXMLFilterImpl::ResolveEntity(
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId,
    /* [out] */ IInputSource** src)
{
    VALIDATE_NOT_NULL(src);

    if (mEntityResolver != NULL) {
        return mEntityResolver->ResolveEntity(publicId, systemId, src);
    } else {
        *src = NULL;
        return NOERROR;
    }
}

ECode CXMLFilterImpl::NotationDecl(
    /* [in] */ const String& name,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId)
{
    if (mDtdHandler != NULL) {
        mDtdHandler->NotationDecl(name, publicId, systemId);
    }

    return NOERROR;
}

ECode CXMLFilterImpl::UnparsedEntityDecl(
    /* [in] */ const String& name,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId,
    /* [in] */ const String& notationName)
{
    if (mDtdHandler != NULL) {
        mDtdHandler->UnparsedEntityDecl(name, publicId, systemId, notationName);
    }

    return NOERROR;
}

ECode CXMLFilterImpl::SetDocumentLocator(
    /* [in] */ ILocator* locator)
{
    mLocator = locator;

    if (mContentHandler != NULL) {
        mContentHandler->SetDocumentLocator(locator);
    }

    return NOERROR;
}

ECode CXMLFilterImpl::StartDocument()
{
    if (mContentHandler != NULL) {
        mContentHandler->StartDocument();
    }

    return NOERROR;
}

ECode CXMLFilterImpl::EndDocument()
{
    if (mContentHandler != NULL) {
        mContentHandler->EndDocument();
    }

    return NOERROR;
}

ECode CXMLFilterImpl::StartPrefixMapping(
    /* [in] */ const String& prefix,
    /* [in] */ const String& uri)
{
    if (mContentHandler != NULL) {
        mContentHandler->StartPrefixMapping(prefix, uri);
    }

    return NOERROR;
}

ECode CXMLFilterImpl::EndPrefixMapping(
    /* [in] */ const String& prefix)
{
    if (mContentHandler != NULL) {
        mContentHandler->EndPrefixMapping(prefix);
    }

    return NOERROR;
}

ECode CXMLFilterImpl::StartElement(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName,
    /* [in] */ IAttributes* atts)
{
    if (mContentHandler != NULL) {
        mContentHandler->StartElement(uri, localName, qName, atts);
    }

    return NOERROR;
}

ECode CXMLFilterImpl::EndElement(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName)
{
    if (mContentHandler != NULL) {
        mContentHandler->EndElement(uri, localName, qName);
    }

    return NOERROR;
}

ECode CXMLFilterImpl::Characters(
    /* [in] */ ArrayOf<Char32>* ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    if (mContentHandler != NULL) {
        mContentHandler->Characters(ch, start, length);
    }

    return NOERROR;
}

ECode CXMLFilterImpl::IgnorableWhitespace(
    /* [in] */ ArrayOf<Char32>* ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    if (mContentHandler != NULL) {
        mContentHandler->IgnorableWhitespace(ch, start, length);
    }

    return NOERROR;
}

ECode CXMLFilterImpl::ProcessingInstruction(
    /* [in] */ const String& target,
    /* [in] */ const String& data)
{
    if (mContentHandler != NULL) {
        return mContentHandler->ProcessingInstruction(target, data);
    }

    return NOERROR;
}

ECode CXMLFilterImpl::SkippedEntity(
    /* [in] */ const String& name)
{
    if (mContentHandler != NULL) {
        mContentHandler->SkippedEntity(name);
    }

    return NOERROR;
}

ECode CXMLFilterImpl::Warning(
    /* [in] */ ECode exception)
{
    if (mErrorHandler != NULL) {
        mErrorHandler->Warning(exception);
    }

    return NOERROR;
}

ECode CXMLFilterImpl::Error(
    /* [in] */ ECode exception)
{
    if (mErrorHandler != NULL) {
        mErrorHandler->Error(exception);
    }

    return NOERROR;
}

ECode CXMLFilterImpl::FatalError(
    /* [in] */ ECode exception)
{
    if (mErrorHandler != NULL) {
        mErrorHandler->FatalError(exception);
    }

    return NOERROR;
}

ECode CXMLFilterImpl::constructor()
{
    return NOERROR;
}

ECode CXMLFilterImpl::constructor(
    /* [in] */ IXMLReader* parent)
{
    SetParent(parent);

    return NOERROR;
}

ECode CXMLFilterImpl::SetupParse()
{
    if (mParent == NULL) {
//        throw new NullPointerException("No parent for filter");
        return E_NULL_POINTER_EXCEPTION;
    }

    mParent->SetEntityResolver(this);
    mParent->SetDTDHandler(this);
    mParent->SetContentHandler(this);
    mParent->SetErrorHandler(this);

    return NOERROR;
}

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org
