
#ifndef __ELASTOS_DROID_SETTINGS_CONFIRMLOCKPATTERN_H__
#define __ELASTOS_DROID_SETTINGS_CONFIRMLOCKPATTERN_H__

#include "elastos/droid/settings/SettingsActivity.h"
#include "elastos/droid/settings/ChooseLockSettingsHelper.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/os/CountDownTimer.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::App::Fragment;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::ILockPatternView;
using Elastos::Droid::Internal::Widget::IOnPatternListener;
using Elastos::Droid::Os::CountDownTimer;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Launch this when you want the user to confirm their lock pattern.
 *
 * Sets an activity result of {@link Activity#RESULT_OK} when the user
 * successfully confirmed their pattern.
 */
class ConfirmLockPattern
    : public SettingsActivity
{
public:
    class ConfirmLockPatternFragment
        : public Fragment
    {
    private:
        class ClearPatternRunnable
            : public Runnable
        {
        public:
            TO_STRING_IMPL("ConfirmLockPattern::ConfirmLockPatternFragment::ClearPatternRunnable")

            ClearPatternRunnable(
                /* [in] */ ConfirmLockPatternFragment* host);

            CARAPI Run();

        private:
            ConfirmLockPatternFragment* mHost;
        };

        class LockPatternViewOnPatternListener
            : public Object
            , public IOnPatternListener
        {
        public:
            TO_STRING_IMPL("ConfirmLockPattern::ConfirmLockPatternFragment::LockPatternViewOnPatternListener")

            CAR_INTERFACE_DECL()

            LockPatternViewOnPatternListener(
                /* [in] */ ConfirmLockPatternFragment* host);

            CARAPI OnPatternStart();

            CARAPI OnPatternCleared();

            CARAPI OnPatternCellAdded(
                /* [in] */ IList* pattern);//List<Cell>

            CARAPI OnPatternDetected(
                /* [in] */ IList* pattern); //List<LockPatternView.Cell>

        private:
            ConfirmLockPatternFragment* mHost;
        };

        class HandleAttemptLockoutCountDownTimer
            : public CountDownTimer
        {
        public:
            TO_STRING_IMPL("ConfirmLockPattern::ConfirmLockPatternFragment::HandleAttemptLockoutCountDownTimer")

            HandleAttemptLockoutCountDownTimer(
                /* [in] */ ConfirmLockPatternFragment* host);

            CARAPI constructor(
                /* [in] */ Int64 millisInFuture,
                /* [in] */ Int64 countDownInterval);

            //@Override
            CARAPI OnTick(
                /* [in] */ Int64 millisUntilFinished);

            //@Override
            CARAPI OnFinish();

        private:
            ConfirmLockPatternFragment* mHost;
        };

    public:
        TO_STRING_IMPL("ConfirmLockPattern::ConfirmLockPatternFragment")

        // required constructor for fragments
        ConfirmLockPatternFragment();

        virtual ~ConfirmLockPatternFragment();

        CARAPI constructor();

        //@Override
        CARAPI OnCreate(
            /* [in] */ IBundle* savedInstanceState);

        //@Override
        CARAPI OnCreateView(
            /* [in] */ ILayoutInflater* inflater,
            /* [in] */ IViewGroup* container,
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IView** result);

        //@Override
        CARAPI OnSaveInstanceState(
            /* [in] */ IBundle* outState);

        //@Override
        CARAPI OnPause();

        //@Override
        CARAPI OnResume();

    private:
        CARAPI_(void) UpdateStage(
            /* [in] */ Stage stage);

        // clear the wrong pattern unless they have started a new one
        // already
        CARAPI_(void) PostClearPatternRunnable();

        CARAPI_(void) HandleAttemptLockout(
            /* [in] */ Int64 elapsedRealtimeDeadline);

    private:
        // how Int64 we wait to clear a wrong pattern
        static const Int32 WRONG_PATTERN_CLEAR_TIMEOUT_MS;// = 2000;

        static const String KEY_NUM_WRONG_ATTEMPTS;// = "num_wrong_attempts";

        AutoPtr<ILockPatternView> mLockPatternView;
        AutoPtr<ILockPatternUtils> mLockPatternUtils;
        Int32 mNumWrongConfirmAttempts;
        AutoPtr<HandleAttemptLockoutCountDownTimer> mCountdownTimer;

        AutoPtr<ITextView> mHeaderTextView;
        AutoPtr<ITextView> mFooterTextView;

        // caller-supplied text for various prompts
        AutoPtr<ICharSequence> mHeaderText;
        AutoPtr<ICharSequence> mFooterText;
        AutoPtr<ICharSequence> mHeaderWrongText;
        AutoPtr<ICharSequence> mFooterWrongText;

        AutoPtr<ClearPatternRunnable> mClearPatternRunnable;

        /**
         * The pattern listener that responds according to a user confirming
         * an existing lock pattern.
         */
        AutoPtr<LockPatternViewOnPatternListener> mConfirmExistingLockPatternListener;
    };

public:
    ConfirmLockPattern();

    ~ConfirmLockPattern();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI GetIntent(
        /* [out] */ IIntent** result);

protected:
    //@Override
    virtual CARAPI_(Boolean) IsValidFragment(
        /* [in] */ const String& fragmentName);

public:
    /**
     * Names of {@link CharSequence} fields within the originating {@link Intent}
     * that are used to configure the keyguard confirmation view's labeling.
     * The view will use the system-defined resource strings for any labels that
     * the caller does not supply.
     */
    static const String PACKAGE;// = "com.android.settings";
    static const String HEADER_TEXT;// = PACKAGE + ".ConfirmLockPattern.header";
    static const String FOOTER_TEXT;// = PACKAGE + ".ConfirmLockPattern.footer";
    static const String HEADER_WRONG_TEXT;// = PACKAGE + ".ConfirmLockPattern.header_wrong";
    static const String FOOTER_WRONG_TEXT;// = PACKAGE + ".ConfirmLockPattern.footer_wrong";
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

namespace Elastos {
namespace Droid {
namespace Settings {

class ConfirmLockPatternInternalActivity
    : public ConfirmLockPattern
    , public IConfirmLockPatternInternalActivity
{
public:
    CAR_INTERFACE_DECL()

    ConfirmLockPatternInternalActivity();

    ~ConfirmLockPatternInternalActivity();

    CARAPI constructor();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CONFIRMLOCKPATTERN_H__