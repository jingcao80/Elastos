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

#ifndef  __ELASTOSX_XML_TRANSFORM_SAX_CSAXRESULT_H__
#define  __ELASTOSX_XML_TRANSFORM_SAX_CSAXRESULT_H__

#include "Elastos.CoreLibrary.External.h"
#include "_Elastosx_Xml_Transform_Sax_CSAXResult.h"
#include "Object.h"

using Elastos::Core::Object;
using Org::Xml::Sax::IContentHandler;
using Org::Xml::Sax::Ext::ILexicalHandler;

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Sax {

/**
 * <p>Acts as an holder for a transformation Result.</p>
 *
 * @author <a href="Jeff.Suttor@Sun.com">Jeff Suttor</a>
 */
CarClass(CSAXResult)
    , public Object
    , public ISAXResult
    , public IResult
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Zero-argument default constructor.
     */
    CSAXResult();

    /**
     * Create a SAXResult that targets a SAX2 {@link org.xml.sax.ContentHandler}.
     *
     * @param handler Must be a non-null ContentHandler reference.
     */
    CARAPI constructor(
        /* [in] */ IContentHandler* handler);

    /**
     * Set the target to be a SAX2 {@link org.xml.sax.ContentHandler}.
     *
     * @param handler Must be a non-null ContentHandler reference.
     */
    CARAPI SetHandler(
        /* [in] */ IContentHandler* handler);

    /**
     * Get the {@link org.xml.sax.ContentHandler} that is the Result.
     *
     * @return The ContentHandler that is to be transformation output.
     */
    CARAPI GetHandler(
        /* [out] */ IContentHandler** handler);

    /**
     * Set the SAX2 {@link org.xml.sax.ext.LexicalHandler} for the output.
     *
     * <p>This is needed to handle XML comments and the like.  If the
     * lexical handler is not set, an attempt should be made by the
     * transformer to cast the {@link org.xml.sax.ContentHandler} to a
     * <code>LexicalHandler</code>.</p>
     *
     * @param handler A non-null <code>LexicalHandler</code> for
     * handling lexical parse events.
     */
    CARAPI SetLexicalHandler(
        /* [in] */ ILexicalHandler* handler);

    /**
     * Get a SAX2 {@link org.xml.sax.ext.LexicalHandler} for the output.
     *
     * @return A <code>LexicalHandler</code>, or null.
     */
    CARAPI GetLexicalHandler(
        /* [out] */ ILexicalHandler** handler);

    /**
     * Method setSystemId Set the systemID that may be used in association
     * with the {@link org.xml.sax.ContentHandler}.
     *
     * @param systemId The system identifier as a URI string.
     */
    CARAPI SetSystemId(
        /* [in] */ const String& systemId);

    /**
     * Get the system identifier that was set with setSystemId.
     *
     * @return The system identifier that was set with setSystemId, or null
     * if setSystemId was not called.
     */
    CARAPI GetSystemId(
        /* [out] */ String* id);

private:
    //////////////////////////////////////////////////////////////////////
    // Internal state.
    //////////////////////////////////////////////////////////////////////

    /**
     * The handler for parse events.
     */
    AutoPtr<IContentHandler> mHandler;

    /**
     * The handler for lexical events.
     */
    AutoPtr<ILexicalHandler> mLexhandler;

    /**
     * The systemID that may be used in association
     * with the node.
     */
    String mSystemId;
};

} // namespace Sax
} // namespace Transform
} // namespace Xml
} // namespace Elastosx

#endif  // __ELASTOSX_XML_TRANSFORM_SAX_CSAXRESULT_H__
