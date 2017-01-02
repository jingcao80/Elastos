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

#ifndef __ORG_XML_SAX_EXT_CATTRIBUTES2IMPL_H__
#define __ORG_XML_SAX_EXT_CATTRIBUTES2IMPL_H__

#include "_Org_Xml_Sax_Ext_CAttributes2Impl.h"
#include "AttributesImpl.h"

using Org::Xml::Sax::Helpers::AttributesImpl;

namespace Org {
namespace Xml {
namespace Sax {
namespace Ext {

/**
 * SAX2 extension helper for additional Attributes information,
 * implementing the {@link Attributes2} interface.
 *
 * <blockquote>
 * <em>This module, both source code and documentation, is in the
 * Public Domain, and comes with <strong>NO WARRANTY</strong>.</em>
 * </blockquote>
 *
 * <p>This is not part of core-only SAX2 distributions.</p>
 *
 * <p>The <em>specified</em> flag for each attribute will always
 * be true, unless it has been set to false in the copy constructor
 * or using {@link #setSpecified}.
 * Similarly, the <em>declared</em> flag for each attribute will
 * always be false, except for defaulted attributes (<em>specified</em>
 * is false), non-CDATA attributes, or when it is set to true using
 * {@link #setDeclared}.
 * If you change an attribute's type by hand, you may need to modify
 * its <em>declared</em> flag to match.
 * </p>
 *
 * @since SAX 2.0 (extensions 1.1 alpha)
 * @author David Brownell
 * @version TBS
 */
CarClass(CAttributes2Impl)
    , public AttributesImpl
    , public IAttributes2Impl
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI SetAttributes(
        /* [in] */ IAttributes* atts);

    CARAPI AddAttribute(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [in] */ const String& name,
        /* [in] */ const String& type,
        /* [in] */ const String& value);

    CARAPI SetAttribute(
        /* [in] */ Int32 index,
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [in] */ const String& name,
        /* [in] */ const String& type,
        /* [in] */ const String& value);

    CARAPI RemoveAttribute(
        /* [in] */ Int32 index);

    CARAPI IsSpecified(
        /* [in] */ Int32 index,
        /* [out] */ Boolean* isSpecified);

    CARAPI IsSpecified(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [out] */ Boolean* isSpecified);

    CARAPI IsSpecified(
        /* [in] */ const String& qName,
        /* [out] */ Boolean* isSpecified);

    CARAPI IsDeclared(
        /* [in] */ Int32 index,
        /* [out] */ Boolean * isDeclared);

    CARAPI IsDeclared(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [out] */ Boolean * isDeclared);

    CARAPI IsDeclared(
        /* [in] */ const String& qName,
        /* [out] */ Boolean * isDeclared);

    CARAPI SetDeclared(
        /* [in] */ Int32 index,
        /* [in] */ Boolean value);

    CARAPI SetSpecified(
        /* [in] */ Int32 index,
        /* [in] */ Boolean value);

    /**
     * Construct a new, empty Attributes2Impl object.
     */
    CARAPI constructor();

    /**
     * Copy an existing Attributes or Attributes2 object.
     * If the object implements Attributes2, values of the
     * <em>specified</em> and <em>declared</em> flags for each
     * attribute are copied.
     * Otherwise the flag values are defaulted to assume no DTD was used,
     * unless there is evidence to the contrary (such as attributes with
     * type other than CDATA, which must have been <em>declared</em>).
     *
     * <p>This constructor is especially useful inside a
     * {@link org.xml.sax.ContentHandler#startElement startElement} event.</p>
     *
     * @param atts The existing Attributes object.
     */
    CARAPI constructor(
        /* [in] */ IAttributes* atts);

private:
    AutoPtr<ArrayOf<Boolean> > mDeclared;
    AutoPtr<ArrayOf<Boolean> > mSpecified;
};

} // namespace Ext
} // namespace Sax
} // namespace Xml
} // namespace Org

#endif // __ORG_XML_SAX_EXT_CATTRIBUTES2IMPL_H__
