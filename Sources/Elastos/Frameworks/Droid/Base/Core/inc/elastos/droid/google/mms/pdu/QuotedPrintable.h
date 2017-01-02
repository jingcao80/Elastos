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

#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_QUOTEDPRINTABLE_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_QUOTEDPRINTABLE_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

class QuotedPrintable
{
public:
    /**
      * Decodes an array quoted-printable characters into an array of original bytes.
      * Escaped characters are converted back to their original representation.
      *
      * <p>
      * This function implements a subset of
      * quoted-printable encoding specification (rule #1 and rule #2)
      * as defined in RFC 1521.
      * </p>
      *
      * @param bytes array of quoted-printable characters
      * @return array of original bytes,
      *         null if quoted-printable decoding is unsuccessful.
      */
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) DecodeQuotedPrintable(
        /* [in] */ ArrayOf<Byte>* bytes);

private:
    static Byte ESCAPE_CHAR;
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_QUOTEDPRINTABLE_H__

