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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CREPROCESSFORMATSMAP_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CREPROCESSFORMATSMAP_H__

#include "_Elastos_Droid_Hardware_Camera2_Params_CReprocessFormatsMap.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::ISurface;
using Elastos::Droid::Utility::ISize;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CarClass(CReprocessFormatsMap)
    , public Object
    , public IReprocessFormatsMap
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CReprocessFormatsMap();

    virtual ~CReprocessFormatsMap();

    CARAPI constructor();

    /**
     * Create a new {@link ReprocessFormatsMap}
     *
     * <p>This value is encoded as a variable-size array-of-arrays.
     * The inner array always contains {@code [format, length, ...]} where ... has length elements.
     * An inner array is followed by another inner array if the total metadata entry size hasn't
     * yet been exceeded.</p>
     *
     * <p>Entry must not be {@code null}. Empty array is acceptable.</p>
     *
     * <p>The entry array ownership is passed to this instance after construction; do not
     * write to it afterwards.</p>
     *
     * @param entry Array of ints, not yet deserialized (not-null)
     *
     * @throws IllegalArgumentException
     *              if the data was poorly formatted
     *              (missing output format length or too few output formats)
     *              or if any of the input/formats were not valid
     * @throws NullPointerException
     *              if entry was null
     *
     * @see StreamConfigurationMap#checkArgumentFormatInternal
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Int32>* entry);

    /**
     * Get a list of all input image formats that can be used to reprocess an input
     * stream into an output stream.
     *
     * <p>Use this input format to look up the available output formats with {@link #getOutputs}.
     * </p>
     *
     * @return an array of inputs (possibly empty, but never {@code null})
     *
     * @see ImageFormat
     * @see #getOutputs
     */
    CARAPI GetInputs(
        /* [out, callee] */ ArrayOf<Int32>** inputs);

    /**
     * Get the list of output formats that can be reprocessed into from the input {@code format}.
     *
     * <p>The input {@code format} must be one of the formats returned by {@link #getInputs}.</p>
     *
     * @param format an input format
     *
     * @return list of output image formats
     *
     * @see ImageFormat
     * @see #getInputs
     */
    CARAPI GetOutputs(
        /* [in] */ Int32 format,
        /* [out, callee] */ ArrayOf<Int32>** outputs);

    /**
     * Check if this {@link ReprocessFormatsMap} is equal to another
     * {@link ReprocessFormatsMap}.
     *
     * <p>These two objects are only equal if and only if each of the respective elements is equal.
     * </p>
     *
     * @return {@code true} if the objects were equal, {@code false} otherwise
     */
    //@Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* value);

    /**
    * {@inheritDoc}
     */
    //@Override
    CARAPI GetHashCode(
        /* [out] */ Int32* value);

private:
    AutoPtr<ArrayOf<Int32> > mEntry;
    /*
     * Dependent fields: values are derived from mEntry
     */
    Int32 mInputCount;
};

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CREPROCESSFORMATSMAP_H__
