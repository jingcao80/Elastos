
#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_CPINYINIME_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_CPINYINIME_H__

#include "_Elastos_Droid_InputMethod_Pinyin_CPinyinIME.h"
#include "elastos/droid/inputmethod/pinyin/EnglishInputProcessor.h"
#include "elastos/droid/inputmethod/pinyin/Environment.h"
#include "elastos/droid/inputmethod/pinyin/InputModeSwitcher.h"
#include <elastos/droid/inputmethodservice/InputMethodService.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/os/HandlerRunnable.h>
#include <elastos/droid/view/GestureDetector.h>
#include <elastos/core/Object.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/utility/etl/Vector.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IActivityThread;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::INotification;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::InputMethodService::InputMethodService;
using Elastos::Droid::InputMethodService::IAbstractInputMethodSessionImpl;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::HandlerRunnable;
using Elastos::Droid::View::GestureDetector;
using Elastos::Droid::View::IGestureDetector;
using Elastos::Droid::View::IGestureDetectorOnGestureListener;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Droid::Widget::IPopupWindow;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Core::Object;
using Elastos::Core::StringBuffer;
using Elastos::Utility::Etl::Vector;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * Main class of the Pinyin input method.
 */
CarClass(CPinyinIME)
    , public Elastos::Droid::InputMethodService::InputMethodService
{
public:
    /**
     * Used to notify IME that the user selects a candidate or performs an
     * gesture.
     */
    class ChoiceNotifier
        : public Handler
        , public ICandidateViewListener
    {
    public:
        CAR_INTERFACE_DECL();

        ChoiceNotifier(
            /* [in] */ CPinyinIME* ime);

        CARAPI OnClickChoice(
            /* [in] */ Int32 choiceId);

        CARAPI OnToLeftGesture();

        CARAPI OnToRightGesture();

        CARAPI OnToTopGesture();

        CARAPI OnToBottomGesture();

    private:
        CPinyinIME* mIme;
    };

    class OnGestureListener
        : public GestureDetector::SimpleOnGestureListener
    {
    public:
        OnGestureListener(
            /* [in] */ Boolean reponseGestures,
            /* [in] */ IPinyinIME* ime);

        CARAPI OnDown(
            /* [in] */ IMotionEvent* e,
            /* [out] */ Boolean* res);

        CARAPI OnScroll(
            /* [in] */ IMotionEvent* e1,
            /* [in] */ IMotionEvent* e2,
            /* [in] */ Float distanceX,
            /* [in] */ Float distanceY,
            /* [out] */ Boolean* res);

        CARAPI OnFling(
            /* [in] */ IMotionEvent* me1,
            /* [in] */ IMotionEvent* me2,
            /* [in] */ Float velocityX,
            /* [in] */ Float velocityY,
            /* [out] */ Boolean* res);

        CARAPI_(void) OnDirectionGesture(
            /* [in] */ Int32 gravity);

    private:
        /**
         * When user presses and drags, the minimum x-distance to make a
         * response to the drag event.
         */
        static const Int32 MIN_X_FOR_DRAG;

        /**
         * When user presses and drags, the minimum y-distance to make a
         * response to the drag event.
         */
        static const Int32 MIN_Y_FOR_DRAG;

        /**
         * Velocity threshold for a screen-move gesture. If the minimum
         * x-velocity is less than it, no gesture.
         */
        static const Float VELOCITY_THRESHOLD_X1;

        /**
         * Velocity threshold for a screen-move gesture. If the maximum
         * x-velocity is less than it, no gesture.
         */
        static const Float VELOCITY_THRESHOLD_X2;

        /**
         * Velocity threshold for a screen-move gesture. If the minimum
         * y-velocity is less than it, no gesture.
         */
        static const Float VELOCITY_THRESHOLD_Y1;

        /**
         * Velocity threshold for a screen-move gesture. If the maximum
         * y-velocity is less than it, no gesture.
         */
        static const Float VELOCITY_THRESHOLD_Y2;

        /** If it FALSE, we will not response detected gestures. */
        Boolean mReponseGestures;

        /** The minimum X velocity observed in the gesture. */
        Float mMinVelocityX;

        /** The minimum Y velocity observed in the gesture. */
        Float mMinVelocityY;

        /** The first down time for the series of touch events for an action. */
        Int64 mTimeDown;

        /** The last time when onScroll() is called. */
        Int64 mTimeLastOnScroll;

        /** This flag used to indicate that this gesture is not a gesture. */
        Boolean mNotGesture;

        /** This flag used to indicate that this gesture has been recognized. */
        Boolean mGestureRecognized;

        AutoPtr<IPinyinIME> mIme;
    };

    /**
     * Connection used for binding to the Pinyin decoding service.
     */
    class PinyinDecoderServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL();

        PinyinDecoderServiceConnection(
            /* [in] */ CPinyinIME* ime);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        CPinyinIME* mIme;
    };

    class DecodingInfo
        : public Object
    {
    public:
        DecodingInfo(
            /* [in] */ CPinyinIME* ime);

        CARAPI_(void) Reset();

        CARAPI_(Boolean) IsCandidatesListEmpty();

        CARAPI_(Boolean) IsSplStrFull();

        CARAPI_(void) AddSplChar(
            /* [in] */ Char32 ch,
            /* [in] */ Boolean reset);

        // Prepare to delete before cursor. We may delete a spelling char if
        // the cursor is in the range of unfixed part, delete a whole spelling
        // if the cursor in inside the range of the fixed part.
        // This function only marks the position used to delete.
        CARAPI_(void) PrepareDeleteBeforeCursor();

        CARAPI_(Int32) GetLength();

        CARAPI_(Char32) GetCharAt(
            /* [in] */ Int32 index);

        CARAPI_(AutoPtr<StringBuffer>) GetOrigianlSplStr();

        CARAPI_(Int32) GetSplStrDecodedLen();

        CARAPI_(AutoPtr< ArrayOf<Int32> >) GetSplStart();

        CARAPI_(String) GetComposingStr();

        CARAPI_(String) GetComposingStrActivePart();

        CARAPI_(Int32) GetActiveCmpsLen();

        CARAPI_(String) GetComposingStrForDisplay();

        CARAPI_(Int32) GetActiveCmpsDisplayLen();

        CARAPI GetFullSent(
            /* [out] */ String* value);

        CARAPI GetCurrentFullSent(
            /* [in] */ Int32 activeCandPos,
            /* [out] */ String* value);

        CARAPI ResetCandidates();

        CARAPI CandidatesFromApp(
            /* [out] */ Boolean* state);

        CARAPI CanDoPrediction(
            /* [out] */ Boolean* state);

        CARAPI SelectionFinished(
            /* [out] */ Boolean* state);

        CARAPI GetCandidate(
            /* [in] */ Int32 candId,
            /* [out] */ String* candidate);

        CARAPI_(Boolean) PageReady(
            /* [in] */ Int32 pageNo);

        CARAPI_(Boolean) PreparePage(
            /* [in] */ Int32 pageNo);

        CARAPI PreparePredicts(
            /* [in] */ ICharSequence* history);

        CARAPI GetCurrentPageSize(
            /* [in] */ Int32 currentPage,
            /* [out] */ Int32* value);

        CARAPI GetCurrentPageStart(
            /* [in] */ Int32 currentPage,
            /* [out] */ Int32* value);

        CARAPI PageForwardable(
            /* [in] */ Int32 currentPage,
            /* [out] */ Boolean* state);

        CARAPI PageBackwardable(
            /* [in] */ Int32 currentPage,
            /* [out] */ Boolean* state);

        CARAPI CharBeforeCursorIsSeparator(
            /* [out] */ Boolean* state);

        CARAPI GetCursorPos(
            /* [out] */ Int32* pos);

        CARAPI GetCursorPosInCmps(
            /* [out] */ Int32* pos);

        CARAPI_(Int32) GetCursorPosInCmpsDisplay();

        CARAPI MoveCursorToEdge(
            /* [in] */ Boolean left);

        CARAPI MoveCursor(
            /* [in] */ Int32 offset);

        CARAPI GetSplNum(
            /* [out] */ Int32* value);

        CARAPI GetFixedLen(
            /* [out] */ Int32* value);

        CARAPI SetPinyinDecoderService(
            /* [in] */ IPinyinDecoderService* pinyinService);

        CARAPI GetPinyinDecoderService(
            /* [out] */ IPinyinDecoderService** pinyinService);

        CARAPI PrepareAppCompletions(
            /* [in] */ ArrayOf<ICompletionInfo*>* completions);

        CARAPI GetAppCompletions(
            /* [out, callee] */ ArrayOf<ICompletionInfo*>** completions);

        // After the user chooses a candidate, input method will do a
        // re-decoding and give the new candidate list.
        // If candidate id is less than 0, means user is inputting Pinyin,
        // not selecting any choice.
        CARAPI ChooseDecodingCandidate(
            /* [in] */ Int32 candId);

        CARAPI ChoosePredictChoice(
            /* [in] */ Int32 choiceId);

    private:
        CARAPI_(void) UpdateDecInfoForSearch(
            /* [in] */ Int32 totalChoicesNum);

        CARAPI_(void) GetCandiagtesForCache();

    public:
        /**
         * The total number of choices for display. The list may only contains
         * the first part. If user tries to navigate to next page which is not
         * in the result list, we need to get these items.
         **/
        Int32 mTotalChoicesNum;

        /**
         * Candidate list. The first one is the full-sentence candidate.
         */
        Vector<String> mCandidatesList;

        /**
         * Element i stores the starting position of page i.
         */
        Vector<Int32> mPageStart;

        /**
         * Element i stores the number of characters to page i.
         */
        Vector<Int32> mCnToPage;

        /**
         * The position to delete in Pinyin string. If it is less than 0, IME
         * will do an incremental search, otherwise IME will do a deletion
         * operation. if {@link #mIsPosInSpl} is TRUE, IME will delete the whole
         * string for mPosDelSpl-th spelling, otherwise it will only delete
         * mPosDelSpl-th character in the Pinyin string.
         */
        Int32 mPosDelSpl;

        /**
         * If {@link #mPosDelSpl} is big than or equal to 0, this member is used
         * to indicate that whether the postion is counted in spelling id or
         * character.
         */
        Boolean mIsPosInSpl;

    private:
        /**
         * Maximum length of the Pinyin string
         */
        static const Int32 PY_STRING_MAX;

        /**
         * Maximum number of candidates to display in one page.
         */
        static const Int32 MAX_PAGE_SIZE_DISPLAY;

        /**
         * Spelling (Pinyin) string.
         */
        AutoPtr<StringBuffer> mSurface;

        /**
         * Byte buffer used as the Pinyin string parameter for native function
         * call.
         */
        AutoPtr<ArrayOf<Byte> > mPyBuf;

        /**
         * The length of surface string successfully decoded by engine.
         */
        Int32 mSurfaceDecodedLen;

        /**
         * Composing string.
         */
        String mComposingStr;

        /**
         * Length of the active composing string.
         */
        Int32 mActiveCmpsLen;

        /**
         * Composing string for display, it is copied from mComposingStr, and
         * add spaces between spellings.
         **/
        String mComposingStrDisplay;

        /**
         * Length of the active composing string for display.
         */
        Int32 mActiveCmpsDisplayLen;

        /**
         * The first full sentence choice.
         */
        String mFullSent;

        /**
         * Number of characters which have been fixed.
         */
        Int32 mFixedLen;

        /**
         * If this flag is TRUE, selection is finished.
         */
        Boolean mFinishSelection;

        /**
         * The starting position for each spelling. The first one is the number
         * of the real starting position elements.
         */
        AutoPtr<ArrayOf<Int32> > mSplStart;

        /**
         * Editing cursor in mSurface.
         */
        Int32 mCursorPos;

        /**
         * Remote Pinyin-to-Hanzi decoding engine service.
         */
        AutoPtr<IPinyinDecoderService> mIPinyinDecoderService;

        /**
         * The complication information suggested by application.
         */
        AutoPtr<ArrayOf<ICompletionInfo*> > mAppCompletions;

        CPinyinIME* mHost;
    };

