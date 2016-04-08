
#ifndef __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINIME_H__
#define __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINIME_H__

#include <ext/frameworkdef.h>
#include "_CPinyinIME.h"
#include <inputmethodservice/InputMethodService.h>
#include "EnglishInputProcessor.h"
#include "elastos/droid/os/HandlerRunnable.h"
#include "elastos/droid/os/HandlerBase.h"

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IActivityThread;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::INotification;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::Inputmethods::PinyinIME::ICandidatesContainer;
using Elastos::Droid::Inputmethods::PinyinIME::IBalloonHint;
using Elastos::Droid::Inputmethods::PinyinIME::IInputModeSwitcher;
using Elastos::Droid::Inputmethods::PinyinIME::IComposingView;
using Elastos::Droid::Inputmethods::PinyinIME::IDecodingInfo;
using Elastos::Droid::Inputmethods::PinyinIME::ImeState;
using Elastos::Droid::Inputmethods::PinyinIME::ISoftKey;
using Elastos::Droid::Inputmethods::PinyinIME::ISkbContainer;
using Elastos::Droid::Inputmethods::PinyinIME::ICandidateViewListener;
using Elastos::Droid::Inputmethods::PinyinIME::IPinyinEnvironment;
using Elastos::Droid::InputMethodService::IAbstractInputMethodSessionImpl;
using Elastos::Droid::Os::HandlerRunnable;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::View::IGestureDetector;
using Elastos::Droid::View::IGestureDetectorOnGestureListener;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::Widget::IPopupWindow;
using Elastos::Droid::Widget::ILinearLayout;


namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {


/**
 * Main class of the Pinyin input method.
 */
CarClass(CPinyinIME), public Elastos::Droid::InputMethodService::InputMethodService
{
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
    /**
     * Used to notify IME that the user selects a candidate or performs an
     * gesture.
     */
    class ChoiceNotifier
        : public HandlerBase
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

    /**
     * Connection used for binding to the Pinyin decoding service.
     */
    class PinyinDecoderServiceConnection
        : public ElRefBase
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

    CARAPI GetCompatibilityInfo(
        /* [in] */ Int32 displayId,
        /* [out] */ ICompatibilityInfoHolder** infoHolder);

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
    AutoPtr<IPinyinEnvironment> mEnvironment;

    /**
     * Used to switch input mode.
     */
    AutoPtr<IInputModeSwitcher> mInputModeSwitcher;

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
    AutoPtr<IDecodingInfo> mDecInfo;

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

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINIME_H__
