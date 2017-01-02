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

#ifndef __ELASTOS_DROID_MEDIA_PROJECTION_CMEDIAPROJECTIONMANAGER_H__
#define __ELASTOS_DROID_MEDIA_PROJECTION_CMEDIAPROJECTIONMANAGER_H__

#include "_Elastos_Droid_Media_Projection_CMediaProjectionManager.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/core/Runnable.h>
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/media/projection/CMediaProjection.h"

using Elastos::Core::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Media::Projection::CMediaProjection;
using Elastos::Droid::Media::Projection::IIMediaProjectionWatcherCallback;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Projection {

/**
 * Manages the retrieval of certain types of {@link MediaProjection} tokens.
 *
 * <p>
 * Get an instance of this class by calling {@link
 * android.content.Context#getSystemService(java.lang.String)
 * Context.getSystemService()} with the argument {@link
 * android.content.Context#MEDIA_PROJECTION_SERVICE}.
 * </p>
 */
CarClass(CMediaProjectionManager)
    , public Object
    , public IMediaProjectionManager
{
private:
    class CallbackDelegate
        : public Object
        , public IIMediaProjectionWatcherCallback
    {
    public:
        CAR_INTERFACE_DECL()

        CallbackDelegate(
            /* [in] */ CMediaProjectionManager* host,
            /* [in] */ IMediaProjectionManagerCallback * cb,
            /* [in] */ IHandler * handler);

        CARAPI OnStart(
            /* [in] */ IMediaProjectionInfo * info);

        CARAPI OnStop(
            /* [in] */ IMediaProjectionInfo * info);

    private:
        CMediaProjectionManager* mHost;
        AutoPtr<IMediaProjectionManagerCallback> mCallback;
        AutoPtr<IHandler> mHandler;
    };

    class OnStartRunnable
        : public Runnable
    {
    public:
        OnStartRunnable(
            /* [in] */ CallbackDelegate* host,
            /* [in] */ IMediaProjectionManagerCallback * cb,
            /* [in] */ IMediaProjectionInfo * info)
            : mHost(host)
            , mCallback(cb)
            , mInfo(info)
        {}

        CARAPI Run();

    private:
        CallbackDelegate* mHost;
        AutoPtr<IMediaProjectionManagerCallback> mCallback;
        AutoPtr<IMediaProjectionInfo> mInfo;
    };

    class OnStopRunnable
        : public Runnable
    {
    public:
        OnStopRunnable(
            /* [in] */ CallbackDelegate* host,
            /* [in] */ IMediaProjectionManagerCallback * cb,
            /* [in] */ IMediaProjectionInfo * info)
            : mHost(host)
            , mCallback(cb)
            , mInfo(info)
        {}

        CARAPI Run();

    private:
        CallbackDelegate* mHost;
        AutoPtr<IMediaProjectionManagerCallback> mCallback;
        AutoPtr<IMediaProjectionInfo> mInfo;
    };

public:
    CMediaProjectionManager();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CMediaProjectionManager();

    /** @hide */
    CARAPI constructor(
        /* [in] */ IContext * context);

    /**
     * Returns an Intent that <b>must</b> passed to startActivityForResult()
     * in order to start screen capture. The activity will prompt
     * the user whether to allow screen capture.  The result of this
     * activity should be passed to getMediaProjection.
     */
    CARAPI CreateScreenCaptureIntent(
        /* [out] */ IIntent ** result);

    /**
     * Retrieve the MediaProjection obtained from a succesful screen
     * capture request. Will be null if the result from the
     * startActivityForResult() is anything other than RESULT_OK.
     *
     * @param resultCode The result code from {@link android.app.Activity#onActivityResult(int,
     * int, android.content.Intent)}
     * @param resultData The resulting data from {@link android.app.Activity#onActivityResult(int,
     * int, android.content.Intent)}
     */
    CARAPI GetMediaProjection(
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent * resultData,
        /* [out] */ IMediaProjection ** result);

    /**
     * Get the {@link MediaProjectionInfo} for the active {@link MediaProjection}.
     * @hide
     */
    CARAPI GetActiveProjectionInfo(
        /* [out] */ IMediaProjectionInfo ** result);

    /**
     * Stop the current projection if there is one.
     * @hide
     */
    CARAPI StopActiveProjection();

    /**
     * Add a callback to monitor all of the {@link MediaProjection}s activity.
     * Not for use by regular applications, must have the MANAGE_MEDIA_PROJECTION permission.
     * @hide
     */
    CARAPI AddCallback(
        /* [in] */ IMediaProjectionManagerCallback * cb,
        /* [in] */ IHandler * handler);

    /**
     * Remove a MediaProjection monitoring callback.
     * @hide
     */
    CARAPI RemoveCallback(
        /* [in] */ IMediaProjectionManagerCallback * cb);

private:
    static String TAG; // = "MediaProjectionManager";
    IContext* mContext;
    AutoPtr<IArrayMap> mCallbacks;
    AutoPtr<IIMediaProjectionManager> mService;

};

} // namespace Projection
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_PROJECTION_CMEDIAPROJECTIONMANAGER_H__
