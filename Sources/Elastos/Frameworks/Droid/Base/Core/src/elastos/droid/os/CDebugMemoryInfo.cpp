
#include "elastos/droid/os/CDebugMemoryInfo.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CDebugMemoryInfo, Object, IDebugMemoryInfo)

CAR_OBJECT_IMPL(CDebugMemoryInfo)

CDebugMemoryInfo::CDebugMemoryInfo()
{
    mOtherStats = ArrayOf<Int32>::Alloc(IDebugMemoryInfo::NUM_OTHER_STATS * 3);
}

CDebugMemoryInfo::~CDebugMemoryInfo()
{}

ECode CDebugMemoryInfo::constructor()
{
    return NOERROR;
}

ECode CDebugMemoryInfo::GetTotalPss(
    /* [out] */ Int32* totalPss)
{
    VALIDATE_NOT_NULL(totalPss);
    *totalPss = mDalvikPss + mNativePss + mOtherPss;
    return NOERROR;
}

ECode CDebugMemoryInfo::GetTotalPrivateDirty(
    /* [out] */ Int32* totalPrivateDirty)
{
    VALIDATE_NOT_NULL(totalPrivateDirty);
    *totalPrivateDirty = mDalvikPrivateDirty + mNativePrivateDirty + mOtherPrivateDirty;
    return NOERROR;
}

ECode CDebugMemoryInfo::GetTotalSharedDirty(
    /* [out] */ Int32* totalSharedDirty)
{
    VALIDATE_NOT_NULL(totalSharedDirty);
    *totalSharedDirty = mDalvikSharedDirty + mNativeSharedDirty + mOtherSharedDirty;
    return NOERROR;
}

ECode CDebugMemoryInfo::GetOtherPss(
    /* [in] */ Int32 which,
    /* [out] */ Int32* otherPss)
{
    VALIDATE_NOT_NULL(otherPss);
    *otherPss = (*mOtherStats)[which * 3];
    return NOERROR;
}

ECode CDebugMemoryInfo::GetOtherPrivateDirty(
    /* [in] */ Int32 which,
    /* [out] */ Int32* otherPrivateDirty)
{
    VALIDATE_NOT_NULL(otherPrivateDirty);
    *otherPrivateDirty = (*mOtherStats)[which * 3 + 1];
    return NOERROR;
}

ECode CDebugMemoryInfo::GetOtherSharedDirty(
    /* [in] */ Int32 which,
    /* [out] */ Int32* otherSharedDirty)
{
    VALIDATE_NOT_NULL(otherSharedDirty);
    *otherSharedDirty = (*mOtherStats)[which * 3 + 2];
    return NOERROR;
}

ECode CDebugMemoryInfo::GetOtherLabel(
    /* [in] */ Int32 which,
    /* [out] */ String* label)
{
    VALIDATE_NOT_NULL(label);

    switch (which) {
        case 0: {
            *label = String("Cursor");
            break;
        }
        case 1:  {
            *label = String("Ashmem");
            break;
        }
        case 2:  {
            *label = String("Other dev");
            break;
        }
        case 3:  {
            *label = String(".so mmap");
            break;
        }
        case 4:  {
            *label = String(".jar mmap");
            break;
        }
        case 5:  {
            *label = String(".apk mmap");
            break;
        }
        case 6:  {
            *label = String(".ttf mmap");
            break;
        }
        case 7:  {
            *label = String(".dex mmap");
            break;
        }
        case 8:  {
            *label = String("Other mmap");
            break;
        }
        default:  {
            *label = String("????");
            break;
        }
    }

    return NOERROR;
}

ECode CDebugMemoryInfo::GetDalvikPss(
    /* [out] */ Int32* dalvikPss)
{
    VALIDATE_NOT_NULL(dalvikPss);
    *dalvikPss = mDalvikPss;
    return NOERROR;
}

ECode CDebugMemoryInfo::SetDalvikPss(
    /* [in] */ Int32 dalvikPss)
{
    mDalvikPss = dalvikPss;
    return NOERROR;
}

ECode CDebugMemoryInfo::GetDalvikPrivateDirty(
    /* [out] */ Int32* dalvikPrivateDirty)
{
    VALIDATE_NOT_NULL(dalvikPrivateDirty);
    *dalvikPrivateDirty = mDalvikPrivateDirty;
    return NOERROR;
}

ECode CDebugMemoryInfo::SetDalvikPrivateDirty(
    /* [in] */ Int32 dalvikPrivateDirty)
{
    mDalvikPrivateDirty = dalvikPrivateDirty;
    return NOERROR;
}

ECode CDebugMemoryInfo::GetDalvikSharedDirty(
    /* [out] */ Int32* dalvikSharedDirty)
{
    VALIDATE_NOT_NULL(dalvikSharedDirty);
    *dalvikSharedDirty = mDalvikSharedDirty;
    return NOERROR;
}

