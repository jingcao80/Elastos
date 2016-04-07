
#ifndef __ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CInputMethodManagerService.h"
#define HASH_FOR_OS
#define HASH_FOR_TEXT_STYLE
#include "elastos/droid/ext/frameworkhash.h"
#include "elastos/droid/content/BroadcastReceiver.h"
// #include "elastos/droid/content/ContentResolver.h"

#include <elastos/droid/widget/ArrayAdapter.h>
#include <elastos/droid/database/ContentObserver.h>
#include "elastos/droid/internal/content/PackageMonitor.h"
#include "elastos/droid/server/statusbar/CStatusBarManagerService.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IRemoteCallback;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::App::IKeyguardManager;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IIUserSwitchObserver;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Text::Style::ISuggestionSpan;
using Elastos::Droid::View::IInputMethodCallback;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::InputMethod::IInputBinding;
using Elastos::Droid::View::InputMethod::IInputMethodInfo;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Droid::View::InputMethod::IInputMethodSession;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Droid::Internal::StatusBar::IIStatusBarService;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Internal::Content::IPackageMonitor;
using Elastos::Droid::Internal::InputMethod::IInputMethodSubtypeSwitchingController;
using Elastos::Droid::Internal::InputMethod::IImeSubtypeListItem;
using Elastos::Droid::Internal::InputMethod::IInputMethodUtils;
using Elastos::Droid::Internal::InputMethod::IInputMethodSettings;
using Elastos::Droid::Internal::Os::IHandlerCaller;
using Elastos::Droid::Internal::Os::IHandlerCallerCallback;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Utility::IFastXmlSerializer;
using Elastos::Droid::Internal::View::IIInputContext;
using Elastos::Droid::Internal::View::IIInputMethod;
using Elastos::Droid::Internal::View::IIInputSessionCallback;
using Elastos::Droid::Internal::View::IInputMethodClient;
using Elastos::Droid::Internal::View::IIInputMethodManager;
using Elastos::Droid::Internal::View::IIInputMethodSession;
using Elastos::Droid::Internal::View::IInputBindResult;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Server::Wm::IOnHardKeyboardStatusChangeListener;
using Elastos::Droid::Server::StatusBar::CStatusBarManagerService;

using Elastos::Core::IComparable;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {
    class CWindowManagerService;
}}}}

using Elastos::Droid::Server::Wm::CWindowManagerService;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * This class provides a system service that manages input methods.
 */

