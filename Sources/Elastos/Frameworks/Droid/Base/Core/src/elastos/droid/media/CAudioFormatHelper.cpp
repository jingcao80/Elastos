#include "elastos/droid/media/CAudioFormatHelper.h"
#include "elastos/droid/media/CAudioFormat.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioFormatHelper, Singleton, IAudioFormatHelper)

CAR_SINGLETON_IMPL(CAudioFormatHelper)

ECode CAudioFormatHelper::ChannelCountFromInChannelMask(
    /* [in] */ Int32 mask,
    /* [out] */ Int32* result)
{
    return CAudioFormat::ChannelCountFromInChannelMask(mask, result);
}

ECode CAudioFormatHelper::ChannelCountFromOutChannelMask(
    /* [in] */ Int32 mask,
    /* [out] */ Int32* result)
{
    return CAudioFormat::ChannelCountFromOutChannelMask(mask, result);
}

ECode CAudioFormatHelper::ConvertChannelOutMaskToNativeMask(
    /* [in] */ Int32 javaMask,
    /* [out] */ Int32* result)
{
    return CAudioFormat::ConvertChannelOutMaskToNativeMask(javaMask, result);
}

ECode CAudioFormatHelper::ConvertNativeChannelMaskToOutMask(
    /* [in] */ Int32 nativeMask,
    /* [out] */ Int32* result)
{
    return CAudioFormat::ConvertNativeChannelMaskToOutMask(nativeMask, result);
}

ECode CAudioFormatHelper::GetBytesPerSample(
    /* [in] */ Int32 audioFormat,
    /* [out] */ Int32* result)
{
    return CAudioFormat::GetBytesPerSample(audioFormat, result);
}

ECode CAudioFormatHelper::IsValidEncoding(
    /* [in] */ Int32 audioFormat,
    /* [out] */ Boolean* result)
{
    return CAudioFormat::IsValidEncoding(audioFormat, result);
}

ECode CAudioFormatHelper::IsEncodingLinearPcm(
    /* [in] */ Int32 audioFormat,
    /* [out] */ Boolean* result)
{
    return CAudioFormat::IsEncodingLinearPcm(audioFormat, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
