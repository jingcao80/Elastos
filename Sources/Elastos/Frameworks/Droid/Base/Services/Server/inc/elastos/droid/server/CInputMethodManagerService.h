
#ifndef __ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICE_H__
#define  __ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CInputMethodManagerService.h"
#include "wm/CWindowManagerService.h"
#include "CStatusBarManagerService.h"
#include "elastos/droid/os/HandlerCaller.h"
#include "elastos/droid/content/BroadcastReceiver.h"
// #include "elastos/droid/content/ContentResolver.h"
#include "elastos/droid/content/PackageMonitor.h"

#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/widget/ArrayAdapter.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <elastos/core/StringBuilder.h>
#include <database/ContentObserver.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Etl::List;
using Elastos::Core::IComparable;
using Elastos::Utility::IObjectStringMap;
using Elastos::Droid::App::IUserSwitchObserver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Internal::View::IInputBindResult;
using Elastos::Droid::Internal::View::IInputMethodClient;
using Elastos::Droid::Internal::View::IInputContext;
using Elastos::Droid::Internal::View::IIInputMethod;
using Elastos::Droid::Internal::View::IIInputMethodSession;
using Elastos::Droid::Os::HandlerCaller;
using Elastos::Droid::Os::IHandlerCallerCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IRemoteCallback;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Style::ISuggestionSpan;
using Elastos::Droid::View::IInputMethodCallback;
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
using Elastos::Droid::StatusBar::IIStatusBarService;
using Elastos::Droid::Server::Wm::CWindowManagerService;


namespace Elastos {
namespace Droid {
namespace Server {


typedef List< AutoPtr<IInputMethodInfo> > InputMethodInfoList;
typedef typename InputMethodInfoList::Iterator InputMethodInfoListIterator;
typedef typename InputMethodInfoList::ReverseIterator InputMethodInfoListReverseIterator;

typedef List< AutoPtr<IInputMethodSubtype> > InputMethodSubtypeList;
typedef typename InputMethodSubtypeList::Iterator InputMethodSubtypeListIterator;

typedef HashMap<AutoPtr<IInputMethodInfo>, AutoPtr< InputMethodSubtypeList > > InputMethodInfoSubtypeMap;

typedef Pair<AutoPtr<IInputMethodInfo>, AutoPtr<List<String> > > InputMethodInfoStingListPair;
typedef List< AutoPtr<InputMethodInfoStingListPair> > InputMethodInfoStingListPairList;
typedef typename InputMethodInfoStingListPairList::Iterator InputMethodInfoStingListPairIterator;

typedef Pair<String, String> StringStringPair;
typedef List< AutoPtr<StringStringPair> > StringStringPairList;
typedef typename StringStringPairList::Iterator StringStringPairListIterator;

typedef Pair<String, AutoPtr<List<String> > > StringStringListPair;
typedef List< AutoPtr<StringStringListPair> > StringStringListPairList;
typedef typename StringStringListPairList::Iterator StringStringListPairListIterator;

/**
 * This class provides a system service that manages input methods.
 */

CarClass(CInputMethodManagerService)
{
    friend class CInputMethodManagerServiceMethodCallback;
    friend class CInputMethodManagerServiceUserSwitchObserver;

private:
    class MyHandlerCallerCallback
        : public ElRefBase
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
        : public ElRefBase
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
        : public ElRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL()

        SessionState(
            /* [in] */ ClientState* client,
            /* [in] */ IIInputMethod* method,
            /* [in] */ IIInputMethodSession* session);

    public:
        ClientState* mClient;
        AutoPtr<IIInputMethod> mMethod;
        AutoPtr<IIInputMethodSession> mSession;
    };

    class ClientState
        : public ElRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL()

        ClientState(
            /* [in] */ IInputMethodClient* client,
            /* [in] */ IInputContext* inputContext,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 pid);

        ~ClientState();

    public:
        AutoPtr<IInputMethodClient> mClient;
        AutoPtr<IInputContext> mInputContext;
        const Int32 mUid;
        const Int32 mPid;
        AutoPtr<IInputBinding> mBinding;

