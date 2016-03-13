
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwWebResourceResponse.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/AwWebResourceResponse_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

AwWebResourceResponse::AwWebResourceResponse(
    /* [in] */ const String& mimeType,
    /* [in] */ const String& encoding,
    /* [in] */ IInputStream* data)
    : mMimeType(mimeType)
    , mCharset(encoding)
    , mData(data)
    , mStatusCode(0)
{
}

AwWebResourceResponse::AwWebResourceResponse(
    /* [in] */ const String& mimeType,
    /* [in] */ const String& encoding,
    /* [in] */ IInputStream* data,
    /* [in] */ Int32 statusCode,
    /* [in] */ const String& reasonPhrase,
    /* [in] */ Map<String, String> &responseHeaders)
    : mMimeType(mimeType)
    , mCharset(encoding)
    , mData(data)
    , mStatusCode(statusCode)
    , mReasonPhrase(reasonPhrase)
{
    Int32 size = responseHeaders.GetSize();
    //if (responseHeaders != null)
    if (size > 0)
    {
        mResponseHeaderNames = ArrayOf<String>::Alloc(size);//new String[responseHeaders.size()];
        mResponseHeaderValues = ArrayOf<String>::Alloc(size);//new String[responseHeaders.size()];
        int i = 0;
        Map<String, String>::Iterator iter = responseHeaders.Begin();
        //for (Map.Entry<String, String> entry : responseHeaders.entrySet())
        while (iter != responseHeaders.End())
        {
            (*mResponseHeaderNames)[i] = iter->mFirst;//entry.getKey();
            (*mResponseHeaderValues)[i] = iter->mSecond;//entry.getValue();
            ++iter;
            ++i;
        }
    }
}

//@CalledByNative
String AwWebResourceResponse::GetMimeType()
{
    return mMimeType;
}

//@CalledByNative
String AwWebResourceResponse::GetCharset()
{
    return mCharset;
}

//@CalledByNative
AutoPtr<IInputStream> AwWebResourceResponse::GetData()
{
    return mData;
}

//@CalledByNative
Int32 AwWebResourceResponse::GetStatusCode()
{
    return mStatusCode;
}

//@CalledByNative
String AwWebResourceResponse::GetReasonPhrase()
{
    return mReasonPhrase;
}

//@CalledByNative
AutoPtr<ArrayOf<String> > AwWebResourceResponse::GetResponseHeaderNames()
{
    return mResponseHeaderNames;
}

//@CalledByNative
AutoPtr<ArrayOf<String> > AwWebResourceResponse::GetResponseHeaderValues()
{
    return mResponseHeaderValues;
}

//callback function definition
String AwWebResourceResponse::GetMimeType(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwWebResourceResponse> mObj = (AwWebResourceResponse*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwWebResourceResponse", "AwWebResourceResponse::GetMimeType, mObj is NULL");
        return String(NULL);
    }
    return mObj->GetMimeType();
}

String AwWebResourceResponse::GetCharset(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwWebResourceResponse> mObj = (AwWebResourceResponse*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwWebResourceResponse", "AwWebResourceResponse::GetCharset, mObj is NULL");
        return String(NULL);
    }
    return mObj->GetCharset();
}

AutoPtr<IInterface> AwWebResourceResponse::GetData(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwWebResourceResponse> mObj = (AwWebResourceResponse*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwWebResourceResponse", "AwWebResourceResponse::GetData, mObj is NULL");
        return NULL;
    }
    return mObj->GetData();
}

Int32 AwWebResourceResponse::GetStatusCode(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwWebResourceResponse> mObj = (AwWebResourceResponse*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwWebResourceResponse", "AwWebResourceResponse::GetStatusCode, mObj is NULL");
        return 0;
    }
    return mObj->GetStatusCode();
}

String AwWebResourceResponse::GetReasonPhrase(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwWebResourceResponse> mObj = (AwWebResourceResponse*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwWebResourceResponse", "AwWebResourceResponse::GetReasonPhrase, mObj is NULL");
        return String(NULL);
    }
    return mObj->GetReasonPhrase();
}

AutoPtr<ArrayOf<String> > AwWebResourceResponse::GetResponseHeaderNames(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwWebResourceResponse> mObj = (AwWebResourceResponse*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwWebResourceResponse", "AwWebResourceResponse::GetResponseHeaderNames, mObj is NULL");
        return NULL;
    }
    return mObj->GetResponseHeaderNames();
}

AutoPtr<ArrayOf<String> > AwWebResourceResponse::GetResponseHeaderValues(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwWebResourceResponse> mObj = (AwWebResourceResponse*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwWebResourceResponse", "AwWebResourceResponse::GetResponseHeaderValues, mObj is NULL");
        return NULL;
    }
    return mObj->GetResponseHeaderValues();
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
