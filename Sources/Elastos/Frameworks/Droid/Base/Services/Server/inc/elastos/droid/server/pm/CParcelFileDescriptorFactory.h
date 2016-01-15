
#ifndef __ELASTOS_DROID_SERVER_PM_CPARCELFILEDESCRIPTORFACTORY_H__
#define __ELASTOS_DROID_SERVER_PM_CPARCELFILEDESCRIPTORFACTORY_H__

#include "_Elastos_Droid_Server_Pm_CParcelFileDescriptorFactory.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Internal::Os::IIParcelFileDescriptorFactory;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

CarClass(CParcelFileDescriptorFactory)
    , public Object
    , public IIParcelFileDescriptorFactory
    , public IBinder
{
public:
    constructor(
      /* [in] */ IFile* codeFile);

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Open(
        /* [in] */ const String& name,
        /* [in] */ Int32 mode,
        /* [out] */ IParcelFileDescriptor** pfd);

private:
    AutoPtr<IFile> mCodeFile;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_CPARCELFILEDESCRIPTORFACTORY_H__
