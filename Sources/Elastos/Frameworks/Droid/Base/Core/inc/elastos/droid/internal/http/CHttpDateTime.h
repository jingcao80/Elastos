
#ifndef __ELASTOS_DROID_INTERNAL_HTTP_CHTTPDATETIME_H__
#define __ELASTOS_DROID_INTERNAL_HTTP_CHTTPDATETIME_H__

#include "_Elastos_Droid_Internal_Http_CHttpDateTime.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {

CarClass(CHttpDateTime)
    , public Singleton
    , public IHttpDateTime
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Parse(
        /* [in] */ const String& timeString,
        /* [out] */ Int64* time);
};

} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_HTTP_CHTTPDATETIME_H__
