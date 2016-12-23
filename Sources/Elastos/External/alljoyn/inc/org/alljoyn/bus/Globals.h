
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
