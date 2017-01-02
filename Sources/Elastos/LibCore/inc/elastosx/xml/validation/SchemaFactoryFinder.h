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

#ifndef __ELASTOSX_XML_VALIDATION_SCHEMAFACTORYFINDER_H__
#define __ELASTOSX_XML_VALIDATION_SCHEMAFACTORYFINDER_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Core::IClassLoader;
using Elastos::Utility::IIterable;
using Org::Xml::Sax::IErrorHandler;
using Org::W3c::Dom::Ls::ILSResourceResolver;
using Elastos::Utility::IProperties;
using Elastos::IO::IInputStream;

namespace Elastosx {
namespace Xml {
namespace Validation {

class SchemaFactoryFinder
    : public Object
    , public ISchemaFactoryFinder
{
public:
    CAR_INTERFACE_DECL()

    SchemaFactoryFinder(
        /* [in] */ IClassLoader* loader);

    CARAPI NewFactory(
        /* [in] */ const String& schemaLanguage,
        /* [out] */ ISchemaFactory** factory);

    CARAPI CreateInstance(
        /* [in] */ const String& className,
        /* [out] */ ISchemaFactory** factory);

private:
    CARAPI _NewFactory(
        /* [in] */ const String& schemaLanguage,
        /* [out] */ ISchemaFactory** factory);

    CARAPI CreateServiceFileIterator(
        /* [out] */ IIterable** it);

    CARAPI LoadFromServicesFile(
        /* [in] */ const String& schemaLanguage,
        /* [in] */ const String& resourceName,
        /* [in] */ IInputStream* in,
        /* [out] */ ISchemaFactory** factory);

    CARAPI Which(
        /* [in] */ ClassID clazz,
        /* [out] */ String* str);

private:
    /**
     * <p>Cache properties for performance.</p>
     */
    static AutoPtr<IProperties> sCacheProps;

    /**
     * <p>First time requires initialization overhead.</p>
     */
    static Boolean sFirstTime;

    /**
     * Default columns per line.
     */
    static Int32 DEFAULT_LINE_LENGTH;

    static String SERVICE_CLASS;

    static const String W3C_XML_SCHEMA10_NS_URI; // = "http://www.w3.org/XML/XMLSchema/v1.0";
    static const String W3C_XML_SCHEMA11_NS_URI; // = "http://www.w3.org/XML/XMLSchema/v1.1";

    AutoPtr<IClassLoader> mClassLoader;
};

} // namespace Validation
} // namespace Xml
} // namespace Elastosx
#endif