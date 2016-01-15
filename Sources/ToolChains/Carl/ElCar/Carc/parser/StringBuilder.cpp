
#include "StringBuilder.h"
#include <stdlib.h>
#include <string.h>

StringBuilder::StringBuilder(
    /* [in] */ int cap)
    : mSize(0)
    , mCapacity(cap)
{
    mBuf = (char*)malloc(mCapacity);
}

StringBuilder::~StringBuilder()
{
    free(mBuf);
    mCapacity = mSize = 0;
}

void StringBuilder::Append(
    /* [in] */ const char* str)
{
    int size = strlen(str);
    if (mSize + size + 1 > mCapacity) {
        int newSize = ((mSize + size) * 3) / 2;
        char* data = (char*)realloc(mBuf, newSize);
        mBuf = data;
        mCapacity = newSize;
    }
    strcpy(mBuf + mSize, str);
    mSize += size;
}

char* StringBuilder::ToString()
{
    return mBuf;
}

int StringBuilder::GetLength()
{
    return mSize;
}

void StringBuilder::Reset()
{
    mSize = 0;
}
