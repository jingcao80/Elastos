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

#ifndef __ELASTOS_DROID_PROVIDER_CTELEPHONYSMSDRAFT_H__
#define __ELASTOS_DROID_PROVIDER_CTELEPHONYSMSDRAFT_H__

#include "_Elastos_Droid_Provider_CTelephonySmsDraft.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IContentResolver;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CTelephonySmsDraft)
    , public Singleton
    , public ITelephonySmsDraft
    , public IBaseColumns
    , public ITelephonyTextBasedSmsColumns
{
public:
    CAR_INTERFACE_DECL();
    CAR_SINGLETON_DECL();

    /**
      * Add an SMS to the Draft box.
      *
      * @param resolver the content resolver to use
      * @param address the address of the sender
      * @param body the body of the message
      * @param subject the pseudo-subject of the message
      * @param date the timestamp for the message
      * @param read TRUE if the message has been read, FALSE if not
      * @return the URI for the new message
      * @hide
      */
    CARAPI AddMessage(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& address,
        /* [in] */ const String& body,
        /* [in] */ const String& subject,
        /* [in] */ Int64 date,
        /* [out] */ IUri** uri);

    /**
      * Add an SMS to the Draft box.
      *
      * @param resolver the content resolver to use
      * @param address the address of the sender
      * @param body the body of the message
      * @param subject the psuedo-subject of the message
      * @param date the timestamp for the message
      * @param read TRUE if the message has been read, FALSE if not
      * @param subId the sub_id which the message belongs to
      * @return the URI for the new message
      * @hide
      */
    CARAPI AddMessage(
        /* [in] */ Int64 subId,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& address,
        /* [in] */ const String& body,
        /* [in] */ const String& subject,
        /* [in] */ Int64 date,
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CTELEPHONYSMSDRAFT_H__
