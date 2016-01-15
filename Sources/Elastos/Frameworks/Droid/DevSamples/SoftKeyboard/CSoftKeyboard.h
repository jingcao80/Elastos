
#ifndef __CSOFTKEYBOARD_H__
#define __CSOFTKEYBOARD_H__

#include <ext/frameworkdef.h>
#include "_CSoftKeyboard.h"
#include <inputmethodservice/InputMethodService.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Core::StringBuffer;
using Elastos::Droid::App::IActivityThread;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::INotification;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Graphics::IRect;
//using Elastos::Droid::InputMethodService::InputMethodService;
using Elastos::Droid::InputMethodService::IKeyboard;
using Elastos::Droid::InputMethodService::IKeyboardView;
using Elastos::Droid::InputMethodService::ICandidateView;
using Elastos::Droid::InputMethodService::IAbstractInputMethodSessionImpl;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Droid::View::InputMethod::IEditorInfo;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SoftKeyboard {

CarClass(CSoftKeyboard), public Elastos::Droid::InputMethodService::InputMethodService
{
public:
    CSoftKeyboard();

    ~CSoftKeyboard();

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

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

    CARAPI OnCreate();

    CARAPI OnStart(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 startId);

    CARAPI OnStartCommand(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 startId,
        /* [out] */ Int32* result);

    CARAPI OnDestroy();

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

    CARAPI OnEvaluateFullscreenMode(
        /* [out] */ Boolean* screenMode);

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

    CARAPI OnCreateCandidatesView(
        /* [out] */ IView** view);

    CARAPI OnCreateInputView(
        /* [out] */ IView** view);

    CARAPI OnStartInputView(
        /* [in] */ IEditorInfo* info,
        /* [in] */ Boolean restarting);

    CARAPI OnFinishInputView(
        /* [in] */ Boolean finishingInput);

    CARAPI OnStartCandidatesView(
        /* [in] */ IEditorInfo* info,
        /* [in] */ Boolean restarting);

    CARAPI OnFinishCandidatesView(
        /* [in] */ Boolean finishingInput);

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

    CARAPI OnStartInput(
        /* [in] */ IEditorInfo* attribute,
        /* [in] */ Boolean restarting);

    CARAPI OnFinishInput();

    CARAPI OnDisplayCompletions(
        /* [in] */ ArrayOf<ICompletionInfo *>* completions);

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

    CARAPI RequestHideSelf(
        /* [in] */ Int32 flags);

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

    CARAPI OnPress(
        /* [in] */ Int32 primaryCode);

    CARAPI OnRelease(
        /* [in] */ Int32 primaryCode);

    CARAPI OnKey(
        /* [in] */ Int32 primaryCode,
        /* [in] */ ArrayOf<Int32>* keyCodes);

    CARAPI OnText(
        /* [in] */ ICharSequence* text);

    CARAPI SwipeLeft();

    CARAPI SwipeRight();

    CARAPI SwipeDown();

    CARAPI SwipeUp();

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI OnLowMemory();

    CARAPI OnTrimMemory(
        /* [in] */ Int32 level);

    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnKeyLongPress(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnKeyMultiple(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 count,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

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

    CARAPI constructor();

    CARAPI_(void) SetSuggestions(
        /* [in] */ ArrayOf<ICharSequence*>* suggestions,
        /* [in] */ Boolean completions,
        /* [in] */ Boolean typedWordValid);

    CARAPI_(Boolean) IsWordSeparator(
        /* [in] */ Int32 code);

    CARAPI_(void) PickDefaultCandidate();

    CARAPI PickSuggestionManually(
        /* [in] */ Int32 index);

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

    CARAPI UnregisterReceiver(
        /* [in] */ IBroadcastReceiver* receiver);

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

private:
    /**
     * This translates incoming hard key events in to edit operations on an
     * InputConnection.  It is only needed when using the
     * PROCESS_HARD_KEYS option.
     */
    CARAPI_(Boolean) TranslateKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event);

    /**
     * Helper function to commit any text being composed in to the editor.
     */
    CARAPI_(void) CommitTyped(
        /* [in] */ IInputConnection* inputConnection);

    /**
     * Helper to update the shift state of our keyboard based on the initial
     * editor state.
     */
    CARAPI_(void) UpdateShiftKeyState(
        /* [in] */ IEditorInfo* attr);

    /**
     * Helper to determine if a given character code is alphabetic.
     */
    CARAPI_(Boolean) IsAlphabet(
        /* [in] */ Int32 code);

    /**
     * Helper to send a key down / key up pair to the current editor.
     */
    CARAPI_(void) KeyDownUp(
        /* [in] */ Int32 keyEventCode);

    /**
     * Helper to send a character to the editor as raw key events.
     */
    CARAPI_(void) SendKey(
        /* [in] */ Int32 keyCode);

    /**
     * Update the list of available candidates from the current composing
     * text.  This will need to be filled in by however you are determining
     * candidates.
     */
    CARAPI_(void) UpdateCandidates();

    CARAPI_(void) HandleBackspace();

    CARAPI_(void) HandleShift();

    CARAPI_(void) HandleCharacter(
        /* [in] */ Int32 primaryCode,
        /* [in] */ ArrayOf<Int32>* keyCodes);

    CARAPI_(void) HandleClose();

    CARAPI_(void) CheckToggleCapsLock();

private:
    /**
     * This Boolean indicates the optional example code for performing
     * processing of hard keys in addition to regular text generation
     * from on-screen interaction.  It would be used for input methods that
     * perform language translations (such as converting text entered on
     * a QWERTY keyboard to Chinese), but may not be used for input methods
     * that are primarily intended to be used for on-screen text entry.
     */
    static const Boolean PROCESS_HARD_KEYS = TRUE;

    AutoPtr<IKeyboardView>           mInputView;
    AutoPtr<ICandidateView>          mCandidateView;
    AutoPtr<ArrayOf<ICompletionInfo *> > mCompletions;

    StringBuffer                     mComposing;
    Boolean                          mPredictionOn;
    Boolean                          mCompletionOn;
    Int32                            mLastDisplayWidth;
    Boolean                          mCapsLock;
    Int64                            mLastShiftTime;
    Int64                            mMetaState;

    AutoPtr<IKeyboard>               mSymbolsKeyboard;
    AutoPtr<IKeyboard>               mSymbolsShiftedKeyboard;
    AutoPtr<IKeyboard>               mQwertyKeyboard;
    AutoPtr<IKeyboard>               mCurKeyboard;

    String                           mWordSeparators;
};

} // namespace SoftKeyboard
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CSOFTKEYBOARD_H__
