//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "FilePathToURI.h"
#include "StringBuilder.h"
#include "Character.h"
#include "CFile.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::Character;
using Elastos::IO::CFile;

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Stream {

AutoPtr<ArrayOf<Boolean> > FilePathToURI::gNeedEscaping;
AutoPtr<ArrayOf<Char32> > FilePathToURI::gAfterEscaping1;
AutoPtr<ArrayOf<Char32> > FilePathToURI::gAfterEscaping2;
Char32 FilePathToURI::gHexChs[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

Boolean FilePathToURI::sInit = FilePathToURI::InitArrays();

Boolean FilePathToURI::InitArrays()
{
    gNeedEscaping = ArrayOf<Boolean>::Alloc(128);
    gAfterEscaping1 = ArrayOf<Char32>::Alloc(128);
    gAfterEscaping2 = ArrayOf<Char32>::Alloc(128);

    for (Int32 i = 0; i <= 0x1f; i++) {
        (*gNeedEscaping)[i] = TRUE;
        (*gAfterEscaping1)[i] = gHexChs[i >> 4];
        (*gAfterEscaping2)[i] = gHexChs[i & 0xf];
    }
    (*gNeedEscaping)[0x7f] = TRUE;
    (*gAfterEscaping1)[0x7f] = '7';
    (*gAfterEscaping2)[0x7f] = 'F';
    Char32 escChs[] = {' ', '<', '>', '#', '%', '"', '{', '}',
                     '|', '\\', '^', '~', '[', ']', '`'};
    Int32 len = sizeof(escChs)/sizeof(Char32);
    Char32 ch;
    for (Int32 i = 0; i < len; i++) {
        ch = escChs[i];
        (*gNeedEscaping)[ch] = TRUE;
        (*gAfterEscaping1)[ch] = gHexChs[ch >> 4];
        (*gAfterEscaping2)[ch] = gHexChs[ch & 0xf];
    }
    return TRUE;
}

String FilePathToURI::Filepath2URI(
    /* [in] */ const String& _path)
{
    // return null if path is null.
    if (_path == NULL)
        return String(NULL);

    Char32 separator = CFile::sSeparatorChar;
    String path = _path.Replace(separator, '/');

    Int32 len = path.GetLength(), ch;
    StringBuilder buffer/* = new StringBuilder(len*3)*/;
    buffer.Append("file://");
    // change C:/blah to /C:/blah
    if (len >= 2 && path.GetChar(1) == ':') {
        ch = Character::ToUpperCase(path.GetChar(0));
        if (ch >= 'A' && ch <= 'Z') {
            buffer.AppendChar('/');
        }
    }

    // for each character in the path
    Int32 i = 0;
    for (; i < len; i++) {
        ch = path.GetChar(i);
        // if it's not an ASCII character, break here, and use UTF-8 encoding
        if (ch >= 128)
            break;
        if ((*gNeedEscaping)[ch]) {
            buffer.AppendChar('%');
            buffer.AppendChar((*gAfterEscaping1)[ch]);
            buffer.AppendChar((*gAfterEscaping2)[ch]);
            // record the fact that it's escaped
        }
        else {
            buffer.AppendChar(ch);
        }
    }

    // we saw some non-ascii character
    if (i < len) {
        // get UTF-8 bytes for the remaining sub-string
        AutoPtr<ArrayOf<Byte> > bytes;
        byte b;
        // try {
        assert(0);
        //TODO: use java.string->Charsets.toUtf8Bytes
        // bytes = path.Substring(i).GetBytes("UTF-8");
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
                buffer.AppendChar('%');
                buffer.AppendChar(gHexChs[ch >> 4]);
                buffer.AppendChar(gHexChs[ch & 0xf]);
            }
            else if ((*gNeedEscaping)[b]) {
                buffer.AppendChar('%');
                buffer.AppendChar((*gAfterEscaping1)[b]);
                buffer.AppendChar((*gAfterEscaping2)[b]);
            }
            else {
                buffer.AppendChar((Char32)b);
            }
        }
    }

    return buffer.ToString();
}

} // namespace Stream
} // namespace Transform
} // namespace Xml
} // namespace Elastosx
