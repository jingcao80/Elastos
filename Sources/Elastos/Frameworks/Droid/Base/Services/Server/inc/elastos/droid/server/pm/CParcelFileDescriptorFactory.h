
#ifndef __ELASTOS_DROID_SERVER_PM_CPARCELFILEDESCRIPTORFACTORY_H__
#define __ELASTOS_DROID_SERVER_PM_CPARCELFILEDESCRIPTORFACTORY_H__

#include "_Elastos_Droid_Server_Pm_CParcelFileDescriptorFactory.h"
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Os::IIParcelFileDescriptorFactory;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::IO::IFile;

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
    CARAPI constructor(
      /* [in] */ IFile* codeFile);

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Open(
        /* [in] */ const String& name,
        /* [in] */ Int32 mode,
        /* [out] */ IParcelFileDescriptor** pfd);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<IFile> mCodeFile;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_CPARCELFILEDESCRIPTORFACTORY_H__
