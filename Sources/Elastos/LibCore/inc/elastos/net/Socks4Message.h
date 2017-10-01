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

#ifndef __ELASTOS_NET_SOCKS4MESSAGE_H__
#define __ELASTOS_NET_SOCKS4MESSAGE_H__

#include "Elastos.CoreLibrary.Net.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Net {

class Socks4Message
    : public Object
    , public ISocks4Message
{
public:
    static const Int32 COMMAND_CONNECT = 1;
    static const Int32 COMMAND_BIND = 2;
    static const Int32 RETURN_SUCCESS = 90;
    static const Int32 RETURN_FAILURE = 91;
    static const Int32 RETURN_CANNOT_CONNECT_TO_IDENTD = 92;
    static const Int32 RETURN_DIFFERENT_USER_IDS = 93;
    static const Int32 REPLY_LENGTH = 8;
    static const Int32 INDEX_VERSION = 0;

private:
    static const Int32 SOCKS_VERSION = 4;
    static const Int32 INDEX_COMMAND = 1;
    static const Int32 INDEX_PORT = 2;
    static const Int32 INDEX_IP = 4;
    static const Int32 INDEX_USER_ID = 8;
    static const Int32 BUFFER_LENGTH = 256;
    static const Int32 MAX_USER_ID_LENGTH = BUFFER_LENGTH - INDEX_USER_ID;

protected:
    AutoPtr<ArrayOf<Byte> > mBuffer;

public:
    CAR_INTERFACE_DECL()

    //Socks4Message();

    CARAPI constructor();

    /**
     * Get the request's command or result.
     */
    CARAPI GetCommandOrResult(
        /* [out] */ Int32* result);

    /**
     * Set the request's command or result.
     */
    CARAPI SetCommandOrResult(
        /* [in] */ Int32 command);

    /**
     * Returns the request's port number.
     */
    CARAPI GetPort(
        /* [out] */ Int32* port);

    /**
     * Set the request's port number.
     */
    CARAPI SetPort(
        /* [in] */ Int32 port);

    /**
     * Returns the IP address of the request as an integer.
     */
    CARAPI GetIP(
        /* [out] */ Int32* ip);

    /**
     * Set the IP address. This expects an array of four bytes in host order.
     */
    CARAPI SetIP(
        /* [in] */ ArrayOf<Byte>* ip);

    /**
     * Returns the user id for authentication.
     */
    CARAPI GetUserId(
        /* [out] */ String* id);
    /**
     * Set the user id for authentication.
     */
    CARAPI SetUserId(
        /* [in] */ const String& id);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Returns the total number of bytes used for the request. This method
     * searches for the end of the user id, then searches for the end of the
     * password and returns the final index as the requests length.
     */
    CARAPI GetLength(
        /* [out] */ Int32* length);
    /**
     * Returns an error string corresponding to the given error value.
     */
    CARAPI GetErrorString(
        /* [in] */ Int32 error,
        /* [out] */ String* str);
    /**
     * Returns the message's byte buffer.
     */
    CARAPI GetBytes(
        /* [out, callee] */ ArrayOf<Byte>** bytes);

private:
    CARAPI_(Int32) GetInt16(
        /* [in] */ Int32 offset);

    CARAPI_(Int32) GetInt32(
        /* [in] */ Int32 offset);

    CARAPI_(String) GetString(
         /* [in] */ Int32 offset,
         /* [in] */ Int32 maxLength);

    CARAPI_(Int32) GetVersionNumber();

    CARAPI_(void) SetInt16(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 value);

    CARAPI_(void) SetString(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 maxLength,
        /* [in] */ const String& theString);

    CARAPI_(void) SetVersionNumber(int number);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_SOCKS4MESSAGE_H__
