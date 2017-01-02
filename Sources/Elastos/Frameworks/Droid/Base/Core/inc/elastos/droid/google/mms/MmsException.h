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

#ifndef __ELASTOS_DROID_GOOGLE_MMS_MMSEXCEPTION_H__
#define __ELASTOS_DROID_GOOGLE_MMS_MMSEXCEPTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {

/**
  * A generic exception that is thrown by the Mms client.
  */
class MmsException
    : public Object
    //, public Exception
{
public:
    /**
      * Creates a new MmsException.
      */
    MmsException();

    /**
      * Creates a new MmsException with the specified detail message.
      *
      * @param message the detail message.
      */
    MmsException(
        /* [in] */ const String& message);

    /**
      * Creates a new MmsException with the specified cause.
      *
      * @param cause the cause.
      */
    //MmsException(
        ///* [in] */ Throwable* cause);

    /**
      * Creates a new MmsException with the specified detail message and cause.
      *
      * @param message the detail message.
      * @param cause the cause.
      */
    //MmsException(
        ///* [in] */ const String& message,
        ///* [in] */ Throwable* cause);

private:
    static const Int64 serialVersionUID = -7323249827281485390L;
};

} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_MMSEXCEPTION_H__