ECode CDebugMemoryInfo::SetDalvikSharedDirty(
    /* [in] */ Int32 dalvikSharedDirty)
{
    mDalvikSharedDirty = dalvikSharedDirty;
    return NOERROR;
}

ECode CDebugMemoryInfo::GetNativePss(
    /* [out] */ Int32* nativePss)
{
    VALIDATE_NOT_NULL(nativePss);
    *nativePss = mNativePss;
    return NOERROR;
}

ECode CDebugMemoryInfo::SetNativePss(
    /* [in] */ Int32 nativePss)
{
    mNativePss = nativePss;
    return NOERROR;
}

ECode CDebugMemoryInfo::GetNativePrivateDirty(
    /* [out] */ Int32* nativePrivateDirty)
{
    VALIDATE_NOT_NULL(nativePrivateDirty);
    *nativePrivateDirty = mNativePrivateDirty;
    return NOERROR;
}

ECode CDebugMemoryInfo::SetNativePrivateDirty(
    /* [in] */ Int32 nativePrivateDirty)
{
    mNativePrivateDirty = nativePrivateDirty;
    return NOERROR;
}

ECode CDebugMemoryInfo::GetNativeSharedDirty(
    /* [out] */ Int32* nativeSharedDirty)
{
    VALIDATE_NOT_NULL(nativeSharedDirty);
    *nativeSharedDirty = mNativeSharedDirty;
    return NOERROR;
}

ECode CDebugMemoryInfo::SetNativeSharedDirty(
    /* [in] */ Int32 nativeSharedDirty)
{
    mNativeSharedDirty = nativeSharedDirty;
    return NOERROR;
}

ECode CDebugMemoryInfo::GetOtherPss(
    /* [out] */ Int32* otherPss)
{
    VALIDATE_NOT_NULL(otherPss);
    *otherPss = mOtherPss;
    return NOERROR;
}

ECode CDebugMemoryInfo::SetOtherPss(
    /* [in] */ Int32 otherPss)
{
    mOtherPss = otherPss;
    return NOERROR;
}

ECode CDebugMemoryInfo::GetOtherPrivateDirty(
    /* [out] */ Int32* otherPrivateDirty)
{
    VALIDATE_NOT_NULL(otherPrivateDirty);
    *otherPrivateDirty = mOtherPrivateDirty;
    return NOERROR;
}

ECode CDebugMemoryInfo::SetOtherPrivateDirty(
    /* [in] */ Int32 otherPrivateDirty)
{
    mOtherPrivateDirty = otherPrivateDirty;
    return NOERROR;
}

ECode CDebugMemoryInfo::GetOtherSharedDirty(
    /* [out] */ Int32* otherSharedDirty)
{
    VALIDATE_NOT_NULL(otherSharedDirty);
    *otherSharedDirty = mOtherSharedDirty;
    return NOERROR;
}

ECode CDebugMemoryInfo::SetOtherSharedDirty(
    /* [in] */ Int32 otherSharedDirty)
{
    mOtherSharedDirty = otherSharedDirty;
    return NOERROR;
}

ECode CDebugMemoryInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    source->ReadInt32(&mDalvikPss);
    source->ReadInt32(&mDalvikPrivateDirty);
    source->ReadInt32(&mDalvikSharedDirty);
    source->ReadInt32(&mNativePss);
    source->ReadInt32(&mNativePrivateDirty);
    source->ReadInt32(&mNativeSharedDirty);
    source->ReadInt32(&mOtherPss);
    source->ReadInt32(&mOtherPrivateDirty);
    source->ReadInt32(&mOtherSharedDirty);
    source->ReadArrayOf((Handle32*)(&mOtherStats));

    return NOERROR;
}

ECode CDebugMemoryInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    dest->WriteInt32(mDalvikPss);
    dest->WriteInt32(mDalvikPrivateDirty);
    dest->WriteInt32(mDalvikSharedDirty);
    dest->WriteInt32(mNativePss);
    dest->WriteInt32(mNativePrivateDirty);
    dest->WriteInt32(mNativeSharedDirty);
    dest->WriteInt32(mOtherPss);
    dest->WriteInt32(mOtherPrivateDirty);
    dest->WriteInt32(mOtherSharedDirty);
    dest->WriteArrayOf((Handle32)mOtherStats.Get());

    return NOERROR;
}

ECode CDebugMemoryInfo::SetOtherStats(
    /* [in] */ Int32 index,
    /* [in] */ Int32 value)
{
    assert(index >= 0 && index < mOtherStats->GetLength());

    (*mOtherStats)[index] = value;
    return NOERROR;
}

ECode CDebugMemoryInfo::GetOtherStats(
    /* [out, callee] */ ArrayOf<Int32>** stats)
{
    VALIDATE_NOT_NULL(stats);

    *stats = mOtherStats;
    REFCOUNT_ADD(*stats);
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
