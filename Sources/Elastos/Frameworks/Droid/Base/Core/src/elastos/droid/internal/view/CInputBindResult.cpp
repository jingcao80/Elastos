
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/view/CInputBindResult.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

const String CInputBindResult::TAG("InputBindResult");

CAR_INTERFACE_IMPL(CInputBindResult, Object, IInputBindResult)

CAR_OBJECT_IMPL(CInputBindResult)

ECode CInputBindResult::constructor()
{
    return NOERROR;
}

ECode CInputBindResult::constructor(
    /* [in] */ IIInputMethodSession* method,
    /* [in] */ IInputChannel* channel,
    /* [in] */ const String& id,
    /* [in] */ Int32 sequence,
    /* [in] */ Int32 userActionNotificationSequenceNumber)
{
    mMethod = method;
    mChannel = channel;
    mId = id;
    mSequence = sequence;
    mUserActionNotificationSequenceNumber = userActionNotificationSequenceNumber;
    return NOERROR;
}

ECode CInputBindResult::ReadFromParcel(
    /* [in] */ IParcel *source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    mMethod = IIInputMethodSession::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mChannel = IInputChannel::Probe(obj);
    source->ReadString(&mId);
    source->ReadInt32(&mSequence);
    source->ReadInt32(&mUserActionNotificationSequenceNumber);
    return NOERROR;
}

ECode CInputBindResult::WriteToParcel(
    /* [in] */ IParcel *dest)
{
    dest->WriteInterfacePtr((IInterface*)mMethod.Get());
    dest->WriteInterfacePtr((IInterface*)mChannel.Get());
    dest->WriteString(mId);
    dest->WriteInt32(mSequence);
    dest->WriteInt32(mUserActionNotificationSequenceNumber);
    return NOERROR;
}

ECode CInputBindResult::GetMethod(
    /* [out] */ IIInputMethodSession** session)
{
    VALIDATE_NOT_NULL(session);
    *session = mMethod;
    REFCOUNT_ADD(*session);
    return NOERROR;
}

ECode CInputBindResult::GetChannel(
    /* [out] */ IInputChannel** channel)
{
    VALIDATE_NOT_NULL(channel);
    *channel = mChannel;
    REFCOUNT_ADD(*channel);
    return NOERROR;
}

ECode CInputBindResult::GetId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode CInputBindResult::GetSequence(
    /* [out] */ Int32* sequence)
{
    VALIDATE_NOT_NULL(sequence);
    *sequence = mSequence;
    return NOERROR;
}

ECode CInputBindResult::GetUserActionNotificationSequenceNumber(
    /* [out] */ Int32* userActionNotificationSequenceNumber)
{
    VALIDATE_NOT_NULL(userActionNotificationSequenceNumber);
    *userActionNotificationSequenceNumber = mUserActionNotificationSequenceNumber;
    return NOERROR;
}

ECode CInputBindResult::ToString(
    /* [out] */ String* str)
{
    String strMethod;
    IObject::Probe(mMethod)->ToString(&strMethod);
    *str = NULL;
    str->AppendFormat("InputBindResult{%s %s sequence:%d userActionNotificationSequenceNumber:%d}",
        strMethod.string(), mId.string(), mSequence, mUserActionNotificationSequenceNumber);
    return NOERROR;
}

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
