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

#ifndef __ELASTOSX_XML_PARSERS_SAXPARSER_H__
#define __ELASTOSX_XML_PARSERS_SAXPARSER_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;
using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Org::Xml::Sax::IDocumentHandler;
using Org::Xml::Sax::IDTDHandler;
using Org::Xml::Sax::IInputSource;
using Elastosx::Xml::Validation::ISchema;

namespace Elastosx {
namespace Xml {
namespace Parsers {

class SAXParser
    : public ISAXParser
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Reset();

    CARAPI Parse(
        /* [in] */ IInputStream* is,
        /* [in] */ IDocumentHandler* hb);

    CARAPI Parse(
        /* [in] */ IInputStream* is,
        /* [in] */ IDocumentHandler* hb,
        /* [in] */ const String& systemId);

    CARAPI Parse(
        /* [in] */ IInputStream* is,
        /* [in] */ IDTDHandler* dh);

    CARAPI Parse(
        /* [in] */ IInputStream* is,
        /* [in] */ IDTDHandler* dh,
        /* [in] */ const String& systemId);

    CARAPI Parse(
        /* [in] */ const String& uri,
        /* [in] */ IDocumentHandler* hb);

    CARAPI Parse(
        /* [in] */ const String& uri,
        /* [in] */ IDTDHandler* dh);

    CARAPI Parse(
        /* [in] */ IFile* f,
        /* [in] */ IDocumentHandler* hb);

    CARAPI Parse(
        /* [in] */ IFile* f,
        /* [in] */ IDTDHandler* dh);

    CARAPI Parse(
        /* [in] */ IInputSource* is,
        /* [in] */ IDocumentHandler* hb);

    CARAPI Parse(
        /* [in] */ IInputSource* is,
        /* [in] */ IDTDHandler* dh);

    CARAPI GetSchema(
        /* [out] */ ISchema** schema);

    CARAPI IsXIncludeAware(
        /* [out] */ Boolean* isAware);
};

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx
#endif
