// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/event_log.h"
#include "base/elastos/api/EventLog_api.h"

namespace base {
namespace android {

void EventLogWriteInt(int tag, int value) {
  DCHECK(sElaEventLogCallback);
  //LLJava_EventLog_writeEvent(AttachCurrentThread(), tag, value);
  sElaEventLogCallback->elastos_EventLog_writeEvent(tag, value);
}

bool RegisterEventLog() {
  return RegisterNativesImpl();
}

}  // namespace android
}  // namespace base
