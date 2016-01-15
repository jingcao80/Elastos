#ifndef __ELASTOS_DROID_SYSTEMUI_SYSTEMUI_H__
#define __ELASTOS_DROID_SYSTEMUI_SYSTEMUI_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;

namespace Elastos {
namespace Droid {
namespace SystemUI {

class SystemUI
{
public:
    SystemUI();

    virtual ~SystemUI();

    virtual CARAPI Start() = 0;

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI SetContext(
        /* [in] */ IContext* context);

    AutoPtr<IContext> GetContext();

public:
    AutoPtr<IContext> mContext;
};

}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SYSTEMUI_H__
