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

#ifndef __ELASTOSX_XML_PARSERS_SAXPARSERFACTORY_H__
#define __ELASTOSX_XML_PARSERS_SAXPARSERFACTORY_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;
using Elastos::Core::IClassLoader;
using Elastosx::Xml::Validation::ISchema;

namespace Elastosx {
namespace Xml {
namespace Parsers {

class SAXParserFactory
    : public Object
    , public ISAXParserFactory
{
public:
    CAR_INTERFACE_DECL()

    CARAPI SetNamespaceAware(
        /* [in]*/ Boolean awareness);

    CARAPI SetValidating(
        /* [in]*/ Boolean validating);

    CARAPI IsNamespaceAware(
        /* [out]*/ Boolean* isAware);

    CARAPI IsValidating(
        /* [out]*/ Boolean* isValidating);

    CARAPI GetSchema(
        /* [out]*/ ISchema** schema);

    CARAPI SetSchema(
        /* [in]*/ ISchema* schema);

    CARAPI SetXIncludeAware(
        /* [in]*/ Boolean state);

    CARAPI IsXIncludeAware(
        /* [out]*/ Boolean* isAware);

    static CARAPI NewInstance(
        /* [out]*/ ISAXParserFactory** instance);

    static CARAPI NewInstance(
        /* [in]*/ const String& factoryClassName,
        /* [in]*/ IClassLoader* classLoader,
        /* [out]*/ ISAXParserFactory** instance);

protected:
    SAXParserFactory();

private:
    Boolean mValidating;
    Boolean mNamespaceAware;
};

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx
#endif
