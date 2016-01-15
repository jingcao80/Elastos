
#ifndef __ORG_APACHE_HTTP_IMPL_IO_IDENTITYINPUTSTREAM_H__
#define __ORG_APACHE_HTTP_IMPL_IO_IDENTITYINPUTSTREAM_H__

#include "elastos/io/InputStream.h"

using Elastos::IO::InputStream;
using Org::Apache::Http::IO::ISessionInputBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

/**
 * A stream for reading from a {@link SessionInputBuffer session input buffer}.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 560358 $
 *
 * @since 4.0
 */
class IdentityInputStream : public InputStream
{
public:
    IdentityInputStream(
        /* [in] */ ISessionInputBuffer* in);

    CARAPI Available(
        /* [out] */ Int32* number);

    CARAPI Close();

    CARAPI Read(
        /* [out] */ Int32* value);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* number);

private:
    AutoPtr<ISessionInputBuffer> mIn;
    Boolean mClosed;
};

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_IO_IDENTITYINPUTSTREAM_H__
