#include "SelectionKeyImpl.h"
#include "AutoLock.h"
#include "SelectorImpl.h"

using Elastos::Core::AutoLock;
using Elastos::IO::Channels::EIID_ISelectableChannel;

namespace Elastos {
namespace IO {

SelectionKeyImpl::SelectionKeyImpl(
    /* [in] */ IAbstractSelectableChannel* channel,
    /* [in] */ Int32 ops,
    /* [in] */ IObject* attachment,
    /* [in] */ IAbstractSelector* selector)
{
    REFCOUNT_ADD(channel);
    this->mChannel = channel;
    this->mInterestOps = ops;
    REFCOUNT_ADD(selector);
    this->mSelector = selector;
    AutoPtr<IObject> tempObj;
    Attach(attachment, (IObject**)&tempObj);
}

ECode SelectionKeyImpl::GetChannel(
    /* [out] */ ISelectableChannel** channel)
{
    VALIDATE_NOT_NULL(channel)
    *channel = ISelectableChannel::Probe(mChannel);
    REFCOUNT_ADD(*channel);
    return NOERROR;
}

ECode SelectionKeyImpl::GetInterestOps(
    /* [out] */ Int32* opts)
{
    VALIDATE_NOT_NULL(opts)
    *opts = 0;

    FAIL_RETURN(CheckValid())
    AutoLock lock(((SelectorImpl*)mSelector.Get())->mKeysLock);
    *opts = mInterestOps;
    return NOERROR;
}

Int32 SelectionKeyImpl::InterestOpsNoCheck()
{
    AutoLock lock(((SelectorImpl*)mSelector.Get())->mKeysLock);
    return mInterestOps;
}

ECode SelectionKeyImpl::GetInterestOps(
    /* [in] */ Int32 operations,
    /* [out] */ ISelectionKey** key)
{
    VALIDATE_NOT_NULL(key)
    *key = NULL;

    FAIL_RETURN(CheckValid());
    Int32 ops;
    ISelectableChannel::Probe(mChannel)->GetValidOps(&ops);
    if ((operations & ~ops) != 0)
    {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(((SelectorImpl*)mSelector.Get())->mKeysLock);
    mInterestOps = operations;
    *key = this;
    REFCOUNT_ADD(*key)

    return NOERROR;
}

ECode SelectionKeyImpl::GetReadyOps(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)

    FAIL_RETURN(CheckValid());
    *ret = mReadyOps;
    return NOERROR;
}

ECode SelectionKeyImpl::GetSelector(
    /* [out] */ ISelector** selector)
{
    VALIDATE_NOT_NULL(selector)

    *selector = ISelector::Probe(mSelector);
    REFCOUNT_ADD(*selector);
    return NOERROR;
}

ECode SelectionKeyImpl::SetReadyOps(
    /* [in] */ Int32 readyOps)
{
    mReadyOps = readyOps;
    return NOERROR;
}

ECode SelectionKeyImpl::CheckValid()
{
    Boolean isValid;
    IsValid(&isValid);
    if (FALSE == isValid) {
        return E_CANCELLED_KEY_EXCEPTION;
    }
    return NOERROR;;
}

ECode SelectionKeyImpl::IsConnected(
    /* [out] */ Boolean* isConnected)
{
    VALIDATE_NOT_NULL(isConnected)
    *isConnected = FALSE;

    if (ISocketChannel::Probe(mChannel) == NULL) {
        *isConnected = TRUE;
        return NOERROR;
    }

    Boolean isConn = FALSE;
    ISocketChannel::Probe(mChannel)->IsConnected(isConnected);
    *isConnected = isConn;
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
