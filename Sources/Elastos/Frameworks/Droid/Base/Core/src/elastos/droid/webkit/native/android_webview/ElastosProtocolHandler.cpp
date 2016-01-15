
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/webkit/native/android_webview/ElastosProtocolHandler.h"
#include "elastos/droid/webkit/native/android_webview/api/ElastosProtocolHandler_dec.h"
//TODO #include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/utility/CTypedValue.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUriHelper;
//TODO using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Net::IURLConnection;
using Elastos::Net::IURLConnectionHelper;
//TODO using Elastos::Net::CURLConnectionHelper;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

const String ElastosProtocolHandler::TAG("ElastosProtocolHandler");
const String ElastosProtocolHandler::FILE_SCHEME("file");
const String ElastosProtocolHandler::CONTENT_SCHEME("content");

/**
 * Open an InputStream for an Android resource.
 * @param context The context manager.
 * @param url The url to load.
 * @return An InputStream to the Android resource.
 */
//@CalledByNative return IInputStream IContext
AutoPtr<IInterface> ElastosProtocolHandler::Open(
    /* [in] */ IInterface* icontext,
    /* [in] */ const String& url)
{
    AutoPtr<IContext> context = IContext::Probe(icontext);
    AutoPtr<IUri> uri = VerifyUrl(url);
    if (uri == NULL) {
        return NULL;
    }
    //try {
        String path;
        uri->GetPath(&path);
        String scheme;
        uri->GetScheme(&scheme);
        if (scheme.Equals(FILE_SCHEME)) {
            if (path.StartWith(NativeGetAndroidAssetPath())) {
                AutoPtr<IInputStream> inputStream = OpenAsset(context, uri);
                return inputStream;
            }
            else if (path.StartWith(NativeGetAndroidResourcePath())) {
                AutoPtr<IInputStream> inputStream = OpenResource(context, uri);
                return inputStream;
            }
        }
        else if (scheme.Equals(CONTENT_SCHEME)) {
            AutoPtr<IInputStream> inputStream = OpenContent(context, uri);
            return inputStream;
        }
    //} catch (Exception ex) {
    //    Log.e(TAG, "Error opening inputstream: " + url);
    //}
    return NULL;
}

Int32 ElastosProtocolHandler::GetFieldId(
    /* [in] */ IContext* context,
    /* [in] */ const String& assetType,
    /* [in] */ const String& assetName)
{
    /*TODO
    Class<?> d = context.getClassLoader()
        .loadClass(context.getPackageName() + ".R$" + assetType);
    java.lang.reflect.Field field = d.getField(assetName);
    int id = field.getInt(null);
    */
    Logger::E(TAG, "ElastosProtocolHandler::GetFieldId not impl");
    assert(0);
    return 0;
}

Int32 ElastosProtocolHandler::GetValueType(
    /* [in] */ IContext* context,
    /* [in] */ Int32 fieldId)
{
    AutoPtr<ITypedValue> value;
    CTypedValue::New((ITypedValue**)&value);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetValue(fieldId, value, TRUE);
    Int32 type;
    value->GetType(&type);
    return type;
}

AutoPtr<IInputStream> ElastosProtocolHandler::OpenResource(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri)
{
    String scheme;
    uri->GetScheme(&scheme);
    String path;
    uri->GetPath(&path);
    assert(scheme.Equals(FILE_SCHEME));
    assert(path != NULL);
    assert(path.StartWith(NativeGetAndroidResourcePath()));
    // The path must be of the form "/android_res/asset_type/asset_name.ext".
    AutoPtr<ArrayOf<IInterface*> > pathSegments;
    AutoPtr<IList> pathSegmentsList;
    uri->GetPathSegments((Elastos::Utility::IList**)&pathSegmentsList);
    pathSegmentsList->ToArray((ArrayOf<IInterface*>**)&pathSegments);
    Int32 size = pathSegments->GetLength();
    if (size != 3) {
        Logger::E(TAG, "Incorrect resource path: %s", path.string());
        return NULL;
    }
    AutoPtr<ICharSequence> csq0 = ICharSequence::Probe((*pathSegments)[0]);
    String assetPath;
    csq0->ToString(&assetPath);
    AutoPtr<ICharSequence> csq1 = ICharSequence::Probe((*pathSegments)[1]);
    String assetType;
    csq1->ToString(&assetType);
    AutoPtr<ICharSequence> csq2 = ICharSequence::Probe((*pathSegments)[2]);
    String assetName;
    csq2->ToString(&assetName);
    String strRes = String("/") + assetPath + String("/");
    if (!strRes.Equals(NativeGetAndroidResourcePath())) {
        Logger::E(TAG, "Resource path does not start with %s:%s", NativeGetAndroidResourcePath().string(),
               path.string());
        return NULL;
    }
    // Drop the file extension.
    AutoPtr<ArrayOf<String> > splitOut;
    StringUtils::Split(assetName, String("\\."), (ArrayOf<String>**)&splitOut);
    assetName = (*splitOut)[0];
    //try {
        // Use the application context for resolving the resource package name so that we do
        // not use the browser's own resources. Note that if 'context' here belongs to the
        // test suite, it does not have a separate application context. In that case we use
        // the original context object directly.
        AutoPtr<IContext> appContext;
        context->GetApplicationContext((IContext**)&appContext);
        if (appContext != NULL) {
            context = appContext;
        }
        Int32 fieldId = GetFieldId(context, assetType, assetName);
        Int32 valueType = GetValueType(context, fieldId);
        if (valueType == ITypedValue::TYPE_STRING) {
            AutoPtr<IResources> res;
            context->GetResources((IResources**)&res);
            AutoPtr<IInputStream> inputStream;
            res->OpenRawResource(fieldId, (IInputStream**)&inputStream);
            return inputStream;
        }
        else {
            Logger::E(TAG, "Asset not of type string: %s", path.string());
            return NULL;
        }
    //} catch (ClassNotFoundException e) {
    //    Log.e(TAG, "Unable to open resource URL: " + uri, e);
    //    return null;
    //} catch (NoSuchFieldException e) {
    //    Log.e(TAG, "Unable to open resource URL: " + uri, e);
    //    return null;
    //} catch (IllegalAccessException e) {
    //    Log.e(TAG, "Unable to open resource URL: " + uri, e);
    //    return null;
    //}
}

