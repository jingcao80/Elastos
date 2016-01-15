
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Extensions.h>
#include "elastos/droid/content/pm/CMacAuthenticatedInputStream.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL(CMacAuthenticatedInputStream, FilterInputStream, IMacAuthenticatedInputStream)

CAR_OBJECT_IMPL(CMacAuthenticatedInputStream)

CMacAuthenticatedInputStream::CMacAuthenticatedInputStream()
{}

CMacAuthenticatedInputStream::~CMacAuthenticatedInputStream()
{}

ECode CMacAuthenticatedInputStream::constructor(
    /* [in] */ IInputStream* inStream,
    /* [in] */ IMac* mac)
{
    FAIL_RETURN(FilterInputStream::constructor(inStream))

    mMac = mac;

    return NOERROR;
}

ECode CMacAuthenticatedInputStream::IsTagEqual(
    /* [in] */ ArrayOf<Byte>* tag,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<ArrayOf<Byte> > actualTag;
    mMac->DoFinal((ArrayOf<Byte>**)&actualTag);

    if (tag == NULL || actualTag == NULL || tag->GetLength() != actualTag->GetLength()) {
        *result = FALSE;
        return NOERROR;
    }

    /*
     * Attempt to prevent timing attacks by doing the same amount of work
     * whether the first byte matches or not. Do not change this to a loop
     * that exits early when a byte does not match.
     */
    Int32 value = 0;
    for (Int32 i = 0; i < tag->GetLength(); i++) {
        value |= (*tag)[i] ^ (*actualTag)[i];
    }

    *result = value == 0;
    return NOERROR;
}

ECode CMacAuthenticatedInputStream::Read(
    /* [out] */ Int32* value)
{
    Int32 b;
    FilterInputStream::Read(&b);
    if (b >= 0) {
        mMac->Update((Byte) b);
    }
    *value = b;
    return NOERROR;
}

ECode CMacAuthenticatedInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [out] */ Int32* number)
{
    Int32 numRead;
    FilterInputStream::Read(buffer, offset, count, &numRead);
    if (numRead > 0) {
        mMac->Update(buffer, offset, numRead);
    }
    *number = numRead;
    return NOERROR;
}

}
}
}
}

