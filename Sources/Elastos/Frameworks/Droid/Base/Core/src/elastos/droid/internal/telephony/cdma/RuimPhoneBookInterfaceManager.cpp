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

#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/RuimPhoneBookInterfaceManager.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Core::StringUtils;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                    RuimPhoneBookInterfaceManager
//=====================================================================
CAR_INTERFACE_IMPL(RuimPhoneBookInterfaceManager, IccPhoneBookInterfaceManager, IRuimPhoneBookInterfaceManager);
const String RuimPhoneBookInterfaceManager::LOGTAG("RuimPhoneBookIM");

RuimPhoneBookInterfaceManager::~RuimPhoneBookInterfaceManager()
{
    // try {
    // IccPhoneBookInterfaceManager::Finalize();
    // } catch (Throwable throwable) {
    //     Logger::E(LOGTAG, "Error while finalizing:", throwable);
    // }
    if(DBG) Logger::D(LOGTAG, "RuimPhoneBookInterfaceManager finalized");
}

ECode RuimPhoneBookInterfaceManager::constructor(
    /* [in] */ ICDMAPhone* phone)
{
    IccPhoneBookInterfaceManager::constructor(IPhoneBase::Probe(phone));
    //NOTE service "simphonebook" added by IccSmsInterfaceManagerProxy
    return NOERROR;
}

ECode RuimPhoneBookInterfaceManager::Dispose()
{
    return IccPhoneBookInterfaceManager::Dispose();
}

ECode RuimPhoneBookInterfaceManager::GetAdnRecordsSize(
    /* [in] */ Int32 efid,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);

    if (DBG) Logd(String("getAdnRecordsSize: efid=") + StringUtils::ToString(efid));

    AutoLock lock(mLock);
    CheckThread();
    mRecordSize = ArrayOf<Int32>::Alloc(3);

    //Using mBaseHandler, no difference in EVENT_GET_SIZE_DONE handling
    AutoPtr<IAtomicBoolean> status;
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&status);
    AutoPtr<IMessage> response;
    mBaseHandler->ObtainMessage(EVENT_GET_SIZE_DONE, status, (IMessage**)&response);

    AutoPtr<IIccFileHandler> fh;
    mPhone->GetIccFileHandler((IIccFileHandler**)&fh);
    //IccFileHandler can be null if there is no icc card present.
    if (fh != NULL) {
        fh->GetEFLinearRecordSize(efid, response);
        WaitForResult(status);
    }

    *result = mRecordSize;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode RuimPhoneBookInterfaceManager::Logd(
    /* [in] */ const String& msg)
{
    Logger::D(LOGTAG, "[RuimPbInterfaceManager] %d", msg.string());
    return NOERROR;
}

ECode RuimPhoneBookInterfaceManager::Loge(
    /* [in] */ const String& msg)
{
    Logger::E(LOGTAG, "[RuimPbInterfaceManager] %d", msg.string());
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
