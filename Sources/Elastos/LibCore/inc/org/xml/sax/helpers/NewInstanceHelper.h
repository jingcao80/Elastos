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

#ifndef __ORG_XML_SAX_HELPERS_NEWINSTANCEHELPER_H__
#define __ORG_XML_SAX_HELPERS_NEWINSTANCEHELPER_H__

#include "Object.h"

using Elastos::Core::IClassLoader;
using namespace Elastos;

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

class NewInstanceHelper {

public:
    /**
     * Creates a new instance of the specified class name
     *
     * Package private so this code is not exposed at the API level.
     */
    static AutoPtr<IInterface> NewInstance (
        /* [in] */ IClassLoader* classLoader,
        /* [in] */ const String& className);

    /**
     * Figure out which ClassLoader to use.  For JDK 1.2 and later use
     * the context ClassLoader.
     */
    static AutoPtr<IClassLoader> GetClassLoader ();

private:
    NewInstanceHelper();
    NewInstanceHelper(const NewInstanceHelper&);
};

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org

#endif // __ORG_XML_SAX_HELPERS_NEWINSTANCEHELPER_H__
