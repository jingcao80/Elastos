
#include "CJarOutputStream.h"
#include "CZipEntry.h"

using Elastos::IO::EIID_IOutputStream;
using Elastos::Utility::Zip::CZipEntry;

namespace Elastos {
namespace Utility {
namespace Jar {

CAR_INTERFACE_IMPL(CJarOutputStream, ZipOutputStream, IJarOutputStream)

CAR_OBJECT_IMPL(CJarOutputStream)

ECode CJarOutputStream::constructor(
    /* [in] */ IOutputStream* os,
    /* [in] */ IManifest* manifest)
{
    FAIL_RETURN(ZipOutputStream::constructor(os))
    if (manifest == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mManifest = manifest;
    AutoPtr<IZipEntry> ze;
    CZipEntry::New(IJarFile::MANIFEST_NAME, (IZipEntry**)&ze);
    FAIL_RETURN(PutNextEntry(ze))
    FAIL_RETURN(mManifest->Write(THIS_PROBE(IOutputStream)))
    CloseEntry();
    return NOERROR;
}

ECode CJarOutputStream::constructor(
    /* [in] */ IOutputStream* os)
{
    return ZipOutputStream::constructor(os);
}


} // namespace Jar
} // namespace Utility
} // namespace Elastos

