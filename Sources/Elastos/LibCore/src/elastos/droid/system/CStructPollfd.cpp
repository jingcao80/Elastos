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

#include "Elastos.CoreLibrary.IO.h"
#include "CStructPollfd.h"

namespace Elastos {
namespace Droid {
namespace System {

CAR_OBJECT_IMPL(CStructPollfd)

CAR_INTERFACE_IMPL(CStructPollfd, Object, IStructPollfd)

ECode CStructPollfd::constructor()
{
    return NOERROR;
}

ECode CStructPollfd::GetFd(
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);

    *fd = mFd;
    REFCOUNT_ADD(*fd)
    return NOERROR;
}

ECode CStructPollfd::SetFd(
    /* [in] */ IFileDescriptor* fd)
{
    mFd = fd;
    return NOERROR;
}

ECode CStructPollfd::GetEvents(
    /* [out] */ Int16* events)
{
    VALIDATE_NOT_NULL(events);

    *events = mEvents;
    return NOERROR;
}

ECode CStructPollfd::SetEvents(
    /* [in] */ Int16 events)
{
    mEvents = events;
    return NOERROR;
}

ECode CStructPollfd::GetRevents(
    /* [out] */ Int16* revents)
{
    VALIDATE_NOT_NULL(revents);

    *revents = mRevents;
    return NOERROR;
}

ECode CStructPollfd::SetRevents(
    /* [in] */ Int16 revents)
{
    mRevents = revents;
    return NOERROR;
}

ECode CStructPollfd::GetUserData(
    /* [out] */ IInterface** userData)
{
    VALIDATE_NOT_NULL(userData);

    *userData = mUserData;
    REFCOUNT_ADD(*userData);
    return NOERROR;
}

ECode CStructPollfd::SetUserData(
    /* [in] */ IInterface* userData)
{
    mUserData = userData;
    return NOERROR;
}

} // namespace System
} // namespace Droid
} // namespace Elastos
