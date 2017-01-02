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

#ifndef  __ELASTOSX_XML_TRANSFORM_SAX_SAXTRANSFORMERFACTORY_H__
#define  __ELASTOSX_XML_TRANSFORM_SAX_SAXTRANSFORMERFACTORY_H__

#include "TransformerFactory.h"

using Org::Xml::Sax::IXMLFilter;
using Elastosx::Xml::Transform::TransformerFactory;

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Sax {

/**
 * This class extends TransformerFactory to provide SAX-specific
 * factory methods.  It provides two types of ContentHandlers,
 * one for creating Transformers, the other for creating Templates
 * objects.
 *
 * <p>If an application wants to set the ErrorHandler or EntityResolver
 * for an XMLReader used during a transformation, it should use a URIResolver
 * to return the SAXSource which provides (with getXMLReader) a reference to
 * the XMLReader.</p>
 */
class SAXTransformerFactory
    : public TransformerFactory
    , public ISAXTransformerFactory
{
protected:
    /**
     * The default constructor is protected on purpose.
     */
    SAXTransformerFactory();

public:
    CAR_INTERFACE_DECL()

    /**
     * Get a TransformerHandler object that can process SAX
     * ContentHandler events into a Result, based on the transformation
     * instructions specified by the argument.
     *
     * @param src The Source of the transformation instructions.
     *
     * @return TransformerHandler ready to transform SAX events.
     *
     * @throws TransformerConfigurationException If for some reason the
     * TransformerHandler can not be created.
     */
    virtual CARAPI NewTransformerHandler(
        /* [in] */ ISource* src,
        /* [out] */ ITransformerHandler** handler) = 0 /*throws TransformerConfigurationException*/;

    /**
     * Get a TransformerHandler object that can process SAX
     * ContentHandler events into a Result, based on the Templates argument.
     *
     * @param templates The compiled transformation instructions.
     *
     * @return TransformerHandler ready to transform SAX events.
     *
     * @throws TransformerConfigurationException If for some reason the
     * TransformerHandler can not be created.
     */
    virtual CARAPI NewTransformerHandler(
        /* [in] */ ITemplates* templates,
        /* [out] */ ITransformerHandler** handler) = 0 /*throws TransformerConfigurationException*/;

    /**
     * Get a TransformerHandler object that can process SAX
     * ContentHandler events into a Result. The transformation
     * is defined as an identity (or copy) transformation, for example
     * to copy a series of SAX parse events into a DOM tree.
     *
     * @return A non-null reference to a TransformerHandler, that may
     * be used as a ContentHandler for SAX parse events.
     *
     * @throws TransformerConfigurationException If for some reason the
     * TransformerHandler cannot be created.
     */
    virtual CARAPI NewTransformerHandler(
        /* [out] */ ITransformerHandler** handler) = 0 /*throws TransformerConfigurationException*/;

    /**
     * Get a TemplatesHandler object that can process SAX
     * ContentHandler events into a Templates object.
     *
     * @return A non-null reference to a TransformerHandler, that may
     * be used as a ContentHandler for SAX parse events.
     *
     * @throws TransformerConfigurationException If for some reason the
     * TemplatesHandler cannot be created.
     */
    virtual CARAPI NewTemplatesHandler(
        /* [out] */ ITemplatesHandler** handler) = 0 /*throws TransformerConfigurationException*/;

    /**
     * Create an XMLFilter that uses the given Source as the
     * transformation instructions.
     *
     * @param src The Source of the transformation instructions.
     *
     * @return An XMLFilter object, or null if this feature is not supported.
     *
     * @throws TransformerConfigurationException If for some reason the
     * TemplatesHandler cannot be created.
     */
    virtual CARAPI NewXMLFilter(
        /* [in] */ ISource* src,
        /* [out] */ IXMLFilter** filter) = 0 /*throws TransformerConfigurationException*/;

    /**
     * Create an XMLFilter, based on the Templates argument..
     *
     * @param templates The compiled transformation instructions.
     *
     * @return An XMLFilter object, or null if this feature is not supported.
     *
     * @throws TransformerConfigurationException If for some reason the
     * TemplatesHandler cannot be created.
     */
    virtual CARAPI NewXMLFilter(
        /* [in] */ ITemplates* templates,
        /* [out] */ IXMLFilter** filter) = 0 /*throws TransformerConfigurationException*/;
};

} // namespace Sax
} // namespace Transform
} // namespace Xml
} // namespace Elastosx

#endif  // __ELASTOSX_XML_TRANSFORM_SAX_SAXTRANSFORMERFACTORY_H__
