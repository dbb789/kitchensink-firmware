#include "fileinstream.h"

#include "types/dataref.h"
#include "types/outstream.h"

FileInStream::FileInStream(FILE* file)
    : mFile(file)
{ }

std::size_t FileInStream::read(OutStream& os, std::size_t len)
{
    uint8_t     buf[4096];
    std::size_t toRead = len < sizeof(buf) ? len : sizeof(buf);
    std::size_t n      = fread(buf, 1, toRead, mFile);
    if (n == 0)
    {
        return 0;
    }
    return os.write(DataRef(buf, buf + n));
}
