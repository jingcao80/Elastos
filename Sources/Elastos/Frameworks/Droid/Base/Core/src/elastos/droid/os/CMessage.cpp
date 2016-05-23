
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/CMessage.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/utility/TimeUtils.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Core::ICloneable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Os {

const Int32 CMessage::FLAG_IN_USE;
const Int32 CMessage::FLAG_ASYNCHRONOUS;
const Int32 CMessage::FLAGS_TO_CLEAR_ON_COPY_FROM;

Object CMessage::sPoolSync;
AutoPtr<CMessage> CMessage::sPool;
Int32 CMessage::sPoolSize = 0;
const Int32 CMessage::MAX_POOL_SIZE;
Boolean CMessage::gCheckRecycle = TRUE;

CAR_INTERFACE_IMPL_2(CMessage, Object, IMessage, IParcelable)

CAR_OBJECT_IMPL(CMessage)

CMessage::CMessage()
    : mWhat(0)
    , mArg1(0)
    , mArg2(0)
    , mSendingUid(-1)
    , mFlags(0)
    , mWhen(0)
{}

ECode CMessage::constructor()
{
    return NOERROR;
}

AutoPtr<IMessage> CMessage::Obtain()
{
    {
        AutoLock lock(sPoolSync);

        if (sPool != NULL) {
            AutoPtr<CMessage> m = sPool;
            sPool = m->mNext;
            m->mNext = NULL; // clear in-use flag
            m->mFlags = 0;
            sPoolSize--;
            return (IMessage*)m.Get();
        }
    }
    AutoPtr<IMessage> m;
    CMessage::New((IMessage**)&m);
    return m;
}

AutoPtr<IMessage> CMessage::Obtain(
    /* [in] */ IMessage* orig)
{
    AutoPtr<IMessage> m = Obtain();
    Int32 value;
    orig->GetWhat(&value);
    m->SetWhat(value);
    orig->GetArg1(&value);
    m->SetArg1(value);
    orig->GetArg2(&value);
    m->SetArg2(value);
    AutoPtr<IInterface> obj;
    orig->GetObj((IInterface**)&obj);
    m->SetObj(obj);
    AutoPtr<IMessenger> replyTo;
    orig->GetReplyTo((IMessenger**)&replyTo);
    m->SetReplyTo(replyTo);
    orig->GetSendingUid(&value);
    m->SetSendingUid(value);
    AutoPtr<IBundle> data;
    orig->GetData((IBundle**)&data);
    if (data != NULL) {
        m->SetData(data);
    }

    AutoPtr<IHandler> h;
    orig->GetTarget((IHandler**)&h);
    m->SetTarget(h);

    AutoPtr<IRunnable> callback;
    orig->GetCallback((IRunnable**)&callback);
    m->SetCallback(callback);
    return m;
}

AutoPtr<IMessage> CMessage::Obtain(
    /* [in] */ IHandler* h)
{
    AutoPtr<IMessage> m = Obtain();
    m->SetTarget(h);

    return m;
}

AutoPtr<IMessage> CMessage::Obtain(
    /* [in] */ IHandler* h,
    /* [in] */ IRunnable* callback)
{
    AutoPtr<IMessage> m = Obtain();
    m->SetTarget(h);
    m->SetCallback(callback);
    return m;
}

AutoPtr<IMessage> CMessage::Obtain(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what)
{
    AutoPtr<IMessage> m = Obtain();
    m->SetTarget(h);
    m->SetWhat(what);

    return m;
}

AutoPtr<IMessage> CMessage::Obtain(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMessage> m = Obtain();
    m->SetTarget(h);
    m->SetWhat(what);
    m->SetObj(obj);

    return m;
}

AutoPtr<IMessage> CMessage::Obtain(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2)
{
    AutoPtr<IMessage> m = Obtain();
    m->SetTarget(h);
    m->SetWhat(what);
    m->SetArg1(arg1);
    m->SetArg2(arg2);

    return m;
}

AutoPtr<IMessage> CMessage::Obtain(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMessage> m = Obtain();
    m->SetTarget(h);
    m->SetWhat(what);
    m->SetArg1(arg1);
    m->SetArg2(arg2);
    m->SetObj(obj);

    return m;
}

ECode CMessage::SetSendingUid(
    /* [in] */ Int32 uid)
{
    return NOERROR;
}

ECode CMessage::GetSendingUid(
    /* [out] */ Int32* uid)
{
    return NOERROR;
}

ECode CMessage::GetNext(
    /* [out] */ IMessage** next)
{
    VALIDATE_NOT_NULL(next);
    *next = (IMessage*)mNext.Get();
    REFCOUNT_ADD(*next);
    return NOERROR;
}

ECode CMessage::SetNext(
    /* [in] */ IMessage* next)
{
    mNext = (CMessage*)next;
    return NOERROR;
}

ECode CMessage::UpdateCheckRecycle(
    /* [in] */ Int32 targetSdkVersion)
{
    if (targetSdkVersion < Build::VERSION_CODES::LOLLIPOP) {
        gCheckRecycle = FALSE;
    }
    return NOERROR;
}

ECode CMessage::Recycle()
{
    Boolean value;
    if (IsInUse(&value), value) {
        if (gCheckRecycle) {
            // throw new IllegalStateException("This message cannot be recycled because it "
            //         + "is still in use.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        return NOERROR;
    }

    return RecycleUnchecked();
}

ECode CMessage::RecycleUnchecked()
{
    // Mark the message as in use while it remains in the recycled object pool.
    // Clear out all other details.
    mFlags = FLAG_IN_USE;
    mWhat = 0;
    mArg1 = 0;
    mArg2 = 0;
    mObj = NULL;
    mReplyTo = NULL;
    mSendingUid = -1;
    mWhen = 0;
    mTarget = NULL;
    mCallback = NULL;
    mData = NULL;

    {    AutoLock syncLock(sPoolSync);
        if (sPoolSize < MAX_POOL_SIZE) {
            mNext = sPool;
            sPool = this;
            sPoolSize++;
        }
    }
    return NOERROR;
}

ECode CMessage::CopyFrom(
    /* [in] */ IMessage* o)
{
    CMessage* m = (CMessage*)o;
    mFlags = m->mFlags & ~FLAGS_TO_CLEAR_ON_COPY_FROM;
    mWhat = m->mWhat;
    mArg1 = m->mArg1;
    mArg2 = m->mArg2;
    mObj = m->mObj;
    mReplyTo = m->mReplyTo;
    mSendingUid = m->mSendingUid;

    mData = NULL;
    if (m->mData != NULL) {
        AutoPtr<IInterface> obj;
        ICloneable::Probe(m->mData)->Clone((IInterface**)&obj);
        mData = IBundle::Probe(obj);
    }
    return NOERROR;
}

ECode CMessage::GetWhen(
    /* [out] */ Int64* when)
{
    VALIDATE_NOT_NULL(when);
    *when = mWhen;
    return NOERROR;
}

ECode CMessage::SetWhen(
    /* [in] */ Int64 when)
{
    mWhen = when;
    return NOERROR;
}

ECode CMessage::SetTarget(
    /* [in] */ IHandler* target)
{
    mTarget = target;
    return NOERROR;
}

/**
 * Retrieve the a {@link android.os.Handler Handler} implementation that
 * will receive this message. The object must implement
 * {@link android.os.Handler#handleMessage(android.os.Message)
 * Handler.handleMessage()}. Each Handler has its own name-space for
 * message codes, so you do not need to
 * worry about yours conflicting with other handlers.
 */
ECode CMessage::GetTarget(
    /* [out] */ IHandler** target)
{
    VALIDATE_NOT_NULL(target);
    *target = mTarget;
    REFCOUNT_ADD(*target);
    return NOERROR;
}

ECode CMessage::SetCallback(
    /* [in] */ IRunnable* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode CMessage::GetCallback(
    /* [out] */ IRunnable** callback)
{
    VALIDATE_NOT_NULL(callback);
    *callback = mCallback;
    REFCOUNT_ADD(*callback);
    return NOERROR;
}

/**
 * Sends this Message to the Handler specified by {@link #getTarget}.
 * Throws a null pointer exception if this field has not been set.
 */
ECode CMessage::SendToTarget()
{
    Boolean result;
    mTarget->SendMessage(this, &result);
    return NOERROR;
}

ECode CMessage::SetData(
    /* [in] */ IBundle* data)
{
    mData = data;
    return NOERROR;
}

ECode CMessage::GetData(
    /* [in] */ IBundle** data)
{
    VALIDATE_NOT_NULL(data);
    if (mData == NULL) {
        CBundle::New((IBundle**)&mData);
    }

    *data = mData;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode CMessage::PeekData(
    /* [in] */ IBundle** data)
{
    VALIDATE_NOT_NULL(data);

    *data = mData;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode CMessage::SetWhat(
    /* [in] */ Int32 val)
{
    mWhat = val;
    return NOERROR;
}

ECode CMessage::GetWhat(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    *val = mWhat;
    return NOERROR;
}

ECode CMessage::SetReplyTo(
    /* [in] */ IMessenger* replyTo)
{
    mReplyTo = replyTo;
    return NOERROR;
}

ECode CMessage::GetReplyTo(
    /* [out] */ IMessenger** replyTo)
{
    VALIDATE_NOT_NULL(replyTo);
    *replyTo = mReplyTo;
    REFCOUNT_ADD(*replyTo);
    return NOERROR;
}

ECode CMessage::SetArg1(
    /* [in] */ Int32 arg1)
{
    mArg1 = arg1;
    return NOERROR;
}

ECode CMessage::GetArg1(
    /* [out] */ Int32* arg1)
{
    VALIDATE_NOT_NULL(arg1);
    *arg1 = mArg1;
    return NOERROR;
}

ECode CMessage::SetArg2(
    /* [in] */ Int32 arg2)
{
    mArg2 = arg2;
    return NOERROR;
}

ECode CMessage::GetArg2(
    /* [out] */ Int32* arg2)
{
    VALIDATE_NOT_NULL(arg2);
    *arg2 = mArg2;
    return NOERROR;
}

ECode CMessage::SetObj(
    /* [in] */ IInterface* obj)
{
    mObj = obj;
    return NOERROR;
}

ECode CMessage::GetObj(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = mObj;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CMessage::IsAsynchronous(
    /* [out] */ Boolean* async)
{
    VALIDATE_NOT_NULL(async);
    *async = ((mFlags & FLAG_ASYNCHRONOUS) != 0);
    return NOERROR;
}

ECode CMessage::SetAsynchronous(
    /* [in] */ Boolean async)
{
    if (async) {
        mFlags |= FLAG_ASYNCHRONOUS;
    }
    else {
        mFlags &= ~FLAG_ASYNCHRONOUS;
    }
    return NOERROR;
}

ECode CMessage::IsInUse(
    /* [out] */ Boolean* async)
{
    VALIDATE_NOT_NULL(async);
    *async = ((mFlags & FLAG_IN_USE) == FLAG_IN_USE);
    return NOERROR;
}

ECode CMessage::MarkInUse()
{
    mFlags |= FLAG_IN_USE;
    return NOERROR;
}

ECode CMessage::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mWhat);
    source->ReadInt32(&mArg1);
    source->ReadInt32(&mArg2);
    Int32 flag = 0;
    source->ReadInt32(&flag);
    if (flag != 0) {
        source->ReadInterfacePtr((Handle32*)&mObj);
        source->ReadInt32(&mFlags); // TODO
    }

    source->ReadInt64(&mWhen);
    source->ReadInterfacePtr((Handle32*)&mData);
    source->ReadInterfacePtr((Handle32*)&mReplyTo);
    // replyTo = Messenger.readMessengerOrNullFromParcel(source);
    source->ReadInt32(&mSendingUid);
    return NOERROR;
}

ECode CMessage::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (mCallback != NULL) {
        Logger::E("CMessage", "Can't marshal callbacks across processes.");
        return E_RUNTIME_EXCEPTION;
    }

    dest->WriteInt32(mWhat);
    dest->WriteInt32(mArg1);
    dest->WriteInt32(mArg2);
    if (mObj != NULL) {
        AutoPtr<IParcelable> p = IParcelable::Probe(mObj);
        if (p) {
            dest->WriteInt32(1);
            dest->WriteInterfacePtr(p);
            dest->WriteInt32(mFlags); // TODO
        }
        else {
            Logger::E("CMessage", "Can't marshal non-Parcelable objects across processes.");
        }
    }
    else {
        dest->WriteInt32(0);
    }

    dest->WriteInt64(mWhen);
    dest->WriteInterfacePtr(mData);
    dest->WriteInterfacePtr(mReplyTo);

    // CMessenger::WriteMessengerOrNullToParcel(mReplyTo, dest);
    dest->WriteInt32(mSendingUid);
    return NOERROR;
}

ECode CMessage::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = ToString(SystemClock::GetUptimeMillis());
    return NOERROR;
}

String CMessage::ToString(
    /* [in] */ Int64 now)
{
    StringBuilder b;
    b.Append("{ when=");
    TimeUtils::FormatDuration(mWhen - now, b);

    if (mTarget != NULL) {
        if (mCallback != NULL) {
            b.Append(" callback=");
            b.Append(mCallback/*.getClass().getName()*/);
        }
        else {
            b.Append(" what=");
            b.Append(mWhat);
        }

        if (mArg1 != 0) {
            b.Append(" arg1=");
            b.Append(mArg1);
        }

        if (mArg2 != 0) {
            b.Append(" arg2=");
            b.Append(mArg2);
        }

        if (mObj != NULL) {
            b.Append(" obj=");
            b.Append(mObj);
        }

        b.Append(" target=");
        b.Append(mTarget/*.getClass().getName()*/);
    }
    else {
        b.Append(" barrier=");
        b.Append(mArg1);
    }

    b.Append(" }");
    return b.ToString();
}

} // namespace Os
} // namespace Droid
} // Elastos
