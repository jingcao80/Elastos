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

#include "CSchemaFactoryHelper.h"
#include "SchemaFactory.h"

namespace Elastosx {
namespace Xml {
namespace Validation {

CAR_SINGLETON_IMPL(CSchemaFactoryHelper)

CAR_INTERFACE_IMPL(CSchemaFactoryHelper, Singleton, ISchemaFactoryHelper)

ECode CSchemaFactoryHelper::NewInstance(
    /* [in] */ const String& schemaLanguage,
    /* [out] */ ISchemaFactory** factory)
{
    return SchemaFactory::NewInstance(schemaLanguage, factory);
}

ECode CSchemaFactoryHelper::NewInstance(
    /* [in] */ const String& schemaLanguage,
    /* [in] */ const String& factoryClassName,
    /* [in] */ IClassLoader* classLoader,
    /* [out] */ ISchemaFactory** factory)
{
    return SchemaFactory::NewInstance(schemaLanguage, factoryClassName, classLoader, factory);
}

} // namespace Validation
} // namespace Xml
} // namespace Elastosx