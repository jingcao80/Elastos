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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOASYNCQUERYHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOASYNCQUERYHELPER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "_Elastos_Droid_Internal_Telephony_CCallerInfoAsyncQueryHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CCallerInfoAsyncQueryHelper)
    , public Singleton
    , public ICallerInfoAsyncQueryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Factory method to start query with a Uri query spec
     */
    CARAPI StartQuery(
        /* [in] */ Int32 token,
        /* [in] */ IContext* ctx,
        /* [in] */ IUri* contactRef,
        /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
        /* [in] */ IInterface* cookie,
        /* [out] */ ICallerInfoAsyncQuery** cia);

    /**
     * Factory method to start the query based on a number.
     *
     * Note: if the number contains an "@" character we treat it
     * as a SIP address, and look it up directly in the Data table
     * rather than using the PhoneLookup table.
     * TODO: But eventually we should expose two separate methods, one for
     * numbers and one for SIP addresses, and then have
     * PhoneUtils.startGetCallerInfo() decide which one to call based on
     * the phone type of the incoming connection.
     */
    CARAPI StartQuery(
        /* [in] */ Int32 token,
        /* [in] */ IContext* ctx,
        /* [in] */ const String& number,
        /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
        /* [in] */ IInterface* cookie,
        /* [out] */ ICallerInfoAsyncQuery** cia);

    /**
     * Factory method to start the query based on a number with specific subscription.
     *
     * Note: if the number contains an "@" character we treat it
     * as a SIP address, and look it up directly in the Data table
     * rather than using the PhoneLookup table.
     * TODO: But eventually we should expose two separate methods, one for
     * numbers and one for SIP addresses, and then have
     * PhoneUtils.startGetCallerInfo() decide which one to call based on
     * the phone type of the incoming connection.
     */
    CARAPI StartQuery(
        /* [in] */ Int32 token,
        /* [in] */ IContext* ctx,
        /* [in] */ const String& number,
        /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
        /* [in] */ IInterface* cookie,
        /* [in] */ Int64 subId,
        /* [out] */ ICallerInfoAsyncQuery** cia);
};

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOASYNCQUERYHELPER_H__
