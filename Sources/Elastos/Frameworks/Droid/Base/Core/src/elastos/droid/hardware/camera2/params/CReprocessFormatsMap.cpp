
#include "elastos/droid/hardware/camera2/params/CReprocessFormatsMap.h"
#include "elastos/droid/hardware/camera2/params/StreamConfigurationMap.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/core/Math.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Slogger;

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
    //FAIL_RETURN(Preconditions::CheckNotNull(entry, String("entry must not be null")))
    if (entry == NULL) {
        Slogger::E("CMeteringRectangle", "entry must not be null");
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
            // throw new IllegalArgumentException(
            //         String.format("Input %x had no output format length listed", inputFormat));
            Slogger::E("CReprocessFormatsMap", "Input %x had no output format length listed", inputFormat);
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
                // throw new IllegalArgumentException(
                //         String.format(
                //                 "Input %x had too few output formats listed (actual: %d, " +
                //                 "expected: %d)", inputFormat, left, length));
                Slogger::E("CReprocessFormatsMap", "Input %x had too few output formats listed (actual: %d, expected: %d)", inputFormat, left, length);
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
            // throw new AssertionError(
            //         String.format("Input %x had no output format length listed", format));
            Slogger::E("CReprocessFormatsMap", "Input %x had no output format length listed", format);
            return E_ASSERTION_ERROR;
        }

        const Int32 length = (*mEntry)[i];
        left--;
        i++;

        if (length > 0) {
            if (left < length) {
                // throw new AssertionError(
                //         String.format(
                //                 "Input %x had too few output formats listed (actual: %d, " +
                //                 "expected: %d)", format, left, length));
                Slogger::E("CReprocessFormatsMap", "Input %x had too few output formats listed (actual: %d, expected: %d)", format, left, length);
                return E_ASSERTION_ERROR;
            }

            i += length;
            left -= length;
        }

        (*_inputs)[j] = format;
    }
    assert(0);
    //FAIL_RETURN(StreamConfigurationMap::ImageFormatToPublic(_inputs))
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
            // throw new AssertionError(
            //         String.format("Input %x had no output format length listed", format));
            Slogger::E("CReprocessFormatsMap", "Input %x had no output format length listed", format);
            return E_ASSERTION_ERROR;
        }

        const Int32 length = (*mEntry)[i];
        left--;
        i++;

        if (length > 0) {
            if (left < length) {
                // throw new AssertionError(
                //         String.format(
                //                 "Input %x had too few output formats listed (actual: %d, " +
                //                 "expected: %d)", format, left, length));
                Slogger::E("CReprocessFormatsMap", "Input %x had too few output formats listed (actual: %d, expected: %d)", format, left, length);
                return E_ASSERTION_ERROR;
            }
        }

        if (inputFormat == format) {
            AutoPtr<ArrayOf<Int32> > _outputs = ArrayOf<Int32>::Alloc(length);

            // Copying manually faster than System.arraycopy for small arrays
            for (Int32 k = 0; k < length; ++k) {
                (*_outputs)[k] = (*mEntry)[i + k];
            }
            assert(0);
            //FAIL_RETURN(StreamConfigurationMap::ImageFormatToPublic(_outputs))
            *outputs = _outputs;
            REFCOUNT_ADD(*outputs);
            return NOERROR;
        }

        i += length;
        left -= length;
    }

    // throw new IllegalArgumentException(
    //         String.format("Input format %x was not one in #getInputs", format));
    Slogger::E("CReprocessFormatsMap", "Input format %x was not one in #getInputs", format);
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CReprocessFormatsMap::Equals(
    /* [in] */ IInterface *obj,
    /* [out] */ Boolean *equal)
{
    VALIDATE_NOT_NULL(equal);

    if (obj == NULL) {
        *equal = FALSE;
        return NOERROR;
    }
    else if (TO_IINTERFACE(this) == TO_IINTERFACE(obj)) {
        *equal = TRUE;
        return NOERROR;
    }
    else if (IReprocessFormatsMap::Probe(obj) != NULL) {
        const AutoPtr<CReprocessFormatsMap> other = (CReprocessFormatsMap*)IReprocessFormatsMap::Probe(obj);
        // Do not compare anything besides mEntry, since the rest of the values are derived
        *equal = Arrays::Equals(mEntry, other->mEntry);
        return NOERROR;
    }
    *equal = FALSE;
    return NOERROR;
}

ECode CReprocessFormatsMap::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    // Do not hash anything besides mEntry since the rest of the values are derived
    assert(0 && "TODO: weit Hardware/Camera2/Utils/HashCodeHelpers");
    //*hashCode = HashCodeHelpers::GetHashCode(mEntry);
    return NOERROR;
}

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
