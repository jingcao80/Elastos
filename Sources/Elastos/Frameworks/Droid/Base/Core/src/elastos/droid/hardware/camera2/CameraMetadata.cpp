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

#include "elastos/droid/hardware/camera2/CameraMetadata.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

const String CameraMetadata::TAG("CameraMetadataAb");
const Boolean CameraMetadata::VERBOSE = FALSE;//Log.isLoggable(TAG, Log.VERBOSE);

CAR_INTERFACE_IMPL(CameraMetadata, Object, ICameraMetadata)

CameraMetadata::CameraMetadata()
{
}

ECode CameraMetadata::GetKeys(
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist);
    *outlist = NULL;

    assert(0);
    // Class<CameraMetadata<TKey>> thisClass = (Class<CameraMetadata<TKey>>) getClass();
    // return Collections.unmodifiableList(
    //         getKeysStatic(thisClass, getKeyClass(), this, /*filterTags*/null));
    return NOERROR;
}

ECode CameraMetadata::GetKeysStatic(
    /* [in] */ ClassID type,
    /* [in] */ ClassID keyClass,
    /* [in] */ ICameraMetadata* instance,
    /* [in] */ ArrayOf<Int32>* filterTags,
    /* [out] */ IArrayList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = NULL;

    if (VERBOSE) Logger::V(TAG, "getKeysStatic for %d", type);

    assert(0);
    // TotalCaptureResult does not have any of the keys on it, use CaptureResult instead
    // if (type.equals(TotalCaptureResult.class)) {
    //     type = CaptureResult.class;
    // }

    // if (filterTags != null) {
    //     Arrays.sort(filterTags);
    // }

    // ArrayList<TKey> keyList = new ArrayList<TKey>();

    // Field[] fields = type.getDeclaredFields();
    // for (Field field : fields) {
    //     // Filter for Keys that are public
    //     if (field.getType().isAssignableFrom(keyClass) &&
    //             (field.getModifiers() & Modifier.PUBLIC) != 0) {

    //         TKey key;
    //         try {
    //             key = (TKey) field.get(instance);
    //         } catch (IllegalAccessException e) {
    //             throw new AssertionError("Can't get IllegalAccessException", e);
    //         } catch (IllegalArgumentException e) {
    //             throw new AssertionError("Can't get IllegalArgumentException", e);
    //         }

    //         if (instance == null || instance.getProtected(key) != null) {
    //             if (shouldKeyBeAdded(key, field, filterTags)) {
    //                 keyList.add(key);

    //                 if (VERBOSE) {
    //                     Log.v(TAG, "getKeysStatic - key was added - " + key);
    //                 }
    //             } else if (VERBOSE) {
    //                 Log.v(TAG, "getKeysStatic - key was filtered - " + key);
    //             }
    //         }
    //     }
    // }

    // return keyList;
    return NOERROR;
}

// ECode CameraMetadata::ShouldKeyBeAdded(
//     /* [in] */ IInterface* key,
//     /* [in] */ Field field,
//     /* [in] */ ArrayOf<Int32>* filterTags,
//     /* [out] */ Boolean* result)
// {
//     VALIDATE_NOT_NULL(outlist);
//     *result = FALSE;

//     assert(0);
//     // if (key == null) {
//     //     throw new NullPointerException("key must not be null");
//     // }

//     // CameraMetadataNative.Key nativeKey;

//     // /*
//     //  * Get the native key from the public api key
//     //  */
//     // if (key instanceof CameraCharacteristics.Key) {
//     //     nativeKey = ((CameraCharacteristics.Key)key).getNativeKey();
//     // } else if (key instanceof CaptureResult.Key) {
//     //     nativeKey = ((CaptureResult.Key)key).getNativeKey();
//     // } else if (key instanceof CaptureRequest.Key) {
//     //     nativeKey = ((CaptureRequest.Key)key).getNativeKey();
//     // } else {
//     //     // Reject fields that aren't a key
//     //     throw new IllegalArgumentException("key type must be that of a metadata key");
//     // }

//     // if (field.getAnnotation(PublicKey.class) == null) {
//     //     // Never expose @hide keys up to the API user
//     //     return false;
//     // }

//     // // No filtering necessary
//     // if (filterTags == null) {
//     //     return true;
//     // }

//     // if (field.getAnnotation(SyntheticKey.class) != null) {
//     //     // This key is synthetic, so calling #getTag will throw IAE

//     //     // TODO: don't just assume all public+synthetic keys are always available
//     //     return true;
//     // }

//     // /*
//     //  * Regular key: look up it's native tag and see if it's in filterTags
//     //  */

//     // int keyTag = nativeKey.getTag();

//     // // non-negative result is returned iff the value is in the array
//     // return Arrays.binarySearch(filterTags, keyTag) >= 0;
//     return NOERROR;
// }

} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos