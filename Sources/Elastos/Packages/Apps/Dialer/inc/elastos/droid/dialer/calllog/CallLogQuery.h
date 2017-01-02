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

#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGQUERY_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGQUERY_H__

#include "_Elastos.Droid.Dialer.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

/**
 * The query for the call log table.
 */
class CallLogQuery
{
public:
    static const AutoPtr<ArrayOf<String> > _PROJECTION;

    static const Int32 ID = 0;
    static const Int32 NUMBER = 1;
    static const Int32 DATE = 2;
    static const Int32 DURATION = 3;
    static const Int32 CALL_TYPE = 4;
    static const Int32 COUNTRY_ISO = 5;
    static const Int32 VOICEMAIL_URI = 6;
    static const Int32 GEOCODED_LOCATION = 7;
    static const Int32 CACHED_NAME = 8;
    static const Int32 CACHED_NUMBER_TYPE = 9;
    static const Int32 CACHED_NUMBER_LABEL = 10;
    static const Int32 CACHED_LOOKUP_URI = 11;
    static const Int32 CACHED_MATCHED_NUMBER = 12;
    static const Int32 CACHED_NORMALIZED_NUMBER = 13;
    static const Int32 CACHED_PHOTO_ID = 14;
    static const Int32 CACHED_FORMATTED_NUMBER = 15;
    static const Int32 IS_READ = 16;
    static const Int32 NUMBER_PRESENTATION = 17;
    static const Int32 ACCOUNT_COMPONENT_NAME = 18;
    static const Int32 ACCOUNT_ID = 19;
    static const Int32 FEATURES = 20;
    static const Int32 DATA_USAGE = 21;
    static const Int32 TRANSCRIPTION = 22;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CALLLOGQUERY_H__
