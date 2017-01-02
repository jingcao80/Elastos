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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_DTMFLOCALTONEPLAYER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_DTMFLOCALTONEPLAYER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IToneGenerator;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Plays DTMF tones locally for the caller to hear. In order to reduce (1) the amount of times we
 * check the "play local tones" setting and (2) the length of time we keep the tone generator, this
 * class employs a concept of a call "session" that starts and stops when the foreground call
 * changes.
 */
class DtmfLocalTonePlayer
    : public CallsManagerListenerBase
{
public:
    CARAPI constructor(
        /* [in] */ IContext* context);

    /** {@inheritDoc} */
    // @Override
    CARAPI OnForegroundCallChanged(
        /* [in] */ ICall* oldForegroundCall,
        /* [in] */ ICall* newForegroundCall);

    /**
     * Starts playing the dtmf tone specified by c.
     *
     * @param call The associated call.
     * @param c The digit to play.
     */
    CARAPI PlayTone(
        /* [in] */ ICall* call,
        /* [in] */ Char32 c);

    /**
     * Stops any currently playing dtmf tone.
     *
     * @param call The associated call.
     */
    CARAPI StopTone(
        /* [in] */ ICall* call);

private:
    /**
     * Runs initialization requires to play local tones during a call.
     *
     * @param call The call associated with this dtmf session.
     */
    CARAPI StartDtmfSession(
        /* [in] */ ICall* call);

    /**
     * Releases resources needed for playing local dtmf tones.
     *
     * @param call The call associated with the session to end.
     */
    CARAPI EndDtmfSession(
        /* [in] */ ICall* call);

    static CARAPI_(AutoPtr<IMap>) InitTONE_MAP(); /* ImmutableMap */

private:
    static AutoPtr<IMap> TONE_MAP;

    /** Generator used to actually play the tone. */
    AutoPtr<IToneGenerator> mToneGenerator;

    /** The current call associated with an existing dtmf session. */
    AutoPtr<ICall> mCall;

    /** The context. */
    AutoPtr<IContext> mContext;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_DTMFLOCALTONEPLAYER_H__
