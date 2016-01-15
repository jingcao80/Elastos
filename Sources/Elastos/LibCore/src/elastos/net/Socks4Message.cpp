
#include "Socks4Message.h"
#include "Math.h"
#include "StringBuilder.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(Socks4Message, Object, ISocks4Message)

ECode Socks4Message::constructor()
{
    mBuffer = ArrayOf<Byte>::Alloc(BUFFER_LENGTH);
    SetVersionNumber(SOCKS_VERSION);
    return NOERROR;
}

ECode Socks4Message::GetCommandOrResult(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (*mBuffer)[INDEX_COMMAND];
    return NOERROR;
}

ECode Socks4Message::SetCommandOrResult(
    /* [in] */ Int32 command)
{
    (*mBuffer)[INDEX_COMMAND] = (Byte) command;
    return NOERROR;
}

ECode Socks4Message::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port)

    *port = GetInt16(INDEX_PORT);
    return NOERROR;
}

ECode Socks4Message::SetPort(
    /* [in] */ Int32 port)
{
    SetInt16(INDEX_PORT, port);
    return NOERROR;
}

ECode Socks4Message::GetIP(
    /* [out] */ Int32* ip)
{
    VALIDATE_NOT_NULL(ip)

    *ip = GetInt32(INDEX_IP);
    return NOERROR;
}

ECode Socks4Message::SetIP(
    /* [in] */ ArrayOf<Byte>* ip)
{
    (*mBuffer)[INDEX_IP] = (*ip)[0];
    (*mBuffer)[INDEX_IP + 1] = (*ip)[1];
    (*mBuffer)[INDEX_IP + 2] = (*ip)[2];
    (*mBuffer)[INDEX_IP + 3] = (*ip)[3];
    return NOERROR;
}

ECode Socks4Message::GetUserId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id)

    *id = GetString(INDEX_USER_ID, MAX_USER_ID_LENGTH);
    return NOERROR;
}

ECode Socks4Message::SetUserId(
    /* [in] */ const String& id)
{
    SetString(INDEX_USER_ID, MAX_USER_ID_LENGTH, id);
    return NOERROR;
}

ECode Socks4Message::ToString(
    /* [out] */ String* str )
{
    VALIDATE_NOT_NULL(str)

    Int32 version, command, ip, port;
    version = GetVersionNumber();
    GetPort(&port);
    GetCommandOrResult(&command);
    GetIP(&ip);
    String userId;
    GetUserId(&userId);

    StringBuilder buf (50);
    buf.Append("Version: ");
    buf.Append(StringUtils::ToHexString(version));
    buf.Append(" Command: ");
    buf.Append(StringUtils::ToHexString(command));
    buf.Append(" Port: ");
    buf.Append(port);
    buf.Append(" IP: ");
    buf.Append(StringUtils::ToHexString(ip));
    buf.Append(" User ID: ");
    buf.Append(userId);
    *str = buf.ToString();
    return NOERROR;
}

ECode Socks4Message::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)

    Int32 index = 0;

    // Look for the end of the user id.
    for (index = INDEX_USER_ID; (*mBuffer)[index] != 0; index++) {
    /*
     * Finds the end of the user id by searching for the null
     * termination of the user id string.
     */
    }

    // Increment the index to include the NULL character in the length;
    index++;
    *length = index;
    return NOERROR;
}

ECode Socks4Message::GetErrorString(
    /* [in] */ Int32 error,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    switch (error) {
    case RETURN_FAILURE:
        *str = String("Failure to connect to SOCKS server");
        return  NOERROR;
    case RETURN_CANNOT_CONNECT_TO_IDENTD:
        *str = String("Unable to connect to identd to verify user");
        return  NOERROR;
    case RETURN_DIFFERENT_USER_IDS:
        *str = String("Failure - user ids do not match");
        return  NOERROR;
    default:
        *str = String("Success");
        return  NOERROR;
    }
    return NOERROR;
}

ECode Socks4Message::GetBytes(
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes)

    *bytes = mBuffer;
    REFCOUNT_ADD(*bytes);
    return NOERROR;
}

//private-------------------------------------
Int32 Socks4Message::GetInt16(
    /* [in] */ Int32 offset)
{
    return ((((*mBuffer)[offset] & 0xFF) << 8) + ((*mBuffer)[offset + 1] & 0xFF));
}

Int32 Socks4Message::GetInt32(
    /* [in] */ Int32 offset)
{
    return (((*mBuffer)[offset + 3] & 0xFF)
        + (((*mBuffer)[offset + 2] & 0xFF) << 8)
        + (((*mBuffer)[offset + 1] & 0xFF) << 16)
        + (((*mBuffer)[offset + 0] & 0xFF) << 24));

}

String Socks4Message::GetString(
     /* [in] */ Int32 offset,
     /* [in] */ Int32 maxLength)
{
   Int32 index = offset;
   Int32 lastIndex = index + maxLength;
   while (index < lastIndex && ((*mBuffer)[index] != 0)) {
       index++;
   }
   AutoPtr< ArrayOf<Char32> > outchar = ArrayOf<Char32>::Alloc(index - offset);
   for (Int32 i = offset; i < index - offset; ++i) {
       (*outchar)[i] = (*mBuffer)[i];
   }
   return String(*outchar, 0, index - offset/*, Charsets.ISO_8859_1*/);
}

Int32 Socks4Message::GetVersionNumber()
{
    return (*mBuffer)[INDEX_VERSION];
}

void Socks4Message::SetInt16(
    /* [in] */ Int32 offset,
    /* [in] */ Int32 value)
{
    (*mBuffer)[offset] = (Byte) (value >> 8 & 0xFF);
    (*mBuffer)[offset + 1] = (Byte) (value & 0xFF);
}

void Socks4Message::SetString(
    /* [in] */ Int32 offset,
    /* [in] */ Int32 maxLength,
    /* [in] */ const String& theString)
{
    AutoPtr< ArrayOf<Char32> > stringBytes = theString.GetChars(/*Charsets.ISO_8859_1*/);
    Int32 length = Elastos::Core::Math::Min(stringBytes->GetLength(), maxLength);
    for (Int32 i = 0; i < length; ++i) {
        (*mBuffer)[offset + i] = (*stringBytes)[i] & 0xFF;
    }
    (*mBuffer)[offset + length] = 0;
}

void Socks4Message::SetVersionNumber(int number)
{
    (*mBuffer)[INDEX_VERSION] = (Byte) number;
}

} // namespace Net
} // namespace Elastos
