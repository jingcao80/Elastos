
#include "CSerialService.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/Manifest.h"
#include "elastos/droid/R.h"
#include <fcntl.h>

using Elastos::Utility::Etl::List;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::CFileDescriptor;
using Elastos::Droid::Os::CParcelFileDescriptor;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

ECode CSerialService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    assert(res!=NULL);
    return res->GetStringArray(R::array::config_serialPorts, (ArrayOf<String>**)&mSerialPorts);
}

ECode CSerialService::GetSerialPorts(
    /* [out, callee] */ ArrayOf<String>** serialPorts)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::SERIAL_PORT, String(NULL)));

    List<String> ports;
    Int32 length = mSerialPorts->GetLength();
    for (Int32 i = 0; i < length; i++) {
        String path((*mSerialPorts)[i]);
        AutoPtr<IFile> file;
        CFile::New(path, (IFile**)&file);
        Boolean isExist;
        file->Exists(&isExist);
        if (isExist) {
            ports.PushBack(path);
        }
    }

    *serialPorts = ArrayOf<String>::Alloc(ports.GetSize());
    REFCOUNT_ADD(*serialPorts);
    List<String>::Iterator it = ports.Begin();
    for (Int32 i = 0; it != ports.End(); ++it, i++) {
        (**serialPorts)[i] = *it;
    }

    return NOERROR;
}

ECode CSerialService::OpenSerialPort(
    /* [in] */ const String& path,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor);
    *descriptor = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::SERIAL_PORT, String(NULL)));
    return NativeOpen(path, descriptor);
}

ECode CSerialService::NativeOpen(
    /* [in] */ const String& path,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor);
    *descriptor = NULL;

    Int32 fd = open(path.string(), O_RDWR | O_NOCTTY);
    if (fd < 0) {
        Logger::E("CSerialService", "could not open %s", path.string());
        *descriptor = NULL;
        return NOERROR;
    }

    AutoPtr<IFileDescriptor> fileDes;
    CFileDescriptor::New((IFileDescriptor**)&fileDes);
    fileDes->SetDescriptor(fd);
    return CParcelFileDescriptor::New(fileDes, descriptor);
}

ECode CSerialService::ToString(
        /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);
    return E_NOT_IMPLEMENTED;
}

} // Server
} // Droid
} // Elastos
