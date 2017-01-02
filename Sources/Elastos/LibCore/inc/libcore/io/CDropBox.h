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

#ifndef __LIBCORE_IO_CDROPBOX_H__
#define __LIBCORE_IO_CDROPBOX_H__

#include "core/Object.h"
#include "Singleton.h"
#include "_Libcore_IO_CDropBox.h"

using Elastos::Core::Object;
using Elastos::Core::Singleton;

namespace Libcore {
namespace IO {

CarClass(CDropBox)
    , public Singleton
    , public IDropBox
{
private:
        /**
     * Default Reporter which reports events to the log.
     */
    class DefaultReporter
        : public Object
        , public IReporter
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI AddData(
            /* [in] */ const String& tag,
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 flags);

        CARAPI AddText(
            /* [in] */ const String& tag,
            /* [in] */ const String& data);
    };

public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI SetReporter(
        /* [in] */  IReporter* reporter);

    /**
     * Returns non-null Reporter.
     */
    CARAPI GetReporter(
        /* [out] */  IReporter** reporter);

    CARAPI AddData(
        /* [in] */  const String& tag,
        /* [in] */  ArrayOf<Byte>* data,
        /* [in] */  Int32 flags);

    CARAPI AddText(
        /* [in] */  const String& tag,
        /* [in] */  const String& data);

private:
    static AutoPtr<IReporter> REPORTER;

};

} // namespace IO
} // namespace Libcore
#endif
