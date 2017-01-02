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

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Os.h"
#include "elastos/droid/hardware/SerialPort.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

using Elastos::IO::IBuffer;

namespace Elastos {
namespace Droid {
namespace Hardware {

String SerialPort::TAG("SerialPort");

CAR_INTERFACE_IMPL(SerialPort, Object, ISerialPort)

SerialPort::SerialPort()
    : mNativeContext(0)
{
}

SerialPort::~SerialPort()
{
}

ECode SerialPort::constructor(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode SerialPort::Open(
    /* [in] */ IParcelFileDescriptor* pfd,
    /* [in] */ Int32 speed)
{
    AutoPtr<IFileDescriptor> fd;
    pfd->GetFileDescriptor((IFileDescriptor**)&fd);
    native_open(fd, speed);
    mFileDescriptor = pfd;
    return NOERROR;
}

ECode SerialPort::Close()
{
    if (mFileDescriptor != NULL) {
        mFileDescriptor->Close();
        mFileDescriptor = NULL;
    }
    native_close();
    return NOERROR;
}

ECode SerialPort::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode SerialPort::Read(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(buffer);
    VALIDATE_NOT_NULL(num);

    Boolean isDirect = FALSE, hasArray = FALSE;

    Int32 remaining = 0;
    IBuffer::Probe(buffer)->GetRemaining(&remaining);
    if (IBuffer::Probe(buffer)->IsDirect(&isDirect), isDirect) {
        *num = native_read_direct(buffer, remaining);
    } else if (IBuffer::Probe(buffer)->HasArray(&hasArray), hasArray) {
        AutoPtr<ArrayOf<Byte> > array;
        buffer->GetArray((ArrayOf<Byte>**)&array);
        *num = native_read_array(array, remaining);
    } else {
        // throw new IllegalArgumentException("buffer is not direct and has no array");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode SerialPort::Write(
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 length)
{
    VALIDATE_NOT_NULL(buffer);

    Boolean isDirect = FALSE, hasArray = FALSE;

    if (IBuffer::Probe(buffer)->IsDirect(&isDirect), isDirect) {
        native_write_direct(buffer, length);
    } else if (IBuffer::Probe(buffer)->HasArray(&hasArray), hasArray) {
        AutoPtr<ArrayOf<Byte> > array;
        buffer->GetArray((ArrayOf<Byte>**)&array);
        native_write_array(array, length);
    } else {
        // throw new IllegalArgumentException("buffer is not direct and has no array");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode SerialPort::SendBreak()
{
    native_send_break();
    return NOERROR;
}

void SerialPort::native_open(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [in] */ Int32 speed)
{
    switch (speed) {
        case 50:
            speed = B50;
            break;
        case 75:
            speed = B75;
            break;
        case 110:
            speed = B110;
            break;
        case 134:
            speed = B134;
            break;
        case 150:
            speed = B150;
            break;
        case 200:
            speed = B200;
            break;
        case 300:
            speed = B300;
            break;
        case 600:
            speed = B600;
            break;
        case 1200:
            speed = B1200;
            break;
        case 1800:
            speed = B1800;
            break;
        case 2400:
            speed = B2400;
            break;
        case 4800:
            speed = B4800;
            break;
        case 9600:
            speed = B9600;
            break;
        case 19200:
            speed = B19200;
            break;
        case 38400:
            speed = B38400;
            break;
        case 57600:
            speed = B57600;
            break;
        case 115200:
            speed = B115200;
            break;
        case 230400:
            speed = B230400;
            break;
        case 460800:
            speed = B460800;
            break;
        case 500000:
            speed = B500000;
            break;
        case 576000:
            speed = B576000;
            break;
        case 921600:
            speed = B921600;
            break;
        case 1000000:
            speed = B1000000;
            break;
        case 1152000:
            speed = B1152000;
            break;
        case 1500000:
            speed = B1500000;
            break;
        case 2000000:
            speed = B2000000;
            break;
        case 2500000:
            speed = B2500000;
            break;
        case 3000000:
            speed = B3000000;
            break;
        case 3500000:
            speed = B3500000;
            break;
        case 4000000:
            speed = B4000000;
            break;
        default:
            assert(0);
            // jniThrowException(env, "java/lang/IllegalArgumentException",
            //                   "Unsupported serial port speed");
            return;
    }

    // Int32 fd = jniGetFDFromFileDescriptor(env, fileDescriptor);
    Int32 fd = 0;
    assert(fileDescriptor != NULL);
    fileDescriptor->GetDescriptor(&fd);

    // duplicate the file descriptor, since ParcelFileDescriptor will eventually close its copy
    fd = dup(fd);
    if (fd < 0) {
        assert(0);
        // jniThrowException(env, "java/io/IOException", "Could not open serial port");
        return;
    }

    // env->SetIntField(thiz, field_context, fd);
    mNativeContext = fd;

    struct termios tio;
    if (tcgetattr(fd, &tio))
        memset(&tio, 0, sizeof(tio));

    tio.c_cflag =  speed | CS8 | CLOCAL | CREAD;
    // Disable output processing, including messing with end-of-line characters.
    tio.c_oflag &= ~OPOST;
    tio.c_iflag = IGNPAR;
    tio.c_lflag = 0; /* turn of CANON, ECHO*, etc */
    /* no timeout but request at least one character per read */
    tio.c_cc[VTIME] = 0;
    tio.c_cc[VMIN] = 1;
    tcsetattr(fd, TCSANOW, &tio);
    tcflush(fd, TCIFLUSH);
}

void SerialPort::native_close()
{
    close(mNativeContext);
    mNativeContext = -1;
}

Int32 SerialPort::native_read_array(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 length)
{
    Int32 fd = mNativeContext;
    Byte* buf = (Byte *)malloc(length);
    if (!buf) {
        assert(0);
        // jniThrowException(env, "java/lang/OutOfMemoryError", NULL);
        return -1;
    }

    Int32 ret = read(fd, buf, length);
    if (ret > 0) {
        // copy data from native buffer to Java buffer
        // env->SetByteArrayRegion(buffer, 0, ret, buf);
        memcpy(buffer->GetPayload(), buf, ret);
    }

    free(buf);
    if (ret < 0) {
        assert(0);
        // jniThrowException(env, "java/io/IOException", NULL);
    }

    return ret;
}

Int32 SerialPort::native_read_direct(
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 length)
{
    Int32 fd = mNativeContext /*env->GetIntField(thiz, field_context)*/;

    assert(0);
    // jbyte* buf = (jbyte *)env->GetDirectBufferAddress(buffer);
    Byte* buf = NULL;
    if (!buf) {
        assert(0);
        // jniThrowException(env, "java/lang/IllegalArgumentException", "ByteBuffer not direct");
        return -1;
    }

    Int32 ret = read(fd, buf, length);
    if (ret < 0) {
        assert(0);
        // jniThrowException(env, "java/io/IOException", NULL);
    }
    return ret;
}

void SerialPort::native_write_array(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 length)
{
    Int32 fd = mNativeContext;
    Byte* buf = (Byte *)malloc(length);
    if (!buf) {
        assert(0);
        // jniThrowException(env, "java/lang/OutOfMemoryError", NULL);
        return;
    }

    assert(0);
    // env->GetByteArrayRegion(buffer, 0, length, buf);

    Int32 ret = write(fd, buf, length);
    free(buf);
    if (ret < 0) {
        assert(0);
        // jniThrowException(env, "java/io/IOException", NULL);
    }
}

void SerialPort::native_write_direct(
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 length)
{
    Int32 fd = mNativeContext;

    assert(0);
    Byte* buf = NULL/*(jbyte *)env->GetDirectBufferAddress(buffer)*/;
    if (!buf) {
        assert(0);
        // jniThrowException(env, "java/lang/IllegalArgumentException", "ByteBuffer not direct");
        return;
    }
    Int32 ret = write(fd, buf, length);
    if (ret < 0) {
        assert(0);
        // jniThrowException(env, "java/io/IOException", NULL);
    }
}

void SerialPort::native_send_break()
{
    Int32 fd = mNativeContext;
    tcsendbreak(fd, 0);
}


} // namespace Hardware
} // namespace Droid
} // namespace Elastos
