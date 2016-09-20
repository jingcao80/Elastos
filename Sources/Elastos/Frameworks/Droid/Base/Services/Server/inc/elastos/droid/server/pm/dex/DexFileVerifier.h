
#ifndef __ELASTOS_DROID_SERVER_PM_DEX_DEXFILEVERIFIER_H__
#define __ELASTOS_DROID_SERVER_PM_DEX_DEXFILEVERIFIER_H__

#include "elastos/droid/server/pm/DexFile.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Server::Pm::DexFile;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {
namespace Dex {

class DexFileVerifier : public Object
{
public:
    static CARAPI_(Boolean) Verify(
        /* [in] */ const DexFile* dex_file,
        /* [in] */ const Byte* begin,
        /* [in] */ size_t size,
        /* [in] */ const char* location,
        /* [out] */ String* error_msg);
};

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_DEX_DEXFILEVERIFIER_H__
