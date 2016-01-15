
#include "elastos/droid/webkit/native/content/browser/ChildProcessConnection.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

// Names of items placed in the bind intent or connection bundle.
const String ChildProcessConnection::EXTRA_COMMAND_LINE("com.google.android.apps.chrome.extra.command_line");

// Note the FDs may only be passed in the connection bundle.
const String ChildProcessConnection::EXTRA_FILES_PREFIX("com.google.android.apps.chrome.extra.extraFile_");

const String ChildProcessConnection::EXTRA_FILES_ID_SUFFIX("_id");

const String ChildProcessConnection::EXTRA_FILES_FD_SUFFIX("_fd");

// Used to pass the CPU core count to child processes.
const String ChildProcessConnection::EXTRA_CPU_COUNT("com.google.android.apps.chrome.extra.cpu_count");

// Used to pass the CPU features mask to child processes.
const String ChildProcessConnection::EXTRA_CPU_FEATURES("com.google.android.apps.chrome.extra.cpu_features");

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
