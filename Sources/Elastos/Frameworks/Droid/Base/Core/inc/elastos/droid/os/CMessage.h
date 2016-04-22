
#ifndef __ELASTOS_DROID_OS_CMESSAGE_H__
#define __ELASTOS_DROID_OS_CMESSAGE_H__

#include "_Elastos_Droid_Os_CMessage.h"
#include <elastos/core/Object.h>

using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CMessage)
    , public Object
    , public IMessage
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CMessage();

    CARAPI constructor();

    /**
     * Return a new Message instance from the global pool. Allows us to
     * avoid allocating new objects in many cases.
     */
    static CARAPI_(AutoPtr<IMessage>) Obtain();

    /**
     * Same as {@link #obtain()}, but copies the values of an existing
     * message (including its target) into the new one.
     * @param orig Original message to copy.
     * @return A Message object from the global pool.
     */
    static CARAPI_(AutoPtr<IMessage>) Obtain(
        /* [in] */ IMessage* orig);

    /**
     * Same as {@link #obtain()}, but sets the value for the <em>target</em> member on the Message returned.
     * @param h  Handler to assign to the returned Message object's <em>target</em> member.
     * @return A Message object from the global pool.
     */
    static CARAPI_(AutoPtr<IMessage>) Obtain(
        /* [in] */ IHandler* h);

    /**
     * Same as {@link #obtain(Handler)}, but assigns a callback Runnable on
     * the Message that is returned.
     * @param h  Handler to assign to the returned Message object's <em>target</em> member.
     * @param callback Runnable that will execute when the message is handled.
     * @return A Message object from the global pool.
     */
    static CARAPI_(AutoPtr<IMessage>) Obtain(
        /* [in] */ IHandler* h,
        /* [in] */ IRunnable* callback);

    /**
     * Same as {@link #obtain()}, but sets the values for both <em>target</em> and
     * <em>what</em> members on the Message.
     * @param h  Value to assign to the <em>target</em> member.
     * @param what  Value to assign to the <em>what</em> member.
     * @return A Message object from the global pool.
     */
    static CARAPI_(AutoPtr<IMessage>) Obtain(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what);

    /**
     * Same as {@link #obtain()}, but sets the values of the <em>target</em>, <em>what</em>, and <em>obj</em>
     * members.
     * @param h  The <em>target</em> value to set.
     * @param what  The <em>what</em> value to set.
     * @param obj  The <em>object</em> method to set.
     * @return  A Message object from the global pool.
     */
    static CARAPI_(AutoPtr<IMessage>) Obtain(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Same as {@link #obtain()}, but sets the values of the <em>target</em>, <em>what</em>,
     * <em>arg1</em>, and <em>arg2</em> members.
     *
     * @param h  The <em>target</em> value to set.
     * @param what  The <em>what</em> value to set.
     * @param arg1  The <em>arg1</em> value to set.
     * @param arg2  The <em>arg2</em> value to set.
     * @return  A Message object from the global pool.
     */
    static CARAPI_(AutoPtr<IMessage>) Obtain(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2);

    /**
     * Same as {@link #obtain()}, but sets the values of the <em>target</em>, <em>what</em>,
     * <em>arg1</em>, <em>arg2</em>, and <em>obj</em> members.
     *
     * @param h  The <em>target</em> value to set.
     * @param what  The <em>what</em> value to set.
     * @param arg1  The <em>arg1</em> value to set.
     * @param arg2  The <em>arg2</em> value to set.
     * @param obj  The <em>obj</em> value to set.
     * @return  A Message object from the global pool.
     */
    static CARAPI_(AutoPtr<IMessage>) Obtain(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj);

    CARAPI SetSendingUid(
        /* [in] */ Int32 uid);

    CARAPI GetSendingUid(
        /* [out] */ Int32* uid);

    CARAPI GetNext(
        /* [out] */ IMessage** next);

    CARAPI SetNext(
        /* [in] */ IMessage* next);

    /**
     * Return a Message instance to the global pool.
     * <p>
     * You MUST NOT touch the Message after calling this function because it has
     * effectively been freed.  It is an error to recycle a message that is currently
     * enqueued or that is in the process of being delivered to a Handler.
     * </p>
     */
    CARAPI Recycle();

    /**
     * Make this message like o.  Performs a shallow copy of the data field.
     * Does not copy the linked list fields, nor the timestamp or
     * target/callback of the original message.
     */
    CARAPI CopyFrom(
        /* [in] */ IMessage* o);

    /**
     * Return the targeted delivery time of this message, in milliseconds.
     */
    CARAPI GetWhen(
        /* [out] */ Int64* when);

    CARAPI SetWhen(
        /* [in] */ Int64 when);

    CARAPI SetTarget(
        /* [in] */ IHandler* target);

    /**
     * Retrieve the a {@link android.os.Handler Handler} implementation that
     * will receive this message. The object must implement
     * {@link android.os.Handler#handleMessage(android.os.Message)
     * Handler.handleMessage()}. Each Handler has its own name-space for
     * message codes, so you do not need to
     * worry about yours conflicting with other handlers.
     */
    CARAPI GetTarget(
        /* [out] */ IHandler** target);

    CARAPI SetCallback(
        /* [in] */ IRunnable* runnable);

    CARAPI GetCallback(
        /* [out] */ IRunnable** runnable);

    /**
     * Sends this Message to the Handler specified by {@link #getTarget}.
     * Throws a null pointer exception if this field has not been set.
     */
    CARAPI SendToTarget();

    CARAPI SetData(
        /* [in] */ IBundle* data);

    CARAPI GetData(
        /* [in] */ IBundle** data);

    CARAPI PeekData(
        /* [in] */ IBundle** data);

    CARAPI SetWhat(
        /* [in] */ Int32 val);

    CARAPI GetWhat(
        /* [out] */ Int32* val);

    CARAPI SetReplyTo(
        /* [in] */ IMessenger* replyTo);

    CARAPI GetReplyTo(
        /* [out] */ IMessenger** replyTo);

    CARAPI SetArg1(
        /* [in] */ Int32 arg1);

    CARAPI GetArg1(
        /* [out] */ Int32* arg1);

    CARAPI SetArg2(
        /* [in] */ Int32 arg2);

    CARAPI GetArg2(
        /* [out] */ Int32* arg2);

    CARAPI SetObj(
        /* [in] */ IInterface* obj);

    CARAPI GetObj(
        /* [out] */ IInterface** obj);

    CARAPI IsAsynchronous(
        /* [out] */ Boolean* async);

    CARAPI SetAsynchronous(
        /* [in] */ Boolean async);

    CARAPI IsInUse(
        /* [out] */ Boolean* async);

    CARAPI MarkInUse();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    static CARAPI UpdateCheckRecycle(
        /* [in] */ Int32 targetSdkVersion);

    CARAPI RecycleUnchecked();

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI_(String) ToString(
        /* [in] */ Int64 now);

public:
    /**
     * User-defined message code so that the recipient can identify
     * what this message is about. Each {@link Handler} has its own name-space
     * for message codes, so you do not need to worry about yours conflicting
     * with other handlers.
     */
    Int32 mWhat;

    /**
     * arg1 and arg2 are lower-cost alternatives to using
     * {@link #setData(Bundle) setData()} if you only need to store a
     * few integer values.
     */
    Int32 mArg1;

    /**
     * arg1 and arg2 are lower-cost alternatives to using
     * {@link #setData(Bundle) setData()} if you only need to store a
     * few integer values.
     */
    Int32 mArg2;

    /**
     * An arbitrary object to send to the recipient.  When using
     * {@link Messenger} to send the message across processes this can only
     * be non-null if it contains a Parcelable of a framework class (not one
     * implemented by the application).   For other data transfer use
     * {@link #setData}.
     *
     * <p>Note that Parcelable objects here are not supported prior to
     * the {@link android.os.Build.VERSION_CODES#FROYO} release.
     */
    AutoPtr<IInterface> mObj;

    /**
     * Optional Messenger where replies to this message can be sent.  The
     * semantics of exactly how this is used are up to the sender and
     * receiver.
     */
    AutoPtr<IMessenger> mReplyTo;

    /**
     * Optional field indicating the uid that sent the message.  This is
     * only valid for messages posted by a {@link Messenger}; otherwise,
     * it will be -1.
     */
    Int32 mSendingUid;// = -1;

    /** If set message is in use.
     * This flag is set when the message is enqueued and remains set while it
     * is delivered and afterwards when it is recycled.  The flag is only cleared
     * when a new message is created or obtained since that is the only time that
     * applications are allowed to modify the contents of the message.
     *
     * It is an error to attempt to enqueue or recycle a message that is already in use.
     */
    /*package*/ static const Int32 FLAG_IN_USE = 1 << 0;

    /** If set message is asynchronous */
    /*package*/ static const Int32 FLAG_ASYNCHRONOUS = 1 << 1;

    /** Flags to clear in the copyFrom method */
    /*package*/ static const Int32 FLAGS_TO_CLEAR_ON_COPY_FROM = FLAG_IN_USE;

    /*package*/ Int32 mFlags;

    /*package*/ Int64 mWhen;

    /*package*/ AutoPtr<IBundle> mData;

    /*package*/ AutoPtr<IHandler> mTarget;

    /*package*/ AutoPtr<IRunnable> mCallback;

    // sometimes we store linked lists of these things
    /*package*/ AutoPtr<CMessage> mNext;

private:
    static Object sPoolSync;
    static AutoPtr<CMessage> sPool;
    static Int32 sPoolSize;

    static const Int32 MAX_POOL_SIZE = 50;

    static Boolean gCheckRecycle;// = true;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CMESSAGE_H__
