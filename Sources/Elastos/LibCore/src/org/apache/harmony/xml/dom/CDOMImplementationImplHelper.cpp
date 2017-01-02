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

#include "org/apache/harmony/xml/dom/CDOMImplementationImplHelper.h"
#include "org/apache/harmony/xml/dom/CDOMImplementationImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_SINGLETON_IMPL(CDOMImplementationImplHelper)
CAR_INTERFACE_IMPL(CDOMImplementationImplHelper, Singleton, IDOMImplementationImplHelper)

AutoPtr<IDOMImplementation> CDOMImplementationImplHelper::instance;

ECode CDOMImplementationImplHelper::GetInstance(
    /* [out] */ Org::W3c::Dom::IDOMImplementation ** ppInstance)
{
    VALIDATE_NOT_NULL(ppInstance);
    if (instance == NULL) {
        instance = new CDOMImplementationImpl();
    }

    *ppInstance = instance;
    REFCOUNT_ADD(*ppInstance);
    return NOERROR;
}

}
}
}
}
}

