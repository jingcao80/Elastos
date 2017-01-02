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

#include "MonkeyCommandEvent.h"
#include <unistd.h>
#include <sys/wait.h>

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyCommandEvent::MonkeyCommandEvent()
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
{
}

MonkeyCommandEvent::MonkeyCommandEvent(
    /* [in] */ const String& cmd)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
    , mCmd(cmd)
{
}

ECode MonkeyCommandEvent::Init(
    /* [in] */ const String& cmd)
{
    mCmd = cmd;
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_ACTIVITY);
    return NOERROR;
}

Int32 MonkeyCommandEvent::InjectEvent(
    /* [in] */ IIWindowManager* iwm,
    /* [in] */ IIActivityManager* iam,
    /* [in] */ Int32 verbose)
{
    if(!mCmd.IsNull())
    {
        Int32 pid = fork();
        if(pid == 0) {
            PFL_EX("command: %s", mCmd.string());
            execl(mCmd, (char*)NULL);
            return IMonkeyEvent::INJECT_SUCCESS;
        }
        else if(pid == -1) {
            PFL_EX("// Exception from %s: <errorcode>", mCmd.string());
        }
        else {
            Int32 status = 0;
            waitpid(pid, &status, 0);
            PFL_EX("// Shell command %s status was %d", mCmd.string(), status);
        }
    }
    return IMonkeyEvent::INJECT_SUCCESS;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
