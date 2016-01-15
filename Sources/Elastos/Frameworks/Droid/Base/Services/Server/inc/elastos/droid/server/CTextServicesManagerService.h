
#ifndef __ELASTOS_DROID_SERVER_CTEXTSERVICESMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CTEXTSERVICESMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Server_CTextServicesManagerService.h"
#include "elastos/droid/content/PackageMonitor.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Droid::App::IUserSwitchObserver;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IRemoteCallback;
using Elastos::Droid::View::Textservice::ISpellCheckerInfo;
using Elastos::Droid::View::Textservice::ISpellCheckerSubtype;
using Elastos::Droid::View::Textservice::IISpellCheckerService;
using Elastos::Droid::View::Textservice::IISpellCheckerSession;
using Elastos::Droid::View::Textservice::ITextServicesSessionListener;
using Elastos::Droid::View::Textservice::ISpellCheckerSessionListener;
using Elastos::Droid::View::Textservice::ISpellCheckerInfo;
using Elastos::Droid::Internal::Content::PackageMonitor;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CTextServicesManagerService)
{
private:
    class CTSMSUserSwitchObserver;
    class TextServicesMonitor;
    class InternalServiceConnection;
    class SpellCheckerBindGroup;
    class InternalDeathRecipient;
    class TextServicesSettings;

    class CTSMSUserSwitchObserver
        : public ElRefBase
        , public IUserSwitchObserver
    {
    public:
        CTSMSUserSwitchObserver(
            /* [in] */ CTextServicesManagerService* owner);

        ~CTSMSUserSwitchObserver();

        CAR_INTERFACE_DECL();

        CARAPI OnUserSwitching(
            /* [in] */ Int32 newUserId,
            /* [in] */ IRemoteCallback* reply);

        CARAPI OnUserSwitchComplete(
            /* [in] */ Int32 newUserId);

    private:
        CTextServicesManagerService* mOwner;
    };

    class TextServicesMonitor : public PackageMonitor
    {
    public:
        TextServicesMonitor(
            /* [in] */ CTextServicesManagerService* owner);

        ~TextServicesMonitor();

        CARAPI OnSomePackagesChanged();

    private:
        CARAPI_(Boolean) IsChangingPackagesOfCurrentUser();

    private:
        CTextServicesManagerService* mOwner;
    };

    class InternalServiceConnection
        : public ElRefBase
        , public IServiceConnection
    {
    public:
        InternalServiceConnection(
            /* [in] */ const String& id,
            /* [in] */ const String& locale,
            /* [in] */ IBundle* bundle,
            /* [in] */ CTextServicesManagerService* owner);

        virtual ~InternalServiceConnection();

        CAR_INTERFACE_DECL();

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceConnectedInnerLocked(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

        CARAPI_(String) GetSciId();

        CARAPI_(String) ToString();

    private:
        String mSciId;
        String mLocale;
        AutoPtr<IBundle> mBundle;
        CTextServicesManagerService* mOwner;
    };

    // SpellCheckerBindGroup contains active text service session listeners.
    // If there are no listeners anymore, the SpellCheckerBindGroup instance will be removed from
    // mSpellCheckerBindGroups
    class SpellCheckerBindGroup : public ElRefBase
    {
    public:
        SpellCheckerBindGroup(
            /* [in] */ InternalServiceConnection* connection,
            /* [in] */ ITextServicesSessionListener* listener,
            /* [in] */ const String& locale,
            /* [in] */ ISpellCheckerSessionListener* scListener,
            /* [in] */ Int32 uid,
            /* [in] */ IBundle* bundle,
            /* [in] */ CTextServicesManagerService* owner);

        virtual ~SpellCheckerBindGroup();

        CARAPI OnServiceConnected(
            /* [in] */ IISpellCheckerService* spellChecker);

        CARAPI AddListener(
            /* [in] */ ITextServicesSessionListener* tsListener,
            /* [in] */ const String& locale,
            /* [in] */ ISpellCheckerSessionListener* scListener,
            /* [in] */ Int32 uid,
            /* [in] */ IBundle* bundle,
            /* [out] */ InternalDeathRecipient** recipient);

        CARAPI RemoveListener(
            /* [in] */ ISpellCheckerSessionListener* listener);

        CARAPI RemoveAll();

        CARAPI_(String) ToString();

    private:
        CARAPI CleanLocked();

    public:
        const AutoPtr<InternalServiceConnection> mInternalConnection;
        Boolean mBound;
        Boolean mConnected;
        AutoPtr<IISpellCheckerService> mSpellChecker;
        //const CopyOnWriteArrayList<InternalDeathRecipient> mListeners; //=new CopyOnWriteArrayList<InternalDeathRecipient>();
        /*const*/ List<AutoPtr<InternalDeathRecipient> > mListeners;
        typedef List<AutoPtr<InternalDeathRecipient> >::Iterator ManagedInternalDeathRecipientListIt;
        CTextServicesManagerService* mOwner;

    private:
        const String TAG;// = SpellCheckerBindGroup.class.getSimpleName();
    };

    class InternalDeathRecipient
        : public ElRefBase
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        InternalDeathRecipient(
            /* [in] */ SpellCheckerBindGroup* group,
            /* [in] */ ITextServicesSessionListener* tsListener,
            /* [in] */ const String& scLocale,
            /* [in] */ ISpellCheckerSessionListener* scListener,
            /* [in] */ Int32 uid,
            /* [in] */ IBundle* bundle,
            /* [in] */ CTextServicesManagerService* owner);

        virtual ~InternalDeathRecipient();

        CARAPI_(Boolean) HasSpellCheckerListener(
            /* [in] */ ISpellCheckerSessionListener* listener);

        CARAPI ProxyDied();

    public:
        AutoPtr<SpellCheckerBindGroup> mGroup;
        AutoPtr<ITextServicesSessionListener> mTsListener;
        String mScLocale;
        AutoPtr<ISpellCheckerSessionListener> mScListener;
        Int32 mUid;
        AutoPtr<IBundle> mBundle;

    private:
        CTextServicesManagerService* mOwner;
    };

    class TextServicesSettings : public ElRefBase
    {
    public:
        TextServicesSettings(
            /* [in] */ IContentResolver* resolver,
            /* [in] */ Int32 userId,
            /* [in] */ CTextServicesManagerService* owner);

        virtual ~TextServicesSettings();

        CARAPI SetCurrentUserId(
            /* [in] */ Int32 userId);

        CARAPI GetCurrentUserId(
            /* [out] */ Int32* id);

        CARAPI PutSelectedSpellChecker(
            /* [in] */ const String& sciId);

        CARAPI PutSelectedSpellCheckerSubtype(
            /* [in] */ Int32 hashCode);

        CARAPI SetSpellCheckerEnabled(
            /* [in] */ Boolean enabled);

        CARAPI GetSelectedSpellChecker(
            /* [out] */ String* spellChecker);

        CARAPI GetSelectedSpellCheckerSubtype(
            /* [out] */ String* subtype);

        CARAPI IsSpellCheckerEnabled(
            /* [out] */ Boolean* enabled);

    private:
        AutoPtr<IContentResolver> mResolver;
        Int32 mCurrentUserId;
        CTextServicesManagerService* mOwner;
    };

public:
    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI GetCurrentSpellChecker(
        /* [in] */ const String& locale,
        /* [out] */ ISpellCheckerInfo** info);

    CARAPI GetCurrentSpellCheckerSubtype(
        /* [in] */ const String& locale,
        /* [in] */ Boolean allowImplicitlySelectedSubtype,
        /* [out] */ ISpellCheckerSubtype** subtype);

    CARAPI GetSpellCheckerService(
        /* [in] */ const String& sciId,
        /* [in] */ const String& locale,
        /* [in] */ ITextServicesSessionListener* tsListener,
        /* [in] */ ISpellCheckerSessionListener* scListener,
        /* [in] */ IBundle* bundle);

    CARAPI FinishSpellCheckerService(
        /* [in] */ ISpellCheckerSessionListener* listener);

    CARAPI SetCurrentSpellChecker(
        /* [in] */ const String& locale,
        /* [in] */ const String& sciId);

    CARAPI SetCurrentSpellCheckerSubtype(
        /* [in] */ const String& locale,
        /* [in] */ Int32 hashCode);

    CARAPI SetSpellCheckerEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsSpellCheckerEnabled(
        /* [out] */ Boolean* ebl);

    CARAPI GetEnabledSpellCheckers(
        /* [out, callee] */ ArrayOf<ISpellCheckerInfo*>** infoArray);

    CARAPI SystemReady();

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI SwitchUserLocked(
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) CalledFromValidUser();

    CARAPI BindCurrentSpellCheckerService(
                /* [in] */ IIntent* service,
                /* [in] */ IServiceConnection* conn,
                /* [in] */ Int32 flags,
                /* [out] */ Boolean* result);

    CARAPI UnbindServiceLocked();

    CARAPI FindAvailSpellCheckerLocked(
        /* [in] */ const String& locale,
        /* [in] */ const String& prefPackage,
        /* [out] */ ISpellCheckerInfo** info);

    CARAPI StartSpellCheckerServiceInnerLocked(
        /* [in] */ ISpellCheckerInfo* info,
        /* [in] */ const String& locale,
        /* [in] */ ITextServicesSessionListener* tsListener,
        /* [in] */ ISpellCheckerSessionListener* scListener,
        /* [in] */ Int32 uid,
        /* [in] */ IBundle* bundle);

    CARAPI SetCurrentSpellCheckerLocked(
        /* [in] */ const String& sciId);

    CARAPI SetCurrentSpellCheckerSubtypeLocked(
        /* [in] */ Int32 hashCode);

    CARAPI SetSpellCheckerEnabledLocked(
        /* [in] */ Boolean enabled);

    CARAPI IsSpellCheckerEnabledLocked(
        /* [out] */ Boolean* result);

    static CARAPI BuildSpellCheckerMapLocked(
        /* [in] */ IContext* context,
        /* [in] */ List<AutoPtr<ISpellCheckerInfo> >& list,
        /* [in] */ HashMap<String, AutoPtr<ISpellCheckerInfo> >& map,
        /* [in] */ TextServicesSettings* settings);
    // ----------------------------------------------------------------------
    // Utilities for debug
    static CARAPI GetStackTrace(
        /* [out] */ String* trace);

private:
    static const String TAG; //= TextServicesManagerService.class.getSimpleName();
    static const Boolean DBG; //= false;

    AutoPtr<IContext> mContext;
    Boolean mSystemReady;
    AutoPtr<TextServicesMonitor> mMonitor;
    HashMap<String, AutoPtr<ISpellCheckerInfo> > mSpellCheckerMap;// = new HashMap<String, SpellCheckerInfo>();
    typedef HashMap<String, AutoPtr<ISpellCheckerInfo> >::Iterator ManagedISpellCheckerInfoMapIt;
    List<AutoPtr<ISpellCheckerInfo> > mSpellCheckerList;// = new ArrayList<SpellCheckerInfo>();
    typedef List<AutoPtr<ISpellCheckerInfo> >::Iterator ManagedISpellCheckerInfoListIt;
    HashMap<String, AutoPtr<SpellCheckerBindGroup> > mSpellCheckerBindGroups; //= new HashMap<String, SpellCheckerBindGroup>();
    typedef HashMap<String, AutoPtr<SpellCheckerBindGroup> >::Iterator ManagedSpellCheckerBindGroupMapIt;
    AutoPtr<TextServicesSettings> mSettings;
    Object mSpellCheckerMapLock;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_CTEXTSERVICESMANAGERSERVICE_H__
