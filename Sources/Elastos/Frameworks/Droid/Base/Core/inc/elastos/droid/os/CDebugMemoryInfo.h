
#ifndef __ELASTOS_DROID_OS_CDEBUGMEMORYINFO_H__
#define __ELASTOS_DROID_OS_CDEBUGMEMORYINFO_H__

#include "_Elastos_Droid_Os_CDebugMemoryInfo.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * This class is used to retrieved various statistics about the memory mappings for this
 * process. The returns info broken down by dalvik, native, and other. All results are in kB.
 */
CarClass(CDebugMemoryInfo)
    , public Object
    , public IDebugMemoryInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDebugMemoryInfo();

    ~CDebugMemoryInfo();

    CARAPI constructor();

    CARAPI GetTotalUss(
        /* [out] */ Int32* totalUss);

    CARAPI GetTotalSwappablePss(
        /* [out] */ Int32* totalUss);

    CARAPI GetTotalPss(
        /* [out] */ Int32* totalPss);

    CARAPI GetTotalPrivateDirty(
        /* [out] */ Int32* totalPrivateDirty);

    CARAPI GetTotalSharedDirty(
        /* [out] */ Int32* totalSharedDirty);

    CARAPI GetOtherPss(
        /* [in] */ Int32 which,
        /* [out] */ Int32* otherPss);

    CARAPI GetOtherPrivateDirty(
        /* [in] */ Int32 which,
        /* [out] */ Int32* otherPrivateDirty);

    CARAPI GetOtherSharedDirty(
        /* [in] */ Int32 which,
        /* [out] */ Int32* otherSharedDirty);

    CARAPI GetDalvikPss(
        /* [out] */ Int32* dalvikPss);

    CARAPI SetDalvikPss(
        /* [in] */ Int32 mDalvikPss);

    CARAPI GetDalvikPrivateDirty(
        /* [out] */ Int32* dalvikPrivateDirty);

    CARAPI SetDalvikPrivateDirty(
        /* [in] */ Int32 dalvikPrivateDirty);

    CARAPI GetDalvikSharedDirty(
        /* [out] */ Int32* dalvikSharedDirty);

    CARAPI SetDalvikSharedDirty(
        /* [in] */ Int32 dalvikSharedDirty);

    CARAPI GetNativePss(
        /* [out] */ Int32* nativePss);

    CARAPI SetNativePss(
        /* [in] */ Int32 nativePss);

    CARAPI GetNativePrivateDirty(
        /* [out] */ Int32* nativePrivateDirty);

    CARAPI SetNativePrivateDirty(
        /* [in] */ Int32 nativePrivateDirty);

    CARAPI GetNativeSharedDirty(
        /* [out] */ Int32* nativeSharedDirty);

    CARAPI SetNativeSharedDirty(
        /* [in] */ Int32 nativeSharedDirty);

    CARAPI GetOtherPss(
        /* [out] */ Int32* otherPss);

    CARAPI SetOtherPss(
        /* [in] */ Int32 otherPss);

    CARAPI GetOtherPrivateDirty(
        /* [out] */ Int32* otherPrivateDirty);

    CARAPI SetOtherPrivateDirty(
        /* [in] */ Int32 otherPrivateDirty);

    CARAPI GetOtherSharedDirty(
        /* [out] */ Int32* otherSharedDirty);

    CARAPI SetOtherSharedDirty(
        /* [in] */ Int32 otherSharedDirty);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    friend class Debug;

    CARAPI SetOtherStats(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI GetOtherStats(
        /* [out] */ ArrayOf<Int32>** stats);

public:
    static CARAPI GetOtherLabel(
        /* [in] */ Int32 which,
        /* [out] */ String* label);

public:
    /** The proportional set size for dalvik. */
    Int32 mDalvikPss;

    /** The private dirty pages used by dalvik. */
    Int32 mDalvikPrivateDirty;

    /** The shared dirty pages used by dalvik. */
    Int32 mDalvikSharedDirty;

    /** The proportional set size for the native heap. */
    Int32 mNativePss;

    /** The private dirty pages used by the native heap. */
    Int32 mNativePrivateDirty;

    /** The shared dirty pages used by the native heap. */
    Int32 mNativeSharedDirty;

    /** The proportional set size for everything else. */
    Int32 mOtherPss;

    /** The private dirty pages used by everything else. */
    Int32 mOtherPrivateDirty;

    /** The shared dirty pages used by everything else. */
    Int32 mOtherSharedDirty;

private:
    AutoPtr<ArrayOf<Int32> > mOtherStats;/* = new int[NUM_OTHER_STATS*3]*/
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CDEBUGMEMORYINFO_H__
