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

#ifndef __ELASTOSX_XML_VALIDATION_SCHEMAFACTORY_H__
#define __ELASTOSX_XML_VALIDATION_SCHEMAFACTORY_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Core::IClassLoader;
using Elastos::IO::IFile;
using Elastos::Net::IURL;
using Org::Xml::Sax::IErrorHandler;
using Org::W3c::Dom::Ls::ILSResourceResolver;
using Elastosx::Xml::Transform::ISource;

namespace Elastosx {
namespace Xml {
namespace Validation {

class SchemaFactory
    : public Object
    , public ISchemaFactory
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean* feature);

    CARAPI SetFeature(
        /* [in] */ const String& name,
        /* [in] */ Boolean value);

    CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ IInterface* object);

    CARAPI GetProperty(
        /* [in] */ const String& name,
        /* [out] */ IInterface** property);

    CARAPI NewSchema(
        /* [in] */ ISource* schema,
        /* [out] */ ISchema** instance);

    CARAPI NewSchema(
        /* [in] */ IFile* schema,
        /* [out] */ ISchema** instance);

    CARAPI NewSchema(
        /* [in] */ IURL* schema,
        /* [out] */ ISchema** instance);

    using ISchemaFactory::NewSchema;

    static CARAPI NewInstance(
        /* [in] */ const String& schemaLanguage,
        /* [out] */ ISchemaFactory** factory);

    static CARAPI NewInstance(
        /* [in] */ const String& schemaLanguage,
        /* [in] */ const String& factoryClassName,
        /* [in] */ IClassLoader* classLoader,
        /* [out] */ ISchemaFactory** factory);
};

} // namespace Validation
} // namespace Xml
} // namespace Elastosx
#endif