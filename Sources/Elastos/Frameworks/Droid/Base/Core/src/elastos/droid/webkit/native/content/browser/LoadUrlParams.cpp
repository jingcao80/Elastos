
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/webkit/native/content/browser/LoadUrlParams.h"
#include "elastos/droid/webkit/native/content/api/LoadUrlParams_dec.h"
#include "elastos/droid/webkit/native/content/browser/PageTransitionTypes.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterable;
using Elastos::Utility::EIID_IIterable;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

//=====================================================================
//                            LoadUrlParams
//=====================================================================

const Int32 LoadUrlParams::LOAD_TYPE_DEFAULT;
const Int32 LoadUrlParams::LOAD_TYPE_BROWSER_INITIATED_HTTP_POST;
const Int32 LoadUrlParams::LOAD_TYPE_DATA;
const Int32 LoadUrlParams::UA_OVERRIDE_INHERIT;
const Int32 LoadUrlParams::UA_OVERRIDE_FALSE;
const Int32 LoadUrlParams::UA_OVERRIDE_TRUE;

LoadUrlParams::LoadUrlParams(
    /* [in] */ const String& url)
    : mUrl(url)
    , mLoadUrlType(LOAD_TYPE_DEFAULT)
    , mTransitionType(PageTransitionTypes::PAGE_TRANSITION_LINK)
    , mUaOverrideOption(UA_OVERRIDE_INHERIT)
    , mCanLoadLocalResources(FALSE)
    , mIsRendererInitiated(FALSE)
{
}

LoadUrlParams::LoadUrlParams(
    /* [in] */ const String& url,
    /* [in] */ Int32 transitionType)
    : mUrl(url)
    , mLoadUrlType(LOAD_TYPE_DEFAULT)
    , mTransitionType(transitionType)
    , mUaOverrideOption(UA_OVERRIDE_INHERIT)
    , mCanLoadLocalResources(FALSE)
    , mIsRendererInitiated(FALSE)
{
}

AutoPtr<LoadUrlParams> LoadUrlParams::CreateLoadDataParams(
    /* [in] */ const String& data,
    /* [in] */ const String& mimeType,
    /* [in] */ Boolean isBase64Encoded)
{
    return CreateLoadDataParams(data, mimeType, isBase64Encoded, String(NULL));
}

AutoPtr<LoadUrlParams> LoadUrlParams::CreateLoadDataParams(
    /* [in] */ const String& data,
    /* [in] */ const String& mimeType,
    /* [in] */ Boolean isBase64Encoded,
    /* [in] */ const String& charset)
{
    AutoPtr<StringBuilder> dataUrl = new StringBuilder("data:");
    dataUrl->Append(mimeType);
    if (charset != NULL && !charset.IsEmpty()) {
        dataUrl->Append(";charset=");
        dataUrl->Append(charset);
    }

    if (isBase64Encoded) {
        dataUrl->Append(";base64");
    }

    dataUrl->Append(",");
    dataUrl->Append(data);

    AutoPtr<LoadUrlParams> params = new LoadUrlParams(dataUrl->ToString());
    params->SetLoadType(LoadUrlParams::LOAD_TYPE_DATA);
    params->SetTransitionType(PageTransitionTypes::PAGE_TRANSITION_TYPED);

    return params;
}

AutoPtr<LoadUrlParams> LoadUrlParams::CreateLoadDataParamsWithBaseUrl(
    /* [in] */ const String& data,
    /* [in] */ const String& mimeType,
    /* [in] */ Boolean isBase64Encoded,
    /* [in] */ const String& baseUrl,
    /* [in] */ const String& historyUrl)
{
    return CreateLoadDataParamsWithBaseUrl(data, mimeType, isBase64Encoded,
             baseUrl, historyUrl, String(NULL));
}

