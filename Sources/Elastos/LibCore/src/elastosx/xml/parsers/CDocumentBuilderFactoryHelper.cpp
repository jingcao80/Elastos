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

#include "CDocumentBuilderFactoryHelper.h"
#include "DocumentBuilderFactory.h"

namespace Elastosx {
namespace Xml {
namespace Parsers {

CAR_SINGLETON_IMPL(CDocumentBuilderFactoryHelper)

CAR_INTERFACE_IMPL(CDocumentBuilderFactoryHelper, Singleton, IDocumentBuilderFactoryHelper)

ECode CDocumentBuilderFactoryHelper::NewInstance(
    /* [out] */ IDocumentBuilderFactory** instance)
{
    return DocumentBuilderFactory::NewInstance(instance);
}

ECode CDocumentBuilderFactoryHelper::NewInstance(
    /* [in] */ const String& factoryClassName,
    /* [in] */ IClassLoader* classLoader,
    /* [out] */ IDocumentBuilderFactory** instance)
{
    return DocumentBuilderFactory::NewInstance(factoryClassName, classLoader, instance);
}

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx