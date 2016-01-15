#ifndef __ELASTOSX_XML_PARSERS_FILEPATHTOURI_H__
#define __ELASTOSX_XML_PARSERS_FILEPATHTOURI_H__

#include "Object.h"

namespace Elastosx {
namespace Xml {
namespace Parsers {

#define HEXCHS_NUM 16

class FilePathToURI
{
public:
    static CARAPI_(String) Filepath2URI(
        /* [in] */ const String& path);

private:
    static CARAPI_(Boolean) Init();

private:
    // which ASCII characters need to be escaped
    static Boolean gNeedEscaping[128];
    // the first hex character if a character needs to be escaped
    static Char32 gAfterEscaping1[128];
    // the second hex character if a character needs to be escaped
    static Char32 gAfterEscaping2[128];
    static Char32 gHexChs[HEXCHS_NUM];

    static Boolean sIsInitStatic;
};

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx

#endif
