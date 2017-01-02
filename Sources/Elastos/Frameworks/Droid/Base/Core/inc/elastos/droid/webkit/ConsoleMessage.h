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

#ifndef __ELASTOS_DROID_WEBKIT_CONSOLEMESSAGE_H__
#define __ELASTOS_DROID_WEBKIT_CONSOLEMESSAGE_H__

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * Public class representing a JavaScript console message from WebCore. This could be a issued
 * by a call to one of the <code>console</code> logging functions (e.g.
 * <code>console.log('...')</code>) or a JavaScript error on the  page. To receive notifications
 * of these messages, override the
 * {@link WebChromeClient#onConsoleMessage(ConsoleMessage)} function.
 */
class ConsoleMessage
    : public Object
    , public IConsoleMessage
{
public:
    CAR_INTERFACE_DECL()

    ConsoleMessage();

    ConsoleMessage(
        /* [in] */ const String& message,
        /* [in] */ const String& sourceId,
        /* [in] */ Int32 lineNumber,
        /* [in] */ /*MessageLevel*/Int32 msgLevel);

    CARAPI constructor(
        /* [in] */ const String& message,
        /* [in] */ const String& sourceId,
        /* [in] */ Int32 lineNumber,
        /* [in] */ Elastos::Droid::Webkit::MessageLevel msgLevel);

    CARAPI MessageLevel(
        /* [out] */ Int32* level);

    CARAPI Message(
        /* [out] */ String* message);

    CARAPI SourceId(
        /* [out] */ String* id);

    CARAPI LineNumber(
        /* [out] */ Int32* number);

    CARAPI ToString(
        /* [out] */ String* info);

protected:
    CARAPI_(void) Init(
        /* [in] */ const String& message,
        /* [in] */ const String& sourceId,
        /* [in] */ Int32 lineNumber,
        /* [in] */ /*MessageLevel*/Int32 msgLevel);

private:
    /*MessageLevel*/Int32 mLevel;
    String mMessage;
    String mSourceId;
    Int32 mLineNumber;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_CONSOLEMESSAGE_H__
