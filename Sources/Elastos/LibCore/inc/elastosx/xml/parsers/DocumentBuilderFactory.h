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

#ifndef __ELASTOSX_XML_PARSERS_DOCUMENTBUILDERFACTORY_H__
#define __ELASTOSX_XML_PARSERS_DOCUMENTBUILDERFACTORY_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;
using Elastos::Core::IClassLoader;
using Elastosx::Xml::Validation::ISchema;

namespace Elastosx {
namespace Xml {
namespace Parsers {

class DocumentBuilderFactory
    : public Object
    , public IDocumentBuilderFactory
{
public:
    CAR_INTERFACE_DECL()

    CARAPI SetNamespaceAware(
        /* [in] */ Boolean awareness);

    CARAPI SetValidating(
        /* [in] */ Boolean validating);

    CARAPI SetIgnoringElementContentWhitespace(
        /* [in] */ Boolean whitespace);

    CARAPI SetExpandEntityReferences(
        /* [in] */ Boolean expandEntityRef);

    CARAPI SetIgnoringComments(
        /* [in] */ Boolean ignoreComments);

    CARAPI SetCoalescing(
        /* [in] */ Boolean coalescing);

    CARAPI IsNamespaceAware(
        /* [out] */ Boolean* isAware);

    CARAPI IsValidating(
        /* [out] */ Boolean* isValidating);

    CARAPI IsIgnoringElementContentWhitespace(
        /* [out] */ Boolean* isIgnoringElementContentWhiteSpace);

    CARAPI IsExpandEntityReferences(
        /* [out] */ Boolean* isExpandEntityReferences);

    CARAPI IsIgnoringComments(
        /* [out] */ Boolean* comments);

    CARAPI IsCoalescing(
        /* [out] */ Boolean* isCoalescing);

    CARAPI GetSchema(
        /* [out] */ ISchema** schema);

    CARAPI SetSchema(
        /* [in] */ ISchema* schema);

    CARAPI SetXIncludeAware(
        /* [in] */ Boolean state);

    CARAPI IsXIncludeAware(
        /* [out] */ Boolean* isXIncludeAware);

    static CARAPI NewInstance(
        /* [out] */ IDocumentBuilderFactory** instance);

    static CARAPI NewInstance(
        /* [in] */ const String& factoryClassName,
        /* [in] */ IClassLoader* classLoader,
        /* [out] */ IDocumentBuilderFactory** instance);

protected:
    DocumentBuilderFactory();

private:
    Boolean mValidating;
    Boolean mNamespaceAware;
    Boolean mWhitespace;
    Boolean mExpandEntityRef;
    Boolean mIgnoreComments;
    Boolean mCoalescing;
};

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx

#endif
