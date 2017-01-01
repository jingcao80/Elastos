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

#include "org/alljoyn/bus/Globals.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

// Annotations
//
const String Globals::Annotation_BusInterface("Org.Alljoyn.Bus.Annotation.BusInterface");
const String Globals::Annotation_BusMethod("Org.Alljoyn.Bus.Annotation.BusMethod");
const String Globals::Annotation_BusSignal("Org.Alljoyn.Bus.Annotation.BusSignal");
const String Globals::Annotation_BusProperty("Org.Alljoyn.Bus.Annotation.BusProperty");
const String Globals::Annotation_Secure("Org.Alljoyn.Bus.Annotation.Secure");
const String Globals::Annotation_BusAnnotations("Org.Alljoyn.Bus.Annotation.BusAnnotations");
const String Globals::Annotation_AccessPermission("Org.Alljoyn.Bus.Annotation.AccessPermission");
const String Globals::Annotation_BusSignalHandler("Org.Alljoyn.Bus.Annotation.BusSignalHandler");

const String Globals::BusProperty_EmitsChangedSignal("org.freedesktop.DBus.Property.EmitsChangedSignal");
const Int32 Globals::BusProperty_ANNOTATE_EMIT_CHANGED_SIGNAL = 1;
const Int32 Globals::BusProperty_ANNOTATE_EMIT_CHANGED_SIGNAL_INVALIDATES = 2;
const String Globals::BusProperty_ANNOTATE_EMIT_CHANGED_SIGNAL_STRING("BusProperty.ANNOTATE_EMIT_CHANGED_SIGNAL");
const String Globals::BusProperty_ANNOTATE_EMIT_CHANGED_SIGNAL_INVALIDATES_STRING("BusProperty.ANNOTATE_EMIT_CHANGED_SIGNAL_INVALIDATES");

} // namespace Bus
} // namespace Alljoyn
} // namespace Org