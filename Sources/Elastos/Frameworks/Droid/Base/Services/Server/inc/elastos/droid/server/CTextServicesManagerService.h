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

#ifndef __ELASTOS_DROID_SERVER_CTEXTSERVICESMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CTEXTSERVICESMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Server_CTextServicesManagerService.h"
#include "elastos/droid/internal/content/PackageMonitor.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.View.h>

using Elastos::Droid::App::IIUserSwitchObserver;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IIRemoteCallback;
using Elastos::Droid::View::TextService::ISpellCheckerInfo;
using Elastos::Droid::View::TextService::ISpellCheckerSubtype;
using Elastos::Droid::View::TextService::ISpellCheckerInfo;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Internal::TextService::IISpellCheckerService;
using Elastos::Droid::Internal::TextService::IISpellCheckerSession;
using Elastos::Droid::Internal::TextService::IISpellCheckerSessionListener;
using Elastos::Droid::Internal::TextService::IITextServicesSessionListener;
using Elastos::Droid::Internal::TextService::IITextServicesManager;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CTextServicesManagerService)
    , public Object
    , public IITextServicesManager
    , public IBinder
{
public:
    class TSMSUserSwitchObserver
        : public Object
        , public IIUserSwitchObserver
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        TSMSUserSwitchObserver();

        ~TSMSUserSwitchObserver();

        CARAPI constructor(
            /* [in] */ IITextServicesManager* tsm);

        CARAPI OnUserSwitching(
            /* [in] */ Int32 newUserId,
            /* [in] */ IIRemoteCallback* reply);

        CARAPI OnUserSwitchComplete(
            /* [in] */ Int32 newUserId);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        CTextServicesManagerService* mHost;
    };

private:
    class TextServicesMonitor;
    class InternalServiceConnection;
    class SpellCheckerBindGroup;
    class InternalDeathRecipient;
    class TextServicesSettings;

    class TextServicesMonitor
        : public PackageMonitor
    {
    public:
        TextServicesMonitor(
            /* [in] */ CTextServicesManagerService* owner);

        ~TextServicesMonitor();

        CARAPI OnSomePackagesChanged();

    private:
        CARAPI_(Boolean) IsChangingPackagesOfCurrentUser();

    private:
        CTextServicesManagerService* mHost;
    };

    class InternalServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        InternalServiceConnection(
            /* [in] */ const String& id,
            /* [in] */ const String& locale,
            /* [in] */ IBundle* bundle,
            /* [in] */ CTextServicesManagerService* owner);

        virtual ~InternalServiceConnection();

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceConnectedInnerLocked(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

        CARAPI_(String) GetSciId();

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        String mSciId;
        String mLocale;
        AutoPtr<IBundle> mBundle;
        CTextServicesManagerService* mHost;
    };

    // SpellCheckerBindGroup contains active text service session listeners.
    // If there are no listeners anymore, the SpellCheckerBindGroup instance will be removed from
    // mSpellCheckerBindGroups
    class SpellCheckerBindGroup
        : public Object
    {
    public:
        SpellCheckerBindGroup(
            /* [in] */ InternalServiceConnection* connection,
            /* [in] */ IITextServicesSessionListener* listener,
            /* [in] */ const String& locale,
            /* [in] */ IISpellCheckerSessionListener* scListener,
            /* [in] */ Int32 uid,
            /* [in] */ IBundle* bundle,
            /* [in] */ CTextServicesManagerService* owner);

        virtual ~SpellCheckerBindGroup();

        CARAPI OnServiceConnected(
            /* [in] */ IISpellCheckerService* spellChecker);

        CARAPI AddListener(
            /* [in] */ IITextServicesSessionListener* tsListener,
            /* [in] */ const String& locale,
            /* [in] */ IISpellCheckerSessionListener* scListener,
            /* [in] */ Int32 uid,
            /* [in] */ IBundle* bundle,
            /* [out] */ InternalDeathRecipient** recipient);

        CARAPI RemoveListener(
            /* [in] */ IISpellCheckerSessionListener* listener);

        CARAPI RemoveAll();

        CARAPI ToString(
            /* [out] */ String* str);

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
        CTextServicesManagerService* mHost;

    private:
        const String TAG;// = SpellCheckerBindGroup.class.getSimpleName();
    };

    class InternalDeathRecipient
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        InternalDeathRecipient(
            /* [in] */ SpellCheckerBindGroup* group,
            /* [in] */ IITextServicesSessionListener* tsListener,
            /* [in] */ const String& scLocale,
            /* [in] */ IISpellCheckerSessionListener* scListener,
            /* [in] */ Int32 uid,
            /* [in] */ IBundle* bundle,
            /* [in] */ CTextServicesManagerService* owner);

        virtual ~InternalDeathRecipient();

        CARAPI_(Boolean) HasSpellCheckerListener(
            /* [in] */ IISpellCheckerSessionListener* listener);

        CARAPI ProxyDied();

    public:
        AutoPtr<SpellCheckerBindGroup> mGroup;
        AutoPtr<IITextServicesSessionListener> mTsListener;
        String mScLocale;
        AutoPtr<IISpellCheckerSessionListener> mScListener;
        Int32 mUid;
        AutoPtr<IBundle> mBundle;

    private:
        CTextServicesManagerService* mHost;
    };

    class TextServicesSettings
        : public Object
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
        CTextServicesManagerService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

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
        /* [in] */ IITextServicesSessionListener* tsListener,
        /* [in] */ IISpellCheckerSessionListener* scListener,
        /* [in] */ IBundle* bundle);

    CARAPI FinishSpellCheckerService(
        /* [in] */ IISpellCheckerSessionListener* listener);

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

    CARAPI SystemRunning();

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
        /* [in] */ IITextServicesSessionListener* tsListener,
        /* [in] */ IISpellCheckerSessionListener* scListener,
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