AutoPtr<LoadUrlParams> LoadUrlParams::CreateLoadDataParamsWithBaseUrl(
    /* [in] */ const String& data,
    /* [in] */ const String& mimeType,
    /* [in] */ Boolean isBase64Encoded,
    /* [in] */ const String& baseUrl,
    /* [in] */ const String& historyUrl,
    /* [in] */ const String& charset)
{
    AutoPtr<LoadUrlParams> params = CreateLoadDataParams(data, mimeType, isBase64Encoded, charset);

    // For WebView compatibility, when the base URL has the 'data:'
    // scheme, we treat it as a regular data URL load and skip setting
    // // baseUrl and historyUrl.
    // TODO(joth): we should just append baseURL and historyURL here, and move the
    // WebView specific transform up to a wrapper factory function in android_webview/.
    assert(0);
    // TODO
    // if (baseUrl == NULL || !baseUrl.ToLowerCase(ILocale::US).StartsWith("data:")) {
    //     params->SetBaseUrlForDataUrl(baseUrl != NULL ? baseUrl : String("about:blank"));
    //     params->SetVirtualUrlForDataUrl(historyUrl != NULL ? historyUrl : String("about:blank"));
    // }

    return params;
}

AutoPtr<LoadUrlParams> LoadUrlParams::CreateLoadHttpPostParams(
    /* [in] */ const String& url,
    /* [in] */ ArrayOf<Byte>* postData)
{
    AutoPtr<LoadUrlParams> params = new LoadUrlParams(url);
    params->SetLoadType(LOAD_TYPE_BROWSER_INITIATED_HTTP_POST);
    params->SetTransitionType(PageTransitionTypes::PAGE_TRANSITION_TYPED);
    params->SetPostData(postData);
    return params;
}

ECode LoadUrlParams::SetUrl(
    /* [in] */ const String& url)
{
    mUrl = url;
    return NOERROR;
}

String LoadUrlParams::GetUrl()
{
    return mUrl;
    return String("");
}

String LoadUrlParams::GetBaseUrl()
{
    return mBaseUrlForDataUrl;
}

ECode LoadUrlParams::SetLoadType(
    /* [in] */ Int32 loadType)
{
    mLoadUrlType = loadType;
    return NOERROR;
}

ECode LoadUrlParams::SetTransitionType(
    /* [in] */ Int32 transitionType)
{
    mTransitionType = transitionType;
    return NOERROR;
}

Int32 LoadUrlParams::GetTransitionType()
{
    return mTransitionType;
}

ECode LoadUrlParams::SetReferrer(
    /* [in] */ Referrer* referrer)
{
    mReferrer = referrer;
    return NOERROR;
}

AutoPtr<Referrer> LoadUrlParams::GetReferrer()
{
    return mReferrer;
}

ECode LoadUrlParams::SetExtraHeaders(
    /* [in] */ IMap* extraHeaders)
{
    mExtraHeaders = extraHeaders;
    return NOERROR;
}

AutoPtr<IMap> LoadUrlParams::GetExtraHeaders()
{
    return mExtraHeaders;
}

String LoadUrlParams::GetExtraHeadersString()
{
    return GetExtraHeadersString(String("\n"), FALSE);
}

String LoadUrlParams::GetExtraHttpRequestHeadersString()
{
    return GetExtraHeadersString(String("\r\n"), TRUE);
}

ECode LoadUrlParams::SetVerbatimHeaders(
    /* [in] */ const String& headers)
{
    mVerbatimHeaders = headers;
    return NOERROR;
}

String LoadUrlParams::GetVerbatimHeaders()
{
    return mVerbatimHeaders;
}

ECode LoadUrlParams::SetOverrideUserAgent(
    /* [in] */ Int32 uaOption)
{
    mUaOverrideOption = uaOption;
    return NOERROR;
}

ECode LoadUrlParams::SetPostData(
    /* [in] */ ArrayOf<Byte>* postData)
{
    mPostData = postData;
    return NOERROR;
}

AutoPtr< ArrayOf<Byte> > LoadUrlParams::GetPostData()
{
    return mPostData;
}

