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

#ifndef __ELASTOSX_SECURITY_CERT_CX509CERTIFICATEHELPER_H__
#define __ELASTOSX_SECURITY_CERT_CX509CERTIFICATEHELPER_H__

#include "_Elastosx_Security_Cert_CX509CertificateHelper.h"
#include "X509CertificateX.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastosx {
namespace Security {
namespace Cert {

CarClass(CX509CertificateHelper)
    , public Singleton
    , public IX509CertificateHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Creates a new {@code X509Certificate} and initializes it from the
     * specified input stream.
     *
     * @param inStream
     *            input stream containing data to initialize the certificate.
     * @return the certificate initialized from the specified input stream
     * @throws CertificateException
     *             if the certificate cannot be created or initialized.
     */
    CARAPI GetInstance(
        /* [in] */ IInputStream *inStream,
        /* [out] */ IX509Certificate **cert);

    /**
     * Creates a new {@code X509Certificate} and initializes it from the
     * specified byte array.
     *
     * @param certData
     *            byte array containing data to initialize the certificate.
     * @return the certificate initialized from the specified byte array
     * @throws CertificateException
     *             if the certificate cannot be created or initialized.
     */
    CARAPI GetInstance(
        /* [in] */ ArrayOf<Byte>* certData,
        /* [out] */ IX509Certificate **cert);
};

} // namespace Cert
} // namespace Security
} // namespace Elastosx

#endif // __ELASTOSX_SECURITY_CERT_CX509CERTIFICATEHELPER_H__
