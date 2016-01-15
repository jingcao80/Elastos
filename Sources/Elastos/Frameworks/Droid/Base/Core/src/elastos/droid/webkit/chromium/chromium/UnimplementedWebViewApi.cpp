
namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

//=========================================================================
//        UnimplementedWebViewApi::UnimplementedWebViewApiException
//=========================================================================

UnimplementedWebViewApi::UnimplementedWebViewApiException::UnimplementedWebViewApiException()
{
    UnsupportedOperationException();
}

//===============================================================
//                  UnimplementedWebViewApi
//===============================================================

String UnimplementedWebViewApi::TAG("UnimplementedWebViewApi");

Boolean UnimplementedWebViewApi::THROW = FALSE;

// By default we keep the traces down to one frame to reduce noise, but for debugging it might
// be useful to set this to true.
Boolean UnimplementedWebViewApi::FULL_TRACE = FALSE;

void UnimplementedWebViewApi::Invoke()
{
    if (THROW) {
    //    throw new UnimplementedWebViewApiException();
        assert(0);
    }
    else {
        if (FULL_TRACE) {
            // Log.w(TAG, "Unimplemented WebView method called in: " +
            //       Log.getStackTraceString(new Throwable()));
        }
        else {
            StackTraceElement[] trace = new Throwable().getStackTrace();
            // The stack trace [0] index is this method (invoke()).
            StackTraceElement unimplementedMethod = trace[1];
            StackTraceElement caller = trace[2];
            // Log.w(TAG, "Unimplemented WebView method " + unimplementedMethod.getMethodName() +
            //         " called from: " + caller.toString());
        }
    }
}

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
