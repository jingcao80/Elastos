
#ifndef __ELASTOS_DROID_APP_CPROFILERINFO_H__
#define __ELASTOS_DROID_APP_CPROFILERINFO_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_App_CProfilerInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * System private API for passing profiler settings.
 *
 * {@hide}
 */
CarClass(CProfilerInfo)
    , public Object
    , public IProfilerInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CProfilerInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& filename,
        /* [in] */ IParcelFileDescriptor* fd,
        /* [in] */ Int32 interval,
        /* [in] */ Boolean autoStop);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    /* Name of profile output file. */
    CARAPI GetProfileFile(
        /* [out] */ String* str);

    CARAPI SetProfileFile(
        /* [in] */ const String& str);

    /* File descriptor for profile output file, can be null. */
    CARAPI GetProfileFd(
        /* [out] */ IParcelFileDescriptor** fd);

    CARAPI SetProfiledFd(
        /* [in] */ IParcelFileDescriptor* fd);

    /* Indicates sample profiling when nonzero, interval in microseconds. */
    CARAPI GetSamplingInterval(
        /* [out] */ Int32* result);

    CARAPI SetSamplingInterval(
        /* [in] */ Int32 result);

    /* Automatically stop the profiler when the app goes idle. */
    CARAPI IsAutoStopProfiler(
        /* [out] */ Boolean* result);

    CARAPI SetAutoStopProfiler(
        /* [in] */ Boolean result);

public:
    /* Name of profile output file. */
    String mProfileFile;

    /* File descriptor for profile output file, can be null. */
    AutoPtr<IParcelFileDescriptor> mProfileFd;

    /* Indicates sample profiling when nonzero, interval in microseconds. */
    Int32 mSamplingInterval;

    /* Automatically stop the profiler when the app goes idle. */
    Boolean mAutoStopProfiler;

};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CPROFILERINFO_H__