private:
    class PopupTimer
        : public HandlerRunnable
    {
    public:
        PopupTimer(
            /* [in] */ IPinyinIME* host);

        void PostShowFloatingWindow();

        void CancelShowing();

        CARAPI Run();

    private:
        Int32 mParentLocation[2];
        AutoPtr<IWeakReference> mWeak;
    };

    class BuilderListener
        : public ElRefBase
        , public IDialogInterfaceOnClickListener
        , public IDialogInterfaceOnDismissListener
    {
    public:
        BuilderListener(
            /* [in] */ CPinyinIME* ime);

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IDialogInterface* di,
            /* [in] */ Int32 position);

        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);

    private:
        CPinyinIME* mHost;
    };

public:
    CPinyinIME();
    ~CPinyinIME();

    CARAPI constructor();

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI OnCreate();

    virtual CARAPI OnEvaluateFullscreenMode(
        /* [out] */ Boolean* screenMode);

    CARAPI OnDestroy();

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnCreateCandidatesView(
        /* [out] */ IView** view);

    CARAPI ResponseSoftKeyEvent(
        /* [in] */ ISoftKey* _sKey);

    CARAPI OnStartInput(
        /* [in] */ IEditorInfo* editorInfo,
        /* [in] */ Boolean restarting);

    CARAPI OnStartInputView(
        /* [in] */ IEditorInfo* editorInfo,
        /* [in] */ Boolean restarting);

    CARAPI OnFinishInputView(
        /* [in] */ Boolean finishingInput);

    CARAPI OnFinishInput();

    CARAPI OnFinishCandidatesView(
        /* [in] */ Boolean finishingInput);

    CARAPI OnDisplayCompletions(
        /* [in] */ ArrayOf<ICompletionInfo *>* completions);

    CARAPI RequestHideSelf(
        /* [in] */ Int32 flags);

    CARAPI ShowOptionsMenu();

    CARAPI GetImeState(
        /* [out] */ ImeState* state);

    CARAPI GetCandidatesContainer(
        /* [out] */ ICandidatesContainer** container);

    CARAPI GetAssets(
        /* [out] */ IAssetManager** assetManager);

    CARAPI GetResources(
        /* [out] */ IResources** resources);

    CARAPI GetPackageManager(
        /* [out] */ IPackageManager** packageManager);

    CARAPI GetContentResolver(
        /* [out] */ IContentResolver** resolver);

    CARAPI GetApplicationContext(
        /* [out] */ IContext** ctx);

    CARAPI GetText(
        /* [in] */ Int32 resId,
        /* [out] */ ICharSequence** text);

    CARAPI GetString(
        /* [in] */ Int32 resId,
        /* [out] */ String* str);

    CARAPI SetTheme(
        /* [in] */ Int32 resid);

    CARAPI GetTheme(
        /* [out] */ IResourcesTheme** theme);

    CARAPI ObtainStyledAttributes(
        /* [in] */ ArrayOf<Int32>* attrs,
        /* [out] */ ITypedArray** styles);

    CARAPI ObtainStyledAttributes(
        /* [in] */ Int32 resid,
        /* [in] */ ArrayOf<Int32>* attrs,
        /* [out] */ ITypedArray** styles);

    CARAPI ObtainStyledAttributes(
        /* [in] */ IAttributeSet* set,
        /* [in] */ ArrayOf<Int32>* attrs,
        /* [out] */ ITypedArray** styles);

    CARAPI ObtainStyledAttributes(
        /* [in] */ IAttributeSet* set,
        /* [in] */ ArrayOf<Int32>* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes,
        /* [out] */ ITypedArray** styles);

    CARAPI GetClassLoader(
        /* [out] */ IClassLoader** loader);

    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI GetApplicationInfo(
        /* [out] */ IApplicationInfo** info);

    CARAPI GetPackageResourcePath(
        /* [out] */ String* path);

    CARAPI GetExternalFilesDir(
        /* [in] */ const String& type,
        /* [out] */ IFile** filesDir);

    CARAPI GetCacheDir(
        /* [out] */ IFile** cacheDir);

    CARAPI GetDir(
        /* [in] */ const String& name,
        /* [in] */ Int32 mode,
        /* [out] */ IFile** dir);

    CARAPI StartActivity(
        /* [in] */ IIntent* intent);

    CARAPI StartIntentSender(
        /* [in] */ IIntentSender* intent,
        /* [in] */ IIntent* fillInIntent,
        /* [in] */ Int32 flagsMask,
        /* [in] */ Int32 flagsValues,
        /* [in] */ Int32 extraFlags);

    CARAPI SendBroadcast(
        /* [in] */ IIntent* intent);

    CARAPI StartService(
        /* [in] */ IIntent* service,
        /* [out] */ IComponentName** name);

    CARAPI StopService(
        /* [in] */ IIntent* service,
        /* [out] */ Boolean* succeeded);

    CARAPI BindService(
        /* [in] */ IIntent* service,
        /* [in] */ IServiceConnection* conn,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* succeeded);

    CARAPI UnbindService(
        /* [in] */ IServiceConnection* conn);

    CARAPI GetSystemService(
        /* [in] */ const String& name,
        /* [out] */ IInterface** object);

    CARAPI CreatePackageContext(
        /* [in] */ const String& capsuleName,
        /* [in] */ Int32 flags,
        /* [out] */ IContext** ctx);

    CARAPI CheckCallingPermission(
        /* [in] */ const String& permission,
        /* [out] */ Int32* value);

    CARAPI EnforceCallingOrSelfPermission(
        /* [in] */ const String& permission,
        /* [in] */ const String& message);

    CARAPI RevokeUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags);

    CARAPI CheckCallingOrSelfPermission(
        /* [in] */ const String& permission,
        /* [out] */ Int32* perm);

    CARAPI CheckPermission(
        /* [in] */ const String& permission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [out] */ Int32 * result);

    CARAPI CheckUriPermission(
        /* [in] */ IUri * uri,
        /* [in] */ const String& readPermission,
        /* [in] */ const String& writePermission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 modeFlags,
        /* [out] */ Int32 * result);

    CARAPI CheckUriPermission(
        /* [in] */ IUri * uri,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 modeFlags,
        /* [out] */ Int32 * result);

    CARAPI GrantUriPermission(
        /* [in] */ const String& toCapsule,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags);

    CARAPI GetBaseContext(
        /* [out] */ IContext** ctx);

    CARAPI Attach(
        /* [in] */ IContext* ctx,
        /* [in] */ IActivityThread* apartment,
        /* [in] */ const String& className,
        /* [in] */ IBinder* token,
        /* [in] */ IApplication* application,
        /* [in] */ IIActivityManager* activityManager);

    CARAPI OnStart(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 startId);

    CARAPI OnStartCommand(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 startId,
        /* [out] */ Int32* result);

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

    CARAPI OnUnbind(
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* succeeded);

    CARAPI OnRebind(
        /* [in] */ IIntent* intent);

    CARAPI GetClassName(
        /* [out] */ String* className);

    CARAPI GetKeyDispatcherState(
        /* [out] */ IDispatcherState** dispatcherState);

    CARAPI OnCreateInputMethodInterface(
        /* [out] */ IAbstractInputMethodImpl** inputMethodImpl);

    CARAPI OnCreateInputMethodSessionInterface(
        /* [out] */ IAbstractInputMethodSessionImpl** abstractInputMethodSessionImpl);

    CARAPI OnTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* state);

    CARAPI OnInitializeInterface();

    CARAPI GetLayoutInflater(
        /* [out] */ ILayoutInflater** inflater);

    CARAPI GetWindow(
        /* [out] */ IDialog** dialog);

    CARAPI GetMaxWidth(
        /* [out] */ Int32* maxWidth);

    CARAPI GetCurrentInputBinding(
        /* [out] */ IInputBinding** binding);

    CARAPI GetCurrentInputConnection(
        /* [out] */ IInputConnection** inputConnection);

    CARAPI GetCurrentInputStarted(
        /* [out] */ Boolean* state);

    CARAPI GetCurrentInputEditorInfo(
        /* [out] */ IEditorInfo** editorInfo);

    CARAPI UpdateFullscreenMode();

    CARAPI OnConfigureWindow(
        /* [in] */ IWindow* win,
        /* [in] */ Boolean isFullscreen,
        /* [in] */ Boolean isCandidatesOnly);

    CARAPI IsFullscreenMode(
        /* [out] */ Boolean* fullScreenMode);

    CARAPI SetExtractViewShown(
        /* [in] */ Boolean shown);

    CARAPI IsExtractViewShown(
        /* [out] */ Boolean* shown);

    CARAPI UpdateInputViewShown();

    CARAPI IsShowInputRequested(
        /* [out] */ Boolean* requested);

    CARAPI IsInputViewShown(
        /* [out] */ Boolean* shown);

    CARAPI OnEvaluateInputViewShown(
        /* [out] */ Boolean* shown);

    CARAPI SetCandidatesViewShown(
        /* [in] */ Boolean shown);

    CARAPI GetCandidatesHiddenVisibility(
        /* [out] */ Int32* visibility);

    CARAPI ShowStatusIcon(
        /* [in] */ Int32 iconResId);

    CARAPI HideStatusIcon();

    CARAPI SwitchInputMethod(
        /* [in] */ const String& id);

    CARAPI SetExtractView(
        /* [in] */ IView* view);

    CARAPI SetCandidatesView(
        /* [in] */ IView* view);

    CARAPI SetInputView(
        /* [in] */ IView* view);

    CARAPI OnCreateExtractTextView(
        /* [out] */ IView** view);

    CARAPI OnCreateInputView(
        /* [out] */ IView** view);

    CARAPI OnStartCandidatesView(
        /* [in] */ IEditorInfo* info,
        /* [in] */ Boolean restarting);

    CARAPI OnShowInputRequested(
        /* [in] */ Int32 flags,
        /* [in] */ Boolean configChange,
        /* [out] */ Boolean* requested);

    CARAPI ShowWindow(
        /* [in] */ Boolean showInput);

    CARAPI HideWindow();

    CARAPI OnWindowShown();

    CARAPI OnWindowHidden();

    CARAPI OnBindInput();

    CARAPI OnUnbindInput();

    CARAPI OnUpdateExtractedText(
        /* [in] */ Int32 token,
        /* [in] */ IExtractedText* text);

    CARAPI OnUpdateSelection(
        /* [in] */ Int32 oldSelStart,
        /* [in] */ Int32 oldSelEnd,
        /* [in] */ Int32 newSelStart,
        /* [in] */ Int32 newSelEnd,
        /* [in] */ Int32 candidatesStart,
        /* [in] */ Int32 candidatesEnd);

    CARAPI OnUpdateCursor(
        /* [in] */ IRect* newCursor);

    CARAPI OnAppPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data);

    CARAPI SendDownUpKeyEvents(
        /* [in] */ Int32 keyEventCode);

    CARAPI SendDefaultEditorAction(
        /* [in] */ Boolean fromEnterKey,
        /* [out] */ Boolean* state);

    CARAPI SendKeyChar(
        /* [in] */ Char32 charCode);

    CARAPI OnExtractedSelectionChanged(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI OnExtractedTextClicked();

    CARAPI OnExtractedCursorMovement(
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy);

    CARAPI OnExtractTextContextMenuItem(
        /* [in] */ Int32 id,
        /* [out] */ Boolean* state);

    CARAPI GetTextForImeAction(
        /* [in] */ Int32 imeOptions,
        /* [out] */ ICharSequence** text);

    CARAPI OnUpdateExtractingVisibility(
        /* [in] */ IEditorInfo* ei);

    CARAPI OnUpdateExtractingViews(
        /* [in] */ IEditorInfo* ei);

    CARAPI OnExtractingInputChanged(
        /* [in] */ IEditorInfo* ei);

    CARAPI OnLowMemory();

    CARAPI OnTrimMemory(
        /* [in] */ Int32 level);

    CARAPI GetMainLooper(
        /* [out] */ ILooper** looper);

    CARAPI RegisterComponentCallbacks(
        /* [in] */ IComponentCallbacks* componentCallback);

    CARAPI UnregisterComponentCallbacks(
        /* [in] */ IComponentCallbacks* componentCallback);

    CARAPI GetString(
        /* [in] */ Int32 resId,
        /* [in] */ ArrayOf<IInterface*>* formatArgs,
        /* [out] */ String* str);

    CARAPI GetThemeResId(
        /* [out] */ Int32* resId);

    CARAPI GetPackageCodePath(
        /* [out] */ String* codePath);

    CARAPI GetSharedPrefsFile(
        /* [in] */ const String& name,
        /* [out] */ IFile** file);

    CARAPI GetSharedPreferences(
        /* [in] */ const String& name,
        /* [in] */ Int32 mode,
        /* [out] */ ISharedPreferences** prefs);

    CARAPI OpenFileOutput(
        /* [in] */ const String& name,
        /* [in] */ Int32 mode,
        /* [out] */IFileOutputStream** fileOutputStream);

    CARAPI DeleteFile(
        /* [in] */ const String& name,
        /* [out] */ Boolean* succeeded);

    CARAPI GetFileStreamPath(
        /* [in] */ const String& name,
        /* [out] */ IFile** file);

    CARAPI GetFilesDir(
        /* [out] */ IFile** filesDir);

    CARAPI GetObbDir(
        /* [out] */ IFile** obbDir);

    CARAPI GetFileList(
        /* [out, callee] */ ArrayOf<String>** fileList);

    CARAPI OpenOrCreateDatabase(
        /* [in] */ const String& name,
        /* [in] */ Int32 mode,
        /* [in] */ ISQLiteDatabaseCursorFactory* factory,
        /* [out] */ ISQLiteDatabase** sqliteDB);

    CARAPI OpenOrCreateDatabase(
        /* [in] */ const String& name,
        /* [in] */ Int32 mode,
        /* [in] */ ISQLiteDatabaseCursorFactory* factory,
        /* [in] */ IDatabaseErrorHandler* errorHandler,
        /* [out] */ ISQLiteDatabase** sqliteDB);

    CARAPI DeleteDatabase(
        /* [in] */ const String& name,
        /* [out] */ Boolean* succeeded);

    CARAPI GetDatabasePath(
        /* [in] */ const String& name,
        /* [out] */ IFile** path);

    CARAPI GetDatabaseList(
        /* [out, callee] */ ArrayOf<String>** databaseList);

    CARAPI GetWallpaper(
        /* [out] */ IDrawable** drawable);

    CARAPI PeekWallpaper(
        /* [out] */ IDrawable** drawable);

    CARAPI GetWallpaperDesiredMinimumWidth(
        /* [out] */ Int32* minWidth);

    CARAPI GetWallpaperDesiredMinimumHeight(
        /* [out] */ Int32* minHeight);

    CARAPI SetWallpaper(
        /* [in] */ IBitmap* bitmap);

    CARAPI SetWallpaper(
        /* [in] */ IInputStream* data);

    CARAPI ClearWallpaper();

    CARAPI StartActivityAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user);

    CARAPI StartActivityAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* options,
        /* [in] */ IUserHandle* user);

    CARAPI StartActivities(
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ IBundle* options);

    CARAPI StartIntentSender(
        /* [in] */ IIntentSender* intent,
        /* [in] */ IIntent* fillInIntent,
        /* [in] */ Int32 flagsMask,
        /* [in] */ Int32 flagsValues,
        /* [in] */ Int32 extraFlags,
        /* [in] */ IBundle* options);

    CARAPI SendBroadcast(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& receiverPermission);

    CARAPI SendOrderedBroadcast(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& receiverPermission);

    CARAPI SendOrderedBroadcast(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& receiverPermission,
        /* [in] */ IBroadcastReceiver* resultReceiver,
        /* [in] */ IHandler* scheduler,
        /* [in] */ Int32 initialCode,
        /* [in] */ const String& initialData,
        /* [in] */ IBundle* initialExtras);

    CARAPI SendBroadcastAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user);

    CARAPI SendBroadcastAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user,
        /* [in] */ const String& receiverPermission);

    CARAPI SendOrderedBroadcastAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user,
        /* [in] */ const String& receiverPermission,
        /* [in] */ IBroadcastReceiver* resultReceiver,
        /* [in] */ IHandler* scheduler,
        /* [in] */ Int32 initialCode,
        /* [in] */ const String& initialData,
        /* [in] */ IBundle* initialExtras);

    CARAPI SendStickyBroadcast(
        /* [in] */ IIntent* intent);

    CARAPI SendStickyOrderedBroadcast(
        /* [in] */ IIntent* intent,
        /* [in] */ IBroadcastReceiver* resultReceiver,
        /* [in] */ IHandler* scheduler,
        /* [in] */ Int32 initialCode,
        /* [in] */ const String& initialData,
        /* [in] */ IBundle* initialExtras);

    CARAPI RemoveStickyBroadcast(
        /* [in] */ IIntent* intent);

    CARAPI SendStickyBroadcastAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user);

    CARAPI SendStickyOrderedBroadcastAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user,
        /* [in] */ IBroadcastReceiver* resultReceiver,
        /* [in] */ IHandler* scheduler,
        /* [in] */ Int32 initialCode,
        /* [in] */ const String& initialData,
        /* [in] */ IBundle* initialExtras);

    CARAPI RemoveStickyBroadcastAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user);

    CARAPI StartInstrumentation(
        /* [in] */ IComponentName* className,
        /* [in] */ const String& profileFile,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* succeeded);

    CARAPI EnforcePermission(
        /* [in] */ const String& permission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ const String& message);

    CARAPI EnforceCallingPermission(
        /* [in] */ const String& permission,
        /* [in] */ const String& message);

    CARAPI IsRestricted(
        /* [out] */ Boolean* isRestricted);

    CARAPI OpenFileInput(
        /* [in] */ const String& name,
        /* [out] */ IFileInputStream** fileInputStream);

    CARAPI GetExternalCacheDir(
        /* [out] */ IFile** externalDir);

    CARAPI StartActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* options);

    CARAPI StartActivities(
        /* [in] */ ArrayOf<IIntent*>* intents);

    CARAPI StartActivitiesAsUser(
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ IBundle* options,
        /* [in] */ IUserHandle* userHandle);

    CARAPI RegisterReceiverAsUser(
        /* [in] */ IBroadcastReceiver* receiver,
        /* [in] */ IUserHandle* user,
        /* [in] */ IIntentFilter* filter,
        /* [in] */ const String& broadcastPermission,
        /* [in] */ IHandler* scheduler,
        /* [out] */ IIntent** stickyIntent);

    CARAPI StartServiceAsUser(
        /* [in] */ IIntent* service,
        /* [in] */ IUserHandle* user,
        /* [out] */ IComponentName** name);

    CARAPI StopServiceAsUser(
        /* [in] */ IIntent* service,
        /* [in] */ IUserHandle* user,
        /* [out] */ Boolean* succeeded);

    CARAPI BindService(
        /* [in] */ IIntent* service,
        /* [in] */ Elastos::Droid::Content::IServiceConnection* conn,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* succeeded);

    CARAPI CheckCallingUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags,
        /* [out] */ Int32* result);

    CARAPI CheckCallingOrSelfUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags,
        /* [out] */ Int32* result);

    CARAPI EnforceUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 modeFlags,
        /* [in] */ const String& message);

    CARAPI EnforceCallingUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags,
        /* [in] */ const String& message);

    CARAPI EnforceCallingOrSelfUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags,
        /* [in] */ const String& message);

    CARAPI EnforceUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ const String& readPermission,
        /* [in] */ const String& writePermission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 modeFlags,
        /* [in] */ const String& message);

    CARAPI CreatePackageContextAsUser(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 flags,
        /* [in] */ IUserHandle* user,
        /* [out] */ IContext** ctx);

    CARAPI CreateConfigurationContext(
        /* [in] */ IConfiguration* overrideConfiguration,
        /* [out] */ IContext** ctx);

    CARAPI CreateDisplayContext(
        /* [in] */ IDisplay* display,
        /* [out] */ IContext** ctx);