        Boolean mSessionRequested;
        AutoPtr<SessionState> mCurSession;
    };

    class SettingsObserver : public ContentObserver
    {
    public:
        SettingsObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ CInputMethodManagerService* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        CInputMethodManagerService* mHost;
    };

    class ImmsBroadcastReceiver : public BroadcastReceiver
    {
    public:
        ImmsBroadcastReceiver(
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
        MyPackageMonitor(
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
        HardKeyboardListener(
            /* [in] */ CInputMethodManagerService* host);

        CAR_INTERFACE_DECL()

        //@Override
        CARAPI OnHardKeyboardStatusChange(
            /* [in] */ Boolean available,
            /* [in] */ Boolean enabled);

        CARAPI HandleHardKeyboardStatusChange(
            /* [in] */ Boolean available,
            /* [in] */ Boolean enabled);

    private:
        CInputMethodManagerService* mHost;
    };

    class CheckReceiver : public BroadcastReceiver
    {
    public:
        CheckReceiver(
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

    class ImeSubtypeListItem
        : public ElRefBase
        , public IComparable
    {
    public:
        ImeSubtypeListItem(
            /* [in] */ ICharSequence* imeName,
            /* [in] */ ICharSequence* subtypeName,
            /* [in] */ IInputMethodInfo* imi,
            /* [in] */ Int32 subtypeId,
            /* [in] */ const String& subtypeLocale,
            /* [in] */ const String& systemLocale);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        //@Override
        CARAPI CompareTo(
            /* [in] */ IInterface* _other,
            /* [out] */ Int32* result);

    public:
        AutoPtr<ICharSequence> mImeName;
        AutoPtr<ICharSequence> mSubtypeName;
        AutoPtr<IInputMethodInfo> mImi;
        Int32 mSubtypeId;

    private:
        Boolean mIsSystemLocale;
        Boolean mIsSystemLanguage;
    };

    class ImeSubtypeListAdapter
        : public ElRefBase
        , public ArrayAdapter
        , public IArrayAdapter
    {
    public:
        ImeSubtypeListAdapter(
            /* [in] */ IContext* context,
            /* [in] */ Int32 textViewResourceId,
            /* [in] */ List< AutoPtr<ImeSubtypeListItem> >* itemsList,
            /* [in] */ Int32 checkedItem);

        ~ImeSubtypeListAdapter();

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        //@Override
        CARAPI_(AutoPtr<IView>) GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent);

    private:
        AutoPtr<ILayoutInflater> mInflater;
        Int32 mTextViewResourceId;
        AutoPtr< List< AutoPtr<ImeSubtypeListItem> > > mItemsList;
        Int32 mCheckedItem;
    };

    class InputMethodAndSubtypeListManager : public ElRefBase
    {
    public:
        InputMethodAndSubtypeListManager(
            /* [in] */ IContext* context,
            /* [in] */ CInputMethodManagerService* imms);

        CARAPI_(AutoPtr<ImeSubtypeListItem>) GetNextInputMethod(
            /* [in] */ Boolean onlyCurrentIme,
            /* [in] */ IInputMethodInfo* imi,
            /* [in] */ IInputMethodSubtype* subtype);

        CARAPI_(AutoPtr< List< AutoPtr<ImeSubtypeListItem> > >) GetSortedInputMethodAndSubtypeList();

        CARAPI_(AutoPtr< List< AutoPtr<ImeSubtypeListItem> > >) GetSortedInputMethodAndSubtypeList(
            /* [in] */ Boolean showSubtypes,
            /* [in] */ Boolean inputShown,
            /* [in] */ Boolean isScreenLocked);

    private:
        AutoPtr<IContext> mContext;
        // Used to load label
        AutoPtr<IPackageManager> mPm;
        CInputMethodManagerService* mImms;
        String mSystemLocaleStr;

        // TreeMap<InputMethodInfo, List<InputMethodSubtype>> mSortedImmis =
        //         new TreeMap<InputMethodInfo, List<InputMethodSubtype>>(
        //                 new Comparator<InputMethodInfo>() {
        //                     @Override
        //                     public int compare(InputMethodInfo imi1, InputMethodInfo imi2) {
        //                         if (imi2 == NULL) return 0;
        //                         if (imi1 == NULL) return 1;
        //                         if (mPm == NULL) {
        //                             return imi1.getId().compareTo(imi2.getId());
        //                         }
        //                         CharSequence imiId1 = imi1.loadLabel(mPm) + "/" + imi1.getId();
        //                         CharSequence imiId2 = imi2.loadLabel(mPm) + "/" + imi2.getId();
        //                         return imiId1.toString().compareTo(imiId2.toString());
        //                     }
        //                 });
    };

    /**
     * Utility class for putting and getting settings for InputMethod
     * TODO: Move all putters and getters of settings to this class.
     */
    class InputMethodSettings : public ElRefBase
    {
    public:
        InputMethodSettings(
            /* [in] */ IResources* res,
            /* [in] */ IContentResolver* resolver,
            /* [in] */ HashMap<String, AutoPtr<IInputMethodInfo> >* methodMap,
            /* [in] */ InputMethodInfoList* methodList,
            /* [in] */ Int32 userId,
            /* [in] */ CInputMethodManagerService* host);

        ~InputMethodSettings();

        CARAPI_(void) SetCurrentUserId(
            /* [in] */ Int32 userId);

        CARAPI_(AutoPtr< InputMethodInfoList >) GetEnabledInputMethodListLocked();

        AutoPtr< InputMethodInfoStingListPairList > GetEnabledInputMethodAndSubtypeHashCodeListLocked();

        CARAPI_(AutoPtr< InputMethodSubtypeList >) GetEnabledInputMethodSubtypeListLocked(
            /* [in] */ IInputMethodInfo* imi);

        // AutoPtr<HashMap< AutoPtr<IInputMethodInfo>, InputMethodSubtypeList* > >
        // GetExplicitlyOrImplicitlyEnabledInputMethodsAndSubtypeListLocked();

        // At the initial boot, the settings for input methods are not set,
        // so we need to enable IME in that case.
        CARAPI_(void) EnableAllIMEsIfThereIsNoEnabledIME();

        CARAPI_(void) AppendAndPutEnabledInputMethodLocked(
            /* [in] */ const String& id,
            /* [in] */ Boolean reloadInputMethodStr);

        /**
         * Build and put a string of EnabledInputMethods with removing specified Id.
         * @return the specified id was removed or not.
         */
        CARAPI_(Boolean) BuildAndPutEnabledInputMethodsStrRemovingIdLocked(
            /* [in] */ StringBuilder& builder,
            /* [in] */ StringStringListPairList * imsList,
            /* [in] */ const String& id);

        CARAPI_(void) AddSubtypeToHistory(
            /* [in] */ const String& imeId,
            /* [in] */ const String& subtypeId);

        AutoPtr<StringStringPair> GetLastInputMethodAndSubtypeLocked();

        CARAPI_(String) GetLastSubtypeForInputMethodLocked(
            /* [in] */ const String& imeId);

        CARAPI_(String) GetDisabledSystemInputMethods();

        CARAPI_(void) PutSelectedSubtype(
            /* [in] */ Int32 subtypeId);

        CARAPI_(void) PutSelectedInputMethod(
            /* [in] */ const String& imeId);

        CARAPI_(String) GetSelectedInputMethod();

        CARAPI_(Int32) GetSelectedInputMethodSubtypeHashCode();

        CARAPI_(Int32) GetCurrentUserId();

    private:
        static CARAPI_(void) BuildEnabledInputMethodsSettingString(
            /* [in] */ StringBuilder& builder,
            /* [in] */ StringStringListPair* pair);

        AutoPtr< StringStringListPairList > GetEnabledInputMethodsAndSubtypeListLocked();

        CARAPI_(AutoPtr< InputMethodInfoList >) CreateEnabledInputMethodListLocked(
            /* [in] */ StringStringListPairList * imsList);

        AutoPtr< InputMethodInfoStingListPairList > CreateEnabledInputMethodAndSubtypeHashCodeListLocked(
            /* [in] */ StringStringListPairList * imsList);

        CARAPI_(void) PutEnabledInputMethodsStr(
            /* [in] */ const String& str);

        CARAPI_(String) GetEnabledInputMethodsStr();

        CARAPI_(void) SaveSubtypeHistory(
            /* [in] */ StringStringPairList* savedImes,
            /* [in] */ const String& newImeId,
            /* [in] */ const String& newSubtypeId);

        CARAPI_(void) PutSubtypeHistoryStr(
            /* [in] */ const String& str);

        AutoPtr<StringStringPair> GetLastSubtypeForInputMethodLockedInternal(
            /* [in] */ const String& imeId);

        CARAPI_(String) GetEnabledSubtypeHashCodeForInputMethodAndSubtypeLocked(
            /* [in] */ StringStringListPairList * enabledImes,
            /* [in] */ const String& imeId,
            /* [in] */ const String& subtypeHashCode);

        CARAPI_(String) GetSubtypeHistoryStr();

        AutoPtr< StringStringPairList > LoadInputMethodAndSubtypeHistoryLocked();

    private:
        // The string for enabled input method is saved as follows:
        // example: ("ime0;subtype0;subtype1;subtype2:ime1:ime2;subtype0")
        static const Char32 INPUT_METHOD_SEPARATER;
        static const Char32 INPUT_METHOD_SUBTYPE_SEPARATER;
        AutoPtr<TextUtils::SimpleStringSplitter> mInputMethodSplitter;

        AutoPtr<TextUtils::SimpleStringSplitter> mSubtypeSplitter;

        AutoPtr<IResources> mRes;
        AutoPtr<IContentResolver> mResolver;
        AutoPtr< HashMap<String, AutoPtr<IInputMethodInfo> > > mMethodMap;
        AutoPtr< InputMethodInfoList > mMethodList;

        String mEnabledInputMethodsStrCache;
        Int32 mCurrentUserId;

        CInputMethodManagerService*  mHost;
    };

    // TODO: Cache the state for each user and reset when the cached user is removed.
    class InputMethodFileManager : public ElRefBase
    {
    public:
        InputMethodFileManager(
            /* [in] */ HashMap<String, AutoPtr<IInputMethodInfo> >* methodMap,
            /* [in] */ Int32 userId,
            /* [in] */ CInputMethodManagerService*  host);

        CARAPI_(void) AddInputMethodSubtypes(
            /* [in] */ IInputMethodInfo* imi,
            /* [in] */ ArrayOf<IInputMethodSubtype*>* additionalSubtypes);

        CARAPI_(AutoPtr<IObjectStringMap>) GetAllAdditionalInputMethodSubtypes();

    private:
        CARAPI_(void) DeleteAllInputMethodSubtypes(
            /* [in] */ const String& imiId);

        static CARAPI_(void) WriteAdditionalInputMethodSubtypes(
            /* [in] */ HashMap<String, AutoPtr< InputMethodSubtypeList > >* allSubtypes,
            /* [in] */ IAtomicFile* subtypesFile,
            /* [in] */ HashMap<String, AutoPtr<IInputMethodInfo> >* methodMap);

        static CARAPI_(void) ReadAdditionalInputMethodSubtypes(
            /* [in] */ HashMap<String, AutoPtr< InputMethodSubtypeList > >* allSubtypes,
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
        AutoPtr< HashMap<String, AutoPtr<IInputMethodInfo> > > mMethodMap;
        Object mMethodMapLock;
        HashMap<String, AutoPtr< InputMethodSubtypeList > > mAdditionalSubtypesMap;
        CInputMethodManagerService* mHost;
        friend class MyPackageMonitor;
    };

public:
    CInputMethodManagerService();

    ~CInputMethodManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Handle32 windowManagerService);

    CARAPI_(void) SystemReady(
        /* [in] */ CStatusBarManagerService* statusBar);

    CARAPI GetInputMethodList(
        /* [out] */ IObjectContainer** infos);

    CARAPI GetEnabledInputMethodList(
        /* [out] */ IObjectContainer** list);

    CARAPI GetEnabledInputMethodSubtypeListLocked(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ Boolean allowsImplicitlySelectedSubtypes,
        /* [out] */ IObjectContainer** subtypes);

    CARAPI GetEnabledInputMethodSubtypeList(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ Boolean allowsImplicitlySelectedSubtypes,
        /* [out] */ IObjectContainer** infos);

    CARAPI AddClient(
        /* [in] */ IInputMethodClient* client,
        /* [in] */ IInputContext* inputContext,
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
        /* [in] */ IInputContext* inputContext,
        /* [in] */ IEditorInfo* attribute,
        /* [in] */ Int32 controlFlags,
        /* [out] */ IInputBindResult** result);

    /* packaged */ CARAPI StartInputUncheckedLocked(
        /* [in] */ ClientState* cs,
        /* [in] */ IInputContext* inputContext,
        /* [in] */ IEditorInfo* attribute,
        /* [in] */ Int32 controlFlags,
        /* [out] */ IInputBindResult** result);

    /* packaged */ CARAPI StartInputInnerLocked(
        /* [out] */ IInputBindResult** result);

    CARAPI StartInput(
        /* [in] */ IInputMethodClient* client,
        /* [in] */ IInputContext* inputContext,
        /* [in] */ IEditorInfo* attribute,
        /* [in] */ Int32 controlFlags,
        /* [out] */ IInputBindResult** res);

    CARAPI FinishInput(
        /* [in] */ IInputMethodClient* client);

    CARAPI OnServiceConnected(
        /* [in] */ IComponentName* name,
        /* [in] */ IBinder* service);

    /* packaged */ CARAPI_(void) OnSessionCreated(
        /* [in] */ IIInputMethod* method,
        /* [in] */ IIInputMethodSession* session);

    /* packaged */ CARAPI_(void) UnbindCurrentMethodLocked(
        /* [in] */ Boolean reportToClient,
        /* [in] */ Boolean savePosition);

    /* packaged */ CARAPI_(void) ClearCurMethodLocked();

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

    /* packaged */ CARAPI_(void) UpdateFromSettingsLocked();

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
        /* [in] */ IInputContext* inputContext,
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

    CARAPI GetLastInputMethodSubtype(
        /* [out] */ IInputMethodSubtype** subtype);

    CARAPI SetAdditionalInputMethodSubtypes(
        /* [in] */ const String& imiId,
        /* [in] */ ArrayOf<IInputMethodSubtype*>* subtypes);

    CARAPI HideMySoftInput(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 flags);

    CARAPI ShowMySoftInput(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 flags);

    /* packaged */ CARAPI_(void) SetEnabledSessionInMainThread(
        /* [in] */ SessionState* session);

    /* packaged */ CARAPI_(void) BuildInputMethodListLocked(
        /* [in] */ InputMethodInfoList* list,
        /* [in] */ HashMap<String, AutoPtr<IInputMethodInfo> >* map);

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
        /* [out] */ IObjectContainer** ret);

    CARAPI GetInputContext(
        /* [out] */ IInputContext** ctx);

    CARAPI SetCurrentInputMethodSubtype(
        /* [in] */ IInputMethodSubtype* subtype,
        /* [out] */ Boolean* result);

private:
    CARAPI_(void) ResetDefaultImeLocked(
        /* [in] */ IContext* context);

    CARAPI_(void) ResetAllInternalStateLocked(
        /* [in] */ Boolean updateOnlyWhenLocaleChanged);

    CARAPI_(void) CheckCurrentLocaleChangedLocked();

    CARAPI_(void) SwitchUserLocked(
        /* [in] */ Int32 newUserId);

    CARAPI_(Boolean) IsValidSystemDefaultIme(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) IsSystemImeThatHasEnglishSubtype(
        /* [in] */ IInputMethodInfo* imi);

    static CARAPI_(Boolean) ContainsSubtypeOf(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ const String& language);

    CARAPI_(void) SetImeWindowVisibilityStatusHiddenLocked();

    CARAPI_(void) RefreshImeWindowVisibilityLocked();

    CARAPI_(void) UpdateImeWindowStatusLocked();

    // ---------------------------------------------------------------------------------------
    // Check whether or not this is a valid IPC. Assumes an IPC is valid when either
    // 1) it comes from the system process
    // 2) the calling process' user id is identical to the current user id IMMS thinks.
    CARAPI_(Boolean) CalledFromValidUser();

    CARAPI_(Boolean) BindCurrentInputMethodService(
        /* [in] */ IIntent* service,
        /* [in] */ IServiceConnection* conn,
        /* [in] */ Int32 flags);

    AutoPtr< InputMethodInfoSubtypeMap > GetExplicitlyOrImplicitlyEnabledInputMethodsAndSubtypeListLocked();

    CARAPI_(Int32) GetImeShowFlags();

    CARAPI_(Int32) GetAppShowFlags();

    CARAPI_(void) FinishSession(
        /* [in] */ SessionState* sessionState);

    CARAPI_(Boolean) NeedsToShowImeSwitchOngoingNotification();

    CARAPI SetInputMethodWithSubtypeId(
        /* [in] */ IBinder* token,
        /* [in] */ const String& id,
        /* [in] */ Int32 subtypeId);

    static CARAPI_(Boolean) IsSystemIme(
        /* [in] */ IInputMethodInfo* inputMethod);

    static CARAPI_(AutoPtr< InputMethodSubtypeList >) GetSubtypes(
        /* [in] */ IInputMethodInfo* imi);

    static CARAPI_(AutoPtr< InputMethodSubtypeList >) GetOverridingImplicitlyEnabledSubtypes(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ const String& mode);

    CARAPI_(AutoPtr<IInputMethodInfo>) GetMostApplicableDefaultIMELocked();

    CARAPI_(Boolean) ChooseNewDefaultIMELocked();

    CARAPI_(void) ShowInputMethodMenu();

    CARAPI_(void) ShowInputMethodSubtypeMenu();

    CARAPI_(void) ShowInputMethodAndSubtypeEnabler(
        /* [in] */ const String& inputMethodId);

    CARAPI_(void) ShowConfigureInputMethods();

    CARAPI_(Boolean) IsScreenLocked();

    CARAPI_(void) ShowInputMethodMenuInternal(
        /* [in] */ Boolean showSubtypes);

    CARAPI_(Boolean) CanAddToLastInputMethod(
        /* [in] */ IInputMethodSubtype* subtype);

    CARAPI_(void) SaveCurrentInputMethodAndSubtypeToHistory();

    CARAPI_(void) SetSelectedInputMethodAndSubtypeLocked(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ Int32 subtypeId,
        /* [in] */ Boolean setSubtypeOnly);

    CARAPI_(void) ResetSelectedInputMethodAndSubtypeLocked(
        /* [in] */ const String& newDefaultIme);

    CARAPI_(Int32) GetSelectedInputMethodSubtypeId(
        /* [in] */ const String& id);

    static CARAPI_(Boolean) IsValidSubtypeId(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ Int32 subtypeHashCode);

    static CARAPI_(Int32) GetSubtypeIdFromHashCode(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ Int32 subtypeHashCode);

    static CARAPI_(AutoPtr< InputMethodSubtypeList >) GetImplicitlyApplicableSubtypesLocked(
        /* [in] */ IResources* res,
        /* [in] */ IInputMethodInfo* imi);

    /**
     * If there are no selected subtypes, tries finding the most applicable one according to the
     * given locale.
     * @param subtypes this function will search the most applicable subtype in subtypes
     * @param mode subtypes will be filtered by mode
     * @param locale subtypes will be filtered by locale
     * @param canIgnoreLocaleAsLastResort if this function can't find the most applicable subtype,
     * it will return the first subtype matched with mode
     * @return the most applicable subtypeId
     */
    static CARAPI_(AutoPtr<IInputMethodSubtype>) FindLastResortApplicableSubtypeLocked(
        /* [in] */ IResources* res,
        /* [in] */ InputMethodSubtypeList * subtypes,
        /* [in] */ const String& mode,
        /* [in] */ const String& locale,
        /* [in] */ Boolean canIgnoreLocaleAsLastResort);

    // If there are no selected shortcuts, tries finding the most applicable ones.
    // private Pair<InputMethodInfo, InputMethodSubtype>
    //         findLastResortApplicableShortcutInputMethodAndSubtypeLocked(String mode)

    CARAPI_(AutoPtr<IInputMethodSubtype>) GetCurrentInputMethodSubtypeLocked();

    CARAPI_(void) AddShortcutInputMethodAndSubtypes(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IInputMethodSubtype* subtype);

    CARAPI HandleMessage(
        /* [in] */ IMessage* msg,
        /* [out] */ Boolean* result);

    static CARAPI_(AutoPtr<ILocale>) GetEnglishLocale();

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

    static const Int32 MSG_HARD_KEYBOARD_SWITCH_CHANGED = 4000;

    static const Int64 TIME_TO_RECONNECT = 10 * 1000;

    static const Int32 SECURE_SUGGESTION_SPANS_MAX_SIZE = 20;

    AutoPtr<IContext> mContext;
    AutoPtr<IResources> mRes;
    AutoPtr<IHandler> mHandler;
    AutoPtr<InputMethodSettings> mSettings;
    AutoPtr<SettingsObserver> mSettingsObserver;
    AutoPtr<IIWindowManager> mIWindowManager;
    AutoPtr<HandlerCaller> mCaller;

    AutoPtr<IInputBindResult> mNoBinding;

    // All known input methods.  mMethodMap also serves as the global
    // lock for this class.
    AutoPtr< InputMethodInfoList > mMethodList;
    AutoPtr< HashMap<String, AutoPtr<IInputMethodInfo> > > mMethodMap;
    Object mMethodMapLock;

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
    AutoPtr<IInputContext> mCurInputContext;

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

    Int32 mBackDisposition;
    Int32 mImeWindowVis;

private:
    static const Int32 NOT_A_SUBTYPE_ID = -1;
    static String NOT_A_SUBTYPE_ID_STR;
    static String SUBTYPE_MODE_KEYBOARD;
    static String SUBTYPE_MODE_VOICE;
    static String TAG_TRY_SUPPRESSING_IME_SWITCHER;
    static String TAG_ENABLED_WHEN_DEFAULT_IS_NOT_ASCII_CAPABLE;
    static String TAG_ASCII_CAPABLE;
    static AutoPtr<ILocale> ENGLISH_LOCALE;

    AutoPtr<InputMethodFileManager> mFileManager;
    AutoPtr<InputMethodAndSubtypeListManager> mImListManager;
    AutoPtr<HardKeyboardListener> mHardKeyboardListener;
    CWindowManagerService* mWindowManagerService;

    // private final LruCache<SuggestionSpan, InputMethodInfo> mSecureSuggestionSpans =
    //         new LruCache<SuggestionSpan, InputMethodInfo>(SECURE_SUGGESTION_SPANS_MAX_SIZE);
    HashMap<AutoPtr<ISuggestionSpan>, AutoPtr<IInputMethodInfo> > mSecureSuggestionSpans;

    // Ongoing notification
    AutoPtr<INotificationManager> mNotificationManager;
    AutoPtr<IKeyguardManager> mKeyguardManager;
    AutoPtr<CStatusBarManagerService> mStatusBar;
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
    InputMethodInfoSubtypeMap mShortcutInputMethodsAndSubtypes;

    AutoPtr<IAlertDialogBuilder> mDialogBuilder;
    AutoPtr<IAlertDialog> mSwitchingDialog;
    AutoPtr<IView> mSwitchingDialogTitleView;
    AutoPtr< ArrayOf<IInputMethodInfo*> > mIms;
    AutoPtr< ArrayOf<Int32> > mSubtypeIds;
    AutoPtr<ILocale> mLastSystemLocale;
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
