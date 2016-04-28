
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_SECURESETTING_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_SECURESETTING_H__

#include "_SystemUI.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/database/ContentObserver.h>
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

/** Helper for managing a secure setting. **/
class SecureSetting
    : public ContentObserver
    , public IListenable
{
public:
    CAR_INTERFACE_DECL();

    TO_STRING_IMPL("SecureSetting")

    SecureSetting(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ const String& settingName);

    virtual CARAPI_(Int32) GetValue();

    virtual CARAPI_(void) SetValue(
        /* [in] */ Int32 value);

    // @Override
    CARAPI SetListening(
        /* [in] */ Boolean listening);

    // @Override
    CARAPI OnChange(
        /* [in] */ Boolean selfChange);

    virtual CARAPI_(void) SetUserId(
        /* [in] */ Int32 userId);

protected:
    virtual CARAPI_(void) HandleValueChanged(
        /* [in] */ Int32 value) = 0;

private:
    AutoPtr<IContext> mContext;
    String mSettingName;

    Boolean mListening;
    Int32 mUserId;
};

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_SECURESETTING_H__
