
/frameworks/native/services/inputflinger/InputDispatcher.cpp
    InputDispatcher::setInputWindows()
        will call windowHandle->updateInfo()
    InputDispatcher::setFocusedApplication()
        will call inputApplicationHandle->updateInfo()


class InputApplicationHandle : public RefBase
    is defined in:  /frameworks/native/services/inputflinger/InputApplication.h
                    /frameworks/native/services/inputflinger/InputApplication.cpp

class NativeInputApplicationHandle : public InputApplicationHandle
    is defined in:  /frameworks/base/services/core/jni/com_android_server_input_InputApplicationHandle.h
                    /frameworks/base/services/core/jni/com_android_server_input_InputApplicationHandle.cpp


