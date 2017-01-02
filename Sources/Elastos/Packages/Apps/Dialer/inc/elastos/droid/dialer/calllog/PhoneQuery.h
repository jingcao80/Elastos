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

#ifndef __ELASTOS_DROID_DIALER_CALLLOG_PHONEQUERY_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_PHONEQUERY_H__

#include "_Elastos.Droid.Dialer.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

/**
 * The query to look up the {@link ContactInfo} for a given number in the Call Log.
 */
class PhoneQuery
{
public:
    static const AutoPtr<ArrayOf<String> > _PROJECTION;

    static const Int32 PERSON_ID = 0;
    static const Int32 NAME = 1;
    static const Int32 PHONE_TYPE = 2;
    static const Int32 LABEL = 3;
    static const Int32 MATCHED_NUMBER = 4;
    static const Int32 NORMALIZED_NUMBER = 5;
    static const Int32 PHOTO_ID = 6;
    static const Int32 LOOKUP_KEY = 7;
    static const Int32 PHOTO_URI = 8;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_PHONEQUERY_H__
