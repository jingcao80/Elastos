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

#include "elastos/droid/googleremote/VmouseController.h"
#include <elastos/utility/logging/Slogger.h>
#include <fcntl.h>
#include <unistd.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace GoogleRemote {

void VmouseController::OpenVmouse()
{
    NativeOpenVmouse();
}

void VmouseController::CloseVmouse()
{
    NativeCloseVmouse();
}

void VmouseController::DispatchMouseEvent(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 flag)
{
    NativeDispatchMouseEvent(x, y, flag);
}

int fd;

struct mouse_event{
    int flag;
    int x;
    int y;
};

void VmouseController::NativeDispatchMouseEvent(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 flag)
{
    struct mouse_event event;
    event.x = x;
    event.y = y;
    event.flag = flag;
    write(fd, &event, sizeof(event));
}

void VmouseController::NativeOpenVmouse()
{
    if (fd == 0) {
        fd = open("dev/vmouse", O_RDWR);
        if (fd < 0) {
            Slogger::E(String("VmouseController"), "could not open dev/vmouse ");
        }
    }
}

void VmouseController::NativeCloseVmouse()
{
    if (fd != 0) {
        close(fd);
        fd = 0;
    }
}

} // GoogleRemote
} // Droid
} // Elastos