AutoPtr<IInputStream> ElastosProtocolHandler::OpenAsset(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri)
{
    String scheme, pathStr;
    uri->GetScheme(&scheme);
    uri->GetPath(&pathStr);
    assert(scheme.Equals(FILE_SCHEME));
    assert(!pathStr.IsNull());
    assert(pathStr.StartWith(NativeGetAndroidAssetPath()));
    String path;
    StringUtils::ReplaceFirst(pathStr, NativeGetAndroidAssetPath(), String(""), &path);
    //try {
        AutoPtr<IAssetManager> assets;
        context->GetAssets((IAssetManager**)&assets);
        AutoPtr<IInputStream> inputstream;
        assets->Open(path, IAssetManager::ACCESS_STREAMING, (IInputStream**)&inputstream);
        return inputstream;
    //} catch (IOException e) {
    //    Log.e(TAG, "Unable to open asset URL: " + uri);
    //    return null;
    //}
}

AutoPtr<IInputStream> ElastosProtocolHandler::OpenContent(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri)
{
    String scheme;
    uri->GetScheme(&scheme);
    assert(scheme.Equals(CONTENT_SCHEME));
    //try {
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<IInputStream> inputstream;
        resolver->OpenInputStream(uri, (IInputStream**)&inputstream);
        return inputstream;
    //} catch (Exception e) {
    //    Log.e(TAG, "Unable to open content URL: " + uri);
    //    return null;
    //}
}

/**
 * Determine the mime type for an Android resource.
 * @param context The context manager.
 * @param stream The opened input stream which to examine.
 * @param url The url from which the stream was opened.
 * @return The mime type or null if the type is unknown.
 */
//@CalledByNative IContext, IInputStream
String ElastosProtocolHandler::GetMimeType(
    /* [in] */ IInterface* icontext,
    /* [in] */ IInterface* istream,
    /* [in] */ const String& url)
{
    AutoPtr<IContext> context = IContext::Probe(icontext);
    AutoPtr<IInputStream> stream = IInputStream::Probe(istream);
    AutoPtr<IUri> uri = VerifyUrl(url);
    if (uri == NULL) {
        return String(NULL);
    }
    //try {
        String path;
        uri->GetPath(&path);
        // The content URL type can be queried directly.
        String scheme;
        uri->GetScheme(&scheme);
        if (scheme.Equals(CONTENT_SCHEME)) {
            AutoPtr<IContentResolver> resolver;
            context->GetContentResolver((IContentResolver**)&resolver);
            String type;
            resolver->GetType(uri, &type);
            return type;
            // Asset files may have a known extension.
        }
        else if (scheme.Equals(FILE_SCHEME) &&
                   path.StartWith(NativeGetAndroidAssetPath())) {
            AutoPtr<IURLConnectionHelper> helper;
            //TODO CURLConnectionHelper::AcquireSingleton((IURLConnectionHelper**)&helper);
            String mimeType;
            helper->GuessContentTypeFromName(path, &mimeType);
            if (!mimeType.IsNull()) {
                return mimeType;
            }
        }
    //} catch (Exception ex) {
//        Log.e(TAG, "Unable to get mime type" + url);
//        return NULL;
//    }
    // Fall back to sniffing the type from the stream.
    //try {
        AutoPtr<IURLConnectionHelper> helper;
        //TODO CURLConnectionHelper::AcquireSingleton((IURLConnectionHelper**)&helper);

        String type;
        helper->GuessContentTypeFromStream(stream, &type);
        return type;
    //} catch (IOException e) {
    //    return null;
    //}
}

/**
 * Make sure the given string URL is correctly formed and parse it into a Uri.
 * @return a Uri instance, or null if the URL was invalid.
 */
AutoPtr<IUri> ElastosProtocolHandler::VerifyUrl(
    /* [in] */ const String& url)
{
    if (url.IsNull()) {
        return NULL;
    }

    AutoPtr<IUriHelper> helper;
    //TODO CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->Parse(url, (IUri**)&uri);
    if (uri == NULL) {
        Logger::E(TAG, "Malformed URL: %s", url.string());
        return NULL;
    }
    String path;
    uri->GetPath(&path);
    if (path.IsNullOrEmpty()) {
        Logger::E(TAG, "URL does not have a path: %s", url.string());
        return NULL;
    }
    return uri;
}

/**
 * Set the context to be used for resolving resource queries.
 * @param context Context to be used, or null for the default application
 *                context.
 */
void ElastosProtocolHandler::SetResourceContextForTesting(
    /* [in] */ IContext* context)
{
    NativeSetResourceContextForTesting(context);
}

void ElastosProtocolHandler::NativeSetResourceContextForTesting(
    /* [in] */ IContext* context)
{
    Elastos_AndroidProtocolHandler_nativeSetResourceContextForTesting(TO_IINTERFACE(context));
}

String ElastosProtocolHandler::NativeGetAndroidAssetPath()
{
    return Elastos_AndroidProtocolHandler_nativeGetAndroidAssetPath();
}

String ElastosProtocolHandler::NativeGetAndroidResourcePath()
{
    return Elastos_AndroidProtocolHandler_nativeGetAndroidResourcePath();
}

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
