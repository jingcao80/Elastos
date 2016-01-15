
#ifndef __ELASTOS_DROID_MEDIA_MEDIA_UTIL_H__
#define __ELASTOS_DROID_MEDIA_MEDIA_UTIL_H__

#include "elastos/droid/ext/frameworkext.h"
#include <utils/KeyedVector.h>
#include <utils/String8.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <utils/KeyedVector.h>

using Elastos::IO::IByteBuffer;

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

   static CARAPI ConvertMessageToMap(
        /* [in] */ android::sp<android::AMessage>& msg,
        /* [out] */ IObjectStringMap** mymap);

   static CARAPI ConvertKeyValueArraysToKeyedVector(
        /* [in] */ ArrayOf<String>* keys,
        /* [in] */ ArrayOf<String>* values,
        /* [out] */ android::KeyedVector<android::String8,android::String8>* mykeyedVector);

    static CARAPI ConvertKeyValueArraysToMessage(
        /* [in] */ ArrayOf<String>* keys,
        /* [in] */ ArrayOf<IInterface*>* values,
        /* [out] */ android::sp<android::AMessage> * out);

    static CARAPI ThrowExceptionAsNecessary(
        /* [in] */ android::status_t err,
        /* [out] */ Int32* result);
private:
    static const String TAG;
};


} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_MEDIA_UTIL_H__
