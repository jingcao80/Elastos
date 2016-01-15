#include "FilePathToURI.h"
#include "io/File.h"
#include "core/Character.h"
#include "core/StringBuilder.h"

using Elastos::IO::File;
using Elastos::Core::Character;
using Elastos::Core::StringBuilder;

namespace Elastosx {
namespace Xml {
namespace Parsers {

#define ESCCHS_NUM 15

// which ASCII characters need to be escaped
Boolean FilePathToURI::gNeedEscaping[128];

// the first hex character if a character needs to be escaped
Char32 FilePathToURI::gAfterEscaping1[128];

// the second hex character if a character needs to be escaped
Char32 FilePathToURI::gAfterEscaping2[128];

Char32 FilePathToURI::gHexChs[HEXCHS_NUM] = {'0', '1', '2', '3', '4', '5', '6', '7',
             '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

Boolean FilePathToURI::Init()
{
    for (Int32 i = 0; i <= 0x1f; i++) {
        gNeedEscaping[i] = TRUE;
        gAfterEscaping1[i] = gHexChs[i >> 4];
        gAfterEscaping2[i] = gHexChs[i & 0xf];
    }
    gNeedEscaping[0x7f] = TRUE;
    gAfterEscaping1[0x7f] = '7';
    gAfterEscaping2[0x7f] = 'F';
    Char32 escChs[ESCCHS_NUM] = {' ', '<', '>', '#', '%', '"', '{', '}',
                     '|', '\\', '^', '~', '[', ']', '`'};
    Int32 len = ESCCHS_NUM;
    Char32 ch;
    for (Int32 i = 0; i < len; i++) {
        ch = escChs[i];
        Int32 tmp = ch;
        gNeedEscaping[tmp] = TRUE;
        gAfterEscaping1[tmp] = gHexChs[tmp >> 4];
        gAfterEscaping2[tmp] = gHexChs[tmp & 0xf];
    }
    return TRUE;
}

Boolean FilePathToURI::sIsInitStatic = Init();

String FilePathToURI::Filepath2URI(
    /* [in] */ const String& _path)
{
    // return null if path is null.
    if (_path.IsNull()) return _path;

    Char32 separator = File::sSeparatorChar;
    String path = _path.Replace(separator, '/');

    Int32 len = path.GetLength(), ch;
    StringBuilder buffer(len*3);
    buffer += "file://";
    // change C:/blah to /C:/blah
    if (len >= 2 && path.GetChar(1) == ':') {
        ch = Character::ToUpperCase(path.GetChar(0));
        if (ch >= 'A' && ch <= 'Z') {
            buffer += '/';
        }
    }

    // for each character in the path
    Int32 i = 0;
    for (; i < len; i++) {
        ch = path.GetChar(i);
        // if it's not an ASCII character, break here, and use UTF-8 encoding
        if (ch >= 128)
            break;
        if (gNeedEscaping[ch]) {
            buffer += '%';
            buffer += gAfterEscaping1[ch];
            buffer += gAfterEscaping2[ch];
            // record the fact that it's escaped
        }
        else {
            buffer += (Char32)ch;
        }
    }

    // we saw some non-ascii character
    if (i < len) {
        // get UTF-8 bytes for the remaining sub-string
        AutoPtr<ArrayOf<Byte> > bytes;
        Byte b;
        // try {
        bytes = path.Substring(i).GetBytes();
        // } catch (java.io.UnsupportedEncodingException e) {
        //     // should never happen
        //     return path;
        // }
        len = bytes->GetLength();

        // for each byte
        for (i = 0; i < len; i++) {
            b = (*bytes)[i];
            // for non-ascii character: make it positive, then escape
            if (b < 0) {
                ch = b + 256;
                buffer += '%' ;
                buffer += gHexChs[ch >> 4] ;
                buffer += gHexChs[ch & 0xf] ;
            }
            else if (gNeedEscaping[b]) {
                buffer += '%';
                buffer += gAfterEscaping1[b];
                buffer += gAfterEscaping2[b];
            }
            else {
                buffer +=  ((char)b);
            }
        }
    }
    return buffer.ToString();
}

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx