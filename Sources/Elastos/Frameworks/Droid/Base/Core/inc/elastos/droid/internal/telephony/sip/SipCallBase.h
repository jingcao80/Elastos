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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_SIPCALLBASE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_SIPCALLBASE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/telephony/Call.h"

using Elastos::Droid::Internal::Telephony::Call;
using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Utility::IList;


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Sip {

class SipCallBase
    : public Call
    //, public ISipCallBase
{
public:
    // @Override
    CARAPI GetConnections(
        /* [out] */ IList/*<Connection>*/** result);

    // @Override
    CARAPI IsMultiparty(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    virtual CARAPI ClearDisconnected();

protected:
    virtual CARAPI_(void) SetState(
        /* [in] */ ICallState newState) = 0;
};

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_SIPCALLBASE_H__

