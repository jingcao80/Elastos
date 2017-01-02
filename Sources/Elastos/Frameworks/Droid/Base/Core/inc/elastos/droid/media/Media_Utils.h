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

#ifndef __ELASTOS_DROID_MEDIA_MEDIA_UTIL_H__
#define __ELASTOS_DROID_MEDIA_MEDIA_UTIL_H__

#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include <utils/KeyedVector.h>
#include <utils/String8.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <utils/KeyedVector.h>

using Elastos::IO::IByteBuffer;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Media {

// Keep these in sync with their equivalents in MediaCodec.java !!!
enum {
    DEQUEUE_INFO_TRY_AGAIN_LATER            = -1,
    DEQUEUE_INFO_OUTPUT_FORMAT_CHANGED      = -2,
    DEQUEUE_INFO_OUTPUT_BUFFERS_CHANGED     = -3,
};

class Media_Utils
{
public:

   static android::status_t ConvertMessageToMap(
        /* [in] */ android::sp<android::AMessage>& msg,
        /* [out] */ IMap** mymap);

   static CARAPI ConvertKeyValueArraysToKeyedVector(
        /* [in] */ ArrayOf<String>* keys,
        /* [in] */ ArrayOf<String>* values,
        /* [out] */ android::KeyedVector<android::String8,android::String8>* mykeyedVector);

    static android::status_t ConvertKeyValueArraysToMessage(
        /* [in] */ ArrayOf<String>* keys,
        /* [in] */ ArrayOf<IInterface*>* values,
        /* [out] */ android::sp<android::AMessage>* out);

private:
    static const String TAG;
};


} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_MEDIA_UTIL_H__
