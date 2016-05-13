
#include "elastos/droid/server/am/CPendingIntentRecord.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/am/ActivityStack.h"
#include "Elastos.Droid.App.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::EIID_IIIntentSender;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

const Int32 CPendingIntentRecord::Key::ODD_PRIME_NUMBER;

CPendingIntentRecord::Key::Key(
    /* [in] */ Int32 t,
    /* [in] */ const String& p,
    /* [in] */ ActivityRecord * a,
    /* [in] */ const String& w,
    /* [in] */ Int32 r,
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ ArrayOf<String>* strings,
    /* [in] */ Int32 f,
    /* [in] */ IBundle* o,
    /* [in] */ Int32 userId)
    : mType(t)
    , mPackageName(p)
    , mActivity(a)
    , mWho(w)
    , mRequestCode(r)
    , mOptions(o)
    , mFlags(f)
    , mUserId(0)
{
    if (intents != NULL && intents->GetLength() > 0) {
        mRequestIntent = (*intents)[intents->GetLength() - 1];
        mAllIntents = intents;
    }

    if (strings != NULL && strings->GetLength() > 0) {
        mRequestResolvedType = (*strings)[strings->GetLength() - 1];
        mAllResolvedTypes = strings;
    }

    Int32 hash = 23;
    hash = (ODD_PRIME_NUMBER*hash) + f;
    hash = (ODD_PRIME_NUMBER*hash) + r;
    hash = (ODD_PRIME_NUMBER*hash) + userId;
    if (!w.IsNull()) {
        hash = (ODD_PRIME_NUMBER*hash) + w.GetHashCode();
    }

    if (a != NULL) {
        hash = (ODD_PRIME_NUMBER*hash) + (Int32)a;
    }

    if (mRequestIntent != NULL) {
        Int32 riHashCode;
        mRequestIntent->FilterHashCode(&riHashCode);
        hash = (ODD_PRIME_NUMBER*hash) + riHashCode;
    }

    if (!mRequestResolvedType.IsNull()) {
        hash = (ODD_PRIME_NUMBER*hash) + mRequestResolvedType.GetHashCode();
    }

    if (!p.IsNull()) {
        hash = (ODD_PRIME_NUMBER*hash) + p.GetHashCode();
    }

    hash = (ODD_PRIME_NUMBER*hash) + t;
    mHashCode = hash;
}

CPendingIntentRecord::Key::~Key()
{
}

ECode CPendingIntentRecord::Key::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = Equals((Key*)IObject::Probe(other));
    return NOERROR;
}

Boolean CPendingIntentRecord::Key::Equals(
    /* [in] */ Key* other)
{
    if (other == NULL) {
        return FALSE;
    }

    if (mType != other->mType) {
        return FALSE;
    }
    if (mUserId != other->mUserId){
        return FALSE;
    }

    if (!mPackageName.IsNull()) {
        if (!mPackageName.Equals(other->mPackageName)) {
            return FALSE;
        }
    }
    else if (!other->mPackageName.IsNull()) {
        return FALSE;
    }

    if (mActivity != other->mActivity) {
        return FALSE;
    }

    if (!mWho.IsNull()) {
        if (!mWho.Equals(other->mWho)) {
            return FALSE;
        }
    }
    else if (!other->mWho.IsNull()) {
        return FALSE;
    }

    if (mRequestCode != other->mRequestCode) {
        return FALSE;
    }

    if (mRequestIntent != other->mRequestIntent) {
        if (mRequestIntent != NULL) {
            Boolean equals = FALSE;
            mRequestIntent->FilterEquals(other->mRequestIntent, &equals);
            if (!equals) {
                return FALSE;
            }
        } else if (other->mRequestIntent != NULL) {
            return FALSE;
        }
    }

    if (!mRequestResolvedType.IsNull()) {
        if (!mRequestResolvedType.Equals(other->mRequestResolvedType)) {
            return FALSE;
        }
    }
    else if (!other->mRequestResolvedType.IsNull()) {
        return FALSE;
    }

    if (mFlags != other->mFlags) {
        return FALSE;
    }
    return TRUE;
}

ECode CPendingIntentRecord::Key::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = mHashCode;
    return NOERROR;
}

ECode CPendingIntentRecord::Key::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = ToString();
    return NOERROR;
}

String CPendingIntentRecord::Key::ToString()
{
    StringBuilder sb("Key{");
    sb += TypeName();
    sb += " pkg=";
    sb += mPackageName;
    sb += " intent=";
    if (mRequestIntent) {
        String temp;
        mRequestIntent->ToShortString(FALSE, TRUE, FALSE, FALSE, &temp);
        sb += temp;
    }
    else {
        sb += "<null>";
    }
    sb += " flags=0x";
    sb += StringUtils::ToString(mFlags, 16);
    sb += " u=";
    sb += mUserId;
    sb += "}";

    return sb.ToString();
}

