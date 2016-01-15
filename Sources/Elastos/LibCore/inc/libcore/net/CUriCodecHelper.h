#ifndef __LIBCORE_NET_URICODECHELPER_H__
#define __LIBCORE_NET_URICODECHELPER_H__

#include "_Libcore_Net_CUriCodecHelper.h"
#include "Singleton.h"

using Elastos::IO::Charset::ICharset;

namespace Libcore {
namespace Net {

CarClass(CUriCodecHelper)
    , public Singleton
    , public IUriCodecHelper
{
public :
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ValidateSimple(
        /* [in] */ const String& s,
        /* [in] */ const String& legal);

    CARAPI Decode(
        /* [in] */ const String& s,
        /* [out] */ String* result);

    CARAPI Decode(
        /* [in] */ const String& s,
        /* [in] */ Boolean convertPlus,
        /* [in] */ ICharset* charset,
        /* [in] */ Boolean throwOnFailure,
        /* [out] */ String* result);
};

} // namespace Net
} // namespace Libcore

#endif // __LIBCORE_NET_URICODECHELPER_H__
