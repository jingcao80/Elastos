
#ifndef __ELASTOS_DROID_CHROMIUM_CHROMIUM_UNIMPLEMENTEDWEBVIEWAPI_H__
#define __ELASTOS_DROID_CHROMIUM_CHROMIUM_UNIMPLEMENTEDWEBVIEWAPI_H__

// import android.util.Log;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

// TODO: remove this when all WebView APIs have been implemented.
class UnimplementedWebViewApi
{
private:
    class UnimplementedWebViewApiException : public UnsupportedOperationException
    {
    public:
        UnimplementedWebViewApiException();
    };

public:
    static CARAPI_(void) Invoke();

private:
    static String TAG;

    static Boolean THROW;

    // By default we keep the traces down to one frame to reduce noise, but for debugging it might
    // be useful to set this to true.
    static Boolean FULL_TRACE;
};

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_CHROMIUM_CHROMIUM_UNIMPLEMENTEDWEBVIEWAPI_H__
