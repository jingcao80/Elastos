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

#ifndef __ELASTOS_DROID_INTERNAL_OS_BASECOMMAND_H__
#define __ELASTOS_DROID_INTERNAL_OS_BASECOMMAND_H__

#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <elastos/io/PrintWriter.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

class ECO_PUBLIC BaseCommand
    : public Object
    , public IBaseCommand
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Call to run the command.
     */
    CARAPI Run(
        /* [in] */ ArrayOf<String>* args);

    /**
     * Convenience to show usage information to error output.
     */
    CARAPI ShowUsage();

    /**
     * Convenience to show usage information to error output along
     * with an error message.
     */
    CARAPI ShowError(
        /* [in] */ const String& message);

    /**
     * Return the next option on the command line -- that is an argument that
     * starts with '-'.  If the next argument is not an option, null is returned.
     */
    CARAPI NextOption(
        /* [out] */ String* nextOption);

    /**
     * Return the next argument on the command line, whatever it is; if there are
     * no arguments left, return null.
     */
    CARAPI NextArg(
        /* [out] */ String* nextArg);

    /**
     * Return the next argument on the command line, whatever it is; if there are
     * no arguments left, throws an IllegalArgumentException to report this to the user.
     */
    CARAPI NextArgRequired(
        /* [out] */ String* nextArg);

protected:
    AutoPtr<ArrayOf<String> > mArgs;

private:
    Int32 mNextArg;
    String mCurArgData;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_BASECOMMAND_H__
