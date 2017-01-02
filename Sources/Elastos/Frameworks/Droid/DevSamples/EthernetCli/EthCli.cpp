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

#include <sys/socket.h>
#include <sys/un.h>
#include <cutils/log.h>

#define LOG_TAG "EthCli"

int main(int argc, char** argv)
{
    int socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        ALOGE("socket() failed!");
        return 1;
    }

    struct sockaddr_un address;

    /* start with a clean address structure */
    memset(&address, 0, sizeof(struct sockaddr_un));

    address.sun_family = AF_UNIX;
    snprintf(address.sun_path, UNIX_PATH_MAX, "/dev/socket/netd");

    if (connect(socket_fd,
            (struct sockaddr *) &address,
            sizeof(struct sockaddr_un)) != 0) {
        ALOGE("connect() failed!");
        return 1;
    }

    char buffer[256];

    int nbytes = snprintf(buffer, 256, "hello from a client");
    write(socket_fd, buffer, nbytes);

    nbytes = read(socket_fd, buffer, 256);
    buffer[nbytes] = 0;

    ALOGD("MESSAGE FROM SERVER: %s", buffer);

    close(socket_fd);

    return 0;
}
