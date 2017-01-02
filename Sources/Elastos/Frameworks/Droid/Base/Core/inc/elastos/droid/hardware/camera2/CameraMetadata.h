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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_CAMERAMETADATA_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_CAMERAMETADATA_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

class ECO_PUBLIC CameraMetadata
    : public Object
    , public ICameraMetadata
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns a list of the keys contained in this map.
     *
     * <p>The list returned is not modifiable, so any attempts to modify it will throw
     * a {@code UnsupportedOperationException}.</p>
     *
     * <p>All values retrieved by a key from this list with {@code #get} are guaranteed to be
     * non-{@code null}. Each key is only listed once in the list. The order of the keys
     * is undefined.</p>
     *
     * @return List of the keys contained in this map.
     */
    // @SuppressWarnings("unchecked")
    CARAPI GetKeys(
        /* [out] */ IList** outlist);

    /**
     * Return a list of all the Key<?> that are declared as a field inside of the class
     * {@code type}.
     *
     * <p>
     * Optionally, if {@code instance} is not null, then filter out any keys with null values.
     * </p>
     *
     * <p>
     * Optionally, if {@code filterTags} is not {@code null}, then filter out any keys
     * whose native {@code tag} is not in {@code filterTags}. The {@code filterTags} array will be
     * sorted as a side effect.
     * </p>
     */
    // /*package*/ @SuppressWarnings("unchecked")
    static CARAPI GetKeysStatic(
        /* [in] */ ClassID type,
        /* [in] */ ClassID keyClass,
        /* [in] */ ICameraMetadata* instance,
        /* [in] */ ArrayOf<Int32>* filterTags,
        /* [out] */ IArrayList** list);

protected:
    /**
     * Set a camera metadata field to a value. The field definitions can be
     * found in {@link CameraCharacteristics}, {@link CaptureResult}, and
     * {@link CaptureRequest}.
     *
     * @param key The metadata field to write.
     * @param value The value to set the field to, which must be of a matching
     * type to the key.
     *
     * @hide
     */
    CameraMetadata();

private:
    // //@SuppressWarnings("rawtypes")
    // static CARAPI ShouldKeyBeAdded(
    //     /* [in] */ IInterface* key,
    //     /* [in] */ Field field,
    //     /* [in] */ ArrayOf<Int32>* filterTags,
    //     /* [out] */ Boolean* result);

private:
    static const String TAG;
    static const Boolean VERBOSE;
};

} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_CAMERAMETADATA_H__
