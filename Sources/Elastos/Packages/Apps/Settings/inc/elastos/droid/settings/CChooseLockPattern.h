//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

        private:
            /**
             * @param text The displayed text for this mode.
             * @param enabled Whether the button should be enabled.
             */
            LeftButtonMode(
                /* [in] */ const String& name,
                /* [in] */ Int32 ordinal,
                /* [in] */ Int32 text,
                /* [in] */ Boolean enabled);

        public:
            static AutoPtr<LeftButtonMode> CANCEL;
            static AutoPtr<LeftButtonMode> CANCEL_DISABLED;
            static AutoPtr<LeftButtonMode> RETRY;
            static AutoPtr<LeftButtonMode> RETRY_DISABLED;
            static AutoPtr<LeftButtonMode> GONE;

            Int32 mText;
            Boolean mEnabled;

        private:
            String mName;
            Int32 mOrdinal;
        };

        /**
         * The states of the right button.
         */
        class RightButtonMode
            : public Object
        {
        public:
            TO_STRING_IMPL("CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode")

        private:
            /**
             * @param text The displayed text for this mode.
             * @param enabled Whether the button should be enabled.
             */
            RightButtonMode(
                /* [in] */ const String& name,
                /* [in] */ Int32 ordinal,
                /* [in] */ Int32 text,
                /* [in] */ Boolean enabled);

        public:
            static AutoPtr<RightButtonMode> CONTINUE;
            static AutoPtr<RightButtonMode> CONTINUE_DISABLED;
            static AutoPtr<RightButtonMode> CONFIRM;
            static AutoPtr<RightButtonMode> CONFIRM_DISABLED;
            static AutoPtr<RightButtonMode> OK;

            Int32 mText;
            Boolean mEnabled;

        private:
            String mName;
            Int32 mOrdinal;
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

            CARAPI_(Int32) GetOrdinal();

            static CARAPI_(AutoPtr< ArrayOf<Stage*> >) GetValues();

        private:
            /**
             * @param headerMessage The message displayed at the top.
             * @param leftMode The mode of the left button.
             * @param rightMode The mode of the right button.
             * @param footerMessage The footer message.
             * @param patternEnabled Whether the pattern widget is enabled.
             */
            Stage(
                /* [in] */ const String& name,
                /* [in] */ Int32 ordinal,
                /* [in] */ Int32 headerMessage,
                /* [in] */ LeftButtonMode* leftMode,
                /* [in] */ RightButtonMode* rightMode,
                /* [in] */ Int32 footerMessage,
                /* [in] */ Boolean patternEnabled);

        public:
            static AutoPtr<Stage> INTRODUCTION;
            static AutoPtr<Stage> HELP_SCREEN;
            static AutoPtr<Stage> CHOICE_TOO_SHORT;
            static AutoPtr<Stage> FIRST_CHOICE_VALID;
            static AutoPtr<Stage> NEED_TO_CONFIRM;
            static AutoPtr<Stage> CONFIRM_WRONG;
            static AutoPtr<Stage> CHOICE_CONFIRMED;

            Int32 mHeaderMessage;
            AutoPtr<LeftButtonMode> mLeftMode;
            AutoPtr<RightButtonMode> mRightMode;
            Int32 mFooterMessage;
            Boolean mPatternEnabled;

        private:
            String mName;
            Int32 mOrdinal;

            static AutoPtr< ArrayOf<Stage*> > sSharedConstantsCache;
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

            CARAPI OnPatternDetected(
                /* [in] */ IList* pattern); //List<LockPatternView.Cell>

            CARAPI OnPatternCellAdded(
                /* [in] */ IList* pattern);//List<Cell>

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
            /* [in] */ Stage* stage);

    private:
        // clear the wrong pattern unless they have started a new one
        // already
        CARAPI_(void) PostClearPatternRunnable();

        CARAPI_(void) SaveChosenPatternAndFinish();

    public:
        static const Int32 CONFIRM_EXISTING_REQUEST;

        // how Int64 after a confirmation message is shown before moving on
        static const Int32 INFORMATION_MSG_TIMEOUT_MS;

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
        static const Int32 WRONG_PATTERN_CLEAR_TIMEOUT_MS;

        static const Int32 ID_EMPTY_MESSAGE = -1;

        static const String KEY_UI_STAGE;
        static const String KEY_PATTERN_CHOICE;

        AutoPtr<ITextView> mFooterLeftButton;
        AutoPtr<ITextView> mFooterRightButton;

        Byte mPatternSize;

        /**
         * The patten used during the help screen to show how to draw a pattern.
         */
        AutoPtr<IList> mAnimatePattern;// List<LockPatternView.Cell>


        AutoPtr<Stage> mUiStage;
        Boolean mDone;

        AutoPtr<ClearPatternRunnable> mClearPatternRunnable;

        AutoPtr<ChooseLockSettingsHelper> mChooseLockSettingsHelper;
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
    static const Int32 RESULT_FINISHED;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Settings::CChooseLockPattern::ChooseLockPatternFragment::Stage, IInterface);

#endif //__ELASTOS_DROID_SETTINGS_CCHOOSELOCKPATTERN_H__