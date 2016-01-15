
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/TemporaryBuffer.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Graphics {

AutoPtr< ArrayOf<Char32> > TemporaryBuffer::sTemp;
Object TemporaryBuffer::sObject;
AutoPtr< ArrayOf<Char32> > TemporaryBuffer::Obtain(
    /* [in] */ Int32 len)
{
    AutoPtr< ArrayOf<Char32> > buf;

    synchronized(sObject) {
        buf = sTemp;
        sTemp = NULL;
    }

    if (buf == NULL || buf->GetLength() < len) {
        buf = ArrayUtils::NewUnpaddedChar32Array(len);
    }

    return buf;
}

void TemporaryBuffer::Recycle(
    /* [in] */ ArrayOf<Char32>* temp)
{
    if (temp->GetLength() > 1000) return;

    synchronized(sObject) {
        sTemp = temp;
    }
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
