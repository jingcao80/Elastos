
#include "elastos/droid/internal/textservice/ITextServicesManager.h"
#include "elastos/droid/os/AndroidParcelUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Droid::Os::AndroidParcelUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace TextService {

const String ITextServicesManagerProxy::TAG("ITextServicesManagerProxy");
const String ITextServicesManagerProxy::DESCRIPTOR("com.android.internal.textservice.ITextServicesManager");
const Int32 ITextServicesManagerProxy::TRANSACTION_getCurrentSpellChecker = android::IBinder::FIRST_CALL_TRANSACTION + 0;
const Int32 ITextServicesManagerProxy::TRANSACTION_getCurrentSpellCheckerSubtype = android::IBinder::FIRST_CALL_TRANSACTION + 1;
const Int32 ITextServicesManagerProxy::TRANSACTION_isSpellCheckerEnabled = android::IBinder::FIRST_CALL_TRANSACTION + 7;

CAR_INTERFACE_IMPL(ITextServicesManagerProxy, Object, IITextServicesManager)

ITextServicesManagerProxy::ITextServicesManagerProxy(
    /* [in] */ android::sp<android::IBinder>& remote)
    : mRemote(remote)
{}

ECode ITextServicesManagerProxy::GetCurrentSpellChecker(
    /* [in] */ const String& locale,
    /* [out] */ ISpellCheckerInfo** info)
{
    VALIDATE_NOT_NULL(info);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteString(data, locale);
    mRemote->transact(TRANSACTION_getCurrentSpellChecker, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<ISpellCheckerInfo> result = AndroidParcelUtils::ReadSpellCheckerInfo(reply);
        *info = result;
        REFCOUNT_ADD(*info);
    }
    else {
        *info = NULL;
    }
    return ec;
}

ECode ITextServicesManagerProxy::GetCurrentSpellCheckerSubtype(
    /* [in] */ const String& locale,
    /* [in] */ Boolean allowImplicitlySelectedSubtype,
    /* [out] */ ISpellCheckerSubtype** subtype)
{
    VALIDATE_NOT_NULL(subtype);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteString(data, locale);
    AndroidParcelUtils::WriteInt32(data, allowImplicitlySelectedSubtype ? 1 : 0);
    mRemote->transact(TRANSACTION_getCurrentSpellCheckerSubtype, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<ISpellCheckerSubtype> result = AndroidParcelUtils::ReadSpellCheckerSubtype(reply);
        *subtype = result;
        REFCOUNT_ADD(*subtype);
    }
    else {
        *subtype = NULL;
    }
    return ec;
}

ECode ITextServicesManagerProxy::GetSpellCheckerService(
    /* [in] */ const String& sciId,
    /* [in] */ const String& locale,
    /* [in] */ IITextServicesSessionListener* tsListener,
    /* [in] */ IISpellCheckerSessionListener* scListener,
    /* [in] */ IBundle* bundle)
{
    Logger::D(TAG, "==== GetSpellCheckerService not implemented ====");
    return E_NOT_IMPLEMENTED;
}

ECode ITextServicesManagerProxy::FinishSpellCheckerService(
    /* [in] */ IISpellCheckerSessionListener* listener)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode ITextServicesManagerProxy::SetCurrentSpellChecker(
    /* [in] */ const String& locale,
    /* [in] */ const String& sciId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode ITextServicesManagerProxy::SetCurrentSpellCheckerSubtype(
    /* [in] */ const String& locale,
    /* [in] */ Int32 hashCode)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode ITextServicesManagerProxy::SetSpellCheckerEnabled(
    /* [in] */ Boolean enabled)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode ITextServicesManagerProxy::IsSpellCheckerEnabled(
    /* [out] */ Boolean* ebl)
{
    VALIDATE_NOT_NULL(ebl);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    mRemote->transact(TRANSACTION_isSpellCheckerEnabled, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    *ebl = AndroidParcelUtils::ReadInt32(reply) != 0;
    return ec;
}

ECode ITextServicesManagerProxy::GetEnabledSpellCheckers(
    /* [out, callee] */ ArrayOf<ISpellCheckerInfo*>** infoArray)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace TextService
} // namespace Internal
} // namespace Droid
} // namespace Elastos
