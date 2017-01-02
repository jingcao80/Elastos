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

#ifndef __ELASTOS_DROID_NET_HTTP_CSSLCERTIFICATEHELPER_H__
#define __ELASTOS_DROID_NET_HTTP_CSSLCERTIFICATEHELPER_H__

#include "_Elastos_Droid_Net_Http_CSslCertificateHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * SSL certificate info (certificate details) class
 */
CarClass(CSslCertificateHelper)
    , public Singleton
    , public ISslCertificateHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Saves the certificate state to a bundle
     * @param certificate The SSL certificate to store
     * @return A bundle with the certificate stored in it or null if fails
     */
    CARAPI SaveState(
        /* [in] */ ISslCertificate* certificate,
        /* [out] */ IBundle** bundle);

    /**
     * Restores the certificate stored in the bundle
     * @param bundle The bundle with the certificate state stored in it
     * @return The SSL certificate stored in the bundle or null if fails
     */
    CARAPI RestoreState(
        /* [in] */ IBundle* bundle,
        /* [out] */ ISslCertificate** certificate);
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_HTTP_CSSLCERTIFICATEHELPER_H__
