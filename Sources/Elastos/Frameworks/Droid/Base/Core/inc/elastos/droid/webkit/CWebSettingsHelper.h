
#ifndef __ELASTOS_DROID_WEBKIT_CWEBSETTINGSHELPER_H__
#define __ELASTOS_DROID_WEBKIT_CWEBSETTINGSHELPER_H__

#include "_Elastos_Droid_Webkit_CWebSettingsHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CWebSettingsHelper)
    , public Singleton
    , public IWebSettingsHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetDefaultUserAgent(
        /* [in] */ IContext* context,
        /* [out] */ String* ua);
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CWEBSETTINGSHELPER_H__
