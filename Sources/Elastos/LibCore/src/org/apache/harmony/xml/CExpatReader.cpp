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
#include "Elastos.CoreLibrary.IO.h"
#include "org/apache/harmony/xml/CExpatReader.h"
#include "org/apache/harmony/xml/ExpatParser.h"
#include "org/xml/sax/CInputSource.h"
#include <elastos/utility/logging/Logger.h>
#include "libcore/io/IoUtils.h"

using Org::Xml::Sax::EIID_IXMLReader;
using Org::Xml::Sax::CInputSource;
using Libcore::IO::IoUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {

//**CExpatReader::Feature
const String CExpatReader::Feature::BASE_URI("http://xml.org/sax/features/");
const String CExpatReader::Feature::VALIDATION = BASE_URI + String("validation");
const String CExpatReader::Feature::NAMESPACES = BASE_URI + String("namespaces");
const String CExpatReader::Feature::NAMESPACE_PREFIXES = BASE_URI + String("namespace-prefixes");
const String CExpatReader::Feature::STRING_INTERNING = BASE_URI + String("string-interning");
const String CExpatReader::Feature::EXTERNAL_GENERAL_ENTITIES = BASE_URI + String("external-general-entities");
const String CExpatReader::Feature::EXTERNAL_PARAMETER_ENTITIES = BASE_URI + String("external-parameter-entities");


CAR_INTERFACE_IMPL_2(CExpatReader, Object, IExpatReader, IXMLReader)

const String CExpatReader::LEXICAL_HANDLER_PROPERTY("http://xml.org/sax/properties/lexical-handler");

ECode CExpatReader::constructor()
{
    processNamespaces = TRUE;
    processNamespacePrefixes = FALSE;
    return NOERROR;
}

ECode CExpatReader::GetFeature(
    /* [in] */ const String& name,
    /* [out] */ Boolean * pFeature)
{
    VALIDATE_NOT_NULL(pFeature);
    if (name.IsNull()) {
        //throw new NullPointerException("name == null");
        assert(0);
        Logger::E("CExpatReader", "name == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (name.Equals(Feature::VALIDATION)
            || name.Equals(Feature::EXTERNAL_GENERAL_ENTITIES)
            || name.Equals(Feature::EXTERNAL_PARAMETER_ENTITIES)) {
        *pFeature = FALSE;
        return NOERROR;
    }

    if (name.Equals(Feature::NAMESPACES)) {
        *pFeature = processNamespaces;
        return NOERROR;
    }

    if (name.Equals(Feature::NAMESPACE_PREFIXES)) {
        *pFeature = processNamespacePrefixes;
        return NOERROR;
    }

    if (name.Equals(Feature::STRING_INTERNING)) {
        *pFeature = TRUE;
        return NOERROR;
    }

    //throw new SAXNotRecognizedException(name);
    Logger::E("CExpatReader", "SAXNotRecognizedException");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CExpatReader::SetFeature(
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
    if (name.IsNull()) {
        //throw new NullPointerException("name == null");
        Logger::E("CExpatReader", " name is null ");
        assert(0);
        return E_NULL_POINTER_EXCEPTION;
    }

    if (name.Equals(Feature::VALIDATION)
            || name.Equals(Feature::EXTERNAL_GENERAL_ENTITIES)
            || name.Equals(Feature::EXTERNAL_PARAMETER_ENTITIES)) {
        if (value) {
            //throw new SAXNotSupportedException("Cannot enable " + name);
            Logger::E("CExpatReader", "SAXNotSupportedException name:%s", name.string());
            assert(0);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        } else {
            // Default.
            return NOERROR;
        }
    }

    if (name.Equals(Feature::NAMESPACES)) {
        processNamespaces = value;
        return NOERROR;
    }

    if (name.Equals(Feature::NAMESPACE_PREFIXES)) {
        processNamespacePrefixes = value;
        return NOERROR;
    }

    if (name.Equals(Feature::STRING_INTERNING)) {
        if (value) {
            // Default.
            return NOERROR;
        } else {
            //throw new SAXNotSupportedException("Cannot disable " + name);
            Logger::E("CExpatReader", "SAXNotSupportedException cannot disable name:%s", name.string());
            assert(0);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
    }

    //throw new SAXNotRecognizedException(name);
    Logger::E("CExpatReader", "SAXNotRecognizedException");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CExpatReader::GetProperty(
    /* [in] */ const String& name,
    /* [out] */ IInterface ** ppValue)
{
    VALIDATE_NOT_NULL(ppValue);
    if (name.IsNull()) {
        //throw new NullPointerException("name == null");
        Logger::E("CExpatReader", "GetProperty name is null ");
        assert(0);
        return E_NULL_POINTER_EXCEPTION;
    }

    if (name.Equals(LEXICAL_HANDLER_PROPERTY)) {
        *ppValue = lexicalHandler;
        REFCOUNT_ADD(*ppValue);
        return NOERROR;
    }

    //throw new SAXNotRecognizedException(name);
    Logger::E("CExpatReader", "SAXNotRecognizedException, GetProperty name:%s", name.string());
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CExpatReader::SetProperty(
    /* [in] */ const String& name,
    /* [in] */ IInterface * pValue)
{
    if (name.IsNull()) {
        //throw new NullPointerException("name == null");
        Logger::E("CExpatReader", "SetProperty name is null ");
        assert(0);
        return E_NULL_POINTER_EXCEPTION;
    }

    if (name.Equals(LEXICAL_HANDLER_PROPERTY)) {
        // The object must implement LexicalHandler
        if (pValue == NULL || ILexicalHandler::Probe(pValue) == NULL) {
            this->lexicalHandler = ILexicalHandler::Probe(pValue);
            return NOERROR;
        }
        //throw new SAXNotSupportedException("value doesn't implement org.xml.sax.ext.LexicalHandler");
        Logger::E("CExpatReader", "SAXNotRecognizedException, value doesn't implement org.xml.sax.ext.LexicalHandler");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    //throw new SAXNotRecognizedException(name);
    Logger::E("CExpatReader", "SAXNotRecognizedException, SetProperty name:%s", name.string());
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CExpatReader::SetEntityResolver(
    /* [in] */ Org::Xml::Sax::IEntityResolver * pResolver)
{
    this->entityResolver = pResolver;
    return NOERROR;
}

ECode CExpatReader::GetEntityResolver(
    /* [out] */ Org::Xml::Sax::IEntityResolver ** ppResolver)
{
    VALIDATE_NOT_NULL(ppResolver);
    *ppResolver = entityResolver;
    REFCOUNT_ADD(*ppResolver);
    return NOERROR;
}

ECode CExpatReader::SetDTDHandler(
    /* [in] */ Org::Xml::Sax::IDTDHandler * pHandler)
{
    this->dtdHandler = pHandler;
    return NOERROR;
}

ECode CExpatReader::GetDTDHandler(
    /* [out] */ Org::Xml::Sax::IDTDHandler ** ppHandler)
{
    VALIDATE_NOT_NULL(ppHandler);
    *ppHandler = dtdHandler;
    REFCOUNT_ADD(*ppHandler);
    return NOERROR;
}

ECode CExpatReader::SetContentHandler(
    /* [in] */ Org::Xml::Sax::IContentHandler * pHandler)
{
    this->contentHandler = pHandler;
    return NOERROR;
}

ECode CExpatReader::GetContentHandler(
    /* [out] */ Org::Xml::Sax::IContentHandler ** ppHandler)
{
    VALIDATE_NOT_NULL(ppHandler);
    *ppHandler = contentHandler;
    REFCOUNT_ADD(*ppHandler);
    return NOERROR;
}

ECode CExpatReader::SetErrorHandler(
    /* [in] */ Org::Xml::Sax::IErrorHandler * pHandler)
{
    this->errorHandler = pHandler;
    return NOERROR;
}

ECode CExpatReader::GetErrorHandler(
    /* [out] */ Org::Xml::Sax::IErrorHandler ** ppHandler)
{
    VALIDATE_NOT_NULL(ppHandler);
    *ppHandler = errorHandler;
    REFCOUNT_ADD(*ppHandler);
    return NOERROR;
}

ECode CExpatReader::Parse(
    /* [in] */ Org::Xml::Sax::IInputSource * pInput)
{
    if (processNamespacePrefixes && processNamespaces) {
        /*
         * Expat has XML_SetReturnNSTriplet, but that still doesn't
         * include xmlns attributes like this feature requires. We may
         * have to implement namespace processing ourselves if we want
         * this (not too difficult). We obviously "support" namespace
         * prefixes if namespaces are disabled.
         */
        //throw new SAXNotSupportedException("The 'namespace-prefix' " +
        //        "feature is not supported while the 'namespaces' " +
        //        "feature is enabled.");
        Logger::E("CExpatReader", "SAXNotRecognizedException, The 'namespace-prefix' feature is not supported while the 'namespaces' feature is enabled.");
        assert(0);
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    // Try the character stream.
    AutoPtr<IReader> reader;
    pInput->GetCharacterStream((IReader**)&reader);
    if (reader != NULL) {
        //try {
        String publicId, systemId;
        pInput->GetPublicId(&publicId);
        pInput->GetSystemId(&systemId);
        Parse(reader, publicId, systemId);
        //} finally {
        IoUtils::CloseQuietly(ICloseable::Probe(reader));
        //}
        return NOERROR;
    }

    // Try the byte stream.
    AutoPtr<IInputStream> in;
    pInput->GetByteStream((IInputStream**)&in);
    String encoding;
    pInput->GetEncoding(&encoding);
    if (in != NULL) {
        //try {
        String publicId, systemId;
        pInput->GetPublicId(&publicId);
        pInput->GetSystemId(&systemId);
        Parse(in, encoding, publicId, systemId);
        //} finally {
        IoUtils::CloseQuietly(ICloseable::Probe(in));
        //}
        return NOERROR;
    }

    String publicId, systemId;
    pInput->GetSystemId(&systemId);
    if (systemId.IsNull()) {
        //throw new SAXException("No input specified.");
        Logger::E("CExpatReader", "SAXException, No input specified.");
        assert(0);
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    // Try the system id.
    in = ExpatParser::OpenUrl(systemId);
    pInput->GetPublicId(&publicId);
    //try {
    Parse(in, encoding, publicId, systemId);
    //} finally {
    IoUtils::CloseQuietly(ICloseable::Probe(in));
    //}
    return NOERROR;
}

ECode CExpatReader::Parse(
    /* [in] */ IReader* in,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId)
{
    AutoPtr<ExpatParser> parser = new ExpatParser(
            ExpatParser::CHARACTER_ENCODING,
            this,
            processNamespaces,
            publicId,
            systemId
            );
    parser->ParseDocument(in);
    return NOERROR;
}

ECode CExpatReader::Parse(
    /* [in] */ IInputStream* in,
    /* [in] */ const String& charsetName,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId)
{
    AutoPtr<ExpatParser> parser =
        new ExpatParser(charsetName, this, processNamespaces, publicId, systemId);
    parser->ParseDocument(in);
    return NOERROR;
}

ECode CExpatReader::Parse(
    /* [in] */ const String& systemId)
{
    AutoPtr<IInputSource> is;
    CInputSource::New((IInputSource**)&is);
    Parse(is);
    return NOERROR;
}


ECode CExpatReader::GetLexicalHandler(
    /* [out] */ Org::Xml::Sax::Ext::ILexicalHandler ** ppHandler)
{
    VALIDATE_NOT_NULL(ppHandler);
    *ppHandler = lexicalHandler;
    REFCOUNT_ADD(*ppHandler);
    return NOERROR;
}

ECode CExpatReader::SetLexicalHandler(
    /* [in] */ Org::Xml::Sax::Ext::ILexicalHandler * pLexicalHandler)
{
    this->lexicalHandler = lexicalHandler;
    return NOERROR;
}

ECode CExpatReader::IsNamespaceProcessingEnabled(
    /* [out] */ Boolean * pEnabled)
{
    VALIDATE_NOT_NULL(pEnabled);
    *pEnabled = processNamespaces;
    return NOERROR;
}

ECode CExpatReader::SetNamespaceProcessingEnabled(
    /* [in] */ Boolean processNamespaces)
{
    this->processNamespaces = processNamespaces;
    return NOERROR;
}

}
}
}
}
