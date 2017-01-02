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

#ifndef __ELASTOSX_XML_VALIDATION_CSCHEMAFACTORYHELPER_H__
#define __ELASTOSX_XML_VALIDATION_CSCHEMAFACTORYHELPER_H__

#include "Singleton.h"
#include "_Elastosx_Xml_Validation_CSchemaFactoryHelper.h"

using Elastos::Core::Singleton;
using Elastos::Core::IClassLoader;

namespace Elastosx {
namespace Xml {
namespace Validation {

CarClass(CSchemaFactoryHelper)
    , public Singleton
    , public ISchemaFactoryHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI NewInstance(
        /* [in] */ const String& schemaLanguage,
        /* [out] */ ISchemaFactory** factory);

    CARAPI NewInstance(
        /* [in] */ const String& schemaLanguage,
        /* [in] */ const String& factoryClassName,
        /* [in] */ IClassLoader* classLoader,
        /* [out] */ ISchemaFactory** factory);
};

} // namespace Validation
} // namespace Xml
} // namespace Elastosx

#endif