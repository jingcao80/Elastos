
#include "StandardCharsets.h"
#include "Charset.h"

namespace Elastos {
namespace IO {
namespace Charset {

const AutoPtr<ICharset> StandardCharsets::ISO_8859_1;
const AutoPtr<ICharset> StandardCharsets::US_ASCII;
const AutoPtr<ICharset> StandardCharsets::UTF_8;
const AutoPtr<ICharset> StandardCharsets::UTF_16;
const AutoPtr<ICharset> StandardCharsets::UTF_16BE;
const AutoPtr<ICharset> StandardCharsets::UTF_16LE;
Boolean StandardCharsets::sIsInit = Init();

StandardCharsets::StandardCharsets()
{}

Boolean StandardCharsets::Init()
{
    Charset::ForName(String("ISO-8859-1"), (ICharset**)&ISO_8859_1);
    Charset::ForName(String("US-ASCII"), (ICharset**)&US_ASCII);
    Charset::ForName(String("UTF-8"), (ICharset**)&UTF_8);
    Charset::ForName(String("UTF-16"), (ICharset**)&UTF_16);
    Charset::ForName(String("UTF-16BE"), (ICharset**)&UTF_16BE);
    Charset::ForName(String("UTF-16LE"), (ICharset**)&UTF_16LE);
    return TRUE;
}

} // namespace Charset
} // namespace IO
} // namespace Elastos
