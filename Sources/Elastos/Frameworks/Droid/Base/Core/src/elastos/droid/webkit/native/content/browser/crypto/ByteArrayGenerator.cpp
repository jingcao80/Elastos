
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/webkit/native/content/browser/crypto/ByteArrayGenerator.h"

using Elastos::IO::IInputStream;
using Elastos::IO::EIID_IInputStream;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Crypto {

/**
 * Polls random data to generate the array.
 * @param numBytes Length of the array to generate.
 * @return byte[] containing randomly generated data.
 */
AutoPtr< ArrayOf<Byte> > ByteArrayGenerator::GetBytes(
    /* [in] */ Int32 numBytes)
{
    AutoPtr<IFileInputStream> fis;
    // try {
        assert(0);
        // TODO
        // CFileInputStream::New(String("/dev/urandom"), (IFileInputStream**)&fis);
        AutoPtr< ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(numBytes);
        Int32 readLen;
        AutoPtr<IInputStream> inputStream = IInputStream::Probe(fis);
        inputStream->Read(bytes, &readLen);
        if (bytes->GetLength() != readLen) {
            //throw new GeneralSecurityException("Not enough random data available");
            assert(0);
        }
    //    return bytes;
    // } finally {
        if (fis != NULL) {
            inputStream->Close();
        }
    // }

    return bytes;
}

} // namespace Crypto
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
