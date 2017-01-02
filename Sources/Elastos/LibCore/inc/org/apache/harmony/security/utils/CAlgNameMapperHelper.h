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

#ifndef __ORG_APACHE_HARMONY_SECURITY_UTILS_CALGNAMEMAPPERHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_UTILS_CALGNAMEMAPPERHELPER_H__

#include "_Org_Apache_Harmony_Security_Utils_CAlgNameMapperHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

CarClass(CAlgNameMapperHelper)
    , public Singleton
    , public IAlgNameMapperHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Map2OID(
        /* [in] */ const String& algName,
        /* [out] */ String * pOid);

    CARAPI Map2AlgName(
        /* [in] */ const String& oid,
        /* [out] */ String * pAlgName);

    CARAPI GetStandardName(
        /* [in] */ const String& algName,
        /* [out] */ String * pStandardName);

    CARAPI IsOID(
        /* [in] */ const String& alias,
        /* [out] */ Boolean * pIsOid);

    CARAPI Normalize(
        /* [in] */ const String& oid,
        /* [out] */ String * pNormalString);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_UTILS_CALGNAMEMAPPERHELPER_H__
