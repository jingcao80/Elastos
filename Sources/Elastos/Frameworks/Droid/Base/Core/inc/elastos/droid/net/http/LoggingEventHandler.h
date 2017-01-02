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

#ifndef __ELASTOS_DROID_NET_HTTP_LOGGINGEVENTHANDLER_H__
#define __ELASTOS_DROID_NET_HTTP_LOGGINGEVENTHANDLER_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * {@hide}
 */
class LoggingEventHandler
    : public Object
    , public ILoggingEventHandler
    , public IEventHandler
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI RequestSent();

    CARAPI Status(
        /* [in] */ Int32 major_version,
        /* [in] */ Int32 minor_version,
        /* [in] */ Int32 code, /* Status-Code value */
        /* [in] */ const String& reason_phrase);

     CARAPI Headers(
        /* [in] */ IHeaders* headers);

    CARAPI LocationChanged(
        /* [in] */ const String& newLocation,
        /* [in] */ Boolean permanent);

    CARAPI Data(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 len);

    CARAPI EndData();

    CARAPI Certificate(
        /* [in] */ ISslCertificate* certificate);

    CARAPI Error(
        /* [in] */ Int32 id,
        /* [in] */ const String& description);

    CARAPI HandleSslErrorRequest(
        /* [in] */ ISslError* error,
        /* [out] */ Boolean* result);
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_LOGGINGEVENTHANDLER_H__
