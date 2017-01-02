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

#include "elastos/droid/hardware/camera2/params/CReprocessFormatsMap.h"
#include "elastos/droid/hardware/camera2/params/StreamConfigurationMap.h"
#include "elastos/droid/hardware/camera2/utils/HashCodeHelpers.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/core/Math.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Hardware::Camera2::Utils::HashCodeHelpers;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CAR_INTERFACE_IMPL(CReprocessFormatsMap, Object, IReprocessFormatsMap)

CAR_OBJECT_IMPL(CReprocessFormatsMap)

CReprocessFormatsMap::CReprocessFormatsMap()
{
}

CReprocessFormatsMap::~CReprocessFormatsMap()
{
}

ECode CReprocessFormatsMap::constructor()
{
    return NOERROR;
}

ECode CReprocessFormatsMap::constructor(
    /* [in] */ ArrayOf<Int32>* entry)
{
    if (entry == NULL) {
        Logger::E("CMeteringRectangle", "entry must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }

    Int32 numInputs = 0;
    Int32 left = entry->GetLength();
    for (Int32 i = 0; i < left; ) {
        Int32 inputFormat;
        FAIL_RETURN(StreamConfigurationMap::CheckArgumentFormatInternal((*entry)[i], &inputFormat))

        left--;
        i++;

        if (left < 1) {
            Logger::E("CReprocessFormatsMap", "Input %x had no output format length listed", inputFormat);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        const Int32 length = (*entry)[i];
        left--;
        i++;

        for (Int32 j = 0; j < length; ++j) {
            Int32 outputFormat = (*entry)[i + j];
            Int32 ret;
            FAIL_RETURN(StreamConfigurationMap::CheckArgumentFormatInternal(outputFormat, &ret))
        }

        if (length > 0) {
            if (left < length) {
                Logger::E("CReprocessFormatsMap", "Input %x had too few output formats listed (actual: %d, expected: %d)", inputFormat, left, length);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            i += length;
            left -= length;
        }

        numInputs++;
    }

    mEntry = entry;
    mInputCount = numInputs;
    return NOERROR;
}

ECode CReprocessFormatsMap::GetInputs(
    /* [out, callee] */ ArrayOf<Int32>** inputs)
{
    VALIDATE_NOT_NULL(inputs);
    *inputs = NULL;

    AutoPtr<ArrayOf<Int32> > _inputs = ArrayOf<Int32>::Alloc(mInputCount);

    Int32 left = mEntry->GetLength();
    for (Int32 i = 0, j = 0; i < left; j++) {
        const Int32 format = (*mEntry)[i];

        left--;
        i++;

        if (left < 1) {
            Logger::E("CReprocessFormatsMap", "Input %x had no output format length listed", format);
            return E_ASSERTION_ERROR;
        }

        const Int32 length = (*mEntry)[i];
        left--;
        i++;

        if (length > 0) {
            if (left < length) {
                Logger::E("CReprocessFormatsMap", "Input %x had too few output formats listed (actual: %d, expected: %d)", format, left, length);
                return E_ASSERTION_ERROR;
            }

            i += length;
            left -= length;
        }

        (*_inputs)[j] = format;
    }
    FAIL_RETURN(StreamConfigurationMap::ImageFormatToPublic(_inputs))
    *inputs = _inputs;
    REFCOUNT_ADD(*inputs);
    return NOERROR;
}

ECode CReprocessFormatsMap::GetOutputs(
    /* [in] */ Int32 format,
    /* [out, callee] */ ArrayOf<Int32>** outputs)
{
    VALIDATE_NOT_NULL(outputs);
    *outputs = NULL;

    Int32 left = mEntry->GetLength();
    for (Int32 i = 0; i < left; ) {
        const Int32 inputFormat = (*mEntry)[i];

        left--;
        i++;

        if (left < 1) {
            Logger::E("CReprocessFormatsMap", "Input %x had no output format length listed", format);
            return E_ASSERTION_ERROR;
        }

        const Int32 length = (*mEntry)[i];
        left--;
        i++;

        if (length > 0) {
            if (left < length) {
                Logger::E("CReprocessFormatsMap", "Input %x had too few output formats listed (actual: %d, expected: %d)", format, left, length);
                return E_ASSERTION_ERROR;
            }
        }

        if (inputFormat == format) {
            AutoPtr<ArrayOf<Int32> > _outputs = ArrayOf<Int32>::Alloc(length);

            // Copying manually faster than System.arraycopy for small arrays
            for (Int32 k = 0; k < length; ++k) {
                (*_outputs)[k] = (*mEntry)[i + k];
            }
            FAIL_RETURN(StreamConfigurationMap::ImageFormatToPublic(_outputs))
            *outputs = _outputs;
            REFCOUNT_ADD(*outputs);
            return NOERROR;
        }

        i += length;
        left -= length;
    }

    Logger::E("CReprocessFormatsMap", "Input format %x was not one in #getInputs", format);
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CReprocessFormatsMap::Equals(
    /* [in] */ IInterface *obj,
    /* [out] */ Boolean *equal)
{
    VALIDATE_NOT_NULL(equal);
    *equal = FALSE;

    IReprocessFormatsMap* rfm = IReprocessFormatsMap::Probe(obj);
    if (rfm == NULL) {
        return NOERROR;
    }

    CReprocessFormatsMap* other = (CReprocessFormatsMap*)rfm;
    // Do not compare anything besides mEntry, since the rest of the values are derived
    *equal = Arrays::Equals(mEntry, other->mEntry);
    return NOERROR;
}

ECode CReprocessFormatsMap::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    return HashCodeHelpers::GetHashCode(mEntry, hashCode);
}

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
