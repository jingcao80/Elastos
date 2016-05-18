
#ifndef __ELASTOS_DROID_GRAPHICS_CBITMAPREGIONDECODERHELPER_H__
#define __ELASTOS_DROID_GRAPHICS_CBITMAPREGIONDECODERHELPER_H__

#include "_Elastos_Droid_Graphics_CBitmapRegionDecoderHelper.h"
#include <Elastos.CoreLibrary.h>
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CBitmapRegionDecoderHelper)
    , public Singleton
    , public IBitmapRegionDecoderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI NewInstance(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ Boolean isShareable,
        /* [out] */ IBitmapRegionDecoder** decoder);

    CARAPI NewInstance(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Boolean isShareable,
        /* [out] */ IBitmapRegionDecoder** decoder);

    CARAPI NewInstance(
        /* [in] */ IInputStream* is,
        /* [in] */ Boolean isShareable,
        /* [out] */ IBitmapRegionDecoder** decoder);

    CARAPI NewInstance(
        /* [in] */ const String& pathName,
        /* [in] */ Boolean isShareable,
        /* [out] */ IBitmapRegionDecoder** decoder);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CBITMAPREGIONDECODERHELPER_H__
