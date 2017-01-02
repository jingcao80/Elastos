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

#include "elastos/droid/webkit/ConsoleMessage.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(ConsoleMessage, Object, IConsoleMessage);

ConsoleMessage::ConsoleMessage()
    : mLevel(0)
    , mLineNumber(0)
{
}

ConsoleMessage::ConsoleMessage(
    /* [in] */ const String& message,
    /* [in] */ const String& sourceId,
    /* [in] */ Int32 lineNumber,
    /* [in] */ /*MessageLevel*/Int32 msgLevel)
{
    Init(message, sourceId, lineNumber, msgLevel);
}

ECode ConsoleMessage::constructor(
    /* [in] */ const String& message,
    /* [in] */ const String& sourceId,
    /* [in] */ Int32 lineNumber,
    /* [in] */ Elastos::Droid::Webkit::MessageLevel msgLevel)
{
    Init(message, sourceId, lineNumber, msgLevel);
    return NOERROR;
}

void ConsoleMessage::Init(
    /* [in] */ const String& message,
    /* [in] */ const String& sourceId,
    /* [in] */ Int32 lineNumber,
    /* [in] */ /*MessageLevel*/Int32 msgLevel)
{
    mMessage = message;
    mSourceId = sourceId;
    mLineNumber = lineNumber;
    mLevel = msgLevel;
}

ECode ConsoleMessage::MessageLevel(
    /* [out] */ /*MessageLevel*/Int32* level)
{
    VALIDATE_NOT_NULL(level);
    *level = mLevel;
    return NOERROR;
}

ECode ConsoleMessage::Message(
    /* [out] */ String* message)
{
    VALIDATE_NOT_NULL(message);
    *message = mMessage;
    return NOERROR;
}

ECode ConsoleMessage::SourceId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mSourceId;
    return NOERROR;
}

ECode ConsoleMessage::LineNumber(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = mLineNumber;
    return NOERROR;
}

ECode ConsoleMessage::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "ConsoleMessage";
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos