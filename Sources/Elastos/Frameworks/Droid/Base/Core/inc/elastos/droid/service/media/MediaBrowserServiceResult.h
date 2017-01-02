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

#ifndef __ELASTOS_DROID_SERVICE_MEDIA_MEDIABROWSERSERVICERESULT_H__
#define __ELASTOS_DROID_SERVICE_MEDIA_MEDIABROWSERSERVICERESULT_H__

#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Service {
namespace Media {

/**
 * Completion handler for asynchronous callback methods in {@link MediaBrowserService}.
 * <p>
 * Each of the methods that takes one of these to send the result must call
 * {@link #sendResult} to respond to the caller with the given results.  If those
 * functions return without calling {@link #sendResult}, they must instead call
 * {@link #detach} before returning, and then may call {@link #sendResult} when
 * they are done.  If more than one of those methods is called, an exception will
 * be thrown.
 *
 * @see MediaBrowserService#onLoadChildren
 * @see MediaBrowserService#onLoadIcon
 */
class MediaBrowserServiceResult
    : public Object
{
public:
    MediaBrowserServiceResult();

    CARAPI constructor(
        /* [in] */ IInterface * debug);

    /**
     * Send the result back to the caller.
     */
    CARAPI SendResult(
        /* [in] */ IInterface * result);

    /**
     * Detach this message from the current thread and allow the {@link #sendResult}
     * call to happen later.
     */
    CARAPI Detach();

    CARAPI_(Boolean) IsDone();

    /**
     * Called when the result is sent, after assertions about not being called twice
     * have happened.
     */
    CARAPI OnResultSent(
        /* [in] */ IInterface * result);

private:
    AutoPtr<IInterface> mDebug;
    Boolean mDetachCalled;
    Boolean mSendResultCalled;
};

} // namespace Media
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_MEDIA_MEDIABROWSERSERVICERESULT_H__
