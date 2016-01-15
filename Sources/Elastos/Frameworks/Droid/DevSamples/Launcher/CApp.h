
#ifndef __CAPP_H__
#define __CAPP_H__

#include <elastos.h>
#include "_CApp.h"
#include "GeckoApp.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

class CApp : public GeckoApp
{
public:
    ECode GetPackageName(
        /* [out] */ String* pPackageName);

    ECode GetContentProcessName(
        /* [out] */ String* pProcessName);
};

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__CAPP_H__

