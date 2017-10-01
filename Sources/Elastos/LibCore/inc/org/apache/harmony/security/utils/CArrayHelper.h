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

#ifndef __ORG_APACHE_HARMONY_SECURITY_UTILS_CARRAYHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_UTILS_CARRAYHELPER_H__

#include "_Org_Apache_Harmony_Security_Utils_CArrayHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

/**
 * Utility class for arrays
 *
 */
CarClass(CArrayHelper)
    , public Singleton
    , public IArrayHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetBytesAsString(
        /* [in] */ ArrayOf<Byte> * pData,
        /* [out] */ String * pStr);

    using Singleton::ToString;

    CARAPI ToString(
        /* [in] */ ArrayOf<Byte> * pArray,
        /* [in] */ const String& prefix,
        /* [out] */ String * pStr);
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_UTILS_CARRAYHELPER_H__
