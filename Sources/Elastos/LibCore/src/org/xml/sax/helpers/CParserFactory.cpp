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

#include "CParserFactory.h"
#include "NewInstanceHelper.h"
#include "CSystem.h"
#include <utils/Log.h>

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

CAR_INTERFACE_IMPL(CParserFactory, Singleton, IParserFactory)

CAR_SINGLETON_IMPL(CParserFactory)

ECode CParserFactory::MakeParser(
    /* [out] */ IParser** parser)
{
    return ParserFactory::MakeParser(parser);
}

ECode CParserFactory::MakeParser(
    /* [in] */ const String& className,
    /* [out] */ IParser** parser)
{
    return ParserFactory::MakeParser(className, parser);
}

ECode ParserFactory::MakeParser(
    /* [out] */ IParser** parser)
{
    VALIDATE_NOT_NULL(parser);
    *parser = NULL;

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    String className;
    system->GetProperty(String("org.xml.sax.parser"), &className);
    if (className.IsNullOrEmpty()) {
        ALOGE("ParserFactory::MakeParser NullPointerException: No value for sax.parser property!");
        return E_NULL_POINTER_EXCEPTION;
    } else {
        return ParserFactory::MakeParser(className, parser);
    }

    return NOERROR;
}

ECode ParserFactory::MakeParser(
    /* [in] */ const String& className,
    /* [out] */ IParser** parser)
{
    VALIDATE_NOT_NULL(parser);

    AutoPtr<IClassLoader> icl = NewInstanceHelper::GetClassLoader();
    AutoPtr<IInterface> ointer = NewInstanceHelper::NewInstance(icl, className);
    *parser = IParser::Probe(ointer.Get());
    REFCOUNT_ADD(*parser)

    return NOERROR;
}

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org
