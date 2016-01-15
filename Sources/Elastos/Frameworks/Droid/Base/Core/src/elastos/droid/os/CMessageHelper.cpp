
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/os/CMessage.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CMessageHelper, Singleton, IMessageHelper)

CAR_SINGLETON_IMPL(CMessageHelper)

ECode CMessageHelper::Obtain(
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessage> m = CMessage::Obtain();
    *msg = m;
    REFCOUNT_ADD(*msg);
    return NOERROR;
}

ECode CMessageHelper::Obtain(
    /* [in] */ IMessage* orig,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessage> m = CMessage::Obtain(orig);
    *msg = m;
    REFCOUNT_ADD(*msg);
    return NOERROR;
}

ECode CMessageHelper::Obtain(
    /* [in] */ IHandler* target,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessage> m = CMessage::Obtain(target);
    *msg = m;
    REFCOUNT_ADD(*msg);
    return NOERROR;
}

ECode CMessageHelper::Obtain(
    /* [in] */ IHandler* target,
    /* [in] */ Int32 what,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessage> m = CMessage::Obtain(target, what);
    *msg = m;
    REFCOUNT_ADD(*msg);
    return NOERROR;
}

ECode CMessageHelper::Obtain(
    /* [in] */ IHandler* target,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessage> m = CMessage::Obtain(target, what, obj);
    *msg = m;
    REFCOUNT_ADD(*msg);
    return NOERROR;
}

ECode CMessageHelper::Obtain(
    /* [in] */ IHandler* target,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessage> m = CMessage::Obtain(target, what, arg1, arg2);
    *msg = m;
    REFCOUNT_ADD(*msg);
    return NOERROR;
}

ECode CMessageHelper::Obtain(
    /* [in] */ IHandler* target,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessage> m = CMessage::Obtain(target, what, arg1, arg2, obj);
    *msg = m;
    REFCOUNT_ADD(*msg);
    return NOERROR;
}

ECode CMessageHelper::Obtain(
    /* [in] */ IHandler* target,
    /* [in] */ IRunnable* callback,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessage> m = CMessage::Obtain(target, callback);
    *msg = m;
    REFCOUNT_ADD(*msg);
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
