#include "elastos/droid/internal/telephony/gsm/CSimPhoneBookInterfaceManager.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

const String CSimPhoneBookInterfaceManager::TAG("SimPhoneBookIM");

CAR_INTERFACE_IMPL(CSimPhoneBookInterfaceManager, IccPhoneBookInterfaceManager, ISimPhoneBookInterfaceManager)

CAR_OBJECT_IMPL(CSimPhoneBookInterfaceManager)

CSimPhoneBookInterfaceManager::~CSimPhoneBookInterfaceManager()
{
    // try {
        // super.finalize();
    // } catch (Throwable throwable) {
    //     Rlog.e(TAG, "Error while finalizing:", throwable);
    // }
    if(DBG) Logger::D(TAG, "SimPhoneBookInterfaceManager finalized");
}

ECode CSimPhoneBookInterfaceManager::constructor()
{
    return NOERROR;
}

ECode CSimPhoneBookInterfaceManager::constructor(
    /* [in] */ IGSMPhone* phone)
{
    return IccPhoneBookInterfaceManager::constructor(IPhoneBase::Probe(phone));
    //NOTE service "simphonebook" added by IccSmsInterfaceManagerProxy
}

ECode CSimPhoneBookInterfaceManager::Dispose()
{
    return IccPhoneBookInterfaceManager::Dispose();
}

ECode CSimPhoneBookInterfaceManager::GetAdnRecordsSize(
    /* [in] */ Int32 efid,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Logd(String("getAdnRecordsSize: efid=") + StringUtils::ToString(efid));
    AutoLock syncLock(mLock);
    CheckThread();
    mRecordSize = ArrayOf<Int32>::Alloc(3);

    //Using mBaseHandler, no difference in EVENT_GET_SIZE_DONE handling
    AutoPtr<IAtomicBoolean> status;
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&status);
    AutoPtr<IMessage> response;
    mBaseHandler->ObtainMessage(EVENT_GET_SIZE_DONE, status, (IMessage**)&response);

    AutoPtr<IIccFileHandler> fh;
    mPhone->GetIccFileHandler((IIccFileHandler**)&fh);
    if (fh != NULL) {
        fh->GetEFLinearRecordSize(efid, response);
        WaitForResult(status);
    }

    *result = mRecordSize;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSimPhoneBookInterfaceManager::Logd(
    /* [in] */ const String& msg)
{
    return Logger::D(TAG, String("[SimPbInterfaceManager] ") + msg);
}

ECode CSimPhoneBookInterfaceManager::Loge(
    /* [in] */ const String& msg)
{
    return Logger::E(TAG, String("[SimPbInterfaceManager] ") + msg);
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos