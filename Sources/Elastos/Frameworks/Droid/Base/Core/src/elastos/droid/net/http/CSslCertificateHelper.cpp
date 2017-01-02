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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/http/CSslCertificate.h"
#include "elastos/droid/net/http/CSslCertificateHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(CSslCertificateHelper, Singleton, ISslCertificateHelper)

CAR_SINGLETON_IMPL(CSslCertificateHelper)

ECode CSslCertificateHelper::SaveState(
    /* [in] */ ISslCertificate* certificate,
    /* [out] */ IBundle** bundle)
{
    return CSslCertificate::SaveState(certificate, bundle);
}

ECode CSslCertificateHelper::RestoreState(
    /* [in] */ IBundle* bundle,
    /* [out] */ ISslCertificate** certificate)
{
    return CSslCertificate::RestoreState(bundle, certificate);
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
