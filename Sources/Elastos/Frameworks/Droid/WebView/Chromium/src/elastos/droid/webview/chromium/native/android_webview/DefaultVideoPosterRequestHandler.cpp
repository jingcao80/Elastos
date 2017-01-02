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

//#include <Elastos.CoreLibrary.IO.h>
//#include "Elastos.Droid.Content.h"
//#include "Elastos.Droid.Graphics.h"
//#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/DefaultVideoPosterRequestHandler.h"

#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Droid::Graphics::BitmapCompressFormat_PNG;

using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IRunnable;
using Elastos::IO::IPipedInputStream;
using Elastos::IO::IFlushable;
using Elastos::IO::IOutputStream;
using Elastos::IO::CPipedInputStream;
using Elastos::IO::CPipedOutputStream;
using Elastos::Utility::IRandom;
using Elastos::Utility::CRandom;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

//================================================================================
//                 DefaultVideoPosterRequestHandler::UIRunnable
//================================================================================
CAR_INTERFACE_IMPL(DefaultVideoPosterRequestHandler::UIRunnable, Object, IRunnable);

DefaultVideoPosterRequestHandler::UIRunnable::UIRunnable(
    /* [in] */ DefaultVideoPosterRequestHandler* owner,
    /* [in] */ IPipedOutputStream* outputStream,
    /* [in] */ AwContentsClient* contentClient)
    : mOwner(owner)
    , mOutputStream(outputStream)
    , mContentClient(contentClient)
{
}

ECode DefaultVideoPosterRequestHandler::UIRunnable::Run()
{
    AutoPtr<IBitmap> defaultVideoPoster = mContentClient->GetDefaultVideoPoster();
    AutoPtr<IOutputStream> outputStream = IOutputStream::Probe(mOutputStream);
    if (defaultVideoPoster == NULL) {
        CloseOutputStream(outputStream);
        return NOERROR;
    }

    AutoPtr<IRunnable> runnable =  new AsyncTaskRunnable(mOwner, defaultVideoPoster, mOutputStream);
    AsyncTask::THREAD_POOL_EXECUTOR->Execute(runnable);

    return NOERROR;
}

//================================================================================
//               DefaultVideoPosterRequestHandler::AsyncTaskRunnable
//================================================================================
CAR_INTERFACE_IMPL(DefaultVideoPosterRequestHandler::AsyncTaskRunnable, Object, IRunnable);

DefaultVideoPosterRequestHandler::AsyncTaskRunnable::AsyncTaskRunnable(
    /* [in] */ DefaultVideoPosterRequestHandler* owner,
    /* [in] */ IBitmap* defaultVideoPoster,
    /* [in] */ IPipedOutputStream* outputStream)
    : mOwner(owner)
    , mDefaultVideoPoster(defaultVideoPoster)
    , mOutputStream(outputStream)
{
}

ECode DefaultVideoPosterRequestHandler::AsyncTaskRunnable::Run()
{
    // try {
    AutoPtr<IOutputStream> outputStream = IOutputStream::Probe(mOutputStream);
    Boolean result;
    ECode ecode = mDefaultVideoPoster->Compress(BitmapCompressFormat_PNG, 100, outputStream, &result);
    if (FAILED(ecode))
    {
        Logger::E(TAG, "DefaultVideoPosterRequestHandler::AsyncTaskRunnable::Run, compress ecode:0x%x", ecode);
        CloseOutputStream(outputStream);
        return ecode;
    }
    AutoPtr<IFlushable> outputStreamFlush = IFlushable::Probe(outputStream);
    ecode = outputStreamFlush->Flush();
    if (FAILED(ecode))
    {
        Logger::E(TAG, "DefaultVideoPosterRequestHandler::AsyncTaskRunnable::Run, flush ecode:0x%x", ecode);
        CloseOutputStream(outputStream);
        return ecode;
    }
    // } catch (IOException e) {
    //     Log.e(TAG, null, e);
    // } finally {
    //     closeOutputStream(outputStream);
    // }
    CloseOutputStream(outputStream);

    return NOERROR;
}

//================================================================================
//                      DefaultVideoPosterRequestHandler
//================================================================================

const String DefaultVideoPosterRequestHandler::TAG("DefaultVideoPosterRequestHandler");

DefaultVideoPosterRequestHandler::DefaultVideoPosterRequestHandler(
    /* [in] */ AwContentsClient* contentClient)
    : mDefaultVideoPosterURL(GenerateDefaulVideoPosterURL())
    , mContentClient(contentClient)
{
}

AutoPtr<IInputStream> DefaultVideoPosterRequestHandler::GetInputStream(
    /* [in] */ AwContentsClient* contentClient, DefaultVideoPosterRequestHandler* pThis)
{
    AutoPtr<IPipedInputStream> inputStream;
    CPipedInputStream::New((IPipedInputStream**)&inputStream);
    AutoPtr<IPipedOutputStream> outputStream;
    CPipedOutputStream::New(inputStream, (IPipedOutputStream**)&outputStream);

    // Send the request to UI thread to callback to the client, and if it provides a
    // valid bitmap bounce on to the worker thread pool to compress it into the piped
    // input/output stream.
    AutoPtr<IRunnable> runnable = new UIRunnable(pThis, outputStream, contentClient);
    ThreadUtils::RunOnUiThread(runnable);

    AutoPtr<IInputStream> input = IInputStream::Probe(inputStream);
    return input;
}

void DefaultVideoPosterRequestHandler::CloseOutputStream(
    /* [in] */ IOutputStream* outputStream) {
    // try {
        outputStream->Close();
    // } catch (IOException e) {
        Logger::E(TAG, "DefaultVideoPosterRequestHandler::CloseOutputStream Close error");
    // }
}

/**
 * Used to get the image if the url is mDefaultVideoPosterURL.
 *
 * @param url the url requested
 * @return AwWebResourceResponse which caller can get the image if the url is
 * the default video poster URL, otherwise null is returned.
 */
AutoPtr<AwWebResourceResponse> DefaultVideoPosterRequestHandler::ShouldInterceptRequest(
    /* [in] */ const String& url)
{
    if (!mDefaultVideoPosterURL.Equals(url)) return NULL;

    // try {
    //TODO how to handle the IOEXCEPTION
        AutoPtr<AwWebResourceResponse> response = new AwWebResourceResponse(String("image/png"), String(), GetInputStream(mContentClient, this));
        return response;
    // } catch (IOException e) {
    //     Log.e(TAG, null, e);
    //     return null;
    // }
}

String DefaultVideoPosterRequestHandler::GetDefaultVideoPosterURL()
{
    return mDefaultVideoPosterURL;
}

/**
 * @return a unique URL which has little chance to be used by application.
 */
String DefaultVideoPosterRequestHandler::GenerateDefaulVideoPosterURL()
{
    AutoPtr<IRandom> randomGenerator;
    CRandom::New((IRandom**)&randomGenerator);
    //String path = String.valueOf(randomGenerator.nextLong());
    Int64 randomNumber;
    randomGenerator->NextInt64(&randomNumber);
    String path = StringUtils::ToString(randomNumber);
    // The scheme of this URL should be kept in sync with kAndroidWebViewVideoPosterScheme
    // on the native side (see android_webview/common/url_constants.h)
    String strRet("android-webview-video-poster:default_video_poster/");
    strRet += path;
    return strRet;
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
