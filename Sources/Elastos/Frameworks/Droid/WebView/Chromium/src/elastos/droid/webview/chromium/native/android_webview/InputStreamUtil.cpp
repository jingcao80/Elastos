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

#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/webkit/webview/chromium/native/android_webview/InputStreamUtil.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/InputStreamUtil_dec.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

const String InputStreamUtil::LOGTAG("AwAssets");
const Int32 InputStreamUtil::CALL_FAILED_STATUS;
const Int32 InputStreamUtil::EXCEPTION_THROWN_STATUS;

String InputStreamUtil::LogMessage(
    /* [in] */ const String& method)
{
    String retStr("Got exception when calling ");
    retStr += method;
    retStr += "() on an InputStream returned from ";
    retStr += "shouldInterceptRequest. This will cause the related request to fail.";

    return retStr;
}

//@CalledByNative
void InputStreamUtil::Close(
    /* [in] */ IInputStream* stream)
{
    // try {
        ECode ecode = stream->Close();
        if (FAILED(ecode))
            Logger::E(LOGTAG, "InputStreamUtil::Close, ecode:0x%x", ecode);
    // } catch (IOException e) {
    //     Log.e(LOGTAG, logMessage("close"), e);
    // }
}

//@CalledByNative
Int32 InputStreamUtil::Available(
    /* [in] */ IInputStream* stream)
{
    // try {
        Int32 available;
        ECode ecode = stream->Available(&available);
        if (FAILED(ecode))
        {
            Logger::E(LOGTAG, "InputStreamUtil::Available, ecode:0x%x", ecode);
            return EXCEPTION_THROWN_STATUS;
        }
        return Elastos::Core::Math::Max(CALL_FAILED_STATUS, available);
    // } catch (IOException e) {
    //     Log.e(LOGTAG, logMessage("available"), e);
    //     return EXCEPTION_THROWN_STATUS;
    // }
}

//@CalledByNative
Int32 InputStreamUtil::Read(
    /* [in] */ IInputStream* stream,
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    // try {
        Int32 length;
        ECode ecode = stream->Read(b, off, len, &length);
        if (FAILED(ecode))
        {
            Logger::E(LOGTAG, "InputStreamUtil::Read, ecode:0x%x", ecode);
            return EXCEPTION_THROWN_STATUS;
        }
        return Elastos::Core::Math::Max(CALL_FAILED_STATUS, length);
    // } catch (IOException e) {
    //     Log.e(LOGTAG, logMessage("read"), e);
    //     return EXCEPTION_THROWN_STATUS;
    // }
}

//@CalledByNative
Int64 InputStreamUtil::Skip(
    /* [in] */ IInputStream* stream,
    /* [in] */ Int64 n)
{
    // try {
        Int64 skip;
        ECode ecode = stream->Skip(n, &skip);
        if (FAILED(ecode))
        {
            Logger::E(LOGTAG, "InputStreamUtil::Skip, ecode:0x%x", ecode);
            return EXCEPTION_THROWN_STATUS;
        }
        return Elastos::Core::Math::Max((Int64)CALL_FAILED_STATUS, skip);
    // } catch (IOException e) {
    //     Log.e(LOGTAG, logMessage("skip"), e);
    //     return EXCEPTION_THROWN_STATUS;
    // }
}
//callback function definition
void InputStreamUtil::Close(
    /* [in] */ IInterface* stream)
{
    AutoPtr<IInputStream> is = IInputStream::Probe(stream);
    Close(is);
}

Int32 InputStreamUtil::Available(
    /* [in] */ IInterface* stream)
{
    AutoPtr<IInputStream> is = IInputStream::Probe(stream);
    return Available(is);
}

Int32 InputStreamUtil::Read(
    /* [in] */ IInterface* stream,
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    AutoPtr<IInputStream> is = IInputStream::Probe(stream);
    return Read(is, b, off, len);
}

Int64 InputStreamUtil::Skip(
    /* [in] */ IInterface* stream,
    /* [in] */ Int64 n)
{
    AutoPtr<IInputStream> is = IInputStream::Probe(stream);
    return Skip(is, n);
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
