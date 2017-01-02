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

#ifndef __ELASTOS_DROID_GOOGLE_MMS_INVALIDHEADERVALUEEXCEPTION_H__
#define __ELASTOS_DROID_GOOGLE_MMS_INVALIDHEADERVALUEEXCEPTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/google/mms/MmsException.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {

/**
  * Thrown when an invalid header value was set.
  */
class InvalidHeaderValueException
    : public MmsException
{
public:
    /**
      * Constructs an InvalidHeaderValueException with no detailed message.
      */
    InvalidHeaderValueException();

    /**
      * Constructs an InvalidHeaderValueException with the specified detailed message.
      *
      * @param message the detailed message.
      */
    InvalidHeaderValueException(
        /* [in] */ const String& message);

private:
    static const Int64 serialVersionUID = -2053384496042052262L;
};

} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_INVALIDHEADERVALUEEXCEPTION_H__

