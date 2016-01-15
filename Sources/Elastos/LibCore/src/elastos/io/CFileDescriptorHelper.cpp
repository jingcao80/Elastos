
#include "CFileDescriptorHelper.h"
#include "CFileDescriptor.h"

namespace Elastos {
namespace IO {

ECode CFileDescriptorHelper::GetInFile(
    /* [out] */ IFileDescriptor** inFile)
{
    VALIDATE_NOT_NULL(inFile);

    *inFile = CFileDescriptor::IN;
    REFCOUNT_ADD(*inFile);
    return NOERROR;
}

ECode CFileDescriptorHelper::GetOutFile(
    /* [out] */ IFileDescriptor** outFile)
{
    VALIDATE_NOT_NULL(outFile);

    *outFile = CFileDescriptor::OUT;
    REFCOUNT_ADD(*outFile);
    return NOERROR;
}

ECode CFileDescriptorHelper::GetErrFile(
    /* [out] */ IFileDescriptor** errFile)
{
    VALIDATE_NOT_NULL(errFile);

    *errFile = CFileDescriptor::ERR;
    REFCOUNT_ADD(*errFile);
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
