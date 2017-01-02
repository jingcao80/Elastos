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

#ifndef __ELASTOS_SECURITY_CERT_CCOLLECTIONCERTSTOREPARAMETERS_H__
#define __ELASTOS_SECURITY_CERT_CCOLLECTIONCERTSTOREPARAMETERS_H__

#include "_Elastos_Security_Cert_CCollectionCertStoreParameters.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "core/Object.h"

using Elastos::Utility::ICollection;
using Elastos::Core::ICloneable;

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CCollectionCertStoreParameters)
    , public Object
    , public ICollectionCertStoreParameters
    , public ICertStoreParameters
    , public ICloneable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Clone(
        /* [out] */ IInterface **object);

    CARAPI GetCollection(
        /* [out] */ ICollection **collection);

    CARAPI ToString(
        /* [out] */ String *str);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICollection *collection);

private:
    // Default empty and immutable collection.
    // Used if <code>CollectionCertStoreParameters</code>instance
    // created by the no arg constructor
    static const AutoPtr<ICollection> mDefaultCollection;
    // A <code>Collection</code> of <code>Certificate</code>s
    // and <code>CRL</code>s
    AutoPtr<ICollection> mCollection;
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CCOLLECTIONCERTSTOREPARAMETERS_H__
