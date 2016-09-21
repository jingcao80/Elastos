#include "NativeWindow_api.h"

namespace android {

extern "C" {
struct ElaNativeWindowCallback* sElaNativeWindowCallback;
__attribute__((visibility("default")))
void Elastos_NativeWindow_InitCallback(Elastos::Handle32 cb)
{
    sElaNativeWindowCallback = (struct ElaNativeWindowCallback*)cb;
    DLOG(INFO) << "init pointer for sElaByteBufferCallback is:" << sElaNativeWindowCallback;
}
}; // extern "C"

}  // namespace android