ECode LoadUrlParams::SetBaseUrlForDataUrl(
    /* [in] */ const String& baseUrl)
{
    mBaseUrlForDataUrl = baseUrl;
    return NOERROR;
}

ECode LoadUrlParams::SetVirtualUrlForDataUrl(
    /* [in] */ const String& virtualUrl)
{
    mVirtualUrlForDataUrl = virtualUrl;
    return NOERROR;
}

ECode LoadUrlParams::SetCanLoadLocalResources(
    /* [in] */ Boolean canLoad)
{
    mCanLoadLocalResources = canLoad;
    return NOERROR;
}

Int32 LoadUrlParams::GetLoadUrlType()
{
    return mLoadUrlType;
}

ECode LoadUrlParams::SetIsRendererInitiated(
    /* [in] */ Boolean rendererInitiated)
{
    mIsRendererInitiated = rendererInitiated;
    return NOERROR;
}

Boolean LoadUrlParams::GetIsRendererInitiated()
{
    return mIsRendererInitiated;
}

Boolean LoadUrlParams::IsBaseUrlDataScheme()
{
    // If there's no base url set, but this is a data load then
    // treat the scheme as data:.
    if (mBaseUrlForDataUrl == NULL && mLoadUrlType == LOAD_TYPE_DATA) {
        return TRUE;
    }

    return NativeIsDataScheme(mBaseUrlForDataUrl);
}

String LoadUrlParams::GetExtraHeadersString(
    /* [in] */ const String& delimiter,
    /* [in] */ Boolean addTerminator)
{
    if (mExtraHeaders == NULL) return String(NULL);

    AutoPtr<StringBuilder> headerBuilder = new StringBuilder();
    AutoPtr<ISet> set;
    mExtraHeaders->GetEntrySet((ISet**)&set);
    AutoPtr<IIterable> iterable = IIterable::Probe(set);
    AutoPtr<IIterator> iter;
    iterable->GetIterator((IIterator**)&iter);
    Boolean bHasNext = FALSE;
    AutoPtr<IMapEntry> header;
    for (iter->HasNext(&bHasNext); bHasNext; iter->HasNext(&bHasNext)) {
        if (headerBuilder->GetLength() > 0) {
            headerBuilder->Append(delimiter);
        }

        // Header name should be lower case.
        AutoPtr<ICharSequence> key;
        header->GetKey((IInterface**)&key);
        assert(0);
        // TODO
        // headerBuilder.append(header.getKey().toLowerCase(Locale.US));
        headerBuilder->Append(":");
        AutoPtr<ICharSequence> value;
        header->GetValue((IInterface**)&value);
        String strValue;
        value->ToString(&strValue);
        headerBuilder->Append(strValue);
    }

    if (addTerminator) {
        headerBuilder->Append(delimiter);
    }

    return headerBuilder->ToString();
}

void LoadUrlParams::InitializeConstants(
    /* [in] */ Int32 load_type_default,
    /* [in] */ Int32 load_type_browser_initiated_http_post,
    /* [in] */ Int32 load_type_data,
    /* [in] */ Int32 ua_override_inherit,
    /* [in] */ Int32 ua_override_false,
    /* [in] */ Int32 ua_override_true)
{
    assert(LOAD_TYPE_DEFAULT == load_type_default);
    assert(LOAD_TYPE_BROWSER_INITIATED_HTTP_POST == load_type_browser_initiated_http_post);
    assert(LOAD_TYPE_DATA == load_type_data);
    assert(UA_OVERRIDE_INHERIT == ua_override_inherit);
    assert(UA_OVERRIDE_FALSE == ua_override_false);
    assert(UA_OVERRIDE_TRUE == ua_override_true);
}

Boolean LoadUrlParams::NativeIsDataScheme(
    /* [in] */ const String& url)
{
    return Elastos_LoadUrlParams_nativeIsDataScheme(url);
}

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
