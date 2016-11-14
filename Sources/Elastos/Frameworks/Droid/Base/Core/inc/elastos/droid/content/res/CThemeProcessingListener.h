
#ifndef __ELASTOS_DROID_CONTENT_RES_CTHEMEPROCESSINGLISTENER_H__
#define __ELASTOS_DROID_CONTENT_RES_CTHEMEPROCESSINGLISTENER_H__

#include "_Elastos_Droid_Content_Res_CThemeProcessingListener.h"
#include <elastos/core/Runnable.h>

using Elastos::Core::Runnable;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

class CThemeManager;

CarClass(CThemeProcessingListener)
    , public Object
    , public IIThemeProcessingListener
{
private:
    class OnFinishedProcessingRunnable : public Runnable
    {
    public:
        OnFinishedProcessingRunnable(
            /* [in] */ const String& pkgName,
            /* [in] */ CThemeManager* host)
            : mPkgName(pkgName)
            , mHost(host)
        {}

        CARAPI Run();

    private:
        String mPkgName;
        CThemeManager* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IThemeManager* host);

    CARAPI OnFinishedProcessing(
        /* [in] */ const String& pkgName);

private:
    CThemeManager* mHost;
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_CTHEMEPROCESSINGLISTENER_H__
