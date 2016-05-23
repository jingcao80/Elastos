
#include "elastos/droid/server/tv/TvInputManagerService.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Utility.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/Manifest.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/Build.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/droid/net/ReturnOutValue.h>
#include <Elastos.Droid.View.h>
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/droid/server/IoThread.h>
#include <Elastos.Droid.App.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::View::IInputChannelHelper;
using Elastos::Droid::View::CInputChannelHelper;
using Elastos::Core::CString;
using Elastos::Droid::Server::IoThread;
using Elastos::Droid::Content::IContentProviderOperationHelper;
using Elastos::Droid::Content::CContentProviderOperationHelper;
using Elastos::Droid::Content::IContentProviderOperationBuilder;
using Elastos::Droid::Content::IContentProviderResult;
using Elastos::Core::CInteger32;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::IO::IWriter;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::ISomeArgsHelper;
using Elastos::Droid::Internal::Os::CSomeArgsHelper;
using Elastos::Droid::Media::Tv::ITvContentRatingHelper;
using Elastos::Droid::Media::Tv::CTvContentRatingHelper;
using Elastos::Droid::Media::Tv::ITvContractBaseTvColumns;
using Elastos::Droid::Media::Tv::ITvContractWatchedPrograms;
using Elastos::Droid::Media::Tv::ITvContractChannels;
using Elastos::Droid::Media::Tv::ITvContract;
using Elastos::Droid::Media::Tv::ITvInputService;
using Elastos::Droid::Media::Tv::ITvInputInfoHelper;
using Elastos::Droid::Media::Tv::CTvInputInfoHelper;
using Elastos::Droid::Media::Tv::ITvContentRatingSystemInfo;
using Elastos::Droid::Media::Tv::ITvContentRatingSystemInfoHelper;
using Elastos::Droid::Media::Tv::CTvContentRatingSystemInfoHelper;
using Elastos::Droid::Media::Tv::CTvContract;
using Elastos::Droid::Media::Tv::CTvContractWatchedPrograms;
using Elastos::Core::CObject;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Internal::Content::IPackageMonitor;
using Elastos::Droid::Internal::Utility::CIndentingPrintWriter;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Manifest;
using Elastos::Droid::Media::Tv::EIID_IITvInputManager;
using Elastos::Droid::Media::Tv::EIID_IITvInputServiceCallback;
using Elastos::Droid::Media::Tv::ITvContentRating;
using Elastos::Droid::Media::Tv::ITvInputManager;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::IO::IFile;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Tv {

//=============================================================================
// TvInputManagerService::BinderService
//=============================================================================
CAR_INTERFACE_IMPL_2(TvInputManagerService::BinderService, Object, IITvInputManager, IBinder)

TvInputManagerService::BinderService::BinderService(
    /* [in] */ TvInputManagerService* host)
    : mHost(host)
{}

ECode TvInputManagerService::BinderService::GetTvInputList(
    /* [in] */ Int32 userId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(),
            Binder::GetCallingUid(), userId, String("getTvInputList"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        do {
            AutoPtr<UserState> userState;
            ec = mHost->GetUserStateLocked(resolvedUserId, (UserState**)&userState);
            if (FAILED(ec)) break;

            AutoPtr<IList> inputList;
            CArrayList::New((IList**)&inputList);
            FOR_EACH(iter, Ptr(userState->mInputMap)->Func(IMap::GetValues)) {
                AutoPtr<TvInputState> state = (TvInputState*) IObject::Probe(Ptr(iter)->Func(iter->GetNext));
                inputList->Add(TO_IINTERFACE(state->mInfo));
            }
            FUNC_RETURN(inputList);
        } while(FALSE);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::GetTvInputInfo(
    /* [in] */ const String& inputId,
    /* [in] */ Int32 userId,
    /* [out] */ ITvInputInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(),
            Binder::GetCallingUid(), userId, String("getTvInputInfo"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        do {
            AutoPtr<UserState> userState;
            ec = mHost->GetUserStateLocked(resolvedUserId, (UserState**)&userState);
            if (FAILED(ec)) break;
            AutoPtr<IInterface> obj;
            ec = userState->mInputMap->Get(StringUtils::ParseCharSequence(inputId), (IInterface**)&obj);
            AutoPtr<TvInputState> state = (TvInputState*) IObject::Probe(obj);
            if (FAILED(ec)) break;
            FUNC_RETURN(state == NULL ? NULL : state->mInfo)
        } while(FALSE);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::GetTvContentRatingSystemList(
    /* [in] */ Int32 userId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(),
            Binder::GetCallingUid(), userId, String("getTvContentRatingSystemList"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        do {
            AutoPtr<UserState> userState;
            ec = mHost->GetUserStateLocked(resolvedUserId, (UserState**)&userState);
            if (FAILED(ec)) break;
            FUNC_RETURN(userState->mContentRatingSystemList)
        } while(FALSE);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::RegisterCallback(
    /* [in] */ IITvInputManagerCallback* callback,
    /* [in] */ Int32 userId)
{
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(),
            Binder::GetCallingUid(), userId, String("registerCallback"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        do {
            AutoPtr<UserState> userState;
            ec = mHost->GetUserStateLocked(resolvedUserId, (UserState**)&userState);
            if (FAILED(ec)) break;
            userState->mCallbackSet->Add(callback);
            // try {
            AutoPtr<IProxy> proxy = (IProxy*)callback->Probe(EIID_IProxy);
            if (proxy != NULL) ec = proxy->LinkToDeath(new InnerSub_ProxyDeathRecipient(callback, mHost->mLock, userState), 0);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode)E_REMOTE_EXCEPTION == ec) {
                    Slogger::E(TAG, "client process has already died %d", ec);
                    ec = NOERROR;
                }
                else
                    break;
            }
            // }
            FOR_EACH(iter, Ptr(userState->mInputMap)->Func(IMap::GetValues)) {
                AutoPtr<TvInputState> state = (TvInputState*) IObject::Probe(Ptr(iter)->Func(iter->GetNext));
                ec = mHost->NotifyInputStateChangedLocked(userState, Ptr(state->mInfo)->Func(ITvInputInfo::GetId), state->mState,
                        callback);
                if (FAILED(ec)) break;
            }
        } while(FALSE);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::UnregisterCallback(
    /* [in] */ IITvInputManagerCallback* callback,
    /* [in] */ Int32 userId)
{
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(),
            Binder::GetCallingUid(), userId, String("unregisterCallback"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        do {
            AutoPtr<UserState> userState;
            ec = mHost->GetUserStateLocked(resolvedUserId, (UserState**)&userState);
            if (FAILED(ec)) break;
            userState->mCallbackSet->Remove(callback);
        } while(FALSE);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::IsParentalControlsEnabled(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(),
            Binder::GetCallingUid(), userId, String("isParentalControlsEnabled"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        do {
            AutoPtr<UserState> userState;
            ec = mHost->GetUserStateLocked(resolvedUserId, (UserState**)&userState);
            if (FAILED(ec)) break;
            return userState->mPersistentDataStore->IsParentalControlsEnabled(result);
        } while(FALSE);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::SetParentalControlsEnabled(
    /* [in] */ Boolean enabled,
    /* [in] */ Int32 userId)
{
    EnsureParentalControlsPermission();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(),
            Binder::GetCallingUid(), userId, String("setParentalControlsEnabled"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        do {
            AutoPtr<UserState> userState;
            ec = mHost->GetUserStateLocked(resolvedUserId, (UserState**)&userState);
            if (FAILED(ec)) break;
            userState->mPersistentDataStore->SetParentalControlsEnabled(enabled);
        } while(FALSE);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::IsRatingBlocked(
    /* [in] */ const String& rating,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(),
            Binder::GetCallingUid(), userId, String("isRatingBlocked"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        do {
            AutoPtr<UserState> userState;
            ec = mHost->GetUserStateLocked(resolvedUserId, (UserState**)&userState);
            if (FAILED(ec)) break;
            AutoPtr<ITvContentRatingHelper> helper;
            CTvContentRatingHelper::AcquireSingleton((ITvContentRatingHelper**)&helper);
            AutoPtr<ITvContentRating> unflatten;
            helper->UnflattenFromString(rating, (ITvContentRating**)&unflatten);
            return userState->mPersistentDataStore->IsRatingBlocked(
                    unflatten, result);
        } while(FALSE);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::GetBlockedRatings(
    /* [in] */ Int32 userId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(),
            Binder::GetCallingUid(), userId, String("getBlockedRatings"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        do {
            AutoPtr<UserState> userState;
            ec = mHost->GetUserStateLocked(resolvedUserId, (UserState**)&userState);
            if (FAILED(ec)) break;
            AutoPtr<IList> ratings;
            CArrayList::New((IList**)&ratings);
            AutoPtr<ArrayOf<ITvContentRating*> > blockedRatings;
            ec = userState->mPersistentDataStore->GetBlockedRatings((ArrayOf<ITvContentRating*>**)&blockedRatings);
            if (FAILED(ec)) break;
            for (Int32 i = 0; i < blockedRatings->GetLength(); ++i) {
                AutoPtr<ITvContentRating> rating = (*blockedRatings)[i];
                ratings->Add(StringUtils::ParseCharSequence(Ptr(rating)->Func(rating->FlattenToString)));
            }
            FUNC_RETURN(ratings)
        } while(FALSE);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::AddBlockedRating(
    /* [in] */ const String& rating,
    /* [in] */ Int32 userId)
{
    EnsureParentalControlsPermission();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(),
            Binder::GetCallingUid(), userId, String("addBlockedRating"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        do {
            AutoPtr<UserState> userState;
            ec = mHost->GetUserStateLocked(resolvedUserId, (UserState**)&userState);
            if (FAILED(ec)) break;
            AutoPtr<ITvContentRatingHelper> helper;
            CTvContentRatingHelper::AcquireSingleton((ITvContentRatingHelper**)&helper);
            AutoPtr<ITvContentRating> unflatten;
            ec = helper->UnflattenFromString(rating, (ITvContentRating**)&unflatten);
            if (FAILED(ec)) break;
            ec = userState->mPersistentDataStore->AddBlockedRating(unflatten);
        } while(FALSE);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::RemoveBlockedRating(
    /* [in] */ const String& rating,
    /* [in] */ Int32 userId)
{
    EnsureParentalControlsPermission();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(),
            Binder::GetCallingUid(), userId, String("removeBlockedRating"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        do {
            AutoPtr<UserState> userState;
            ec = mHost->GetUserStateLocked(resolvedUserId, (UserState**)&userState);
            if (FAILED(ec)) break;
            AutoPtr<ITvContentRatingHelper> helper;
            CTvContentRatingHelper::AcquireSingleton((ITvContentRatingHelper**)&helper);
            AutoPtr<ITvContentRating> unflatten;
            ec = helper->UnflattenFromString(rating, (ITvContentRating**)&unflatten);
            if (FAILED(ec)) break;
            ec = userState->mPersistentDataStore->RemoveBlockedRating(unflatten);
        } while(FALSE);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::EnsureParentalControlsPermission()
{
    Int32 permissionCode;
    mHost->mContext->CheckCallingPermission(
            Manifest::permission::MODIFY_PARENTAL_CONTROLS, &permissionCode);
    if (permissionCode != IPackageManager::PERMISSION_GRANTED) {
        Logger::E(TAG, "The caller does not have parental controls permission");
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode TvInputManagerService::BinderService::CreateSession(
    /* [in] */ IITvInputClient* client,
    /* [in] */ const String& inputId,
    /* [in] */ Int32 seq,
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("createSession"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        {    AutoLock syncLock(mHost->mLock);
            AutoPtr<UserState> userState;
            ec = mHost->GetUserStateLocked(resolvedUserId, (UserState**)&userState);
            if (FAILED(ec)) break;
            AutoPtr<IInterface> obj;
            userState->mInputMap->Get(StringUtils::ParseCharSequence(inputId), (IInterface**)&obj);
            AutoPtr<TvInputState> inputState = (TvInputState*) IObject::Probe(obj);
            if (inputState == NULL) {
                Slogger::W(TAG, "Failed to find input state for inputId=%s", inputId.string());
                ec = mHost->SendSessionTokenToClientLocked(client, inputId, NULL, NULL, seq);
                if (FAILED(ec)) break;
                return NOERROR;
            }
            AutoPtr<ITvInputInfo> info = inputState->mInfo;
            obj = NULL;
            userState->mServiceStateMap->Get(Ptr(info)->Func(info->GetComponent), (IInterface**)&obj);
            AutoPtr<ServiceState> serviceState = (ServiceState*)IObject::Probe(obj);
            if (serviceState == NULL) {
                serviceState = new ServiceState(mHost, Ptr(info)->Func(info->GetComponent), resolvedUserId);
                userState->mServiceStateMap->Put(Ptr(info)->Func(info->GetComponent), TO_IINTERFACE(serviceState));
            }
            // Send a null token immediately while reconnecting.
            if (serviceState->mReconnecting == TRUE) {
                ec = mHost->SendSessionTokenToClientLocked(client, inputId, NULL, NULL, seq);
                if (FAILED(ec)) break;
                return NOERROR;
            }
            // Create a new session token and a session state.
            AutoPtr<IBinder> sessionToken;
            CBinder::New((IBinder**)&sessionToken);
            AutoPtr<SessionState> sessionState = new SessionState(mHost, sessionToken, info, client,
                    seq, callingUid, resolvedUserId);
            // Add them to the global session state map of the current user.
            userState->mSessionStateMap->Put(sessionToken, TO_IINTERFACE(sessionState));
            // Also, add them to the session state map of the current service.
            serviceState->mSessionTokens->Add(sessionToken);
            if (serviceState->mService != NULL) {
                ec = mHost->CreateSessionInternalLocked(serviceState->mService, sessionToken,
                        resolvedUserId);
                if (FAILED(ec)) break;
            } else {
                ec = mHost->UpdateServiceConnectionLocked(Ptr(info)->Func(info->GetComponent), resolvedUserId);
                if (FAILED(ec)) break;
            }
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::ReleaseSession(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ Int32 userId)
{
    if (DEBUG) {
        Slogger::D(TAG, "releaseSession(sessionToken=%s)", TO_CSTR(sessionToken));
    }
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("releaseSession"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        ec = mHost->ReleaseSessionLocked(sessionToken, callingUid, resolvedUserId);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::SetMainSession(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ Int32 userId)
{
    if (DEBUG) {
        Slogger::D(TAG, "setMainSession(sessionToken=%s)", TO_CSTR(sessionToken));
    }
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("setMainSession"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        do {
            AutoPtr<UserState> userState;
            ec = mHost->GetUserStateLocked(resolvedUserId, (UserState**)&userState);
            if (FAILED(ec)) break;
            if (userState->mMainSessionToken.Get() == sessionToken) {
                return NOERROR;
            }
            if (DEBUG) {
                Slogger::D(TAG, "mainSessionToken=%s", TO_CSTR(sessionToken));
            }
            AutoPtr<IBinder> oldMainSessionToken = userState->mMainSessionToken;
            userState->mMainSessionToken = sessionToken;
            // Inform the new main session first.
            // See {@link TvInputService.Session#onSetMain}.
            if (sessionToken != NULL) {
                ec = mHost->SetMainLocked(sessionToken, TRUE, callingUid, userId);
                if (FAILED(ec)) break;
            }
            if (oldMainSessionToken != NULL) {
                ec = mHost->SetMainLocked(oldMainSessionToken, FALSE, IProcess::SYSTEM_UID, userId);
                if (FAILED(ec)) break;
            }
        } while(FALSE);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::SetSurface(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("setSurface"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        do {
            // try {
            do {
                AutoPtr<SessionState> sessionState;
                ec = mHost->GetSessionStateLocked(sessionToken, callingUid,
                        resolvedUserId, (SessionState**)&sessionState);
                if (FAILED(ec)) break;
                if (sessionState->mHardwareSessionToken == NULL) {
                    AutoPtr<IITvInputSession> tvInputSession;
                    ec = mHost->GetSessionLocked(sessionState, (IITvInputSession**)&tvInputSession);
                    if (FAILED(ec)) break;
                    ec = tvInputSession->SetSurface(surface);
                    if (FAILED(ec)) break;
                } else {
                    AutoPtr<IITvInputSession> tvInputSession;
                    ec = mHost->GetSessionLocked(sessionState->mHardwareSessionToken,
                            IProcess::SYSTEM_UID, resolvedUserId, (IITvInputSession**)&tvInputSession);
                    if (FAILED(ec)) break;
                    ec = tvInputSession->SetSurface(surface);
                    if (FAILED(ec)) break;
                }
            } while(FALSE);
            // } catch (RemoteException e) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in setSurface %d", ec);
                ec = NOERROR;
            }
            // }
        } while(FALSE);
    }
    // } finally {
    if (surface != NULL) {
        // surface is not used in TvInputManagerService.
        surface->Release();
    }
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::DispatchSurfaceChanged(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("dispatchSurfaceChanged"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        {    AutoLock syncLock(mHost->mLock);
            // try {
            do {
                AutoPtr<SessionState> sessionState;
                ec = mHost->GetSessionStateLocked(sessionToken, callingUid,
                        resolvedUserId, (SessionState**)&sessionState);
                if (FAILED(ec)) break;
                AutoPtr<IITvInputSession> session;
                ec = mHost->GetSessionLocked(sessionState, (IITvInputSession**)&session);
                if (FAILED(ec)) break;
                ec = session->DispatchSurfaceChanged(format, width,
                        height);
                if (FAILED(ec)) break;
                if (sessionState->mHardwareSessionToken != NULL) {
                    AutoPtr<IITvInputSession> tvInputSession;
                    ec = mHost->GetSessionLocked(sessionState->mHardwareSessionToken, IProcess::SYSTEM_UID,
                            resolvedUserId, (IITvInputSession**)&tvInputSession);
                    if (FAILED(ec)) break;
                    ec = tvInputSession->DispatchSurfaceChanged(format, width, height);
                }
            } while(FALSE);
            // } catch (RemoteException e) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in dispatchSurfaceChanged %d", ec);
                ec = NOERROR;
            }
            // }
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::SetVolume(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ Float volume,
    /* [in] */ Int32 userId)
{
    Float REMOTE_VOLUME_ON = 1.0f;
    Float REMOTE_VOLUME_OFF = 0.0f;
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("setVolume"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        {    AutoLock syncLock(mHost->mLock);
            // try {
            do {
                AutoPtr<SessionState> sessionState;
                ec = mHost->GetSessionStateLocked(sessionToken, callingUid,
                        resolvedUserId, (SessionState**)&sessionState);
                if (FAILED(ec)) break;
                AutoPtr<IITvInputSession> session;
                ec = mHost->GetSessionLocked(sessionState, (IITvInputSession**)&session);
                if (FAILED(ec)) break;
                ec = session->SetVolume(volume);
                if (FAILED(ec)) break;
                if (sessionState->mHardwareSessionToken != NULL) {
                    // Here, we let the hardware session know only whether volume is on or
                    // off to prevent that the volume is controlled in the both side.
                    AutoPtr<IITvInputSession> tvInputSession;
                    ec = mHost->GetSessionLocked(sessionState->mHardwareSessionToken,
                            IProcess::SYSTEM_UID, resolvedUserId, (IITvInputSession**)&tvInputSession);
                    if (FAILED(ec)) break;
                    ec = tvInputSession->SetVolume((volume > 0.0f)
                            ? REMOTE_VOLUME_ON : REMOTE_VOLUME_OFF);
                }
            } while(FALSE);
            // } catch (RemoteException e) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in setVolume ec", ec);
                ec = NOERROR;
            }
            // }
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::Tune(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ IUri* channelUri,
    /* [in] */ IBundle* params,
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("tune"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        {    AutoLock syncLock(mHost->mLock);
            // try {
            do {
                AutoPtr<IITvInputSession> session;
                ec = mHost->GetSessionLocked(sessionToken, callingUid, resolvedUserId, (IITvInputSession**)&session);
                if (FAILED(ec)) break;
                ec = session->Tune(channelUri, params);
                if (FAILED(ec)) break;
                AutoPtr<ITvContract> helper;
                CTvContract::AcquireSingleton((ITvContract**)&helper);
                Boolean isInput;
                helper->IsChannelUriForPassthroughInput(channelUri, &isInput);
                if (isInput) {
                    // Do not log the watch history for passthrough inputs.
                    return NOERROR;
                }
                AutoPtr<UserState> userState;
                ec = mHost->GetUserStateLocked(resolvedUserId, (UserState**)&userState);
                if (FAILED(ec)) break;
                AutoPtr<IInterface> obj;
                ec = userState->mSessionStateMap->Get(sessionToken, (IInterface**)&obj);
                if (FAILED(ec)) break;
                AutoPtr<SessionState> sessionState = (SessionState*) IObject::Probe(obj);
                // Log the start of watch.
                AutoPtr<ISomeArgsHelper> args_helper;
                AutoPtr<ISomeArgs> args;
                args_helper->Obtain((ISomeArgs**)&args);
                args->SetObjectArg(1, StringUtils::ParseCharSequence(Ptr(sessionState->mInfo)->GetPtr(ITvInputInfo::GetComponent)->Func(IComponentName::GetPackageName)));
                AutoPtr<ISystem> system_helper;
                CSystem::AcquireSingleton((ISystem**)&system_helper);
                args->SetInt32Arg(2, (Int32)Ptr(system_helper)->Func(ISystem::GetCurrentTimeMillis));
                AutoPtr<IContentUris> uris_helper;
                CContentUris::AcquireSingleton((IContentUris**)&uris_helper);
                Int64 i64;
                ec = uris_helper->ParseId(channelUri, &i64);
                if (FAILED(ec)) break;
                args->SetInt32Arg(3, i64);
                args->SetObjectArg(4, params);
                args->SetObjectArg(5, sessionToken);
                AutoPtr<IMessage> msg;
                ec = mHost->mWatchLogHandler->ObtainMessage(WatchLogHandler::MSG_LOG_WATCH_START, args, (IMessage**)&msg);
                if (FAILED(ec)) break;
                ec = msg->SendToTarget();
            } while(FALSE);
            // } catch (RemoteException e) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in tune %d", ec);
                return NOERROR;
            }
            // }
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::RequestUnblockContent(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ const String& unblockedRating,
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("unblockContent"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        {    AutoLock syncLock(mHost->mLock);
            // try {
            AutoPtr<IITvInputSession> tvInputSession;
            ec = mHost->GetSessionLocked(sessionToken, callingUid, resolvedUserId, (IITvInputSession**)&tvInputSession);
            if (FAILED(ec)) break;
            ec = tvInputSession->RequestUnblockContent(unblockedRating);
            // } catch (RemoteException e) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in requestUnblockContent %d", ec);
                ec = NOERROR;
            }
            // }
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::SetCaptionEnabled(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ Boolean enabled,
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("setCaptionEnabled"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        {    AutoLock syncLock(mHost->mLock);
            // try {
            do {
                AutoPtr<IITvInputSession> tvInputSession;
                ec = mHost->GetSessionLocked(sessionToken, callingUid, resolvedUserId, (IITvInputSession**)&tvInputSession);
                if (FAILED(ec)) break;
                ec = tvInputSession->SetCaptionEnabled(enabled);
            } while(FALSE);
            // } catch (RemoteException e) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in setCaptionEnabled %d", ec);
                ec = NOERROR;
            }
            // }
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::SelectTrack(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ Int32 type,
    /* [in] */ const String& trackId,
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("selectTrack"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        {    AutoLock syncLock(mHost->mLock);
            // try {
            do {
                AutoPtr<IITvInputSession> tvInputSession;
                ec = mHost->GetSessionLocked(sessionToken, callingUid, resolvedUserId, (IITvInputSession**)&tvInputSession);
                if (FAILED(ec)) break;
                ec = tvInputSession->SelectTrack(type, trackId);
            } while(FALSE);
            // } catch (RemoteException e) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in selectTrack %d", ec);
                ec = NOERROR;
            }
            // }
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::SendAppPrivateCommand(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ const String& command,
    /* [in] */ IBundle* data,
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("sendAppPrivateCommand"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        {    AutoLock syncLock(mHost->mLock);
            // try {
            do {
                AutoPtr<IITvInputSession> tvInputSession;
                ec = mHost->GetSessionLocked(sessionToken, callingUid, resolvedUserId, (IITvInputSession**)&tvInputSession);
                if (FAILED(ec)) break;
                ec = tvInputSession->AppPrivateCommand(command, data);
            } while(FALSE);
            // } catch (RemoteException e) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in appPrivateCommand %d", ec);
                ec = NOERROR;
            }
            // }
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::CreateOverlayView(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ IBinder* windowToken,
    /* [in] */ IRect* frame,
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("createOverlayView"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        // try {
        do {
            AutoPtr<IITvInputSession> tvInputSession;
            ec = mHost->GetSessionLocked(sessionToken, callingUid, resolvedUserId, (IITvInputSession**)&tvInputSession);
            if (FAILED(ec)) break;
            ec = tvInputSession->CreateOverlayView(windowToken, frame);
        } while(FALSE);
        // } catch (RemoteException e) {
        if ((ECode)E_REMOTE_EXCEPTION == ec) {
            Slogger::E(TAG, "error in createOverlayView %d", ec);
            ec = NOERROR;
        }
        // }
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::RelayoutOverlayView(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ IRect* frame,
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("relayoutOverlayView"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        // try {
        do {
            AutoPtr<IITvInputSession> tvInputSession;
            ec = mHost->GetSessionLocked(sessionToken, callingUid, resolvedUserId, (IITvInputSession**)&tvInputSession);
            if (FAILED(ec)) break;
            ec = tvInputSession->RelayoutOverlayView(frame);
        } while(FALSE);
        // } catch (RemoteException e) {
        if ((ECode)E_REMOTE_EXCEPTION == ec) {
            Slogger::E(TAG, "error in relayoutOverlayView %d", ec);
            ec = NOERROR;
        }
        // }
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::RemoveOverlayView(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("removeOverlayView"), &resolvedUserId);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    {    AutoLock syncLock(mHost->mLock);
        // try {
        do {
            AutoPtr<IITvInputSession> tvInputSession;
            ec = mHost->GetSessionLocked(sessionToken, callingUid, resolvedUserId, (IITvInputSession**)&tvInputSession);
            if (FAILED(ec)) break;
            ec = tvInputSession->RemoveOverlayView();
        } while(FALSE);
        // } catch (RemoteException e) {
        if ((ECode)E_REMOTE_EXCEPTION == ec) {
            Slogger::E(TAG, "error in removeOverlayView %d", ec);
            ec = NOERROR;
        }
        // }
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::GetHardwareList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 permissionCode;
    mHost->mContext->CheckCallingPermission(Manifest::permission::TV_INPUT_HARDWARE, &permissionCode);
    if (permissionCode != IPackageManager::PERMISSION_GRANTED) {
        *result = NULL;
        return NOERROR;
    }
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec = mHost->mTvInputHardwareManager->GetHardwareList(result);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::AcquireTvInputHardware(
    /* [in] */ Int32 deviceId,
    /* [in] */ IITvInputHardwareCallback* callback,
    /* [in] */ ITvInputInfo* info,
    /* [in] */ Int32 userId,
    /* [out] */ IITvInputHardware** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 permissionCode;
    mHost->mContext->CheckCallingPermission(Manifest::permission::TV_INPUT_HARDWARE, &permissionCode);
    if (permissionCode != IPackageManager::PERMISSION_GRANTED) {
        *result = NULL;
        return NOERROR;
    }
    Int64 identity = Binder::ClearCallingIdentity();
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("acquireTvInputHardware"), &resolvedUserId);
    // try {
    ECode ec = mHost->mTvInputHardwareManager->AcquireHardware(
            deviceId, callback, info, callingUid, resolvedUserId, result);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::ReleaseTvInputHardware(
    /* [in] */ Int32 deviceId,
    /* [in] */ IITvInputHardware* hardware,
    /* [in] */ Int32 userId)
{
    Int32 permissionCode;
    mHost->mContext->CheckCallingPermission(Manifest::permission::TV_INPUT_HARDWARE, &permissionCode);
    if (permissionCode != IPackageManager::PERMISSION_GRANTED) {
        return NOERROR;
    }
    Int64 identity = Binder::ClearCallingIdentity();
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("releaseTvInputHardware"), &resolvedUserId);
    // try {
    ECode ec = mHost->mTvInputHardwareManager->ReleaseHardware(
                deviceId, hardware, callingUid, resolvedUserId);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::GetAvailableTvStreamConfigList(
    /* [in] */ const String& inputId,
    /* [in] */ Int32 userId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 permissionCode;
    mHost->mContext->CheckCallingPermission(Manifest::permission::CAPTURE_TV_INPUT, &permissionCode);
    if (permissionCode != IPackageManager::PERMISSION_GRANTED) {
        Logger::E(TAG, "Requires CAPTURE_TV_INPUT permission");
        return E_SECURITY_EXCEPTION;
    }
    Int64 identity = Binder::ClearCallingIdentity();
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("getAvailableTvStreamConfigList"), &resolvedUserId);
    // try {
    ECode ec = mHost->mTvInputHardwareManager->GetAvailableTvStreamConfigList(
            inputId, callingUid, resolvedUserId, result);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::CaptureFrame(
    /* [in] */ const String& inputId,
    /* [in] */ ISurface* surface,
    /* [in] */ ITvStreamConfig* config,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 permissionCode;
    mHost->mContext->CheckCallingPermission(Manifest::permission::CAPTURE_TV_INPUT, &permissionCode);
    if (permissionCode != IPackageManager::PERMISSION_GRANTED) {
        Logger::E(TAG, "Requires CAPTURE_TV_INPUT permission");
        return E_SECURITY_EXCEPTION;
    }
    Int64 identity = Binder::ClearCallingIdentity();
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("captureFrame"), &resolvedUserId);
    // try {
    ECode ec;
    do {
        String hardwareInputId = String(NULL);
        {    AutoLock syncLock(mHost->mLock);
            AutoPtr<UserState> userState;
            ec = mHost->GetUserStateLocked(resolvedUserId, (UserState**)&userState);
            if (FAILED(ec)) break;
            AutoPtr<IInterface> obj;
            userState->mInputMap->Get(StringUtils::ParseCharSequence(inputId), (IInterface**)&obj);
            if (obj == NULL) {
                Slogger::E(TAG, "input not found for %s", inputId.string());
                *result = FALSE;
                return NOERROR;
            }
            FOR_EACH(iter, Ptr(userState->mSessionStateMap)->Func(IMap::GetValues)) {
                AutoPtr<SessionState> sessionState = (SessionState*) IObject::Probe(Ptr(iter)->Func(iter->GetNext));
                if (Ptr(sessionState->mInfo)->Func(ITvInputInfo::GetId).Equals(inputId)
                        && sessionState->mHardwareSessionToken != NULL) {
                    obj = NULL;
                    userState->mSessionStateMap->Get(
                            sessionState->mHardwareSessionToken, (IInterface**)&obj);
                    ((SessionState*) IObject::Probe(obj))->mInfo->GetId(&hardwareInputId);
                    break;
                }
            }
        }
        ec = mHost->mTvInputHardwareManager->CaptureFrame(
                (hardwareInputId != NULL) ? hardwareInputId : inputId,
                surface, config, callingUid, resolvedUserId, result);
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::IsSingleSessionActive(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int64 identity = Binder::ClearCallingIdentity();
    Int32 callingUid = Binder::GetCallingUid();
    Int32 resolvedUserId;
    mHost->ResolveCallingUserId(Binder::GetCallingPid(), callingUid,
            userId, String("isSingleSessionActive"), &resolvedUserId);
    // try {
    ECode ec;
    do {
        {    AutoLock syncLock(mHost->mLock);
            AutoPtr<UserState> userState;
            ec = mHost->GetUserStateLocked(resolvedUserId, (UserState**)&userState);
            if (FAILED(ec)) break;
            if (Ptr(userState->mSessionStateMap)->Func(IMap::GetSize) == 1) {
                *result = TRUE;
                return NOERROR;
            }
            else if (Ptr(userState->mSessionStateMap)->Func(IMap::GetSize) == 2) {
                AutoPtr<ArrayOf<IInterface*> > sessionStates;
                Ptr(userState->mSessionStateMap)->Func(IMap::GetValues)->ToArray(
                        ArrayOf<IInterface*>::Alloc(0), (ArrayOf<IInterface*>**)&sessionStates);
                // Check if there is a wrapper input.
                if (((SessionState*) IObject::Probe((*sessionStates)[0]))->mHardwareSessionToken != NULL
                        || ((SessionState*) IObject::Probe((*sessionStates)[1]))->mHardwareSessionToken != NULL) {
                    *result = TRUE;
                    return NOERROR;
                }
            }
            *result = FALSE;
            return NOERROR;
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode TvInputManagerService::BinderService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<IIndentingPrintWriter> pw;
    CIndentingPrintWriter::New(IWriter::Probe(writer), String("  "), (IIndentingPrintWriter**)&pw);
    Int32 permissionCode;
    mHost->mContext->CheckCallingOrSelfPermission(Manifest::permission::DUMP, &permissionCode);
    if (permissionCode != IPackageManager::PERMISSION_GRANTED) {
        String s;
        s.AppendFormat("Permission Denial: can't dump TvInputManager from pid=%s, uid=%s",
                StringUtils::ToString(Binder::GetCallingPid()).string(), StringUtils::ToString(Binder::GetCallingUid()).string());
        IPrintWriter::Probe(pw)->Println(s);
        return NOERROR;
    }
    {    AutoLock syncLock(mHost->mLock);
        String s;
        s.AppendFormat("User Ids (Current user: %d):", mHost->mCurrentUserId);
        IPrintWriter::Probe(pw)->Println(s);
        pw->IncreaseIndent();
        for (Int32 i = 0; i < Ptr(mHost->mUserStates)->Func(mHost->mUserStates->GetSize); i++) {
            Int32 userId;
            mHost->mUserStates->KeyAt(i, &userId);
            IPrintWriter::Probe(pw)->Println(StringUtils::ToString(userId));
        }
        pw->DecreaseIndent();
        for (Int32 i = 0; i < Ptr(mHost->mUserStates)->Func(mHost->mUserStates->GetSize); i++) {
            Int32 userId;
            mHost->mUserStates->KeyAt(i, &userId);
            AutoPtr<UserState> userState;
            mHost->GetUserStateLocked(userId, (UserState**)&userState);
            String s;
            s.AppendFormat("UserState (%d):", userId);
            IPrintWriter::Probe(pw)->Println(s);
            pw->IncreaseIndent();
            IPrintWriter::Probe(pw)->Println(String("inputMap: inputId -> TvInputState"));
            pw->IncreaseIndent();
            FOR_EACH(it, Ptr(userState->mInputMap)->Func(IMap::GetEntrySet)) {
                AutoPtr<IMapEntry> entry = IMapEntry::Probe(Ptr(it)->Func(IIterator::GetNext));
                String s;
                s.AppendFormat("%s:%s",
                        Object::ToString(Ptr(entry)->Func(entry->GetKey)).string(),
                        Object::ToString(Ptr(entry)->Func(entry->GetValue)).string());
                IPrintWriter::Probe(pw)->Println(s);
            }
            pw->DecreaseIndent();
            IPrintWriter::Probe(pw)->Println(String("packageSet:"));
            pw->IncreaseIndent();
            FOR_EACH(iter, userState->mPackageSet) {
                String packageName;
                IObject::Probe(Ptr(iter)->Func(iter->GetNext))->ToString(&packageName);
                IPrintWriter::Probe(pw)->Println(packageName);
            }
            pw->DecreaseIndent();
            IPrintWriter::Probe(pw)->Println(String("clientStateMap: ITvInputClient -> ClientState"));
            pw->IncreaseIndent();
            FOR_EACH(set_iter, Ptr(userState->mClientStateMap)->Func(IMap::GetEntrySet)) {
                AutoPtr<IMapEntry> entry = IMapEntry::Probe(Ptr(set_iter)->Func(set_iter->GetNext));
                AutoPtr<IInterface> obj;
                entry->GetValue((IInterface**)&obj);
                AutoPtr<ClientState> client = (ClientState*) IObject::Probe(obj);
                String s;
                s.AppendFormat("%s:%s", Object::ToString(Ptr(entry)->Func(entry->GetKey)).string(), TO_CSTR(client));
                IPrintWriter::Probe(pw)->Println(s);
                pw->IncreaseIndent();
                IPrintWriter::Probe(pw)->Println(String("sessionTokens:"));
                pw->IncreaseIndent();
                FOR_EACH(token_it, client->mSessionTokens) {
                    AutoPtr<IBinder> token = IBinder::Probe(Ptr(token_it)->Func(IIterator::GetNext));
                    IPrintWriter::Probe(pw)->Println(Object::ToString(token));
                }
                pw->DecreaseIndent();
                s = String("clientTokens: ");
                s += Object::ToString(client->mClientToken);
                IPrintWriter::Probe(pw)->Println(s);
                s = String("userId: ");
                s += StringUtils::ToString(client->mUserId);
                IPrintWriter::Probe(pw)->Println(s);
                pw->DecreaseIndent();
            }
            pw->DecreaseIndent();
            IPrintWriter::Probe(pw)->Println(String("serviceStateMap: ComponentName -> ServiceState"));
            pw->IncreaseIndent();
            FOR_EACH(service_it, Ptr(userState->mServiceStateMap)->Func(IMap::GetEntrySet)) {
                AutoPtr<IMapEntry> entry = IMapEntry::Probe(Ptr(service_it)->Func(IIterator::GetNext));
                AutoPtr<IInterface> obj;
                entry->GetValue((IInterface**)&obj);
                AutoPtr<ServiceState> service = (ServiceState*) IObject::Probe(obj);
                obj = NULL;
                entry->GetKey((IInterface**)&obj);
                String s;
                s.AppendFormat("%s:%s", TO_CSTR(obj), TO_CSTR(service));
                IPrintWriter::Probe(pw)->Println(s);
                pw->IncreaseIndent();
                IPrintWriter::Probe(pw)->Println(String("sessionTokens:"));
                pw->IncreaseIndent();
                FOR_EACH(token_it, service->mSessionTokens) {
                    AutoPtr<IBinder> token = IBinder::Probe(Ptr(token_it)->Func(IIterator::GetNext));
                    IPrintWriter::Probe(pw)->Println(String("") + Object::ToString(token));
                }
                pw->DecreaseIndent();
                IPrintWriter::Probe(pw)->Println(String("service: ") + Object::ToString(service->mService));
                IPrintWriter::Probe(pw)->Println(String("callback: ") + Object::ToString(service->mCallback));
                IPrintWriter::Probe(pw)->Println(String("bound: ") + StringUtils::ToString(service->mBound));
                IPrintWriter::Probe(pw)->Println(String("reconnecting: ") + StringUtils::ToString(service->mReconnecting));
                pw->DecreaseIndent();
            }
            pw->DecreaseIndent();
            IPrintWriter::Probe(pw)->Println(String("sessionStateMap: ITvInputSession -> SessionState"));
            pw->IncreaseIndent();
            FOR_EACH(entry_it, Ptr(userState->mSessionStateMap)->Func(IMap::GetEntrySet)) {
                AutoPtr<IMapEntry> entry = IMapEntry::Probe(Ptr(entry_it)->Func(IIterator::GetNext));
                AutoPtr<IInterface> obj;
                entry->GetValue((IInterface**)&obj);
                AutoPtr<SessionState> session = (SessionState*) IObject::Probe(obj);
                obj = NULL;
                entry->GetKey((IInterface**)&obj);
                IPrintWriter::Probe(pw)->Println(Object::ToString(obj) + String(": ") + Object::ToString(session));
                pw->IncreaseIndent();
                IPrintWriter::Probe(pw)->Println(String("info: ") + Object::ToString(session->mInfo));
                IPrintWriter::Probe(pw)->Println(String("client: ") + Object::ToString(session->mClient));
                IPrintWriter::Probe(pw)->Println(String("seq: ") + StringUtils::ToString(session->mSeq));
                IPrintWriter::Probe(pw)->Println(String("callingUid: ") + StringUtils::ToString(session->mCallingUid));
                IPrintWriter::Probe(pw)->Println(String("userId: ") + StringUtils::ToString(session->mUserId));
                IPrintWriter::Probe(pw)->Println(String("sessionToken: ") + Object::ToString(session->mSessionToken));
                IPrintWriter::Probe(pw)->Println(String("session: ") + Object::ToString(session->mSession));
                IPrintWriter::Probe(pw)->Println(String("logUri: ") + Object::ToString(session->mLogUri));
                IPrintWriter::Probe(pw)->Println(String("hardwareSessionToken: ") + Object::ToString(session->mHardwareSessionToken));
                pw->DecreaseIndent();
            }
            pw->DecreaseIndent();
            IPrintWriter::Probe(pw)->Println(String("callbackSet:"));
            pw->IncreaseIndent();
            FOR_EACH(callback_it, userState->mCallbackSet) {
                AutoPtr<IITvInputManagerCallback> callback = IITvInputManagerCallback::Probe(Ptr(callback_it)->Func(IIterator::GetNext));
                IPrintWriter::Probe(pw)->Println(Object::ToString(callback));
            }
            pw->DecreaseIndent();
            IPrintWriter::Probe(pw)->Println(String("mainSessionToken: ") + Object::ToString(userState->mMainSessionToken));
            pw->DecreaseIndent();
        }
    }
    return NOERROR;
}

ECode TvInputManagerService::BinderService::ToString(
    /* [out] */ String* result)
{
    return IObject::Probe(TO_IINTERFACE(this))->ToString(result);
}

//=============================================================================
// TvInputManagerService::UserState
//=============================================================================
TvInputManagerService::UserState::UserState(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId)
{
        mPersistentDataStore = new PersistentDataStore();
        mPersistentDataStore->constructor(context, userId);
        CHashMap::New((IMap**)&mInputMap);
        CHashSet::New((ISet**)&mPackageSet);
        CArrayList::New((IList**)&mContentRatingSystemList);
        CHashMap::New((IMap**)&mClientStateMap);
        CHashMap::New((IMap**)&mServiceStateMap);
        CHashMap::New((IMap**)&mSessionStateMap);
        CHashSet::New((ISet**)&mCallbackSet);
}

//=============================================================================
// TvInputManagerService::ClientState
//=============================================================================
CAR_INTERFACE_IMPL(TvInputManagerService::ClientState, Object, IProxyDeathRecipient)

TvInputManagerService::ClientState::ClientState(
    /* [in] */ TvInputManagerService* host)
    : mHost(host)
    , mUserId(0)
{
    CArrayList::New((IList**)&mSessionTokens);
}

ECode TvInputManagerService::ClientState::constructor(
    /* [in] */ IBinder* clientToken,
    /* [in] */ Int32 userId)
{
    mClientToken = clientToken;
    mUserId = userId;
    return NOERROR;
}

ECode TvInputManagerService::ClientState::IsEmpty(
    /* [out] */ Boolean* result)
{
    return mSessionTokens->IsEmpty(result);
}

ECode TvInputManagerService::ClientState::ProxyDied()
{
    {    AutoLock syncLock(mHost->mLock);
        AutoPtr<UserState> userState;
        mHost->GetUserStateLocked(mUserId, (UserState**)&userState);
        // DO NOT remove the client state of clientStateMap in this method. It will be
        // removed in ReleaseSessionLocked().
        AutoPtr<IInterface> obj;
        userState->mClientStateMap->Get(mClientToken, (IInterface**)&obj);
        AutoPtr<ClientState> clientState = (ClientState*) IObject::Probe(obj);
        if (clientState != NULL) {
            while (Ptr(clientState->mSessionTokens)->Func(IList::GetSize) > 0) {
                obj = NULL;
                clientState->mSessionTokens->Get(0, (IInterface**)&obj);
                mHost->ReleaseSessionLocked(IBinder::Probe(obj), IProcess::SYSTEM_UID, mUserId);
            }
        }
        mClientToken = NULL;
    }
    return NOERROR;
}

//=============================================================================
// TvInputManagerService::ServiceState
//=============================================================================
TvInputManagerService::ServiceState::ServiceState(
    /* [in] */ TvInputManagerService* host,
    /* [in] */ IComponentName* component,
    /* [in] */ Int32 userId)
    : mHost(host)
    , mComponent(component)
    , mIsHardware(FALSE)
    , mBound(FALSE)
    , mReconnecting(FALSE)
{
    CArrayList::New((IList**)&mSessionTokens);
    CArrayList::New((IList**)&mInputList);
    mConnection = new InputServiceConnection(mHost, component, userId);
    mIsHardware = HasHardwarePermission(Ptr(mHost->mContext)->Func(mHost->mContext->GetPackageManager), component);
}

//=============================================================================
// TvInputManagerService::TvInputState
//=============================================================================
TvInputManagerService::TvInputState::TvInputState()
    : mState(ITvInputManager::INPUT_STATE_CONNECTED)
{}

ECode TvInputManagerService::TvInputState::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String rev;
    rev.AppendFormat("info: %s; state: %d", TO_CSTR(mInfo), mState);
    *result = rev;
    return NOERROR;
}

//=============================================================================
// TvInputManagerService::SessionState
//=============================================================================
CAR_INTERFACE_IMPL(TvInputManagerService::SessionState, Object, IProxyDeathRecipient)

TvInputManagerService::SessionState::SessionState(
    /* [in] */ TvInputManagerService* host,
    /* [in] */ IBinder* sessionToken,
    /* [in] */ ITvInputInfo* info,
    /* [in] */ IITvInputClient* client,
    /* [in] */ Int32 seq,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 userId)
    : mHost(host)
    , mInfo(info)
    , mClient(client)
    , mSeq(seq)
    , mCallingUid(callingUid)
    , mUserId(userId)
    , mSessionToken(sessionToken)
{}

ECode TvInputManagerService::SessionState::ProxyDied()
{
    {    AutoLock syncLock(mHost->mLock);
        mSession = NULL;
        if (mClient != NULL) {
            // try {
            ECode ec = mClient->OnSessionReleased(mSeq);
            // } catch(RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode) E_REMOTE_EXCEPTION == ec) {
                    Slogger::E(TAG, "error in onSessionReleased %d", ec);
                }
                else
                    return ec;
            }
            // }
        }
        // If there are any other sessions based on this session, they should be released.
        AutoPtr<UserState> userState;
        mHost->GetUserStateLocked(mUserId, (UserState**)&userState);
        FOR_EACH(it, Ptr(userState->mSessionStateMap)->Func(IMap::GetValues)) {
            AutoPtr<SessionState> sessionState = (SessionState*) IObject::Probe(Ptr(it)->Func(it->GetNext));
            if (mSessionToken == sessionState->mHardwareSessionToken) {
                mHost->ReleaseSessionLocked(sessionState->mSessionToken, IProcess::SYSTEM_UID,
                        mUserId);
                // try {
                ECode ec = sessionState->mClient->OnSessionReleased(sessionState->mSeq);
                // } catch (RemoteException e) {
                if (FAILED(ec)) {
                    if ((ECode)E_REMOTE_EXCEPTION == ec) {
                        Slogger::E(TAG, "error in onSessionReleased %d", ec);
                    }
                    else
                        return ec;
                }
                // }
            }
        }
        mHost->RemoveSessionStateLocked(mSessionToken, mUserId);
    }
    return NOERROR;
}

//=============================================================================
// TvInputManagerService::InputServiceConnection
//=============================================================================
CAR_INTERFACE_IMPL(TvInputManagerService::InputServiceConnection, Object, IServiceConnection)

TvInputManagerService::InputServiceConnection::InputServiceConnection(
    /* [in] */ TvInputManagerService* host,
    /* [in] */ IComponentName* component,
    /* [in] */ Int32 userId)
    : mHost(host)
    , mComponent(component)
    , mUserId(userId)
{}

ECode TvInputManagerService::InputServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* component,
    /* [in] */ IBinder* service)
{
    if (DEBUG) {
        Slogger::D(TAG, "onServiceConnected(component=%s)", TO_CSTR(component));
    }
    {    AutoLock syncLock(mHost->mLock);
        AutoPtr<UserState> userState;
        mHost->GetUserStateLocked(mUserId, (UserState**)&userState);
        AutoPtr<IInterface> obj;
        userState->mServiceStateMap->Get(mComponent, (IInterface**)&obj);
        AutoPtr<ServiceState> serviceState = (ServiceState*) IObject::Probe(obj);
        serviceState->mService = IITvInputService::Probe(service);
        // Register a callback, if we need to.
        if (serviceState->mIsHardware && serviceState->mCallback == NULL) {
            serviceState->mCallback = new ServiceCallback(mHost);
            serviceState->mCallback->constructor(mComponent, mUserId);
            // try {
            ECode ec = serviceState->mService->RegisterCallback(serviceState->mCallback);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode)E_REMOTE_EXCEPTION == ec) {
                    Slogger::E(TAG, "error in registerCallback %d", ec);
                }
                else
                    return ec;
            }
            // }
        }
        // And create sessions, if any.
        FOR_EACH(it, serviceState->mSessionTokens) {
            AutoPtr<IBinder> sessionToken = IBinder::Probe(Ptr(it)->Func(IIterator::GetNext));
            mHost->CreateSessionInternalLocked(serviceState->mService, sessionToken, mUserId);
        }
        FOR_EACH(state_it, Ptr(userState->mInputMap)->Func(IMap::GetValues)) {
            AutoPtr<TvInputState> inputState = (TvInputState*) IObject::Probe(Ptr(state_it)->Func(IIterator::GetNext));
            Boolean isEquals;
            IObject::Probe(Ptr(inputState->mInfo)->Func(ITvInputInfo::GetComponent))->Equals(component, &isEquals);
            if (isEquals
                    && inputState->mState != ITvInputManager::INPUT_STATE_DISCONNECTED) {
                mHost->NotifyInputStateChangedLocked(userState, Ptr(inputState->mInfo)->Func(ITvInputInfo::GetId),
                        inputState->mState, NULL);
            }
        }
        if (serviceState->mIsHardware) {
            AutoPtr<IList> hardwareInfoList;
            mHost->mTvInputHardwareManager->GetHardwareList((IList**)&hardwareInfoList);
            FOR_EACH(info_it, hardwareInfoList) {
                AutoPtr<ITvInputHardwareInfo> hardwareInfo = ITvInputHardwareInfo::Probe(Ptr(info_it)->Func(IIterator::GetNext));
                // try {
                ECode ec = serviceState->mService->NotifyHardwareAdded(hardwareInfo);
                // } catch (RemoteException e) {
                if (FAILED(ec)) {
                    if ((ECode)E_REMOTE_EXCEPTION == ec) {
                        Slogger::E(TAG, "error in notifyHardwareAdded %d", ec);
                    }
                    else
                        return ec;
                }
                // }
            }
            AutoPtr<IList> deviceInfoList;
            mHost->mTvInputHardwareManager->GetHdmiDeviceList((IList**)&deviceInfoList);
            FOR_EACH(deviceInfo_it, deviceInfoList) {
                AutoPtr<IHdmiDeviceInfo> deviceInfo = IHdmiDeviceInfo::Probe(Ptr(deviceInfo_it)->Func(IIterator::GetNext));
                // try {
                ECode ec = serviceState->mService->NotifyHdmiDeviceAdded(deviceInfo);
                // } catch (RemoteException e) {
                if (FAILED(ec)) {
                    if ((ECode)E_REMOTE_EXCEPTION == ec) {
                        Slogger::E(TAG, "error in notifyHdmiDeviceAdded %d", ec);
                    }
                    else
                        return ec;
                }
                // }
            }
        }
    }
    return NOERROR;
}

ECode TvInputManagerService::InputServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* component)
{
    if (DEBUG) {
        Slogger::D(TAG, "onServiceDisconnected(component=%s)", TO_CSTR(component));
    }
    Boolean isEquals;
    IObject::Probe(mComponent)->Equals(component, &isEquals);
    if (!isEquals) {
        Logger::E(TAG, "Mismatched ComponentName: %s (expected), %s (actual).",
                TO_CSTR(mComponent), TO_CSTR(component));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    {    AutoLock syncLock(mHost->mLock);
        AutoPtr<UserState> userState;
        mHost->GetUserStateLocked(mUserId, (UserState**)&userState);
        AutoPtr<IInterface> obj;
        userState->mServiceStateMap->Get(mComponent, (IInterface**)&obj);
        AutoPtr<ServiceState> serviceState = (ServiceState*) IObject::Probe(obj);
        if (serviceState != NULL) {
            serviceState->mReconnecting = TRUE;
            serviceState->mBound = FALSE;
            serviceState->mService = NULL;
            serviceState->mCallback = NULL;
            mHost->AbortPendingCreateSessionRequestsLocked(serviceState, String(NULL), mUserId);
            FOR_EACH(it, Ptr(userState->mInputMap)->Func(IMap::GetValues)) {
                AutoPtr<TvInputState> inputState = (TvInputState*) IObject::Probe(Ptr(it)->Func(it->GetNext));
                IObject::Probe(Ptr(inputState->mInfo)->Func(ITvInputInfo::GetComponent))->Equals(component, &isEquals);
                if (isEquals) {
                    mHost->NotifyInputStateChangedLocked(userState, Ptr(inputState->mInfo)->Func(ITvInputInfo::GetId),
                            ITvInputManager::INPUT_STATE_DISCONNECTED, NULL);
                }
            }
        }
    }
    return NOERROR;
}

//=============================================================================
// TvInputManagerService::ServiceCallback
//=============================================================================
CAR_INTERFACE_IMPL_2(TvInputManagerService::ServiceCallback, Object, IITvInputServiceCallback, IBinder)

TvInputManagerService::ServiceCallback::ServiceCallback(
    /* [in] */ TvInputManagerService* host)
    : mHost(host)
    , mUserId(0)
{}

ECode TvInputManagerService::ServiceCallback::constructor(
    /* [in] */ IComponentName* component,
    /* [in] */ Int32 userId)
{
    mComponent = component;
    mUserId = userId;
    return NOERROR;
}

ECode TvInputManagerService::ServiceCallback::EnsureHardwarePermission()
{
    Int32 permissionCode;
    mHost->mContext->CheckCallingPermission(Manifest::permission::TV_INPUT_HARDWARE, &permissionCode);
    if (permissionCode != IPackageManager::PERMISSION_GRANTED) {
        Logger::E(TAG, "The caller does not have hardware permission");
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode TvInputManagerService::ServiceCallback::EnsureValidInput(
    /* [in] */ ITvInputInfo* inputInfo)
{
    Boolean isEquals;
    IObject::Probe(mComponent)->Equals(Ptr(inputInfo)->Func(inputInfo->GetComponent), &isEquals);
    if (Ptr(inputInfo)->Func(inputInfo->GetId) == NULL || !isEquals) {
        Logger::E(TAG, "Invalid TvInputInfo");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode TvInputManagerService::ServiceCallback::AddTvInputLocked(
    /* [in] */ ITvInputInfo* inputInfo)
{
    AutoPtr<ServiceState> serviceState;
    mHost->GetServiceStateLocked(mComponent, mUserId, (ServiceState**)&serviceState);
    serviceState->mInputList->Add(inputInfo);
    mHost->BuildTvInputListLocked(mUserId, NULL);
    return NOERROR;
}

ECode TvInputManagerService::ServiceCallback::AddHardwareTvInput(
    /* [in] */ Int32 deviceId,
    /* [in] */ ITvInputInfo* inputInfo)
{
    EnsureHardwarePermission();
    EnsureValidInput(inputInfo);
    {    AutoLock syncLock(mHost->mLock);
        mHost->mTvInputHardwareManager->AddHardwareTvInput(deviceId, inputInfo);
        AddTvInputLocked(inputInfo);
    }
    return NOERROR;
}

ECode TvInputManagerService::ServiceCallback::AddHdmiTvInput(
    /* [in] */ Int32 id,
    /* [in] */ ITvInputInfo* inputInfo)
{
    EnsureHardwarePermission();
    EnsureValidInput(inputInfo);
    {    AutoLock syncLock(mHost->mLock);
        mHost->mTvInputHardwareManager->AddHdmiTvInput(id, inputInfo);
        AddTvInputLocked(inputInfo);
    }
    return NOERROR;
}

ECode TvInputManagerService::ServiceCallback::RemoveTvInput(
    /* [in] */ const String& inputId)
{
    EnsureHardwarePermission();
    {    AutoLock syncLock(mHost->mLock);
        AutoPtr<ServiceState> serviceState;
        mHost->GetServiceStateLocked(mComponent, mUserId, (ServiceState**)&serviceState);
        Boolean removed = FALSE;
        FOR_EACH(it, serviceState->mInputList) {
            if (Ptr(ITvInputInfo::Probe(Ptr(it)->Func(it->GetNext)))->Func(ITvInputInfo::GetId).Equals(inputId)) {
                it->Remove();
                removed = TRUE;
                break;
            }
        }
        if (removed) {
            mHost->BuildTvInputListLocked(mUserId, NULL);
            mHost->mTvInputHardwareManager->RemoveTvInput(inputId);
        } else {
            Slogger::E(TAG, "failed to remove input %s", inputId.string());
        }
    }
    return NOERROR;
}

ECode TvInputManagerService::ServiceCallback::ToString(
    /* [out] */ String* result)
{
    return IObject::Probe(TO_IINTERFACE(this))->ToString(result);
}

//=============================================================================
// TvInputManagerService::SessionCallback
//=============================================================================
CAR_INTERFACE_IMPL_2(TvInputManagerService::SessionCallback, Object, IBinder, IITvInputServiceCallback)

TvInputManagerService::SessionCallback::SessionCallback(
    /* [in] */ TvInputManagerService* host)
    : mHost(host)
{}

ECode TvInputManagerService::SessionCallback::constructor(
    /* [in] */ SessionState* sessionState,
    /* [in] */ ArrayOf<IInputChannel*>* channels)
{
    mSessionState = sessionState;
    mChannels = channels;
    return NOERROR;
}

ECode TvInputManagerService::SessionCallback::OnSessionCreated(
    /* [in] */ IITvInputSession* session,
    /* [in] */ IBinder* harewareSessionToken)
{
    if (DEBUG) {
        Slogger::D(TAG, "onSessionCreated(inputId=%s)", Ptr(mSessionState->mInfo)->Func(ITvInputInfo::GetId).string());
    }
    {    AutoLock syncLock(mHost->mLock);
        mSessionState->mSession = session;
        mSessionState->mHardwareSessionToken = harewareSessionToken;
        Boolean b;
        AddSessionTokenToClientStateLocked(session, &b);
        if (session != NULL && b) {
            mHost->SendSessionTokenToClientLocked(mSessionState->mClient,
                    Ptr(mSessionState->mInfo)->Func(ITvInputInfo::GetId),
                    mSessionState->mSessionToken, (*mChannels)[0],
                    mSessionState->mSeq);
        } else {
            mHost->RemoveSessionStateLocked(mSessionState->mSessionToken, mSessionState->mUserId);
            mHost->SendSessionTokenToClientLocked(mSessionState->mClient,
                    Ptr(mSessionState->mInfo)->Func(ITvInputInfo::GetId), NULL, NULL, mSessionState->mSeq);
        }
        (*mChannels)[0]->Dispose();
    }
    return NOERROR;
}

ECode TvInputManagerService::SessionCallback::AddSessionTokenToClientStateLocked(
    /* [in] */ IITvInputSession* session,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    AutoPtr<IProxy> proxy = (IProxy*)session->Probe(EIID_IProxy);
    ECode ec;
    if (proxy != NULL) ec = proxy->LinkToDeath(mSessionState, 0);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec) {
            Slogger::E(TAG, "session process has already died %d", ec);
            *result = FALSE;
            return NOERROR;
        }
        else
            return ec;
    }
    // }
    AutoPtr<IBinder> clientToken = IBinder::Probe(mSessionState->mClient);
    AutoPtr<UserState> userState;
    mHost->GetUserStateLocked(mSessionState->mUserId, (UserState**)&userState);
    AutoPtr<IInterface> obj;
    userState->mClientStateMap->Get(clientToken, (IInterface**)&obj);
    AutoPtr<ClientState> clientState = (ClientState*) IObject::Probe(obj);
    if (clientState == NULL) {
        clientState = new ClientState(mHost);
        clientState->constructor(clientToken, mSessionState->mUserId);
        // try {
        ECode ec;
        AutoPtr<IProxy> proxy = (IProxy*) clientToken->Probe(EIID_IProxy);
        if (proxy != NULL) ec = proxy->LinkToDeath(clientState, 0);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "client process has already died %d", ec);
                *result = FALSE;
                return NOERROR;
            }
            else
                return ec;
        }
        // }
        userState->mClientStateMap->Put(clientToken, TO_IINTERFACE(clientState));
    }
    clientState->mSessionTokens->Add(mSessionState->mSessionToken);
    *result = TRUE;
    return NOERROR;
}

ECode TvInputManagerService::SessionCallback::OnChannelRetuned(
    /* [in] */ IUri* channelUri)
{
    {    AutoLock syncLock(mHost->mLock);
        if (DEBUG) {
            Slogger::D(TAG, "onChannelRetuned(%s)", TO_CSTR(channelUri));
        }
        if (mSessionState->mSession == NULL || mSessionState->mClient == NULL) {
            return NOERROR;
        }
        // try {
        // TODO: Consider adding this channel change in the watch log. When we do
        // that, how we can protect the watch log from malicious tv inputs should
        // be addressed. e.g. add a field which represents where the channel change
        // originated from.
        ECode ec = mSessionState->mClient->OnChannelRetuned(channelUri, mSessionState->mSeq);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in onChannelRetuned %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode TvInputManagerService::SessionCallback::OnTracksChanged(
    /* [in] */ IList* tracks)
{
    {    AutoLock syncLock(mHost->mLock);
        if (DEBUG) {
            Slogger::D(TAG, "onTracksChanged(%s)", TO_CSTR(tracks));
        }
        if (mSessionState->mSession == NULL || mSessionState->mClient == NULL) {
            return NOERROR;
        }
        // try {
        ECode ec = mSessionState->mClient->OnTracksChanged(tracks, mSessionState->mSeq);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in onTracksChanged %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode TvInputManagerService::SessionCallback::OnTrackSelected(
    /* [in] */ Int32 type,
    /* [in] */ const String& trackId)
{
    {    AutoLock syncLock(mHost->mLock);
        if (DEBUG) {
            Slogger::D(TAG, "onTrackSelected(type=%d, trackId=%s)", type, trackId.string());
        }
        if (mSessionState->mSession == NULL || mSessionState->mClient == NULL) {
            return NOERROR;
        }
        // try {
        ECode ec = mSessionState->mClient->OnTrackSelected(type, trackId, mSessionState->mSeq);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in onTrackSelected %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode TvInputManagerService::SessionCallback::OnVideoAvailable()
{
    {    AutoLock syncLock(mHost->mLock);
        if (DEBUG) {
            Slogger::D(TAG, "onVideoAvailable()");
        }
        if (mSessionState->mSession == NULL || mSessionState->mClient == NULL) {
            return NOERROR;
        }
        // try {
        ECode ec = mSessionState->mClient->OnVideoAvailable(mSessionState->mSeq);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in onVideoAvailable %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode TvInputManagerService::SessionCallback::OnVideoUnavailable(
    /* [in] */ Int32 reason)
{
    {    AutoLock syncLock(mHost->mLock);
        if (DEBUG) {
            Slogger::D(TAG, "onVideoUnavailable(%d)", reason);
        }
        if (mSessionState->mSession == NULL || mSessionState->mClient == NULL) {
            return NOERROR;
        }
        // try {
        ECode ec = mSessionState->mClient->OnVideoUnavailable(reason, mSessionState->mSeq);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in onVideoUnavailable %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode TvInputManagerService::SessionCallback::OnContentAllowed()
{
    {    AutoLock syncLock(mHost->mLock);
        if (DEBUG) {
            Slogger::D(TAG, "onContentAllowed()");
        }
        if (mSessionState->mSession == NULL || mSessionState->mClient == NULL) {
            return NOERROR;
        }
        // try {
        ECode ec = mSessionState->mClient->OnContentAllowed(mSessionState->mSeq);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in onContentAllowed %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode TvInputManagerService::SessionCallback::OnContentBlocked(
    /* [in] */ const String& rating)
{
    {    AutoLock syncLock(mHost->mLock);
        if (DEBUG) {
            Slogger::D(TAG, "onContentBlocked()");
        }
        if (mSessionState->mSession == NULL || mSessionState->mClient == NULL) {
            return NOERROR;
        }
        // try {
        ECode ec = mSessionState->mClient->OnContentBlocked(rating, mSessionState->mSeq);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in onContentBlocked %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode TvInputManagerService::SessionCallback::OnLayoutSurface(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    {    AutoLock syncLock(mHost->mLock);
        if (DEBUG) {
            Slogger::D(TAG, "onLayoutSurface (left=%d, top=%d, right=%d, bottom=%d,)",
                left, top, right, bottom);
        }
        if (mSessionState->mSession == NULL || mSessionState->mClient == NULL) {
            return NOERROR;
        }
        // try {
        ECode ec = mSessionState->mClient->OnLayoutSurface(left, top, right, bottom,
                mSessionState->mSeq);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in onLayoutSurface %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode TvInputManagerService::SessionCallback::OnSessionEvent(
    /* [in] */ const String& eventType,
    /* [in] */ IBundle* eventArgs)
{
    {    AutoLock syncLock(mHost->mLock);
        if (DEBUG) {
            Slogger::D(TAG, "onEvent(what=%s, data=%s)", eventType.string(), TO_CSTR(eventArgs));
        }
        if (mSessionState->mSession == NULL || mSessionState->mClient == NULL) {
            return NOERROR;
        }
        // try {
        ECode ec = mSessionState->mClient->OnSessionEvent(eventType, eventArgs, mSessionState->mSeq);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in onSessionEvent %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode TvInputManagerService::SessionCallback::ToString(
    /* [out] */ String* result)
{
    return IObject::Probe(TO_IINTERFACE(this))->ToString(result);
}

//=============================================================================
// TvInputManagerService::WatchLogHandler
//=============================================================================
const Int32 TvInputManagerService::WatchLogHandler::MSG_LOG_WATCH_START = 1;
const Int32 TvInputManagerService::WatchLogHandler::MSG_LOG_WATCH_END = 2;

ECode TvInputManagerService::WatchLogHandler::constructor(
    /* [in] */ IContentResolver* contentResolver,
    /* [in] */ ILooper* looper)
{
    Handler::constructor(looper);
    mContentResolver = contentResolver;
    return NOERROR;
}

ECode TvInputManagerService::WatchLogHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case MSG_LOG_WATCH_START: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
            AutoPtr<IInterface> objArg;
            args->GetObjectArg(1, (IInterface**)&objArg);
            String packageName;
            ICharSequence::Probe(objArg)->ToString(&packageName);
            Int32 i32Arg;
            args->GetInt32Arg(2, &i32Arg);
            Int64 watchStartTime = (Int64) i32Arg;
            args->GetInt32Arg(3, &i32Arg);
            Int64 channelId = (Int64) i32Arg;
            objArg = NULL;
            args->GetObjectArg(4, (IInterface**)&objArg);
            AutoPtr<IBundle> tuneParams = IBundle::Probe(objArg);
            objArg = NULL;
            args->GetObjectArg(5, (IInterface**)&objArg);
            AutoPtr<IBinder> sessionToken = IBinder::Probe(objArg);
            AutoPtr<IContentValues> values;
            CContentValues::New((IContentValues**)&values);
            values->Put(ITvContractBaseTvColumns::COLUMN_PACKAGE_NAME, packageName);
            values->Put(ITvContractWatchedPrograms::COLUMN_WATCH_START_TIME_UTC_MILLIS,
                    watchStartTime);
            values->Put(ITvContractWatchedPrograms::COLUMN_CHANNEL_ID, channelId);
            if (tuneParams != NULL) {
                values->Put(ITvContractWatchedPrograms::COLUMN_INTERNAL_TUNE_PARAMS,
                        EncodeTuneParams(tuneParams));
            }
            values->Put(ITvContractWatchedPrograms::COLUMN_INTERNAL_SESSION_TOKEN,
                    Ptr(sessionToken)->Func(sessionToken->ToString));
            AutoPtr<ITvContractWatchedPrograms> helper;
            CTvContractWatchedPrograms::AcquireSingleton((ITvContractWatchedPrograms**)&helper);
            AutoPtr<IUri> uri;
            mContentResolver->Insert(Ptr(helper)->Func(ITvContractWatchedPrograms::GetCONTENT_URI), values, (IUri**)&uri);
            args->Recycle();
            return NOERROR;
        }
        case MSG_LOG_WATCH_END: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
            AutoPtr<IInterface> objArg;
            args->GetObjectArg(1, (IInterface**)&objArg);
            AutoPtr<IBinder> sessionToken = IBinder::Probe(objArg);
            Int32 i32Arg;
            args->GetInt32Arg(2, &i32Arg);
            Int64 watchEndTime = (Int64) i32Arg;
            AutoPtr<IContentValues> values;
            CContentValues::New((IContentValues**)&values);
            values->Put(ITvContractWatchedPrograms::COLUMN_WATCH_END_TIME_UTC_MILLIS,
                    watchEndTime);
            values->Put(ITvContractWatchedPrograms::COLUMN_INTERNAL_SESSION_TOKEN,
                    Ptr(sessionToken)->Func(sessionToken->ToString));
            AutoPtr<ITvContractWatchedPrograms> helper;
            CTvContractWatchedPrograms::AcquireSingleton((ITvContractWatchedPrograms**)&helper);
            AutoPtr<IUri> uri;
            mContentResolver->Insert(Ptr(helper)->Func(ITvContractWatchedPrograms::GetCONTENT_URI), values, (IUri**)&uri);
            args->Recycle();
            return NOERROR;
        }
        default: {
            Slogger::W(TAG, "Unhandled message code: %d", what);
            return NOERROR;
        }
    }
    return NOERROR;
}

String TvInputManagerService::WatchLogHandler::EncodeTuneParams(
    /* [in] */ IBundle* tuneParams)
{
    StringBuilder builder;
    AutoPtr<ISet> keySet;
    tuneParams->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);
    while (Ptr(it)->Func(it->HasNext)) {
        AutoPtr<IInterface> key;
        it->GetNext((IInterface**)&key);
        AutoPtr<IInterface> value;
        tuneParams->Get(Object::ToString(key), (IInterface**)&value);
        if (value == NULL) {
            continue;
        }
        builder.Append(ReplaceEscapeCharacters(Object::ToString(key)));
        builder.Append(String("="));
        builder.Append(ReplaceEscapeCharacters(Object::ToString(value)));
        if (Ptr(it)->Func(it->HasNext)) {
            builder.Append(String(", "));
        }
    }
    return builder.ToString();
}

String TvInputManagerService::WatchLogHandler::ReplaceEscapeCharacters(
    /* [in] */ const String& src)
{
    const Char32 ESCAPE_CHARACTER = '%';
    const String ENCODING_TARGET_CHARACTERS("%=,");
    StringBuilder builder;
    AutoPtr<ArrayOf<Char32> > charArray = src.GetChars();
    for (Int32 i = 0; i < charArray->GetLength(); ++i) {
        Char32 ch = (*charArray)[i];
        if (ENCODING_TARGET_CHARACTERS.IndexOf(ch) >= 0) {
            builder.AppendChar(ESCAPE_CHARACTER);
        }
        builder.AppendChar(ch);
    }
    return builder.ToString();
}

//=============================================================================
// TvInputManagerService::HardwareListener
//=============================================================================
CAR_INTERFACE_IMPL(TvInputManagerService::HardwareListener, Object, ITvInputHardwareManagerListener)

TvInputManagerService::HardwareListener::HardwareListener(
    /* [in] */ TvInputManagerService* host)
    : mHost(host)
{}

ECode TvInputManagerService::HardwareListener::OnStateChanged(
    /* [in] */ const String& inputId,
    /* [in] */ Int32 state)
{
    {    AutoLock syncLock(mHost->mLock);
        mHost->SetStateLocked(inputId, state, mHost->mCurrentUserId);
    }
    return NOERROR;
}

ECode TvInputManagerService::HardwareListener::OnHardwareDeviceAdded(
    /* [in] */ ITvInputHardwareInfo* info)
{
    {    AutoLock syncLock(mHost->mLock);
        AutoPtr<UserState> userState;
        mHost->GetUserStateLocked(mHost->mCurrentUserId, (UserState**)&userState);
        // Broadcast the event to all hardware inputs.
        FOR_EACH(it, Ptr(userState->mServiceStateMap)->Func(IMap::GetValues)) {
            AutoPtr<ServiceState> serviceState = (ServiceState*) IObject::Probe(Ptr(it)->Func(IIterator::GetNext));
            if (!serviceState->mIsHardware || serviceState->mService == NULL) continue;
            // try {
            ECode ec = serviceState->mService->NotifyHardwareAdded(info);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode)E_REMOTE_EXCEPTION == ec) {
                    Slogger::E(TAG, "error in notifyHardwareAdded %d", ec);
                }
                else
                    return ec;
            }
            // }
        }
    }
    return NOERROR;
}

ECode TvInputManagerService::HardwareListener::OnHardwareDeviceRemoved(
    /* [in] */ ITvInputHardwareInfo* info)
{
    {    AutoLock syncLock(mHost->mLock);
        AutoPtr<UserState> userState;
        mHost->GetUserStateLocked(mHost->mCurrentUserId, (UserState**)&userState);
        // Broadcast the event to all hardware inputs.
        FOR_EACH(it, Ptr(userState->mServiceStateMap)->Func(IMap::GetValues)) {
            AutoPtr<ServiceState> serviceState = (ServiceState*) IObject::Probe(Ptr(it)->Func(IIterator::GetNext));
            if (!serviceState->mIsHardware || serviceState->mService == NULL) continue;
            // try {
            ECode ec = serviceState->mService->NotifyHardwareRemoved(info);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode)E_REMOTE_EXCEPTION == ec) {
                    Slogger::E(TAG, "error in notifyHardwareRemoved %d", ec);
                }
                else
                    return ec;
            }
            // }
        }
    }
    return NOERROR;
}

ECode TvInputManagerService::HardwareListener::OnHdmiDeviceAdded(
    /* [in] */ IHdmiDeviceInfo* deviceInfo)
{
    {    AutoLock syncLock(mHost->mLock);
        AutoPtr<UserState> userState;
        mHost->GetUserStateLocked(mHost->mCurrentUserId, (UserState**)&userState);
        // Broadcast the event to all hardware inputs.
        FOR_EACH(it, Ptr(userState->mServiceStateMap)->Func(IMap::GetValues)) {
            AutoPtr<ServiceState> serviceState = (ServiceState*) IObject::Probe(Ptr(it)->Func(IIterator::GetNext));
            if (!serviceState->mIsHardware || serviceState->mService == NULL) continue;
            // try {
            ECode ec = serviceState->mService->NotifyHdmiDeviceAdded(deviceInfo);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode)E_REMOTE_EXCEPTION == ec) {
                    Slogger::E(TAG, "error in notifyHdmiDeviceAdded %d", ec);
                }
                else
                    return ec;
            }
            // }
        }
    }
    return NOERROR;
}

ECode TvInputManagerService::HardwareListener::OnHdmiDeviceRemoved(
    /* [in] */ IHdmiDeviceInfo* deviceInfo)
{
    {    AutoLock syncLock(mHost->mLock);
        AutoPtr<UserState> userState;
        mHost->GetUserStateLocked(mHost->mCurrentUserId, (UserState**)&userState);
        // Broadcast the event to all hardware inputs.
        FOR_EACH(it, Ptr(userState->mServiceStateMap)->Func(IMap::GetValues)) {
            AutoPtr<ServiceState> serviceState = (ServiceState*) IObject::Probe(Ptr(it)->Func(IIterator::GetNext));
            if (!serviceState->mIsHardware || serviceState->mService == NULL) continue;
            // try {
            ECode ec = serviceState->mService->NotifyHdmiDeviceRemoved(deviceInfo);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode)E_REMOTE_EXCEPTION == ec) {
                    Slogger::E(TAG, "error in notifyHdmiDeviceRemoved %d", ec);
                }
                else
                    return ec;
            }
            // }
        }
    }
    return NOERROR;
}

ECode TvInputManagerService::HardwareListener::OnHdmiDeviceUpdated(
    /* [in] */ const String& inputId,
    /* [in] */ IHdmiDeviceInfo* deviceInfo)
{
    {    AutoLock syncLock(mHost->mLock);
        AutoPtr<IInteger32> state;
        switch (Ptr(deviceInfo)->Func(deviceInfo->GetDevicePowerStatus)) {
            case IHdmiControlManager::POWER_STATUS_ON:
                CInteger32::New(ITvInputManager::INPUT_STATE_CONNECTED, (IInteger32**)&state);
                break;
            case IHdmiControlManager::POWER_STATUS_STANDBY:
            case IHdmiControlManager::POWER_STATUS_TRANSIENT_TO_ON:
            case IHdmiControlManager::POWER_STATUS_TRANSIENT_TO_STANDBY:
                CInteger32::New(ITvInputManager::INPUT_STATE_CONNECTED_STANDBY, (IInteger32**)&state);
                break;
            case IHdmiControlManager::POWER_STATUS_UNKNOWN:
            default:
                state = NULL;
                break;
        }
        if (state != NULL) {
            mHost->SetStateLocked(inputId, Ptr(state)->Func(state->GetValue), mHost->mCurrentUserId);
        }
    }
    return NOERROR;
}

//=============================================================================
// TvInputManagerService::InnerSub_ProxyDeathRecipient
//=============================================================================
CAR_INTERFACE_IMPL(TvInputManagerService::InnerSub_ProxyDeathRecipient, Object, IProxyDeathRecipient)

TvInputManagerService::InnerSub_ProxyDeathRecipient::InnerSub_ProxyDeathRecipient(
    /* [in] */ IITvInputManagerCallback* callback,
    /* [in] */ IObject* lock,
    /* [in] */ UserState* userState)
    : mCallback(callback)
    , mLock(lock)
    , mUserState(userState)
{}

ECode TvInputManagerService::InnerSub_ProxyDeathRecipient::ProxyDied()
{
    {    AutoLock syncLock(mLock);
        if (mUserState->mCallbackSet != NULL) {
            mUserState->mCallbackSet->Remove(mCallback);
        }
    }
    return NOERROR;
}

//=============================================================================
// TvInputManagerService::InnerSub_PackageMonitor
//=============================================================================
TvInputManagerService::InnerSub_PackageMonitor::InnerSub_PackageMonitor(
    /* [in] */ TvInputManagerService* host)
    : mHost(host)
{}

ECode TvInputManagerService::InnerSub_PackageMonitor::BuildTvInputList(
    /* [in] */ ArrayOf<String>* packages)
{
    {    AutoLock syncLock(mHost->mLock);
        Int32 userId;
        GetChangingUserId(&userId);
        mHost->BuildTvInputListLocked(userId, packages);
        mHost->BuildTvContentRatingSystemListLocked(userId);
    }
    return NOERROR;
}

ECode TvInputManagerService::InnerSub_PackageMonitor::OnPackageUpdateFinished(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    if (DEBUG) Slogger::D(TAG, "onPackageUpdateFinished(packageName=%s)", packageName.string());
    // This callback is invoked when the TV input is reinstalled.
    // In this case, isReplacing() always returns true.
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = packageName;
    BuildTvInputList(array);
    return NOERROR;
}

ECode TvInputManagerService::InnerSub_PackageMonitor::OnPackagesAvailable(
    /* [in] */ ArrayOf<String>* packages)
{
    if (DEBUG) {
        Slogger::D(TAG, "onPackagesAvailable(packages=%s)", Arrays::ToString(packages).string());
    }
    // This callback is invoked when the media on which some packages exist become
    // available.
    Boolean isReplacing;
    IsReplacing(&isReplacing);
    if (isReplacing) {
        BuildTvInputList(packages);
    }
    return NOERROR;
}

ECode TvInputManagerService::InnerSub_PackageMonitor::OnPackagesUnavailable(
    /* [in] */ ArrayOf<String>* packages)
{
    // This callback is invoked when the media on which some packages exist become
    // unavailable.
    if (DEBUG)  {
        Slogger::D(TAG, "onPackagesUnavailable(packages=%s)", Arrays::ToString(packages).string());
    }
    Boolean isReplacing;
    IsReplacing(&isReplacing);
    if (isReplacing) {
        BuildTvInputList(packages);
    }
    return NOERROR;
}

ECode TvInputManagerService::InnerSub_PackageMonitor::OnSomePackagesChanged()
{
    // TODO: Use finer-grained methods(e.g. onPackageAdded, onPackageRemoved) to manage
    // the TV inputs.
    if (DEBUG) Slogger::D(TAG, "onSomePackagesChanged()");
    Boolean isReplacing;
    IsReplacing(&isReplacing);
    if (isReplacing) {
        if (DEBUG) Slogger::D(TAG, "Skipped building TV input list due to replacing");
        // When the package is updated, buildTvInputListLocked is called in other
        // methods instead.
        return NOERROR;
    }
    BuildTvInputList(NULL);
    return NOERROR;
}

ECode TvInputManagerService::InnerSub_PackageMonitor::OnPackageRemoved(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    {    AutoLock syncLock(mHost->mLock);
        Int32 userId;
        GetChangingUserId(&userId);
        AutoPtr<UserState> userState;
        mHost->GetUserStateLocked(userId, (UserState**)&userState);
        Boolean isContains;
        userState->mPackageSet->Contains(StringUtils::ParseCharSequence(packageName), &isContains);
        if (!isContains) {
            // Not a TV input package.
            return NOERROR;
        }
    }
    AutoPtr<IArrayList> operations;
    CArrayList::New((IArrayList**)&operations);
    String selection = ITvContractBaseTvColumns::COLUMN_PACKAGE_NAME + "=?";
    AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
    (*selectionArgs)[0] = packageName;
    AutoPtr<IContentProviderOperationHelper> helper;
    CContentProviderOperationHelper::AcquireSingleton((IContentProviderOperationHelper**)&helper);
    AutoPtr<IContentProviderOperationBuilder> builder;
    AutoPtr<ITvContractWatchedPrograms> program_helper;
    CTvContractWatchedPrograms::AcquireSingleton((ITvContractWatchedPrograms**)&program_helper);
    helper->NewDelete(Ptr(program_helper)->Func(ITvContractWatchedPrograms::GetCONTENT_URI), (IContentProviderOperationBuilder**)&builder);
    builder->WithSelection(selection, selectionArgs);
    operations->Add(Ptr(builder)->Func(IContentProviderOperationBuilder::Build));
    builder = NULL;
    helper->NewDelete(Ptr(program_helper)->Func(ITvContractWatchedPrograms::GetCONTENT_URI), (IContentProviderOperationBuilder**)&builder);
    builder->WithSelection(selection, selectionArgs);
    operations->Add(Ptr(builder)->Func(IContentProviderOperationBuilder::Build));
    builder = NULL;
    helper->NewDelete(Ptr(program_helper)->Func(ITvContractWatchedPrograms::GetCONTENT_URI), (IContentProviderOperationBuilder**)&builder);
    builder->WithSelection(selection, selectionArgs);
    operations->Add(Ptr(builder)->Func(IContentProviderOperationBuilder::Build));
    AutoPtr<ArrayOf<IContentProviderResult*> > results;
    // try {
    ECode ec = mHost->mContentResolver->ApplyBatch(ITvContract::AUTHORITY, operations, (ArrayOf<IContentProviderResult*>**)&results);
    // } catch (RemoteException | OperationApplicationException e) {
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec || (ECode)E_OPERATION_APPLICATION_EXCEPTION == ec) {
            Slogger::E(TAG, "error in applyBatch %d", ec);
        }
        else
            return ec;
    }
    // }
    if (DEBUG) {
        Slogger::D(TAG, "onPackageRemoved(packageName=%s, uid=%d)", packageName.string(), uid);
        Slogger::D(TAG, "results=%s", Arrays::ToString(results).string());
    }
    return NOERROR;
}

//=============================================================================
// TvInputManagerService::InnerSub_BroadcastReceiver
//=============================================================================
TvInputManagerService::InnerSub_BroadcastReceiver::InnerSub_BroadcastReceiver(
    /* [in] */ TvInputManagerService* host)
    : mHost(host)
{}

ECode TvInputManagerService::InnerSub_BroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Int32 i32;
    if (IIntent::ACTION_USER_SWITCHED.Equals(action)) {
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, 0, &i32);
        mHost->SwitchUser(i32);
    } else if (IIntent::ACTION_USER_REMOVED.Equals(action)) {
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, 0, &i32);
        mHost->RemoveUser(i32);
    }
    return NOERROR;
}

//=============================================================================
// TvInputManagerService
//=============================================================================
const Boolean TvInputManagerService::DEBUG = FALSE;
const String TvInputManagerService::TAG("TvInputManagerService");

TvInputManagerService::TvInputManagerService()
    : mCurrentUserId(IUserHandle::USER_OWNER)
{
    Elastos::Core::CObject::New((IObject**)&mLock);
    CSparseArray::New((ISparseArray**)&mUserStates);
}

ECode TvInputManagerService::constructor(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    mContext = context;
    context->GetContentResolver((IContentResolver**)&mContentResolver);
    mWatchLogHandler = new WatchLogHandler();
    AutoPtr<ILooper> looper;
    IoThread::Get()->GetLooper((ILooper**)&looper);
    mWatchLogHandler->constructor(mContentResolver, looper);
    mTvInputHardwareManager = new TvInputHardwareManager();
    mTvInputHardwareManager->constructor(context, new HardwareListener(this));
    {    AutoLock syncLock(mLock);
        mUserStates->Put(mCurrentUserId, TO_IINTERFACE(new UserState(mContext, mCurrentUserId)));
    }
    return NOERROR;
}

ECode TvInputManagerService::OnStart()
{
    PublishBinderService(IContext::TV_INPUT_SERVICE, new BinderService(this));
    return NOERROR;
}

ECode TvInputManagerService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == ISystemService::PHASE_SYSTEM_SERVICES_READY) {
        RegisterBroadcastReceivers();
    } else if (phase == ISystemService::PHASE_THIRD_PARTY_APPS_CAN_START) {
        {    AutoLock syncLock(mLock);
            BuildTvInputListLocked(mCurrentUserId, NULL);
            BuildTvContentRatingSystemListLocked(mCurrentUserId);
        }
    }
    mTvInputHardwareManager->OnBootPhase(phase);
    return NOERROR;
}

ECode TvInputManagerService::RegisterBroadcastReceivers()
{
    AutoPtr<IPackageMonitor> monitor = new InnerSub_PackageMonitor(this);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> userHandleAll;
    helper->GetALL((IUserHandle**)&userHandleAll);
    monitor->Register(mContext, NULL, userHandleAll, TRUE);
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_USER_SWITCHED);
    intentFilter->AddAction(IIntent::ACTION_USER_REMOVED);
    AutoPtr<IIntent> iNoUse;
    mContext->RegisterReceiverAsUser(new InnerSub_BroadcastReceiver(this),
            userHandleAll, intentFilter, String(NULL), NULL, (IIntent**)&iNoUse);
    return NOERROR;
}

Boolean TvInputManagerService::HasHardwarePermission(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IComponentName* component)
{
    Int32 permissionCode;
    pm->CheckPermission(Manifest::permission::TV_INPUT_HARDWARE,
            Ptr(component)->Func(component->GetPackageName), &permissionCode);
    return permissionCode == IPackageManager::PERMISSION_GRANTED;
}

ECode TvInputManagerService::BuildTvInputListLocked(
    /* [in] */ Int32 userId,
    /* [in] */ ArrayOf<String>* updatedPackages)
{
    AutoPtr<UserState> userState;
    GetUserStateLocked(userId, (UserState**)&userState);
    userState->mPackageSet->Clear();
    if (DEBUG) Slogger::D(TAG, "buildTvInputList");
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IIntent> intent;
    CIntent::New(ITvInputService::SERVICE_INTERFACE, (IIntent**)&intent);
    AutoPtr<IList> services;
    pm->QueryIntentServices(intent,
            IPackageManager::GET_SERVICES | IPackageManager::GET_META_DATA, (IList**)&services);
    AutoPtr<IList> inputList;
    CArrayList::New((IList**)&inputList);
    FOR_EACH(it, services) {
        AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(Ptr(it)->Func(IIterator::GetNext));
        AutoPtr<IServiceInfo> si;
        ri->GetServiceInfo((IServiceInfo**)&si);
        if (!Manifest::permission::BIND_TV_INPUT.Equals(Ptr(si)->Func(IServiceInfo::GetPermission))) {
            Slogger::W(TAG, "Skipping TV input %s: it does not require the permission %s",
                    Ptr(IPackageItemInfo::Probe(si))->Func(IPackageItemInfo::GetName).string(),
                    Manifest::permission::BIND_TV_INPUT.string());
            continue;
        }
        AutoPtr<IComponentName> component;
        CComponentName::New(Ptr(IPackageItemInfo::Probe(si))->Func(IPackageItemInfo::GetPackageName),
                Ptr(IPackageItemInfo::Probe(si))->Func(IPackageItemInfo::GetName), (IComponentName**)&component);
        if (HasHardwarePermission(pm, component)) {
            AutoPtr<IInterface> obj;
            userState->mServiceStateMap->Get(component, (IInterface**)&obj);
            AutoPtr<ServiceState> serviceState = (ServiceState*) IObject::Probe(obj);
            if (serviceState == NULL) {
                // We see this hardware TV input service for the first time; we need to
                // prepare the ServiceState object so that we can connect to the service and
                // let it add TvInputInfo objects to mInputList if there's any.
                serviceState = new ServiceState(this, component, userId);
                userState->mServiceStateMap->Put(component, TO_IINTERFACE(serviceState));
                UpdateServiceConnectionLocked(component, userId);
            } else {
                inputList->AddAll(ICollection::Probe(serviceState->mInputList));
            }
        } else {
            // try {
            ECode ec;
            AutoPtr<ITvInputInfo> info;
            do {
                AutoPtr<ITvInputInfoHelper> helper;
                CTvInputInfoHelper::AcquireSingleton((ITvInputInfoHelper**)&helper);
                ec = helper->CreateTvInputInfo(mContext, ri, (ITvInputInfo**)&info);
                if (FAILED(ec)) break;
                ec = inputList->Add(info);
            } while(FALSE);
            // } catch (XmlPullParserException | IOException e) {
            if (FAILED(ec)) {
                if ((ECode)E_XML_PULL_PARSER_EXCEPTION == ec || (ECode)E_IO_EXCEPTION == ec) {
                    Slogger::E(TAG, "failed to load TV input %s %d", Ptr(IPackageItemInfo::Probe(si))->Func(IPackageItemInfo::GetName).string(), ec);
                    continue;
                }
                else
                    return ec;
            }
            // }
        }
        userState->mPackageSet->Add(StringUtils::ParseCharSequence(Ptr(IPackageItemInfo::Probe(si))->Func(IPackageItemInfo::GetPackageName)));
    }
    AutoPtr<IMap> inputMap;
    CHashMap::New((IMap**)&inputMap);
    FOR_EACH(iter, inputList) {
        AutoPtr<ITvInputInfo> info = ITvInputInfo::Probe(Ptr(iter)->Func(IIterator::GetNext));
        if (DEBUG) {
            Slogger::D(TAG, "add %s", Ptr(info)->Func(info->GetId).string());
        }
        AutoPtr<IInterface> obj;
        userState->mInputMap->Get(StringUtils::ParseCharSequence(Ptr(info)->Func(info->GetId)), (IInterface**)&obj);
        AutoPtr<TvInputState> state = (TvInputState*) IObject::Probe(obj);
        if (state == NULL) {
            state = new TvInputState();
        }
        state->mInfo = info;
        inputMap->Put(StringUtils::ParseCharSequence(Ptr(info)->Func(info->GetId)), TO_IINTERFACE(state));
    }
    FOR_EACH(id_it, Ptr(inputMap)->Func(inputMap->GetKeySet)) {
        AutoPtr<ICharSequence> inputId = ICharSequence::Probe(Ptr(id_it)->Func(IIterator::GetNext));
        Boolean isContains;
        userState->mInputMap->ContainsKey(inputId, &isContains);
        if (!isContains) {
            NotifyInputAddedLocked(userState, Object::ToString(inputId));
        } else if (updatedPackages != NULL) {
            // Notify the package updates
            AutoPtr<IInterface> obj;
            inputMap->Get(inputId, (IInterface**)&obj);
            AutoPtr<IComponentName> component;
            ((TvInputState*) IObject::Probe(obj))->mInfo->GetComponent((IComponentName**)&component);
            for (Int32 i = 0; i < updatedPackages->GetLength(); ++i) {
                AutoPtr<ICharSequence> updatedPackage;
                CString::New((*updatedPackages)[i], (ICharSequence**)&updatedPackage);
                if (Ptr(component)->Func(component->GetPackageName).Equals(Object::ToString(updatedPackage))) {
                    UpdateServiceConnectionLocked(component, userId);
                    NotifyInputUpdatedLocked(userState, Object::ToString(inputId));
                    break;
                }
            }
        }
    }
    FOR_EACH(input_id_it, Ptr(userState->mInputMap)->Func(IMap::GetKeySet)) {
        AutoPtr<ICharSequence> inputId = ICharSequence::Probe(Ptr(input_id_it)->Func(IIterator::GetNext));
        Boolean isContains;
        inputMap->ContainsKey(inputId, &isContains);
        if (!isContains) {
            AutoPtr<IInterface> obj;
            userState->mInputMap->Get(inputId, (IInterface**)&obj);
            AutoPtr<ITvInputInfo> info = ((TvInputState*) IObject::Probe(obj))->mInfo;
            obj = NULL;
            userState->mServiceStateMap->Get(Ptr(info)->Func(info->GetComponent), (IInterface**)&obj);
            AutoPtr<ServiceState> serviceState = (ServiceState*) IObject::Probe(obj);
            if (serviceState != NULL) {
                AbortPendingCreateSessionRequestsLocked(serviceState, Object::ToString(inputId), userId);
            }
            NotifyInputRemovedLocked(userState, Object::ToString(inputId));
        }
    }
    userState->mInputMap->Clear();
    userState->mInputMap = inputMap;
    return NOERROR;
}

ECode TvInputManagerService::BuildTvContentRatingSystemListLocked(
    /* [in] */ Int32 userId)
{
    AutoPtr<UserState> userState;
    GetUserStateLocked(userId, (UserState**)&userState);
    userState->mContentRatingSystemList->Clear();
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IIntent> intent;
    CIntent::New(ITvInputManager::ACTION_QUERY_CONTENT_RATING_SYSTEMS, (IIntent**)&intent);
    AutoPtr<IList> receivers;
    pm->QueryBroadcastReceivers(intent, IPackageManager::GET_META_DATA, (IList**)&receivers);
    FOR_EACH(it, receivers) {
        AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(Ptr(it)->Func(IIterator::GetNext));
        AutoPtr<IActivityInfo> receiver;
        resolveInfo->GetActivityInfo((IActivityInfo**)&receiver);
        AutoPtr<IBundle> metaData;
        IPackageItemInfo::Probe(receiver)->GetMetaData((IBundle**)&metaData);
        if (metaData == NULL) {
            continue;
        }
        Int32 xmlResId;
        metaData->GetInt32(ITvInputManager::META_DATA_CONTENT_RATING_SYSTEMS, &xmlResId);
        if (xmlResId == 0) {
            Slogger::W(TAG, "Missing meta-data '%s' on receiver %s/%s",
                    ITvInputManager::META_DATA_CONTENT_RATING_SYSTEMS.string(),
                    Ptr(IPackageItemInfo::Probe(receiver))->Func(IPackageItemInfo::GetPackageName).string(),
                    Ptr(IPackageItemInfo::Probe(receiver))->Func(IPackageItemInfo::GetName).string());
            continue;
        }
        AutoPtr<ITvContentRatingSystemInfoHelper> helper;
        CTvContentRatingSystemInfoHelper::AcquireSingleton((ITvContentRatingSystemInfoHelper**)&helper);
        AutoPtr<ITvContentRatingSystemInfo> info;
        helper->CreateTvContentRatingSystemInfo(xmlResId,
                        Ptr(IComponentInfo::Probe(receiver))->Func(IComponentInfo::GetApplicationInfo),
                        (ITvContentRatingSystemInfo**)&info);
        userState->mContentRatingSystemList->Add(info);
    }
    return NOERROR;
}

ECode TvInputManagerService::SwitchUser(
    /* [in] */ Int32 userId)
{
    {    AutoLock syncLock(mLock);
        if (mCurrentUserId == userId) {
            return NOERROR;
        }
        // final int oldUserId = mCurrentUserId;
        // TODO: Release services and sessions in the old user state, if needed.
        mCurrentUserId = userId;
        AutoPtr<IInterface> obj;
        mUserStates->Get(userId, (IInterface**)&obj);
        AutoPtr<UserState> userState = (UserState*) IObject::Probe(obj);
        if (userState == NULL) {
            userState = new UserState(mContext, userId);
        }
        mUserStates->Put(userId, TO_IINTERFACE(userState));
        BuildTvInputListLocked(userId, NULL);
        BuildTvContentRatingSystemListLocked(userId);
    }
    return NOERROR;
}

ECode TvInputManagerService::RemoveUser(
    /* [in] */ Int32 userId)
{
    {    AutoLock syncLock(mLock);
        AutoPtr<IInterface> obj;
        mUserStates->Get(userId, (IInterface**)&obj);
        AutoPtr<UserState> userState = (UserState*) IObject::Probe(obj);
        if (userState == NULL) {
            return NOERROR;
        }
        // Release created sessions.
        FOR_EACH(it, Ptr(userState->mSessionStateMap)->Func(IMap::GetValues)) {
            AutoPtr<SessionState> state = (SessionState*) IObject::Probe(Ptr(it)->Func(IIterator::GetNext));
            if (state->mSession != NULL) {
                // try {
                ECode ec = state->mSession->Release();
                // } catch (RemoteException e) {
                if (FAILED(ec)) {
                    if ((ECode)E_REMOTE_EXCEPTION == ec) {
                        Slogger::E(TAG, "error in release %d", ec);
                    }
                    else
                        return ec;
                }
                // }
            }
        }
        userState->mSessionStateMap->Clear();
        // Unregister all callbacks and unbind all services.
        FOR_EACH(iter, Ptr(userState->mServiceStateMap)->Func(IMap::GetValues)) {
            AutoPtr<ServiceState> serviceState = (ServiceState*) IObject::Probe(Ptr(iter)->Func(IIterator::GetNext));
            if (serviceState->mCallback != NULL) {
                // try {
                ECode ec = serviceState->mService->UnregisterCallback(serviceState->mCallback);
                // } catch (RemoteException e) {
                if (FAILED(ec)) {
                    if ((ECode)E_REMOTE_EXCEPTION == ec) {
                        Slogger::E(TAG, "error in unregisterCallback %d", ec);
                    }
                    else
                        return ec;
                }
                // }
            }
            mContext->UnbindService(serviceState->mConnection);
        }
        userState->mServiceStateMap->Clear();
        // Clear everything else.
        userState->mInputMap->Clear();
        userState->mPackageSet->Clear();
        userState->mContentRatingSystemList->Clear();
        userState->mClientStateMap->Clear();
        userState->mCallbackSet->Clear();
        userState->mMainSessionToken = NULL;
        mUserStates->Remove(userId);
    }
    return NOERROR;
}

ECode TvInputManagerService::GetUserStateLocked(
    /* [in] */ Int32 userId,
    /* [out] */ UserState** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IInterface> obj;
    mUserStates->Get(userId, (IInterface**)&obj);
    AutoPtr<UserState> userState = (UserState*) IObject::Probe(obj);
    if (userState == NULL) {
        Logger::E(TAG, "User state not found for user ID %d", userId);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    FUNC_RETURN(userState)
}

ECode TvInputManagerService::GetServiceStateLocked(
    /* [in] */ IComponentName* component,
    /* [in] */ Int32 userId,
    /* [out] */ ServiceState** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<UserState> userState;
    GetUserStateLocked(userId, (UserState**)&userState);
    AutoPtr<IInterface> obj;
    userState->mServiceStateMap->Get(component, (IInterface**)&obj);
    AutoPtr<ServiceState> serviceState = (ServiceState*) IObject::Probe(obj);
    if (serviceState == NULL) {
        Logger::E(TAG, "Service state not found for %s (userId=%d)",
                TO_CSTR(component), userId);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    FUNC_RETURN(serviceState)
}

ECode TvInputManagerService::GetSessionStateLocked(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 userId,
    /* [out] */ SessionState** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<UserState> userState;
    GetUserStateLocked(userId, (UserState**)&userState);
    AutoPtr<IInterface> obj;
    userState->mSessionStateMap->Get(sessionToken, (IInterface**)&obj);
    AutoPtr<SessionState> sessionState = (SessionState*) IObject::Probe(obj);
    if (sessionState == NULL) {
        Logger::E(TAG, "Session state not found for token %s", TO_CSTR(sessionToken));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // Only the application that requested this session or the system can access it.
    if (callingUid != IProcess::SYSTEM_UID && callingUid != sessionState->mCallingUid) {
        Logger::E(TAG, "Illegal access to the session with token %s from uid %d",
                TO_CSTR(sessionToken), callingUid);
        return E_SECURITY_EXCEPTION;
    }
    FUNC_RETURN(sessionState)
}

ECode TvInputManagerService::GetSessionLocked(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 userId,
    /* [out] */ IITvInputSession** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<SessionState> stateLocked;
    GetSessionStateLocked(sessionToken, callingUid, userId, (SessionState**)&stateLocked);
    return GetSessionLocked(stateLocked, result);
}

ECode TvInputManagerService::GetSessionLocked(
    /* [in] */ SessionState* sessionState,
    /* [out] */ IITvInputSession** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IITvInputSession> session = sessionState->mSession;
    if (session == NULL) {
        Logger::E(TAG, "Session not yet created for token %s",
                TO_CSTR(sessionState->mSessionToken));
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    FUNC_RETURN(session)
}

ECode TvInputManagerService::ResolveCallingUserId(
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 requestedUserId,
    /* [in] */ const String& methodName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    return helper->HandleIncomingUser(callingPid, callingUid, requestedUserId, FALSE,
            FALSE, methodName, String(NULL), result);
}

Boolean TvInputManagerService::ShouldMaintainConnection(
    /* [in] */ ServiceState* serviceState)
{
    Boolean isEmpty;
    serviceState->mSessionTokens->IsEmpty(&isEmpty);
    return !isEmpty || serviceState->mIsHardware;
    // TODO: Find a way to maintain connection to hardware TV input service only when necessary.
}

ECode TvInputManagerService::UpdateServiceConnectionLocked(
    /* [in] */ IComponentName* component,
    /* [in] */ Int32 userId)
{
    AutoPtr<UserState> userState;
    GetUserStateLocked(userId, (UserState**)&userState);
    AutoPtr<IInterface> obj;
    userState->mServiceStateMap->Get(component, (IInterface**)&obj);
    AutoPtr<ServiceState> serviceState = (ServiceState*) IObject::Probe(obj);
    if (serviceState == NULL) {
        return NOERROR;
    }
    if (serviceState->mReconnecting) {
        if (!Ptr(serviceState->mSessionTokens)->Func(IList::IsEmpty)) {
            // wait until all the sessions are removed.
            return NOERROR;
        }
        serviceState->mReconnecting = FALSE;
    }
    Boolean maintainConnection = ShouldMaintainConnection(serviceState);
    if (serviceState->mService == NULL && maintainConnection && userId == mCurrentUserId) {
        // This means that the service is not yet connected but its state indicates that we
        // have pending requests. Then, connect the service.
        if (serviceState->mBound) {
            // We have already bound to the service so we don't try to bind again until after we
            // unbind later on.
            return NOERROR;
        }
        if (DEBUG) {
            Slogger::D(TAG, "bindServiceAsUser(service=%s, userId=%d)", TO_CSTR(component), userId);
        }
        AutoPtr<IIntent> i;
        CIntent::New(ITvInputService::SERVICE_INTERFACE, (IIntent**)&i);
        i->SetComponent(component);
        AutoPtr<IUserHandle> newUserHandle;
        CUserHandle::New(userId, (IUserHandle**)&newUserHandle);
        mContext->BindServiceAsUser(
                i, serviceState->mConnection, IContext::BIND_AUTO_CREATE, newUserHandle, &(serviceState->mBound));
    } else if (serviceState->mService != NULL && !maintainConnection) {
        // This means that the service is already connected but its state indicates that we have
        // nothing to do with it. Then, disconnect the service.
        if (DEBUG) {
            Slogger::D(TAG, "unbindService(service=%s)", TO_CSTR(component));
        }
        mContext->UnbindService(serviceState->mConnection);
        userState->mServiceStateMap->Remove(component);
    }
    return NOERROR;
}

ECode TvInputManagerService::AbortPendingCreateSessionRequestsLocked(
    /* [in] */ ServiceState* serviceState,
    /* [in] */ const String& inputId,
    /* [in] */ Int32 userId)
{
    // Let clients know the create session requests are failed.
    AutoPtr<UserState> userState;
    GetUserStateLocked(userId, (UserState**)&userState);
    AutoPtr<IList> sessionsToAbort;
    CArrayList::New((IList**)&sessionsToAbort);
    FOR_EACH(it, serviceState->mSessionTokens) {
        AutoPtr<IBinder> sessionToken = IBinder::Probe(Ptr(it)->Func(IIterator::GetNext));
        AutoPtr<IInterface> obj;
        userState->mSessionStateMap->Get(sessionToken, (IInterface**)&obj);
        AutoPtr<SessionState> sessionState = (SessionState*) IObject::Probe(obj);
        if (sessionState->mSession == NULL && (inputId == NULL
                || Ptr(sessionState->mInfo)->Func(ITvInputInfo::GetId).Equals(inputId))) {
            sessionsToAbort->Add(TO_IINTERFACE(sessionState));
        }
    }
    FOR_EACH(iter, sessionsToAbort) {
        AutoPtr<SessionState> sessionState = (SessionState*) IObject::Probe(Ptr(iter)->Func(IIterator::GetNext));
        RemoveSessionStateLocked(sessionState->mSessionToken, sessionState->mUserId);
        SendSessionTokenToClientLocked(sessionState->mClient,
                Ptr(sessionState->mInfo)->Func(ITvInputInfo::GetId), NULL, NULL, sessionState->mSeq);
    }
    UpdateServiceConnectionLocked(serviceState->mComponent, userId);
    return NOERROR;
}

ECode TvInputManagerService::CreateSessionInternalLocked(
    /* [in] */ IITvInputService* service,
    /* [in] */ IBinder* sessionToken,
    /* [in] */ Int32 userId)
{
    AutoPtr<UserState> userState;
    GetUserStateLocked(userId, (UserState**)&userState);
    AutoPtr<IInterface> obj;
    userState->mSessionStateMap->Get(sessionToken, (IInterface**)&obj);
    AutoPtr<SessionState> sessionState = (SessionState*) IObject::Probe(obj);
    if (DEBUG) {
        Slogger::D(TAG, "createSessionInternalLocked(inputId=%s)", Ptr(sessionState->mInfo)->Func(ITvInputInfo::GetId).string());
    }
    AutoPtr<IInputChannelHelper> helper;
    CInputChannelHelper::AcquireSingleton((IInputChannelHelper**)&helper);
    AutoPtr<ArrayOf<IInputChannel*> > channels;
    helper->OpenInputChannelPair(Ptr(sessionToken)->Func(sessionToken->ToString), (ArrayOf<IInputChannel*>**)&channels);
    // Set up a callback to send the session token.
    AutoPtr<IITvInputSessionCallback> callback = new SessionCallback(this);
    ((SessionCallback*) callback.Get())->constructor(sessionState, channels);
    // Create a session. When failed, send a null token immediately.
    // try {
    ECode ec = service->CreateSession((*channels)[1], callback, Ptr(sessionState->mInfo)->Func(ITvInputInfo::GetId));
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec) {
            Slogger::E(TAG, "error in createSession %d", ec);
            RemoveSessionStateLocked(sessionToken, userId);
            SendSessionTokenToClientLocked(sessionState->mClient, Ptr(sessionState->mInfo)->Func(ITvInputInfo::GetId), NULL,
                    NULL, sessionState->mSeq);
        }
    }
    // }
    (*channels)[1]->Dispose();
    return NOERROR;
}

ECode TvInputManagerService::SendSessionTokenToClientLocked(
    /* [in] */ IITvInputClient* client,
    /* [in] */ const String& inputId,
    /* [in] */ IBinder* sessionToken,
    /* [in] */ IInputChannel* channel,
    /* [in] */ Int32 seq)
{
    // try {
    ECode ec = client->OnSessionCreated(inputId, sessionToken, channel, seq);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec) {
            Slogger::E(TAG, "error in onSessionCreated %d", ec);
        }
        else
            return ec;
    }
    // }
    return NOERROR;
}

ECode TvInputManagerService::ReleaseSessionLocked(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 userId)
{
    AutoPtr<SessionState> sessionState;
    GetSessionStateLocked(sessionToken, callingUid, userId, (SessionState**)&sessionState);
    if (sessionState->mSession != NULL) {
        AutoPtr<UserState> userState;
        GetUserStateLocked(userId, (UserState**)&userState);
        if (sessionToken == userState->mMainSessionToken) {
            SetMainLocked(sessionToken, FALSE, callingUid, userId);
        }
        // try {
        ECode ec = sessionState->mSession->Release();
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "session process has already died %d", ec);
            }
            else
                return ec;
        }
        // }
        sessionState->mSession = NULL;
    }
    RemoveSessionStateLocked(sessionToken, userId);
    return NOERROR;
}

ECode TvInputManagerService::RemoveSessionStateLocked(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ Int32 userId)
{
    AutoPtr<UserState> userState;
    GetUserStateLocked(userId, (UserState**)&userState);
    if (sessionToken == userState->mMainSessionToken) {
        if (DEBUG) {
            Slogger::D(TAG, "mainSessionToken=null");
        }
        userState->mMainSessionToken = NULL;
    }
    // Remove the session state from the global session state map of the current user.
    AutoPtr<IInterface> obj;
    userState->mSessionStateMap->Remove(sessionToken, (IInterface**)&obj);
    AutoPtr<SessionState> sessionState = (SessionState*) IObject::Probe(obj);
    if (sessionState == NULL) {
        return NOERROR;
    }
    // Also remove the session token from the session token list of the current client and
    // service.
    obj = NULL;
    userState->mClientStateMap->Get(IBinder::Probe(sessionState->mClient), (IInterface**)&obj);
    AutoPtr<ClientState> clientState = (ClientState*) IObject::Probe(obj);
    if (clientState != NULL) {
        clientState->mSessionTokens->Remove(sessionToken);
        if (Ptr(clientState)->Func(clientState->IsEmpty)) {
            userState->mClientStateMap->Remove(IBinder::Probe(sessionState->mClient));
        }
    }
    AutoPtr<ITvInputInfo> info = sessionState->mInfo;
    if (info != NULL) {
        obj = NULL;
        userState->mServiceStateMap->Get(Ptr(info)->Func(info->GetComponent), (IInterface**)&obj);
        AutoPtr<ServiceState> serviceState = (ServiceState*) IObject::Probe(obj);
        if (serviceState != NULL) {
            serviceState->mSessionTokens->Remove(sessionToken);
        }
    }
    UpdateServiceConnectionLocked(Ptr(sessionState->mInfo)->Func(ITvInputInfo::GetComponent), userId);
    // Log the end of watch.
    AutoPtr<ISomeArgsHelper> helper;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&helper);
    AutoPtr<ISomeArgs> args;
    helper->Obtain((ISomeArgs**)&args);
    args->SetObjectArg(1, sessionToken);
    AutoPtr<ISystem> system_helper;
    CSystem::AcquireSingleton((ISystem**)&system_helper);
    args->SetInt32Arg(2, Ptr(system_helper)->Func(ISystem::GetCurrentTimeMillis));
    AutoPtr<IMessage> msg;
    mWatchLogHandler->ObtainMessage(WatchLogHandler::MSG_LOG_WATCH_END, args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode TvInputManagerService::SetMainLocked(
    /* [in] */ IBinder* sessionToken,
    /* [in] */ Boolean isMain,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 userId)
{
    AutoPtr<SessionState> sessionState;
    GetSessionStateLocked(sessionToken, callingUid, userId, (SessionState**)&sessionState);
    if (sessionState->mHardwareSessionToken != NULL) {
        GetSessionStateLocked(sessionState->mHardwareSessionToken,
                IProcess::SYSTEM_UID, userId, (SessionState**)&sessionState);
    }
    AutoPtr<ServiceState> serviceState;
    GetServiceStateLocked(Ptr(sessionState->mInfo)->Func(ITvInputInfo::GetComponent), userId, (ServiceState**)&serviceState);
    if (!serviceState->mIsHardware) {
        return NOERROR;
    }
    AutoPtr<IITvInputSession> session;
    GetSessionLocked(sessionState, (IITvInputSession**)&session);
    // try {
    ECode ec = session->SetMain(isMain);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec) {
            Slogger::E(TAG, "error in setMain %d", ec);
        }
        else
            return ec;
    }
    // }
    return NOERROR;
}

ECode TvInputManagerService::NotifyInputAddedLocked(
    /* [in] */ UserState* userState,
    /* [in] */ const String& inputId)
{
    if (DEBUG) {
        Slogger::D(TAG, "notifyInputAddedLocked(inputId=%s)", inputId.string());
    }
    FOR_EACH(it, userState->mCallbackSet) {
        AutoPtr<IITvInputManagerCallback> callback = IITvInputManagerCallback::Probe(Ptr(it)->Func(IIterator::GetNext));
        // try {
        ECode ec = callback->OnInputAdded(inputId);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "failed to report added input to callback %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode TvInputManagerService::NotifyInputRemovedLocked(
    /* [in] */ UserState* userState,
    /* [in] */ const String& inputId)
{
    if (DEBUG) {
        Slogger::D(TAG, "notifyInputRemovedLocked(inputId=%s)", inputId.string());
    }
    FOR_EACH(it, userState->mCallbackSet) {
        AutoPtr<IITvInputManagerCallback> callback = IITvInputManagerCallback::Probe(Ptr(it)->Func(IIterator::GetNext));
        // try {
        ECode ec = callback->OnInputRemoved(inputId);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "failed to report removed input to callback %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode TvInputManagerService::NotifyInputUpdatedLocked(
    /* [in] */ UserState* userState,
    /* [in] */ const String& inputId)
{
    if (DEBUG) {
        Slogger::D(TAG, "notifyInputUpdatedLocked(inputId=%s)", inputId.string());
    }
    FOR_EACH(it, userState->mCallbackSet) {
        AutoPtr<IITvInputManagerCallback> callback = IITvInputManagerCallback::Probe(Ptr(it)->Func(IIterator::GetNext));
        // try {
        ECode ec = callback->OnInputUpdated(inputId);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "failed to report updated input to callback %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode TvInputManagerService::NotifyInputStateChangedLocked(
    /* [in] */ UserState* userState,
    /* [in] */ const String& inputId,
    /* [in] */ Int32 state,
    /* [in] */ IITvInputManagerCallback* targetCallback)
{
    if (DEBUG) {
        Slogger::D(TAG, "notifyInputStateChangedLocked(inputId=%s, state=%d)",
                inputId.string(), state);
    }
    if (targetCallback == NULL) {
    FOR_EACH(it, userState->mCallbackSet) {
        AutoPtr<IITvInputManagerCallback> callback = IITvInputManagerCallback::Probe(Ptr(it)->Func(IIterator::GetNext));
            // try {
            ECode ec = callback->OnInputStateChanged(inputId, state);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode)E_REMOTE_EXCEPTION == ec) {
                    Slogger::E(TAG, "failed to report state change to callback %d", ec);
                }
                else
                    return ec;
            }
            // }
        }
    } else {
        // try {
        ECode ec = targetCallback->OnInputStateChanged(inputId, state);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "failed to report state change to callback %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode TvInputManagerService::SetStateLocked(
    /* [in] */ const String& inputId,
    /* [in] */ Int32 state,
    /* [in] */ Int32 userId)
{
    AutoPtr<UserState> userState;
    GetUserStateLocked(userId, (UserState**)&userState);
    AutoPtr<IInterface> obj;
    userState->mInputMap->Get(StringUtils::ParseCharSequence(inputId), (IInterface**)&obj);
    AutoPtr<TvInputState> inputState = (TvInputState*) IObject::Probe(obj);
    obj = NULL;
    userState->mServiceStateMap->Get(Ptr(inputState->mInfo)->Func(ITvInputInfo::GetComponent), (IInterface**)&obj);
    AutoPtr<ServiceState> serviceState = (ServiceState*) IObject::Probe(obj);
    Int32 oldState = inputState->mState;
    inputState->mState = state;
    if (serviceState != NULL && serviceState->mService == NULL
            && ShouldMaintainConnection(serviceState)) {
        // We don't notify state change while reconnecting. It should remain disconnected.
        return NOERROR;
    }
    if (oldState != state) {
        NotifyInputStateChangedLocked(userState, inputId, state, NULL);
    }
    return NOERROR;
}

} // namespace Tv
} // namespace Server
} // namespace Droid
} // namespace Elastos
