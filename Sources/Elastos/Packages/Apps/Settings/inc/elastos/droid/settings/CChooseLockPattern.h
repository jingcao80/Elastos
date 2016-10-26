
#ifndef __ELASTOS_DROID_SETTINGS_CCHOOSELOCKPATTERN_H__
#define __ELASTOS_DROID_SETTINGS_CCHOOSELOCKPATTERN_H__

#include "_Elastos_Droid_Settings_CChooseLockPattern.h"
#include "elastos/droid/settings/SettingsActivity.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/settings/ChooseLockSettingsHelper.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::App::Fragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Widget::ILockPatternView;
using Elastos::Droid::Internal::Widget::IOnPatternListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * If the user has a lock pattern set already, makes them confirm the existing one.
 *
 * Then, prompts the user to choose a lock pattern:
 * - prompts for initial pattern
 * - asks for confirmation / restart
 * - saves chosen password when confirmed
 */
CarClass(CChooseLockPattern)
    , public SettingsActivity
    , public IChooseLockPattern
{
public:
    class ChooseLockPatternFragment
        : public Fragment
    {
    public:
        /**
         * The states of the left footer button.
         */
        class LeftButtonMode
            : public Object
        {
        public:
            TO_STRING_IMPL("CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode")

            /**
             * @param text The displayed text for this mode.
             * @param enabled Whether the button should be enabled.
             */
            LeftButtonMode(
                /* [in] */ Int32 text,
                /* [in] */ Boolean enabled);

        public:
            Int32 mText;
            Boolean mEnabled;
        };

        /**
         * The states of the right button.
         */
        class RightButtonMode
            : public Object
        {
        public:
            TO_STRING_IMPL("CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode")

            /**
             * @param text The displayed text for this mode.
             * @param enabled Whether the button should be enabled.
             */
            RightButtonMode(
                /* [in] */ Int32 text,
                /* [in] */ Boolean enabled);

        public:
            Int32 mText;
            Boolean mEnabled;
        };

    protected:
        /**
         * Keep track internally of where the user is in choosing a pattern.
         */
        class Stage
            : public Object
            , public IPatternStage
        {
        public:
            TO_STRING_IMPL("CChooseLockPattern::ChooseLockPatternFragment::Stage")

            CAR_INTERFACE_DECL()

            /**
             * @param headerMessage The message displayed at the top.
             * @param leftMode The mode of the left button.
             * @param rightMode The mode of the right button.
             * @param footerMessage The footer message.
             * @param patternEnabled Whether the pattern widget is enabled.
             */
            Stage(
                /* [in] */ Int32 headerMessage,
                /* [in] */ LeftButtonMode* leftMode,
                /* [in] */ RightButtonMode* rightMode,
                /* [in] */ Int32 footerMessage,
                /* [in] */ Boolean patternEnabled);

            static CARAPI_(AutoPtr< ArrayOf<IPatternStage*> >) GetValues();

            static CARAPI_(Int32) GetOrdinal(
                /* [in] */ IPatternStage* stage);

        public:
            Int32 mHeaderMessage;
            AutoPtr<LeftButtonMode> mLeftMode;
            AutoPtr<RightButtonMode> mRightMode;
            Int32 mFooterMessage;
            Boolean mPatternEnabled;
        };

        class LockPatternViewOnPatternListener
            : public Object
            , public IOnPatternListener
        {
        public:
            TO_STRING_IMPL("CChooseLockPattern::ChooseLockPatternFragment::LockPatternViewOnPatternListener")

            CAR_INTERFACE_DECL()

            LockPatternViewOnPatternListener(
                /* [in] */ ChooseLockPatternFragment* host);

            CARAPI OnPatternStart();

            CARAPI OnPatternCleared();

            CARAPI OnPatternCellAdded(
                /* [in] */ IList* pattern);//List<Cell>

            CARAPI OnPatternDetected(
                /* [in] */ IList* pattern); //List<LockPatternView.Cell>

        private:
            CARAPI_(void) PatternInProgress();

        private:
            ChooseLockPatternFragment* mHost;
        };

    private:
        class InnerListener
            : public Object
            , public IViewOnClickListener
        {
        public:
            CAR_INTERFACE_DECL()

            TO_STRING_IMPL("CChooseLockPattern::ChooseLockPatternFragment::InnerListener")

            InnerListener(
                /* [in] */ ChooseLockPatternFragment* host);

            //@Override
            CARAPI OnClick(
                /* [in] */ IView* v);

        private:
            ChooseLockPatternFragment* mHost;
        };

        class ClearPatternRunnable
            : public Runnable
        {
        public:
            TO_STRING_IMPL("CChooseLockPattern::ChooseLockPatternFragment::ClearPatternRunnable")

            ClearPatternRunnable(
                /* [in] */ ChooseLockPatternFragment* host);

            CARAPI Run();

        private:
            ChooseLockPatternFragment* mHost;
        };

    public:
        TO_STRING_IMPL("CChooseLockPattern::ChooseLockPatternFragment")

        ChooseLockPatternFragment();

        ~ChooseLockPatternFragment();

        CARAPI constructor();

        //@Override
        CARAPI OnActivityResult(
            /* [in] */ Int32 requestCode,
            /* [in] */ Int32 resultCode,
            /* [in] */ IIntent* data);

        //@Override
        CARAPI OnCreate(
            /* [in] */ IBundle* savedInstanceState);

        //@Override
        CARAPI OnCreateView(
            /* [in] */ ILayoutInflater* inflater,
            /* [in] */ IViewGroup* container,
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IView** result);

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnKeyDown(
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        CARAPI OnSaveInstanceState(
            /* [in] */ IBundle* outState);

    protected:
        /**
         * Updates the messages and buttons appropriate to what stage the user
         * is at in choosing a view.  This doesn't handle clearing out the pattern;
         * the pattern is expected to be in the right state.
         * @param stage
         */
        CARAPI_(void) UpdateStage(
            /* [in] */ IPatternStage* stage);

        static CARAPI_(Boolean) InitStatic();

    private:
        // clear the wrong pattern unless they have started a new one
        // already
        CARAPI_(void) PostClearPatternRunnable();

        CARAPI_(void) SaveChosenPatternAndFinish();

    public:
        static AutoPtr<LeftButtonMode> LeftButtonMode_Cancel;
        static AutoPtr<LeftButtonMode> LeftButtonMode_CancelDisabled;
        static AutoPtr<LeftButtonMode> LeftButtonMode_Retry;
        static AutoPtr<LeftButtonMode> LeftButtonMode_RetryDisabled;
        static AutoPtr<LeftButtonMode> LeftButtonMode_Gone;

        static AutoPtr<RightButtonMode> RightButtonMode_Continue;
        static AutoPtr<RightButtonMode> RightButtonMode_ContinueDisabled;
        static AutoPtr<RightButtonMode> RightButtonMode_Confirm;
        static AutoPtr<RightButtonMode> RightButtonMode_ConfirmDisabled;
        static AutoPtr<RightButtonMode> RightButtonMode_Ok;

        static AutoPtr<IPatternStage> Stage_Introduction;
        static AutoPtr<IPatternStage> Stage_HelpScreen;
        static AutoPtr<IPatternStage> Stage_ChoiceTooShort;
        static AutoPtr<IPatternStage> Stage_FirstChoiceValid;
        static AutoPtr<IPatternStage> Stage_NeedToConfirm;
        static AutoPtr<IPatternStage> Stage_ConfirmWrong;
        static AutoPtr<IPatternStage> Stage_ChoiceConfirmed;

        static const Int32 CONFIRM_EXISTING_REQUEST;// = 55;

        // how Int64 after a confirmation message is shown before moving on
        static const Int32 INFORMATION_MSG_TIMEOUT_MS;// = 3000;

    protected:
        AutoPtr<ITextView> mHeaderText;
        AutoPtr<ILockPatternView> mLockPatternView;
        AutoPtr<ITextView> mFooterText;

        AutoPtr<IList> mChosenPattern;// List<LockPatternView.Cell>

        /**
         * The pattern listener that responds according to a user choosing a new
         * lock pattern.
         */
        AutoPtr<LockPatternViewOnPatternListener> mChooseNewLockPatternListener;

    private:
        // how Int64 we wait to clear a wrong pattern
        static const Int32 WRONG_PATTERN_CLEAR_TIMEOUT_MS;// = 2000;

        static const Int32 ID_EMPTY_MESSAGE;// = -1;

        static const String KEY_UI_STAGE;// = "uiStage";
        static const String KEY_PATTERN_CHOICE;// = "chosenPattern";

        AutoPtr<ITextView> mFooterLeftButton;
        AutoPtr<ITextView> mFooterRightButton;

        Byte mPatternSize;// = ILockPatternUtils::PATTERN_SIZE_DEFAULT;

        /**
         * The patten used during the help screen to show how to draw a pattern.
         */
        AutoPtr<IList> mAnimatePattern;// List<LockPatternView.Cell>


        AutoPtr<IPatternStage> mUiStage;// = Stage_Introduction;
        Boolean mDone;// = FALSE;

        AutoPtr<ClearPatternRunnable> mClearPatternRunnable;

        AutoPtr<ChooseLockSettingsHelper> mChooseLockSettingsHelper;
        Boolean mInitstatic;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CChooseLockPattern();

    ~CChooseLockPattern();

    CARAPI constructor();

    //@Override
    CARAPI GetIntent(
        /* [out] */ IIntent** intent);

    static CARAPI_(AutoPtr<IIntent>) CreateIntent(
        /* [in] */ IContext* context,
        /* [in] */ Boolean isFallback,
        /* [in] */ Boolean requirePassword,
        /* [in] */ Boolean confirmCredentials);

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

protected:
    //@Override
    virtual CARAPI_(Boolean) IsValidFragment(
        /* [in] */ const String& fragmentName);

public:
    /**
     * Used by the choose lock pattern wizard to indicate the wizard is
     * finished, and each activity in the wizard should finish.
     * <p>
     * Previously, each activity in the wizard would finish itself after
     * starting the next activity. However, this leads to broken 'Back'
     * behavior. So, now an activity does not finish itself until it gets this
     * result.
     */
    static const Int32 RESULT_FINISHED;// = RESULT_FIRST_USER;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCHOOSELOCKPATTERN_H__