// wuweizuo automatic build .h file from .java file.
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_MEDIARESOURCEGETTER_H_
#define _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_MEDIARESOURCEGETTER_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.content.browser;
// import android.content.Context;
// import android.content.pm.PackageManager;
// import android.media.MediaMetadataRetriever;
// import android.net.ConnectivityManager;
// import android.net.NetworkInfo;
// import android.os.ParcelFileDescriptor;
// import android.text.TextUtils;
// import android.util.Log;
// import com.google.common.annotations.VisibleForTesting;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import org.chromium.base.PathUtils;
// import java.io.File;
// import java.io.IOException;
// import java.net.URI;
// import java.util.ArrayList;
// import java.util.HashMap;
// import java.util.List;
// import java.util.Map;

using Elastos::Core::IInteger32;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Media::IMediaMetadataRetriever;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Text::ITextUtils;
using Elastos::IO::IFile;
using Elastos::Net::IURI;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

/**
  * Java counterpart of android MediaResourceGetter.
  */
// @JNINamespace("content")
class MediaResourceGetter : public Object
{
private:
    // @VisibleForTesting
    class MediaMetadata : public Object
    {
    public:
        MediaMetadata(
            /* [in] */ Int32 durationInMilliseconds,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Boolean success);

        // TODO(andrewhayden): according to the spec, if duration is unknown
        // then we must return NaN. If it is unbounded, then positive infinity.
        // http://www.w3.org/html/wg/drafts/html/master/embedded-content-0.html
        // @CalledByNative("MediaMetadata")
        virtual CARAPI_(Int32) GetDurationInMilliseconds();

        // @CalledByNative("MediaMetadata")
        virtual CARAPI_(Int32) GetWidth();

        // @CalledByNative("MediaMetadata")
        virtual CARAPI_(Int32) GetHeight();

        // @CalledByNative("MediaMetadata")
        virtual CARAPI_(Boolean) IsSuccess();

        // @Override
        CARAPI_(String) ToString();

        // @Override
        CARAPI_(Int32) HashCode();

        // @Override
        CARAPI_(Boolean) Equals(
            /* [in] */ Object* obj);

        static CARAPI_(Int32) GetDurationInMilliseconds(
            /* [in] */ IInterface* obj);

        static CARAPI_(Int32) GetWidth(
            /* [in] */ IInterface* obj);

        static CARAPI_(Int32) GetHeight(
            /* [in] */ IInterface* obj);

        static CARAPI_(Boolean) IsSuccess(
            /* [in] */ IInterface* obj);

    private:
        const Int32 mDurationInMilliseconds;
        const Int32 mWidth;
        const Int32 mHeight;
        const Boolean mSuccess;
    };

public:
    // @VisibleForTesting
    virtual CARAPI_(AutoPtr<MediaMetadata>) Extract(
        /* [in] */ Int32 fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int64 length);

    // @VisibleForTesting
    virtual CARAPI_(AutoPtr<MediaMetadata>) Extract(
        /* [in] */ /*const*/ IContext* context,
        /* [in] */ const String& url,
        /* [in] */ const String& cookies,
        /* [in] */ const String& userAgent);

    // @VisibleForTesting
    virtual CARAPI_(Boolean) Configure(
        /* [in] */ IContext* context,
        /* [in] */ const String& url,
        /* [in] */ const String& cookies,
        /* [in] */ const String& userAgent);

    /**
      * @return true if the device is on an ethernet or wifi network.
      * If anything goes wrong (e.g., permission denied while trying to access
      * the network state), returns false.
      */
    // @VisibleForTesting
    virtual CARAPI_(Boolean) IsNetworkReliable(
        /* [in] */ IContext* context);

    /**
      * @param file the file whose path should be checked
      * @return true if and only if the file is in a location that we consider
      * safe to read from, such as /mnt/sdcard.
      */
    // @VisibleForTesting
    virtual CARAPI_(Boolean) FilePathAcceptable(
        /* [in] */ IFile* file);

    /**
      * Special case handling for device/OS combos that simply do not work.
      * @param model the model of device being examined
      * @param sdkVersion the version of the SDK installed on the device
      * @return true if the device can be used correctly, otherwise false
      */
    // @VisibleForTesting
    static CARAPI_(Boolean) AndroidDeviceOk(
        /* [in] */ const String& model,
        /* [in] */ const Int32& sdkVersion);

    // The methods below can be used by unit tests to fake functionality.
    // @VisibleForTesting
    virtual CARAPI_(AutoPtr<IFile>) UriToFile(
        /* [in] */ const String& path);

    // @VisibleForTesting
    virtual CARAPI_(AutoPtr<IInteger32>) GetNetworkType(
        /* [in] */ IContext* context);

    // @VisibleForTesting
    virtual CARAPI_(String) GetExternalStorageDirectory();

    // @VisibleForTesting
    virtual CARAPI Configure(
        /* [in] */ Int32 fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int64 length);

    // @VisibleForTesting
    virtual CARAPI Configure(
        /* [in] */ const String& url,
        /* [in] */ IMap* headers);

    // @VisibleForTesting
    virtual CARAPI Configure(
        /* [in] */ const String& path);

    // @VisibleForTesting
    virtual CARAPI_(String) ExtractMetadata(
        /* [in] */ Int32 key);

    static CARAPI_(void*) ElaMediaResourceGetterCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) ExtractMediaMetadata(
        /* [in] */ IInterface* context,
        /* [in] */ const String& url,
        /* [in] */ const String& cookies,
        /* [in] */ const String& userAgent);

    // @CalledByNative
    static CARAPI_(AutoPtr<MediaMetadata>) ExtractMediaMetadata(
        /* [in] */ /*const*/ IContext* context,
        /* [in] */ const String& url,
        /* [in] */ const String& cookies,
        /* [in] */ const String& userAgent);

    // @CalledByNative return MediaMetadata
    static CARAPI_(AutoPtr<IInterface>) ExtractMediaMetadataFromFd(
        /* [in] */ Int32 fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int64 length);

    CARAPI_(AutoPtr<MediaMetadata>) DoExtractMetadata();

    // This method covers only typcial expressions for the loopback address
    // to resolve the hostname without a DNS loopup.
    CARAPI_(Boolean) IsLoopbackAddress(
        /* [in] */ const String& host);

    CARAPI_(AutoPtr<IList>) GetRawAcceptableDirectories();

    CARAPI_(AutoPtr<IList>) Canonicalize(
        /* [in] */ IList* paths);

private:
    static const String TAG;
    /*const*/ AutoPtr<MediaMetadata> EMPTY_METADATA;
    /*const*/ AutoPtr<IMediaMetadataRetriever> mRetriever;
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_MEDIARESOURCEGETTER_H_
