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

#ifndef __ORG_ALLJOYN_BUS_GLOBALS_H__
#define __ORG_ALLJOYN_BUS_GLOBALS_H__

#include <elastos.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class Globals {
public:

    // Annotations
    //
    static const String Annotation_BusInterface;
    static const String Annotation_BusMethod;
    static const String Annotation_BusSignal;
    static const String Annotation_BusProperty;
    static const String Annotation_Secure;
    static const String Annotation_BusAnnotations;
    static const String Annotation_AccessPermission;
    static const String Annotation_BusSignalHandler;

    // BusProperty
    //
    static const String BusProperty_EmitsChangedSignal;
    static const Int32 BusProperty_ANNOTATE_EMIT_CHANGED_SIGNAL;
    static const Int32 BusProperty_ANNOTATE_EMIT_CHANGED_SIGNAL_INVALIDATES;
    static const String BusProperty_ANNOTATE_EMIT_CHANGED_SIGNAL_STRING;
    static const String BusProperty_ANNOTATE_EMIT_CHANGED_SIGNAL_INVALIDATES_STRING;

private:
    Globals();
    Globals(const Globals&);
};


} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_GLOBALS_H__
