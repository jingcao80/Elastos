
#ifndef __STRINGBUILDER_H__
#define __STRINGBUILDER_H__

class StringBuilder
{
public:
    StringBuilder(
        /* [in] */ int cap = 128);

    ~StringBuilder();

    void Append(
        /* [in] */ const char* str);

    char* ToString();

    int GetLength();

    void Reset();

private:
    int mSize;
    int mCapacity;
    char* mBuf;
};

#endif //__STRINGBUILDER_H__
