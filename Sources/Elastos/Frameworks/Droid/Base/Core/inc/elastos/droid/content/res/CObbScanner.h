
#ifndef __ELASTOS_DROID_CONTENT_RES_COBBSCANNER_H__
#define __ELASTOS_DROID_CONTENT_RES_COBBSCANNER_H__

#include "_Elastos_Droid_Content_Res_CObbScanner.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CarClass(CObbScanner)
    , public Singleton
    , public IObbScanner
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetObbInfo(
        /* [in] */ const String& filePath,
        /* [out] */ IObbInfo** obbInfo);

private:
    CARAPI NativeGetObbInfo(
        /* [in] */ const String& filePath,
        /* [in, out] */ IObbInfo* obbInfo);
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_COBBSCANNER_H__