CarClass(CInputMethodManagerService)
    , public Object
    , public IIInputMethodManager
    , public IBinder
    , public IServiceConnection
    , public IHandlerCallback
{
    friend class CInputMethodManagerServiceMethodCallback;
    friend class CInputMethodManagerServiceUserSwitchObserver;

private:
    class MyHandlerCallerCallback
        : public Object
        , public IHandlerCallerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyHandlerCallerCallback(
            /* [in] */ IWeakReference* host);

        CARAPI ExecuteMessage(
            /* [in] */ IMessage* msg);

    private:
        AutoPtr<IWeakReference> mHost;
    };

    class VisibleServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);
    };

    class ClientState;

    class SessionState
        : public Object
    {
    public:
        SessionState(
            /* [in] */ ClientState* client,
            /* [in] */ IIInputMethod* method,
            /* [in] */ IIInputMethodSession* session,
            /* [in] */ IInputChannel* channel);

        CARAPI ToString(
            /* [out] */ String* str);

    public:
        ClientState* mClient;
        AutoPtr<IIInputMethod> mMethod;
        AutoPtr<IIInputMethodSession> mSession;
        AutoPtr<IInputChannel> mChannel;
    };

    class ClientState
        : public Object
    {
    public:
        ClientState(
            /* [in] */ IInputMethodClient* client,
            /* [in] */ IIInputContext* inputContext,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 pid);

        CARAPI ToString(
            /* [out] */ String* str);
    public:
        AutoPtr<IInputMethodClient> mClient;
        AutoPtr<IIInputContext> mInputContext;
        const Int32 mUid;
        const Int32 mPid;
        AutoPtr<IInputBinding> mBinding;

        Boolean mSessionRequested;
        AutoPtr<SessionState> mCurSession;
    };

    class SettingsObserver
        : public ContentObserver
    {
    public:
        SettingsObserver();

        CARAPI constructor(
            /* [in] */ IHandler* handler,
            /* [in] */ CInputMethodManagerService* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        String mLastEnabled;
        CInputMethodManagerService* mHost;
    };

    class UpdateFromSettingsLockObserver
        : public ContentObserver
    {
    public:
        UpdateFromSettingsLockObserver();

        CARAPI constructor(
            /* [in] */ IHandler* handler,
            /* [in] */ CInputMethodManagerService* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        CInputMethodManagerService* mHost;
    };

    class ImmsBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        CARAPI constructor(
            /* [in] */ CInputMethodManagerService* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CInputMethodManagerService::ImmsBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CARAPI_(void) UpdateActive();

    private:
        CInputMethodManagerService* mHost;
    };

    class MyPackageMonitor: public PackageMonitor
    {
    public:
        CARAPI constructor(
            /* [in] */ CInputMethodManagerService* host);

        //@Override
        virtual CARAPI OnHandleForceStop(
            /* [in] */ IIntent* intent,
            /* [in] */ ArrayOf<String>* packages,
            /* [in] */ Int32 uid,
            /* [in] */ Boolean doit,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI OnSomePackagesChanged();

    private:
        CARAPI_(Boolean) IsChangingPackagesOfCurrentUser();

    private:
        CInputMethodManagerService* mHost;
    };

    class HardKeyboardListener
        : public Object
        , public IOnHardKeyboardStatusChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        HardKeyboardListener(
            /* [in] */ CInputMethodManagerService* host);

        //@Override
        CARAPI OnHardKeyboardStatusChange(
            /* [in] */ Boolean available);

        CARAPI HandleHardKeyboardStatusChange(
            /* [in] */ Boolean available);

    private:
        CInputMethodManagerService* mHost;
    };

    class CheckReceiver : public BroadcastReceiver
    {
    public:
        CARAPI constructor(
            /* [in] */ CInputMethodManagerService* host);

    protected:
        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CInputMethodManagerService::CheckReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CInputMethodManagerService* mHost;
    };

    class ImeSubtypeListAdapter
        : public ArrayAdapter
    {
    public:
        ImeSubtypeListAdapter();

        ~ImeSubtypeListAdapter();

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ Int32 textViewResourceId,
            /* [in] */ IList* itemsList,
            /* [in] */ Int32 checkedItem);

        //@Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

    public:
        Int32 mCheckedItem;
    private:
        AutoPtr<ILayoutInflater> mInflater;
        Int32 mTextViewResourceId;
        AutoPtr<IList> mItemsList; //List<ImeSubtypeListItem>
    };

    // TODO: Cache the state for each user and reset when the cached user is removed.
    class InputMethodFileManager
        : public Object
    {
    public:
        InputMethodFileManager(
            /* [in] */ IHashMap* methodMap,
            /* [in] */ Int32 userId,
            /* [in] */ CInputMethodManagerService*  host);

        CARAPI_(void) AddInputMethodSubtypes(
            /* [in] */ IInputMethodInfo* imi,
            /* [in] */ ArrayOf<IInputMethodSubtype*>* additionalSubtypes);

        CARAPI_(AutoPtr<IHashMap>) GetAllAdditionalInputMethodSubtypes();

    private:
        CARAPI_(void) DeleteAllInputMethodSubtypes(
            /* [in] */ const String& imiId);

        static CARAPI_(void) WriteAdditionalInputMethodSubtypes(
            /* [in] */ IHashMap* allSubtypes,
            /* [in] */ IAtomicFile* subtypesFile,
            /* [in] */ IHashMap* methodMap);

        static CARAPI ReadAdditionalInputMethodSubtypes(
            /* [in] */ IHashMap* allSubtypes,
            /* [in] */ IAtomicFile* subtypesFile);

    private:
        static const String SYSTEM_PATH;
        static const String INPUT_METHOD_PATH;
        static const String ADDITIONAL_SUBTYPES_FILE_NAME;
        static const String NODE_SUBTYPES;
        static const String NODE_SUBTYPE;
        static const String NODE_IMI;
        static const String ATTR_ID;
        static const String ATTR_LABEL;
        static const String ATTR_ICON;
        static const String ATTR_IME_SUBTYPE_LOCALE;
        static const String ATTR_IME_SUBTYPE_MODE;
        static const String ATTR_IME_SUBTYPE_EXTRA_VALUE;
        static const String ATTR_IS_AUXILIARY;

        AutoPtr<IAtomicFile> mAdditionalInputMethodSubtypeFile;
        AutoPtr<IHashMap> mMethodMap;//HashMap<String, InputMethodInfo>
        AutoPtr<IHashMap> mAdditionalSubtypesMap; //HashMap<String, InputMethodSubtypeList>
        CInputMethodManagerService* mHost;
        friend class MyPackageMonitor;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CInputMethodManagerService();

    ~CInputMethodManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIWindowManager* windowManagerService);

    CARAPI SystemRunning(
        /* [in] */ IIStatusBarService* statusBar);

    CARAPI GetInputMethodList(
        /* [out] */ IList** infos);

    CARAPI GetEnabledInputMethodList(
        /* [out] */ IList** list);

    CARAPI GetEnabledInputMethodSubtypeList(
        /* [in] */ const String& imiId,
        /* [in] */ Boolean allowsImplicitlySelectedSubtypes,
        /* [out] */ IList** infos);

    CARAPI AddClient(
        /* [in] */ IInputMethodClient* client,
        /* [in] */ IIInputContext* inputContext,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid);

    CARAPI RemoveClient(
        /* [in] */ IInputMethodClient* client);

    /* packaged */ CARAPI ExecuteOrSendMessage(
        /* [in] */ IInterface* target,
        /* [in] */ IMessage* msg);

    /* packaged */ CARAPI_(void) UnbindCurrentClientLocked();

    /* packaged */ CARAPI AttachNewInputLocked(
        /* [in] */ Boolean initial,
        /* [out] */ IInputBindResult** result);

    /* packaged */ CARAPI StartInputLocked(
        /* [in] */ IInputMethodClient* client,
        /* [in] */ IIInputContext* inputContext,
        /* [in] */ IEditorInfo* attribute,
        /* [in] */ Int32 controlFlags,
        /* [out] */ IInputBindResult** result);

    /* packaged */ CARAPI StartInputUncheckedLocked(
        /* [in] */ ClientState* cs,
        /* [in] */ IIInputContext* inputContext,
        /* [in] */ IEditorInfo* attribute,
        /* [in] */ Int32 controlFlags,
        /* [out] */ IInputBindResult** result);

    /* packaged */ CARAPI StartInputInnerLocked(
        /* [out] */ IInputBindResult** result);

    CARAPI StartInput(
        /* [in] */ IInputMethodClient* client,
        /* [in] */ IIInputContext* inputContext,
        /* [in] */ IEditorInfo* attribute,
        /* [in] */ Int32 controlFlags,
        /* [out] */ IInputBindResult** res);

    CARAPI FinishInput(
        /* [in] */ IInputMethodClient* client);

    CARAPI OnServiceConnected(
        /* [in] */ IComponentName* name,
        /* [in] */ IBinder* service);

    CARAPI OnSessionCreated(
        /* [in] */ IIInputMethod* method,
        /* [in] */ IIInputMethodSession* session,
        /* [in] */ IInputChannel* channel);

    CARAPI UnbindCurrentMethodLocked(
        /* [in] */ Boolean reportToClient,
        /* [in] */ Boolean savePosition);

    CARAPI RequestClientSessionLocked(
        /* [in] */ ClientState* cs);

    CARAPI ClearClientSessionLocked(
        /* [in] */ ClientState* cs);

    CARAPI ClearCurMethodLocked();

    CARAPI OnServiceDisconnected(
        /* [in] */ IComponentName* name);

    CARAPI UpdateStatusIcon(
        /* [in] */ IBinder* token,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 iconId);

    // Caution! This method is called in this class. Handle multi-user carefully
    //@SuppressWarnings("deprecation")
    //@Override
    CARAPI SetImeWindowStatus(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 vis,
        /* [in] */ Int32 backDisposition);

    //@Override
    CARAPI RegisterSuggestionSpansForNotification(
        /* [in] */ ArrayOf<ISuggestionSpan*>* spans);

    //@Override
    CARAPI NotifySuggestionPicked(
        /* [in] */ ISuggestionSpan* span,
        /* [in] */ const String& originalString,
        /* [in] */ Int32 index,
        /* [out] */ Boolean* picked);

    /* packaged */ CARAPI_(void) UpdateFromSettingsLocked(
        /* [in] */ Boolean enabledMayChange);

    /* packaged */ CARAPI_(void) UpdateInputMethodsFromSettingsLocked(
        /* [in] */ Boolean enabledMayChang);

    CARAPI UpdateKeyboardFromSettingsLocked();

    /* packaged */ CARAPI SetInputMethodLocked(
        /* [in] */ const String& id,
        /* [in] */ Int32 subtypeId);

    CARAPI ShowSoftInput(
        /* [in] */ IInputMethodClient* client,
        /* [in] */ Int32 flags,
        /* [in] */ IResultReceiver* resultReceiver,
        /* [out] */ Boolean* state);

    /* packaged */ CARAPI_(Boolean) ShowCurrentInputLocked(
        /* [in] */ Int32 flags,
        /* [in] */ IResultReceiver* resultReceiver);

    CARAPI HideSoftInput(
        /* [in] */ IInputMethodClient* client,
        /* [in] */ Int32 flags,
        /* [in] */ IResultReceiver* resultReceiver,
        /* [out] */ Boolean* state);

    /* packaged */ CARAPI_(Boolean) HideCurrentInputLocked(
        /* [in] */ Int32 flags,
        /* [in] */ IResultReceiver* resultReceiver);

    CARAPI WindowGainedFocus(
        /* [in] */ IInputMethodClient* client,
        /* [in] */ IBinder* windowToken,
        /* [in] */ Int32 controlFlags,
        /* [in] */ Int32 softInputMode,
        /* [in] */ Int32 windowFlags,
        /* [in] */ IEditorInfo* attribute,
        /* [in] */ IIInputContext* inputContext,
        /* [out] */ IInputBindResult** result);

    CARAPI ShowInputMethodPickerFromClient(
        /* [in] */ IInputMethodClient* client);

    CARAPI SetInputMethod(
        /* [in] */ IBinder* token,
        /* [in] */ const String& id);

    CARAPI SetInputMethodAndSubtype(
        /* [in] */ IBinder* token,
        /* [in] */ const String& id,
        /* [in] */ IInputMethodSubtype* subtype);

    CARAPI ShowInputMethodAndSubtypeEnablerFromClient(
        /* [in] */ IInputMethodClient* client,
        /* [in] */ const String& inputMethodId);

    CARAPI SwitchToLastInputMethod(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    CARAPI SwitchToNextInputMethod(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean onlyCurrentIme,
        /* [out] */ Boolean* result);

    CARAPI ShouldOfferSwitchingToNextInputMethod(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    CARAPI GetLastInputMethodSubtype(
        /* [out] */ IInputMethodSubtype** subtype);

    CARAPI SetAdditionalInputMethodSubtypes(
        /* [in] */ const String& imiId,
        /* [in] */ ArrayOf<IInputMethodSubtype*>* subtypes);

    CARAPI GetInputMethodWindowVisibleHeight(
        /* [out] */ Int32* result);

    CARAPI NotifyUserAction(
        /* [in] */ Int32 sequenceNumber);

    CARAPI HideMySoftInput(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 flags);

    CARAPI ShowMySoftInput(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 flags);

    /* packaged */ CARAPI_(void) SetEnabledSessionInMainThread(
        /* [in] */ SessionState* session);

    /* packaged */ CARAPI_(void) BuildInputMethodListLocked(
        /* [in] */ IArrayList* list,
        /* [in] */ IHashMap* map,
        /* [in] */ Boolean resetDefaultEnabledIme);

    /* packaged */ CARAPI_(void) HideInputMethodMenu();

    /* packaged */ CARAPI_(void) HideInputMethodMenuLocked();

    CARAPI SetInputMethodEnabled(
        /* [in] */ const String& id,
        /* [in] */ Boolean enabled,
        /* [out] */ Boolean* state);

    /* packaged */ CARAPI_(Boolean) SetInputMethodEnabledLocked(
        /* [in] */ const String& id,
        /* [in] */ Boolean enabled);

    /**
     * @return Return the current subtype of this input method.
     */
    CARAPI GetCurrentInputMethodSubtype(
        /* [out] */ IInputMethodSubtype** subtype);

    // TODO: We should change the return type from List to List<Parcelable>
    CARAPI GetShortcutInputMethodsAndSubtypes(
        /* [out] */ IList** ret);

    CARAPI SetCurrentInputMethodSubtype(
        /* [in] */ IInputMethodSubtype* subtype,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);
private:
    CARAPI_(void) ResetDefaultImeLocked(
        /* [in] */ IContext* context);

    CARAPI_(void) ResetAllInternalStateLocked(
        /* [in] */ Boolean updateOnlyWhenLocaleChanged,
        /* [in] */Boolean resetDefaultEnabledIme);

    CARAPI_(void) ResetStateIfCurrentLocaleChangedLocked();

    CARAPI_(void) SwitchUserLocked(
        /* [in] */ Int32 newUserId);

    CARAPI_(void) UpdateCurrentProfileIds();

    CARAPI_(void) SetImeWindowVisibilityStatusHiddenLocked();

    CARAPI_(void) RefreshImeWindowVisibilityLocked();

    CARAPI_(void) UpdateImeWindowStatusLocked();

    // ---------------------------------------------------------------------------------------
    // Check whether or not this is a valid IPC. Assumes an IPC is valid when either
    // 1) it comes from the system process
    // 2) the calling process' user id is identical to the current user id IMMS thinks.
    CARAPI_(Boolean) CalledFromValidUser();

    /**
     * Returns true iff the caller is identified to be the current input method with the token.
     * @param token The window token given to the input method when it was started.
     * @return true if and only if non-null valid token is specified.
     */
    CARAPI_(Boolean) CalledWithValidToken(
        /* [in] */ IBinder* token);

    CARAPI_(Boolean) BindCurrentInputMethodService(
        /* [in] */ IIntent* service,
        /* [in] */ IServiceConnection* conn,
        /* [in] */ Int32 flags);

    CARAPI_(Int32) GetImeShowFlags();

    CARAPI_(Int32) GetAppShowFlags();

    CARAPI_(void) FinishSessionLocked(
        /* [in] */ SessionState* sessionState);

    CARAPI_(Boolean) NeedsToShowImeSwitchOngoingNotification();

    CARAPI_(Boolean) IsKeyguardLocked();

    CARAPI SetInputMethodWithSubtypeId(
        /* [in] */ IBinder* token,
        /* [in] */ const String& id,
        /* [in] */ Int32 subtypeId);

    CARAPI SetInputMethodWithSubtypeIdLocked(
        /* [in] */ IBinder* token,
        /* [in] */ const String& id,
        /* [in] */ Int32 subtypeId);

    CARAPI_(Boolean) ChooseNewDefaultIMELocked();

    CARAPI_(void) ShowInputMethodMenu();

    CARAPI_(void) ShowInputMethodSubtypeMenu();

    CARAPI_(void) ShowInputMethodAndSubtypeEnabler(
        /* [in] */ const String& inputMethodId);

    CARAPI_(void) ShowConfigureInputMethods();

    CARAPI_(Boolean) IsScreenLocked();

    CARAPI_(void) ShowInputMethodMenuInternal(
        /* [in] */ Boolean showSubtypes);

    CARAPI_(void) SetSelectedInputMethodAndSubtypeLocked(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ Int32 subtypeId,
        /* [in] */ Boolean setSubtypeOnly);

    CARAPI_(void) ResetSelectedInputMethodAndSubtypeLocked(
        /* [in] */ const String& newDefaultIme);

    // If there are no selected shortcuts, tries finding the most applicable ones.
    AutoPtr<IPair> /* Pair<InputMethodInfo, InputMethodSubtype> */
    FindLastResortApplicableShortcutInputMethodAndSubtypeLocked(
        /* [in] */ const String& mode);

    CARAPI_(AutoPtr<IInputMethodSubtype>) GetCurrentInputMethodSubtypeLocked();

    CARAPI_(void) AddShortcutInputMethodAndSubtypes(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IInputMethodSubtype* subtype);

    CARAPI HandleMessage(
        /* [in] */ IMessage* msg,
        /* [out] */ Boolean* result);

    AutoPtr<IInputMethodInfo> GetMethodInfoFromMethodMap(
        /* [in] */ const String& inputMethodId);

public:
    static const Boolean DEBUG;
    static const String TAG;

    static const Int32 MSG_SHOW_IM_PICKER = 1;
    static const Int32 MSG_SHOW_IM_SUBTYPE_PICKER = 2;
    static const Int32 MSG_SHOW_IM_SUBTYPE_ENABLER = 3;
    static const Int32 MSG_SHOW_IM_CONFIG = 4;

    static const Int32 MSG_UNBIND_INPUT = 1000;
    static const Int32 MSG_BIND_INPUT = 1010;
    static const Int32 MSG_SHOW_SOFT_INPUT = 1020;
    static const Int32 MSG_HIDE_SOFT_INPUT = 1030;
    static const Int32 MSG_ATTACH_TOKEN = 1040;
    static const Int32 MSG_CREATE_SESSION = 1050;

    static const Int32 MSG_START_INPUT = 2000;
    static const Int32 MSG_RESTART_INPUT = 2010;

    static const Int32 MSG_UNBIND_METHOD = 3000;
    static const Int32 MSG_BIND_METHOD = 3010;
    static const Int32 MSG_SET_ACTIVE = 3020;
    static const Int32 MSG_SET_USER_ACTION_NOTIFICATION_SEQUENCE_NUMBER = 3040;

    static const Int32 MSG_HARD_KEYBOARD_SWITCH_CHANGED = 4000;

    static const Int64 TIME_TO_RECONNECT = 3 * 1000;

    static const Int32 SECURE_SUGGESTION_SPANS_MAX_SIZE = 20;

    AutoPtr<IContext> mContext;
    AutoPtr<IResources> mRes;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IInputMethodSettings> mSettings;
    AutoPtr<SettingsObserver> mSettingsObserver;
    AutoPtr<IIWindowManager> mIWindowManager;
    AutoPtr<IHandlerCaller> mCaller;
    Boolean mHasFeature;

    AutoPtr<IInputBindResult> mNoBinding;

    // All known input methods.  mMethodMap also serves as the global
    // lock for this class.
    AutoPtr<IArrayList> mMethodList; // ArrayList<InputMethodInfo>
    AutoPtr<IHashMap> mMethodMap; //HashMap<String, IInputMethodInfo>
    Object mMethodMapLock;

    AutoPtr<IInputMethodSubtypeSwitchingController> mSwitchingController;

    // Used to bring IME service up to visible adjustment while it is being shown.
    AutoPtr<VisibleServiceConnection> mVisibleConnection;
    Boolean mVisibleBound;

    HashMap<AutoPtr<IBinder>, AutoPtr<ClientState> > mClients;

    /**
     * Set once the system is ready to run third party code.
     */
    Boolean mSystemReady;

    /**
     * Id of the currently selected input method.
     */
    String mCurMethodId;

    /**
     * The current binding sequence number, incremented every time there is
     * a new bind performed.
     */
    Int32 mCurSeq;

    /**
     * The client that is currently bound to an input method.
     */
    AutoPtr<ClientState> mCurClient;

    /**
     * The last window token that gained focus.
     */
    AutoPtr<IBinder> mCurFocusedWindow;

    /**
     * The input context last provided by the current client.
     */
    AutoPtr<IIInputContext> mCurInputContext;

    /**
     * The attributes last provided by the current client.
     */
    AutoPtr<IEditorInfo> mCurAttribute;

    /**
     * The input method ID of the input method service that we are currently
     * connected to or in the process of connecting to.
     */
    String mCurId;

    /**
     * Set to TRUE if our ServiceConnection is currently actively bound to
     * a service (whether or not we have gotten its IBinder back yet).
     */
    Boolean mHaveConnection;

    /**
     * Set if the client has asked for the input method to be shown.
     */
    Boolean mShowRequested;

    /**
     * Set if we were explicitly told to show the input method.
     */
    Boolean mShowExplicitlyRequested;

    /**
     * Set if we were forced to be shown.
     */
    Boolean mShowForced;

    /**
     * Set if we last told the input method to show itself.
     */
    Boolean mInputShown;

    /**
     * The Intent used to connect to the current input method.
     */
    AutoPtr<IIntent> mCurIntent;

    /**
     * The token we have made for the currently active input method, to
     * identify it in the future.
     */
    AutoPtr<IBinder> mCurToken;

    /**
     * If non-NULL, this is the input method service we are currently connected
     * to.
     */
    AutoPtr<IIInputMethod> mCurMethod;

    /**
     * Time that we last initiated a bind to the input method, to determine
     * if we should try to disconnect and reconnect to it.
     */
    Int64 mLastBindTime;

    /**
     * Have we called mCurMethod.bindInput()?
     */
    Boolean mBoundToMethod;

    /**
     * Currently enabled session.  Only touched by service thread, not
     * protected by a lock.
     */
    AutoPtr<SessionState> mEnabledSession;

    /**
     * True if the screen is on.  The value is TRUE initially.
     */
    Boolean mScreenOn;

    Int32 mCurUserActionNotificationSequenceNumber;

    Int32 mBackDisposition;
    Int32 mImeWindowVis;

private:
    static const Int32 NOT_A_SUBTYPE_ID;
    static String TAG_TRY_SUPPRESSING_IME_SWITCHER;

    AutoPtr<InputMethodFileManager> mFileManager;
    AutoPtr<HardKeyboardListener> mHardKeyboardListener;
    CWindowManagerService* mWindowManagerService;
    AutoPtr<IAppOpsManager> mAppOpsManager;

    // private final LruCache<SuggestionSpan, InputMethodInfo> mSecureSuggestionSpans =
    //         new LruCache<SuggestionSpan, InputMethodInfo>(SECURE_SUGGESTION_SPANS_MAX_SIZE);
    //TODO
    HashMap<AutoPtr<ISuggestionSpan>, AutoPtr<IInputMethodInfo> > mSecureSuggestionSpans;

    // Ongoing notification
    AutoPtr<INotificationManager> mNotificationManager;
    AutoPtr<IKeyguardManager> mKeyguardManager;
    CStatusBarManagerService* mStatusBar;
    AutoPtr<INotification> mImeSwitcherNotification;
    AutoPtr<IPendingIntent> mImeSwitchPendingIntent;
    Boolean mShowOngoingImeSwitcherForPhones;
    Boolean mNotificationShown;
    Boolean mImeSelectedOnBoot;

    /**
     * The current subtype of the current input method.
     */
    AutoPtr<IInputMethodSubtype> mCurrentSubtype;

    // This list contains the pairs of InputMethodInfo and InputMethodSubtype.
    //HashMap<InputMethodInfo, ArrayList<InputMethodSubtype>>
    AutoPtr<IHashMap> mShortcutInputMethodsAndSubtypes;

    // Was the keyguard locked when this client became current?
    Boolean mCurClientInKeyguard;

    AutoPtr<IAlertDialogBuilder> mDialogBuilder;
    AutoPtr<IAlertDialog> mSwitchingDialog;
    AutoPtr<IView> mSwitchingDialogTitleView;
    AutoPtr< ArrayOf<IInputMethodInfo*> > mIms;
    AutoPtr< ArrayOf<Int32> > mSubtypeIds;
    AutoPtr<ILocale> mLastSystemLocale;
    Boolean mShowImeWithHardKeyboard;
    AutoPtr<MyPackageMonitor> mMyPackageMonitor;
    AutoPtr<IIPackageManager> mIPackageManager;
    Boolean mInputBoundToKeyguard;

    friend class ScreenOnOffReceiver;
    // friend class MyPackageMonitor;
};

}// namespace Server
}// namespace Droid
}// namespace Elastos

#endif  //__ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICE_H__
