
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_SPEECHRECOGNITIONERROR_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_SPEECHRECOGNITIONERROR_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

class SpeechRecognitionError
{
public:
    static const Int32 NONE = 0;
    static const Int32 ABORTED = 1;
    static const Int32 AUDIO = 2;
    static const Int32 NETWORK = 3;
    static const Int32 NOT_ALLOWED = 4;
    static const Int32 NO_SPEECH = 5;
    static const Int32 NO_MATCH = 6;
    static const Int32 BAD_GRAMMAR = 7;
    static const Int32 LAST = 7;
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_SPEECHRECOGNITIONERROR_H__
