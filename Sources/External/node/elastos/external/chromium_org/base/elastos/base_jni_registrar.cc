// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/base_jni_registrar.h"

#include "base/elastos/api_elastos.h"
#include "base/elastos/application_status_listener.h"
#include "base/elastos/build_info.h"
#include "base/elastos/command_line_android.h"
#include "base/elastos/content_uri_utils.h"
#include "base/elastos/cpu_features.h"
#include "base/elastos/event_log.h"
#include "base/elastos/important_file_writer_android.h"
#include "base/elastos/java_handler_thread.h"
#include "base/elastos/jni_registrar.h"
#include "base/elastos/jni_utils.h"
#include "base/elastos/memory_pressure_listener_android.h"
#include "base/elastos/path_service_android.h"
#include "base/elastos/path_utils.h"
#include "base/elastos/sys_utils.h"
#include "base/elastos/thread_utils.h"
#include "base/elastos/trace_event_binding.h"
#include "base/basictypes.h"
#include "base/debug/trace_event.h"
#include "base/message_loop/message_pump_elastos.h"
#include "base/power_monitor/power_monitor_device_source_elastos.h"

namespace base {
namespace android {

static RegistrationMethod kBaseRegisteredMethods[] = {
  { "ApplicationStatusListener",
      base::android::ApplicationStatusListener::RegisterBindings },
  { "BuildInfo", base::android::BuildInfo::RegisterBindings },
  { "CommandLine", base::android::RegisterCommandLine },
  { "ContentUriUtils", base::RegisterContentUriUtils },
  { "ThreadUtils", base::RegisterThreadUtils },
  { "CpuFeatures", base::android::RegisterCpuFeatures },
  { "JavaHandlerThread", base::android::JavaHandlerThread::RegisterBindings },
  { "TraceEvent", base::android::RegisterTraceEvent },
  { "MemoryPressureListenerAndroid",
      base::android::MemoryPressureListenerAndroid::Register },
  { "ImportantFileWriterAndroid",
    base::android::RegisterImportantFileWriterAndroid },
  { "EventLog", base::android::RegisterEventLog },
  { "JNIUtils", base::android::RegisterJNIUtils },
  { "PathUtils", base::android::RegisterPathUtils },
  { "PathService", base::android::RegisterPathService },
  { "SysUtils", base::android::SysUtils::Register },
  { "SystemMessageHandler", base::MessagePumpForUI::RegisterBindings },
  { "PowerMonitor", base::RegisterPowerMonitor },
};

bool RegisterJni() {
  TRACE_EVENT0("startup", "base_android::RegisterJni");
  return RegisterNativeMethods(kBaseRegisteredMethods,
                               arraysize(kBaseRegisteredMethods));
}

}  // namespace android
}  // namespace base
