
#ifndef __ELASTOS_DROID_GRAPHICS_CMOVIEHELPER_H__
#define __ELASTOS_DROID_GRAPHICS_CMOVIEHELPER_H__

#include "_Elastos_Droid_Graphics_CMovieHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CMovieHelper)
    , public Singleton
    , public IMovieHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI DecodeFile(
        /* [in] */ const String& pathName,
        /* [out] */ IMovie** movie);

    CARAPI DecodeStream(
        /* [in] */ IInputStream* is,
        /* [out] */ IMovie** movie);

    CARAPI DecodeByteArray(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ IMovie** movie);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CMOVIEHELPER_H__
