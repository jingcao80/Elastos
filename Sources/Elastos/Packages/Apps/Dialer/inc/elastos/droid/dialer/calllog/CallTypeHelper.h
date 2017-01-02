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

#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CALLTYPEHELPER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CALLTYPEHELPER_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::IInteger32;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

/**
 * Helper class to perform operations related to call types.
 */
class CallTypeHelper : public Object
{
public:
    CallTypeHelper(
        /* [in] */ IResources* resources);

    /** Returns the text used to represent the given call type. */
    CARAPI_(AutoPtr<ICharSequence>) GetCallTypeText(
        /* [in] */ Int32 callType,
        /* [in] */ Boolean isVideoCall);

    /** Returns the color used to highlight the given call type, null if not highlight is needed. */
    CARAPI_(AutoPtr<IInteger32>) GetHighlightedColor(
        /* [in] */ Int32 callType);

    static CARAPI_(Boolean) IsMissedCallType(
        /* [in] */ Int32 callType);

private:
    /** Name used to identify incoming calls. */
    String mIncomingName;
    /** Name used to identify outgoing calls. */
    String mOutgoingName;
    /** Name used to identify missed calls. */
    String mMissedName;
    /** Name used to identify incoming video calls. */
    String mIncomingVideoName;
    /** Name used to identify outgoing video calls. */
    String mOutgoingVideoName;
    /** Name used to identify missed video calls. */
    String mMissedVideoName;
    /** Name used to identify voicemail calls. */
    String mVoicemailName;
    /** Color used to identify new missed calls. */
    Int32 mNewMissedColor;
    /** Color used to identify new voicemail calls. */
    Int32 mNewVoicemailColor;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CALLTYPEHELPER_H__
