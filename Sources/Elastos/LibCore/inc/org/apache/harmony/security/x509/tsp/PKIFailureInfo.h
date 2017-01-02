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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_PKFAILUREINFO_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_PKFAILUREINFO_H__

#include "Elastos.CoreLibrary.Apache.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

class PKIFailureInfo
    : public Object
    , public IPKIFailureInfo
{
public:
    CAR_INTERFACE_DECL()

    PKIFailureInfo(
        /* [in] */ Int32 value);

    /**
     * @return int value of the failure
     */
    CARAPI GetValue(
        /* [out] */ Int32* value);

    /**
     * @return maximum of values in the enum
     */
    static CARAPI GetMaxValue(
        /* [out] */ Int32* value);

    /**
     * @param value
     * @return
     */
    static CARAPI GetInstance(
        /* [in] */ Int32 value,
        /* [out] */ IPKIFailureInfo** instance);

private:
    Int32 mValue;

    static Int32 mMaxValue;
};

} //namespace Tsp
} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_PKFAILUREINFO_H__
