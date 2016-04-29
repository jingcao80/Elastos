
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_GLOBALSETTING_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_GLOBALSETTING_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/database/ContentObserver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::SystemUI::StatusBar::Policy::IListenable;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

/** Helper for managing a global setting. **/
class GlobalSetting
    : public ContentObserver
    , public IGlobalSetting
    , public IListenable
{
public:
    CAR_INTERFACE_DECL();

    TO_STRING_IMPL("GlobalSetting")

    GlobalSetting(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ const String& settingName);

    CARAPI GetValue(
        /* [out] */ Int32* result);

    CARAPI SetValue(
        /* [in] */ Int32 value);

    // @Override
    CARAPI SetListening(
        /* [in] */ Boolean listening);

    // @Override
    CARAPI OnChange(
        /* [in] */ Boolean selfChange);

protected:
    virtual CARAPI_(void) HandleValueChanged(
        /* [in] */ Int32 value) = 0;

private:
    AutoPtr<IContext> mContext;
    String mSettingName;
};

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_GLOBALSETTING_H__
