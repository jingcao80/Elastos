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

#include "elastos/droid/dialer/calllog/CallLogQuery.h"
#include "Elastos.Droid.Provider.h"

using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::ICalls;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

static AutoPtr<ArrayOf<String> > InitProjection()
{
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(23);
    (*projection)[0] = IBaseColumns::ID;
    (*projection)[1] = ICalls::NUMBER;
    (*projection)[2] = ICalls::DATE;
    (*projection)[3] = ICalls::DURATION;
    (*projection)[4] = ICalls::TYPE;
    (*projection)[5] = ICalls::COUNTRY_ISO;
    (*projection)[6] = ICalls::VOICEMAIL_URI;
    (*projection)[7] = ICalls::GEOCODED_LOCATION;
    (*projection)[8] = ICalls::CACHED_NAME;
    (*projection)[9] = ICalls::CACHED_NUMBER_TYPE;
    (*projection)[10] = ICalls::CACHED_NUMBER_LABEL;
    (*projection)[11] = ICalls::CACHED_LOOKUP_URI;
    (*projection)[12] = ICalls::CACHED_MATCHED_NUMBER;
    (*projection)[13] = ICalls::CACHED_NORMALIZED_NUMBER;
    (*projection)[14] = ICalls::CACHED_PHOTO_ID;
    (*projection)[15] = ICalls::CACHED_FORMATTED_NUMBER;
    (*projection)[16] = ICalls::IS_READ;
    (*projection)[17] = ICalls::NUMBER_PRESENTATION;
    (*projection)[18] = ICalls::PHONE_ACCOUNT_COMPONENT_NAME;
    (*projection)[19] = ICalls::PHONE_ACCOUNT_ID;
    (*projection)[20] = ICalls::FEATURES;
    (*projection)[21] = ICalls::DATA_USAGE;
    (*projection)[22] = ICalls::TRANSCRIPTION;
    return projection;
}
const AutoPtr<ArrayOf<String> > CallLogQuery::_PROJECTION = InitProjection();
const Int32 CallLogQuery::ID;
const Int32 CallLogQuery::NUMBER;
const Int32 CallLogQuery::DATE;
const Int32 CallLogQuery::DURATION;
const Int32 CallLogQuery::CALL_TYPE;
const Int32 CallLogQuery::COUNTRY_ISO;
const Int32 CallLogQuery::VOICEMAIL_URI;
const Int32 CallLogQuery::GEOCODED_LOCATION;
const Int32 CallLogQuery::CACHED_NAME;
const Int32 CallLogQuery::CACHED_NUMBER_TYPE;
const Int32 CallLogQuery::CACHED_NUMBER_LABEL;
const Int32 CallLogQuery::CACHED_LOOKUP_URI;
const Int32 CallLogQuery::CACHED_MATCHED_NUMBER;
const Int32 CallLogQuery::CACHED_NORMALIZED_NUMBER;
const Int32 CallLogQuery::CACHED_PHOTO_ID;
const Int32 CallLogQuery::CACHED_FORMATTED_NUMBER;
const Int32 CallLogQuery::IS_READ;
const Int32 CallLogQuery::NUMBER_PRESENTATION;
const Int32 CallLogQuery::ACCOUNT_COMPONENT_NAME;
const Int32 CallLogQuery::ACCOUNT_ID;
const Int32 CallLogQuery::FEATURES;
const Int32 CallLogQuery::DATA_USAGE;
const Int32 CallLogQuery::TRANSCRIPTION;

} // CallLog
} // Dialer
} // Droid
} // Elastos