//    CARAPI GetCompatibilityInfo(
//        /* [in] */ Int32 displayId,
//        /* [out] */ ICompatibilityInfoHolder** infoHolder);

    CARAPI GetApplication(
        /* [out] */ IApplication** application);

    CARAPI OnTaskRemoved(
        /* [in] */ IIntent* rootIntent);

    CARAPI RegisterReceiver(
        /* [in] */ IBroadcastReceiver* receiver,
        /* [in] */ IIntentFilter* filter,
        /* [out] */ IIntent** intent);

    CARAPI RegisterReceiver(
        /* [in] */ IBroadcastReceiver* receiver,
        /* [in] */ IIntentFilter* filter,
        /* [in] */ const String& broadcastPermission,
        /* [in] */ IHandler* scheduler,
        /* [out] */ IIntent** intent);

    CARAPI StopSelf();

    CARAPI StopSelf(
        /* [in] */ Int32 startId);

    CARAPI StopSelfResult(
        /* [in] */ Int32 startId,
        /* [out] */ Boolean* res);

    CARAPI SetForeground(
        /* [in] */ Boolean isForeground);

    CARAPI StartForeground(
        /* [in] */ Int32 id,
        /* [in] */ INotification* notification);

    CARAPI StopForeground(
        /* [in] */ Boolean removeNotification);

    virtual CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* state);

    virtual CARAPI OnViewClicked(
        /* [in] */ Boolean focusChanged);

    CARAPI EnableHardwareAcceleration(
        /* [out] */ Boolean* enable);

    virtual CARAPI SetBackDisposition(
        /* [in] */ Int32 disposition);

    virtual CARAPI GetBackDisposition(
        /* [out] */ Int32* pos);

    virtual CARAPI OnExtractedReplaceText(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ICharSequence* text);

    virtual CARAPI OnExtractedDeleteText(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    virtual CARAPI OnExtractedSetSpan(
        /* [in] */ IInterface* span,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 flags);

    CARAPI OnKeyLongPress(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnKeyMultiple(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 count,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI UnregisterReceiver(
        /* [in] */ IBroadcastReceiver* receiver);

private:
    Boolean ProcessKey(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Boolean realAction);

    // keyCode can be from both hard key or soft key.
    Boolean ProcessFunctionKeys(
        /* [in] */ Int32 keyCode,
        /* [in] */ Boolean realAction);

    Boolean ProcessStateIdle(
        /* [in] */ Int32 keyChar,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Boolean realAction);

    Boolean ProcessStateInput(
        /* [in] */ Int32 keyChar,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Boolean realAction);

    Boolean ProcessStatePredict(
        /* [in] */ Int32 keyChar,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Boolean realAction);

    Boolean ProcessStateEditComposing(
        /* [in] */ Int32 keyChar,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Boolean realAction);

    Boolean TryInputRawUnicode(
        /* [in] */ const String& str);

    Boolean ProcessSurfaceChange(
        /* [in] */ Int32 keyChar,
        /* [in] */ Int32 keyCode);

    void ChangeToStateComposing(
        /* [in] */ Boolean updateUi);

    void ChangeToStateInput(
        /* [in] */ Boolean updateUi);

    void SimulateKeyEventDownUp(
        /* [in] */ Int32 keyCode);

    void CommitResultText(
        /* [in] */ const String& resultText);

    void UpdateComposingText(
        /* [in] */ Boolean visible);

    void InputCommaPeriod(
        /* [in] */ const String& preEdit,
        /* [in] */ Int32 keyChar,
        /* [in] */ Boolean dismissCandWindow,
        /* [in] */ ImeState nextState);

    void ResetToIdleState(
        /* [in] */ Boolean resetInlineText);

    void ChooseAndUpdate(
        /* [in] */ Int32 candId);

    // If activeCandNo is less than 0, get the current active candidate number
    // from candidate view, otherwise use activeCandNo.
    void ChooseCandidate(
        /* [in] */ Int32 activeCandNo);

    Boolean StartPinyinDecoderService();

    void ShowCandidateWindow(
        /* [in] */ Boolean showComposingView);

    void DismissCandidateWindow();

    void ResetCandidateWindow();

    void UpdateIcon(
        /* [in] */ Int32 iconId);

    void OnChoiceTouched(
        /* [in] */ Int32 activeCandNo);

    void LaunchSettings();

private:
    /**
     * TAG for debug.
     */
    static String TAG;

    /**
     * If is is TRUE, IME will simulate key events for delete key, and send the
     * events back to the application.
     */
    static const Boolean SIMULATE_KEY_DELETE;

    /**
     * Necessary environment configurations like screen size for this IME.
     */
    AutoPtr<Environment> mEnvironment;

    /**
     * Used to switch input mode.
     */
    AutoPtr<InputModeSwitcher> mInputModeSwitcher;

    /**
     * Soft keyboard container view to host real soft keyboard view.
     */
    AutoPtr<ISkbContainer> mSkbContainer;

    /**
     * The floating container which contains the composing view. If necessary,
     * some other view like candiates container can also be put here.
     */
    AutoPtr<ILinearLayout> mFloatingContainer;

    /**
     * View to show the composing string.
     */
    AutoPtr<IComposingView> mComposingView;

    /**
     * Window to show the composing string.
     */
    AutoPtr<IPopupWindow> mFloatingWindow;

    /**
     * Used to show the floating window.
     */
    AutoPtr<PopupTimer> mFloatingWindowTimer;

    /**
     * View to show candidates list.
     */
    AutoPtr<ICandidatesContainer> mCandidatesContainer;

    /**
     * Balloon used when user presses a candidate.
     */
    AutoPtr<IBalloonHint> mCandidatesBalloon;

    /**
     * Used to notify the input method when the user touch a candidate.
     */
    AutoPtr<ChoiceNotifier> mChoiceNotifier;

    /**
     * Used to notify gestures from soft keyboard.
     */
    AutoPtr<IGestureDetectorOnGestureListener> mGestureListenerSkb;

    /**
     * Used to notify gestures from candidates view.
     */
    AutoPtr<IGestureDetectorOnGestureListener> mGestureListenerCandidates;

    /**
     * The on-screen movement gesture detector for soft keyboard.
     */
    AutoPtr<IGestureDetector> mGestureDetectorSkb;

    /**
     * The on-screen movement gesture detector for candidates view.
     */
    AutoPtr<IGestureDetector> mGestureDetectorCandidates;

    /**
     * Option dialog to choose settings and other IMEs.
     */
    AutoPtr<IAlertDialog> mOptionsDialog;

    /**
     * Connection used to bind the decoding service.
     */
    AutoPtr<PinyinDecoderServiceConnection> mPinyinDecoderServiceConnection;

    /**
     * The current IME status.
     *
     * @see com.android.inputmethod.pinyin.PinyinIME.ImeState
     */
    ImeState mImeState;

    /**
     * The decoding information, include spelling(Pinyin) string, decoding
     * result, etc.
     */
    AutoPtr<DecodingInfo> mDecInfo;

    /**
     * For English input.
     */
    AutoPtr<EnglishInputProcessor> mImEn;

    // receive ringer mode changes
    AutoPtr<IBroadcastReceiver> mReceiver;
    //TODO
    // private BroadcastReceiver mReceiver = new BroadcastReceiver() {
    //     @Override
    //     public void onReceive(Context context, Intent intent) {
    //         SoundManager.getInstance(context).updateRingerMode();
    //     }
    // };

};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_CPINYINIME_H__
