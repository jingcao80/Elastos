#ifndef __ELASTOS_DROID_SETTINGS_CHOOSELOCKSETTINGSHELPER_H__
#define __ELASTOS_DROID_SETTINGS_CHOOSELOCKSETTINGSHELPER_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Settings {

class ChooseLockSettingsHelper
    : public Object
{
public:
    ChooseLockSettingsHelper();

    CARAPI constructor(
        /* [in] */ IActivity* activity);

    CARAPI constructor(
        /* [in] */ IActivity* activity,
        /* [in] */ IFragment* fragment);

    virtual CARAPI_(AutoPtr<ILockPatternUtils>) Utils();

    /**
     * If a pattern, password or PIN exists, prompt the user before allowing them to change it.
     * @param message optional message to display about the action about to be done
     * @param details optional detail message to display
     * @return TRUE if one exists and we launched an activity to confirm it
     * @see #OnActivityResult(Int32, Int32, android.content.Intent)
     */
    virtual CARAPI_(Boolean) LaunchConfirmationActivity(
        /* [in] */ Int32 request,
        /* [in] */ ICharSequence* message,
        /* [in] */ ICharSequence* details);

    /**
     * If a pattern, password or PIN exists, prompt the user before allowing them to change it.
     * @param message optional message to display about the action about to be done
     * @param details optional detail message to display
     * @param returnCredentials if TRUE, put credentials into intent. Note that if this is TRUE,
                                this can only be called internally.
     * @return TRUE if one exists and we launched an activity to confirm it
     * @see #OnActivityResult(Int32, Int32, android.content.Intent)
     */
    virtual CARAPI_(Boolean) LaunchConfirmationActivity(
        /* [in] */ Int32 request,
        /* [in] */ ICharSequence* message,
        /* [in] */ ICharSequence* details,
        /* [in] */ Boolean returnCredentials);

private:
    /**
     * Launch screen to confirm the existing lock pattern.
     * @param message shown in header of ConfirmLockPattern if not NULL
     * @param details shown in footer of ConfirmLockPattern if not NULL
     * @param returnCredentials if TRUE, put credentials into intent.
     * @see #OnActivityResult(Int32, Int32, android.content.Intent)
     * @return TRUE if we launched an activity to confirm pattern
     */
    CARAPI_(Boolean) ConfirmPattern(
        /* [in] */ Int32 request,
        /* [in] */ ICharSequence* message,
        /* [in] */ ICharSequence* details,
        /* [in] */ Boolean returnCredentials);

    /**
     * Launch screen to confirm the existing lock password.
     * @param message shown in header of ConfirmLockPassword if not NULL
     * @param returnCredentials if TRUE, put credentials into intent.
     * @see #OnActivityResult(Int32, Int32, android.content.Intent)
     * @return TRUE if we launched an activity to confirm password
     */
    CARAPI_(Boolean) ConfirmPassword(
        /* [in] */ Int32 request,
        /* [in] */ ICharSequence* message,
        /* [in] */ Boolean returnCredentials);

public:
    static const String EXTRA_KEY_TYPE;
    static const String EXTRA_KEY_PASSWORD;

private:
    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    AutoPtr<IActivity> mActivity;
    AutoPtr<IFragment> mFragment;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CHOOSELOCKSETTINGSHELPER_H__