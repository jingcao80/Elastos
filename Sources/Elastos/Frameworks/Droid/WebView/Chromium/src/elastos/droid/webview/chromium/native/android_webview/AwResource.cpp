
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwResource.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/AwResource_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::IO::ICloseable;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::IInputStream;
using Elastos::IO::IReadable;
using Elastos::IO::IReader;
using Elastos::Utility::IScanner;
using Elastos::Utility::CScanner;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

Int32 AwResource::RAW_LOAD_ERROR = 0;
Int32 AwResource::RAW_NO_DOMAIN = 0;
Int32 AwResource::STRING_ARRAY_CONFIG_KEY_SYSTEM_UUID_MAPPING = 0;
AutoPtr<IResources> AwResource::sResources;
//TODO AutoPtr<ISparseArray> AwResource::sResourceCache;
HashMap<Int32, AutoPtr<IWeakReference> > AwResource::sResourceCache;//TODO when SparseArray ok ,try to replace this
const Int32 AwResource::TYPE_STRING;
const Int32 AwResource::TYPE_RAW;

void AwResource::SetResources(
    /* [in] */ IResources* resources)
{
    sResources = resources;
    //sResourceCache = new SparseArray<SoftReference<String>>();
    //TODO CSparseArray::New((ISparseArray**)&sResourceCache);
}

void AwResource::SetErrorPageResources(
    /* [in] */ Int32 loaderror,
    /* [in] */ Int32 nodomain)
{
    RAW_LOAD_ERROR = loaderror;
    RAW_NO_DOMAIN = nodomain;
}

void AwResource::SetDefaultTextEncoding(
    /* [in] */ Int32 encoding)
{
}

void AwResource::SetConfigKeySystemUuidMapping(
    /* [in] */ Int32 config)
{
    STRING_ARRAY_CONFIG_KEY_SYSTEM_UUID_MAPPING = config;
}

//@CalledByNative
String AwResource::GetDefaultTextEncoding()
{
    return String("UTF-8");
}

//@CalledByNative
String AwResource::GetNoDomainPageContent()
{
    return GetResource(RAW_NO_DOMAIN, TYPE_RAW);
}

//@CalledByNative
String AwResource::GetLoadErrorPageContent()
{
    return GetResource(RAW_LOAD_ERROR, TYPE_RAW);
}

AutoPtr<ArrayOf<String> > AwResource::GetConfigKeySystemUuidMapping()
{
    // No need to cache, since this should be called only once.
    AutoPtr<ArrayOf<String> > stringArray;
    sResources->GetStringArray(STRING_ARRAY_CONFIG_KEY_SYSTEM_UUID_MAPPING, (ArrayOf<String>**)&stringArray);
    return stringArray;
}

String AwResource::GetResource(
    /* [in] */ Int32 resid,
    /* [in] */ Int32 type)
{
    assert(resid != 0);
    assert(sResources != NULL);
    //TODO assert(sResourceCache != NULL);

    //SoftReference<String> stringRef = sResourceCache.get(resid);
    AutoPtr<IWeakReference> stringRef = sResourceCache[resid];
    AutoPtr<IInterface> ws;
    stringRef->Resolve(EIID_IInterface, (IInterface**)&ws);
    String result(NULL);
    if (ws) {
        AutoPtr<ICharSequence> ics = ICharSequence::Probe(ws);
        ics->ToString(&result);
    }
    if (result.IsNull()) {
        switch (type) {
            case TYPE_STRING:
                sResources->GetString(resid, &result);
                break;
            case TYPE_RAW:
                result = GetRawFileResourceContent(resid);
                break;
            default:
//                throw new IllegalArgumentException("Unknown resource type");
                Logger::E("AwResource", "GetResource: Unknown resource type");
                assert(0);
        }

        AutoPtr<ICharSequence> ics;
        CString::New(result, (ICharSequence**)&ics);
        //sResourceCache.put(resid, new SoftReference<String>(result));
        AutoPtr<IWeakReferenceSource> source = IWeakReferenceSource::Probe(ics);
        AutoPtr<IWeakReference> wr;
        source->GetWeakReference((IWeakReference**)&wr);
        sResourceCache[resid] = wr;
    }
    return result;
}

String AwResource::GetRawFileResourceContent(
    /* [in] */ Int32 resid)
{
    assert(resid != 0);
    assert(sResources != NULL);

    AutoPtr<IInputStreamReader> isr;
    String result;

    // try {
        AutoPtr<IInputStream> res;
        sResources->OpenRawResource(resid, (IInputStream**)&res);
        CInputStreamReader::New(res, (IInputStreamReader**)&isr);
        // \A tells the scanner to use the beginning of the input
        // as the delimiter, hence causes it to read the entire text.
        AutoPtr<IScanner> scanner;
        CScanner::New(IReadable::Probe(isr), (IScanner**)&scanner);
        AutoPtr<IScanner> _scanner;
        scanner->UseDelimiter(String("\\A"), (IScanner**)&_scanner);
        _scanner->Next(&result);
    // } catch (Resources.NotFoundException e) {
    //     return "";
    // } catch (NoSuchElementException e) {
    //     return "";
    // } finally {
        // try {
            if (isr != NULL) {
                ICloseable::Probe(isr)->Close();
            }
        //} catch (IOException e) {
            // Nothing to do if close() fails.
        //}
    // }
    if (isr)
    {
        ICloseable::Probe(isr)->Close();
    }
    return result;
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
