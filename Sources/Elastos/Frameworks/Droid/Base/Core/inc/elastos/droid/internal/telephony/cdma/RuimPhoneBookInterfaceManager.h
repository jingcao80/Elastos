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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_RUIMPHONEBOOKINTERFACEMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_RUIMPHONEBOOKINTERFACEMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/internal/telephony/IccPhoneBookInterfaceManager.h"

// import java.util.concurrent.atomic.AtomicBoolean;
// import android.os.Message;
// import android.telephony.Rlog;
// import com.android.internal.telephony.uicc.IccFileHandler;

using Elastos::Droid::Internal::Telephony::IccPhoneBookInterfaceManager;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

/**
  * RuimPhoneBookInterfaceManager to provide an inter-process communication to
  * access ADN-like SIM records.
  */
class RuimPhoneBookInterfaceManager
    : public IccPhoneBookInterfaceManager
    , public IRuimPhoneBookInterfaceManager
{
public:
    CAR_INTERFACE_DECL();

    virtual ~RuimPhoneBookInterfaceManager();

    CARAPI constructor(
        /* [in] */ ICDMAPhone* phone);

    // @Override
    CARAPI Dispose();

    // @Override
    CARAPI GetAdnRecordsSize(
        /* [in] */ Int32 efid,
        /* [out] */ ArrayOf<Int32>** result);

protected:
    // @Override
    CARAPI Logd(
        /* [in] */ const String& msg);

    // @Override
    CARAPI Loge(
        /* [in] */ const String& msg);

public:
    static const String LOGTAG;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_RUIMPHONEBOOKINTERFACEMANAGER_H__

