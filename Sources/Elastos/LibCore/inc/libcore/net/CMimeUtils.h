#ifndef __LIBCORE_NET_CMIMEUTILS_H__
#define __LIBCORE_NET_CMIMEUTILS_H__

#include "_Libcore_Net_CMimeUtils.h"
#include "Singleton.h"


namespace Libcore {
namespace Net {

CarClass(CMimeUtils)
    , public Singleton
    , public IMimeUtils
{
public :
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI HasMineType(
        /* [in] */ const String& mimeType,
        /* [out] */ Boolean *result);

    CARAPI GuessMimeTypeFromExtension(
        /* [in] */ const String& extension,
        /* [out] */ String* result);

    CARAPI HasExtension(
        /* [in] */ const String& extension,
        /* [out] */ Boolean* result);

    CARAPI GuessExtensionFromMimeType(
        /* [in] */ const String& mimeType,
        /* [out] */ String* result);
};

} // namespace Net
} // namespace Libcore

#endif // __LIBCORE_NET_CMIMEUTILS_H__
