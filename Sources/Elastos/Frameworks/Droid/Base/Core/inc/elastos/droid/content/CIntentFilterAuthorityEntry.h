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

#ifndef __ELASTOS_DROID_CONTENT_CINTENTFILTERAUTHORITYENTRY_H__
#define __ELASTOS_DROID_CONTENT_CINTENTFILTERAUTHORITYENTRY_H__

#include "_Elastos_Droid_Content_CIntentFilterAuthorityEntry.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CIntentFilterAuthorityEntry)
    , public Object
    , public IIntentFilterAuthorityEntry
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CIntentFilterAuthorityEntry();

    virtual ~CIntentFilterAuthorityEntry();

    CARAPI GetHost(
        /* [out] */ String* host);

    CARAPI GetPort(
        /* [out] */ Int32* port);

    CARAPI GetWild(
        /* [out] */ Boolean* wild);

    CARAPI Match(
        /* [in] */ IIntentFilterAuthorityEntry* other,
        /* [out] */ Boolean* result);

    /**
     * Determine whether this AuthorityEntry matches the given data Uri.
     * <em>Note that this comparison is case-sensitive, unlike formal
     * RFC host names.  You thus should always normalize to lower-case.</em>
     *
     * @param data The Uri to match.
     * @return Returns either {@link IntentFilter#NO_MATCH_DATA},
     * {@link IntentFilter#MATCH_CATEGORY_PORT}, or
     * {@link IntentFilter#MATCH_CATEGORY_HOST}.
     */
    CARAPI Match(
        /* [in] */ IUri* data,
        /* [out] */ Int32* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ const String& port);

private:
    String mOrigHost;
    String mHost;
    Boolean mWild;
    Int32 mPort;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CINTENTFILTERAUTHORITYENTRY_H__