String CPendingIntentRecord::Key::TypeName()
{
    switch (mType) {
        case IActivityManager::INTENT_SENDER_ACTIVITY:
            return String("startActivity");
        case IActivityManager::INTENT_SENDER_BROADCAST:
            return String("broadcastIntent");
        case IActivityManager::INTENT_SENDER_SERVICE:
            return String("startService");
        case IActivityManager::INTENT_SENDER_ACTIVITY_RESULT:
            return String("activityResult");
    }
    return StringUtils::ToString(mType);
}

CAR_INTERFACE_IMPL_3(CPendingIntentRecord, Object, IPendingIntentRecord, IIIntentSender, IBinder)
CAR_OBJECT_IMPL(CPendingIntentRecord)

CPendingIntentRecord::CPendingIntentRecord()
    : mOwner(NULL)
    , mUid(0)
    , mSent(FALSE)
    , mCanceled(FALSE)
{
}

CPendingIntentRecord::~CPendingIntentRecord()
{
    if (!mCanceled) {
        //NOTE:(leliang) in android the code below is called in finalize,
        // and the task of CompleteFinalize will be handed
        // in another thread throught the message.
        // in Elastos, because now in destructor,
        // CompleteFinalize can not be invoked anymore in another thread.
        // so just called here
        //
        //AutoPtr<IMessage> msg;
        //mOwner->mHandler->ObtainMessage(
        //    CActivityManagerService::FINALIZE_PENDING_INTENT_MSG,
        //    TO_IINTERFACE(this), (IMessage**)&msg);
        //Boolean res;
        //mOwner->mHandler->SendMessage(msg, &res);
        CompleteFinalize();
    }
}

ECode CPendingIntentRecord::constructor(
    /* [in] */ IIActivityManager* owner,
    /* [in] */ Handle32 k,
    /* [in] */ Int32 u)
{
    mOwner = (CActivityManagerService*)owner;
    mKey = (Key*)k;
    mUid = u;
    GetWeakReference((IWeakReference**)&mRef);
    return NOERROR;
}

ECode CPendingIntentRecord::Send(
    /* [in] */ Int32 code,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ IIntentReceiver* finishedReceiver,
    /* [in] */ const String& requiredPermission,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = SendInner(code, intent, resolvedType, finishedReceiver,
                requiredPermission, NULL, String(NULL), 0, 0, 0, NULL, NULL);
    return NOERROR;
}

Int32 CPendingIntentRecord::SendInner(
    /* [in] */ Int32 code,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& aResolvedType,
    /* [in] */ IIntentReceiver* finishedReceiver,
    /* [in] */ const String& requiredPermission,
    /* [in] */ IBinder* resultTo,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ IBundle* options,
    /* [in] */ IIActivityContainer* container)
{
    String resolvedType(aResolvedType);
    AutoLock lock(mOwner);

    if (!mCanceled) {
        mSent = TRUE;
        if ((mKey->mFlags & IPendingIntent::FLAG_ONE_SHOT) != 0) {
            mOwner->CancelIntentSenderLocked(this, TRUE);
            mCanceled = TRUE;
        }

        AutoPtr<IIntent> finalIntent;
        if (mKey->mRequestIntent != NULL) {
            CIntent::New(mKey->mRequestIntent, (IIntent**)&finalIntent);
        }
        else {
            CIntent::New((IIntent**)&finalIntent);
        }

        if (intent != NULL) {
            Int32 changes;
            finalIntent->FillIn(intent, mKey->mFlags, &changes);
            if ((changes & IIntent::FILL_IN_DATA) == 0) {
                resolvedType = mKey->mRequestResolvedType;
            }
        }
        else {
            resolvedType = mKey->mRequestResolvedType;
        }

        flagsMask &= ~IIntent::IMMUTABLE_FLAGS;
        flagsValues &= flagsMask;
        Int32 flags;
        finalIntent->GetFlags(&flags);
        finalIntent->SetFlags((flags&~flagsMask) | flagsValues);

        AutoPtr<IBinderHelper> binderHelper;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
        Int64 origId;
        binderHelper->ClearCallingIdentity(&origId);

        Boolean sendFinish = finishedReceiver != NULL;
        Int32 userId = mKey->mUserId;
        if (userId == IUserHandle::USER_CURRENT) {
            userId = mOwner->GetCurrentUserIdLocked();
        }

        Int32 status;
        ECode ec = NOERROR;
        String nullStr;

        switch (mKey->mType) {
        case IActivityManager::INTENT_SENDER_ACTIVITY:
            if (options == NULL) {
                options = mKey->mOptions;
            }
            else if (mKey->mOptions != NULL) {
                AutoPtr<IBundle> opts;
                CBundle::New(mKey->mOptions, (IBundle**)&opts);
                opts->PutAll(options);
                options = opts;
            }

            if (mKey->mAllIntents != NULL && mKey->mAllIntents->GetLength() > 1) {
                Int32 length = mKey->mAllIntents->GetLength();
                AutoPtr< ArrayOf<IIntent*> > allIntents = ArrayOf<IIntent*>::Alloc(length);
                AutoPtr< ArrayOf<String> > allResolvedTypes = ArrayOf<String>::Alloc(length);
                allIntents->Copy(mKey->mAllIntents);
                if (mKey->mAllResolvedTypes != NULL)
                    allResolvedTypes->Copy(mKey->mAllResolvedTypes);

                allIntents->Set(length - 1, finalIntent);
                allResolvedTypes->Set(length -1, resolvedType);
                ec = mOwner->StartActivitiesInPackage(mUid, mKey->mPackageName, allIntents,
                        allResolvedTypes, resultTo, options, userId, &status);
            }
            else {
                ec = mOwner->StartActivityInPackage(mUid, mKey->mPackageName, finalIntent, resolvedType,
                        resultTo, resultWho, requestCode, 0, options, userId, container, NULL, &status);
            }

            if (ec == (ECode)E_RUNTIME_EXCEPTION) {
                Slogger::W(CActivityManagerService::TAG,
                        "Unable to send startActivity intent %08x", ec);
            }
            break;

        case IActivityManager::INTENT_SENDER_ACTIVITY_RESULT:
            mKey->mActivity->mTask->mStack->SendActivityResultLocked(-1, mKey->mActivity,
                    mKey->mWho, mKey->mRequestCode, code, finalIntent);
            break;

        case IActivityManager::INTENT_SENDER_BROADCAST:
            // If a completion callback has been requested, require
            // that the broadcast be delivered synchronously
            ec = mOwner->BroadcastIntentInPackage(mKey->mPackageName, mUid,
                    finalIntent, resolvedType, finishedReceiver, code, nullStr, NULL,
                    requiredPermission, (finishedReceiver != NULL), FALSE, userId, &status);
            sendFinish = FALSE;
            if (ec == (ECode)E_RUNTIME_EXCEPTION) {
                Slogger::W(CActivityManagerService::TAG,
                        "Unable to send startActivity intent %08x", ec);
            }
            break;

        case IActivityManager::INTENT_SENDER_SERVICE:
            AutoPtr<IComponentName> cn;
            ec = mOwner->StartServiceInPackage(mUid, finalIntent, resolvedType, userId, (IComponentName**)&cn);
            if (ec == (ECode)E_RUNTIME_EXCEPTION) {
                Slogger::W(CActivityManagerService::TAG,
                        "Unable to send startService intent %08x", ec);
            }
            break;
        }

        if (sendFinish) {
//            try {
            String nullStr;
            AutoPtr<IIntent> newIntent;
            CIntent::New(finalIntent, (IIntent**)&newIntent);
            finishedReceiver->PerformReceive(newIntent, 0,
                    nullStr, NULL, FALSE, FALSE, mKey->mUserId);
//            } catch (RemoteException e) {
//            }
        }

        binderHelper->RestoreCallingIdentity(origId);
        return 0;
    }

    return IActivityManager::START_CANCELED;
}

ECode CPendingIntentRecord::CompleteFinalize()
{
    AutoLock lock(mOwner);
    CActivityManagerService::PendingIntentRecordHashMap::Iterator it;
    it = mOwner->mIntentSenderRecords.Find(mKey);
    if (it != mOwner->mIntentSenderRecords.End()) {
        if (it->mSecond == mRef) {
            mOwner->mIntentSenderRecords.Erase(it);
        }
    }
    return NOERROR;
}

void CPendingIntentRecord::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    pw->Print(prefix);
    pw->Print(String("uid="));
    pw->Print(mUid);
    pw->Print(String(" packageName="));
    pw->Print(mKey->mPackageName);
    pw->Print(String(" type="));
    pw->Print(mKey->TypeName());
    pw->Print(String(" flags=0x"));
    pw->Println(StringUtils::ToString(mKey->mFlags, 16));
    if (mKey->mActivity != NULL || mKey->mWho != NULL) {
        pw->Print(prefix);
        pw->Print(String("activity="));
        // pw->Print(mKey->mActivity->ToString());
        pw->Print(String(" who="));
        pw->Println(mKey->mWho);
    }
    if (mKey->mRequestCode != 0 || mKey->mRequestResolvedType != NULL) {
        pw->Print(prefix);
        pw->Print(String("requestCode="));
        pw->Print(mKey->mRequestCode);
        pw->Print(String(" requestResolvedType="));
        pw->Println(mKey->mRequestResolvedType);
    }
    if (mKey->mRequestIntent != NULL) {
        pw->Print(prefix);
        pw->Print(String("requestIntent="));
        String str;
        mKey->mRequestIntent->ToShortString(FALSE, TRUE, TRUE, TRUE, &str);
        pw->Println(str);
    }
    if (mSent || mCanceled) {
        pw->Print(prefix);
        pw->Print(String("sent="));
        pw->Print(mSent);
        pw->Print(String(" canceled="));
        pw->Println(mCanceled);
    }
}
ECode CPendingIntentRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    if (!mStringName.IsNull()) {
        *str = mStringName;
        return NOERROR;
    }

    StringBuilder sb(128);
    sb += "PendingIntentRecord{";
    sb += StringUtils::ToString(Int32(this), 16);
    sb += ' ';
    sb += mKey->mPackageName;
    sb += ' ';
    sb += mKey->TypeName();
    sb += '}';
    mStringName = sb.ToString();
    *str = mStringName;
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
