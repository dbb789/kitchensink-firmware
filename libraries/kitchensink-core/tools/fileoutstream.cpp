#include "fileoutstream.h"

#include "types/dataref.h"

FileOutStream::FileOutStream(FILE* file)
    : mFile(file)
{ }

std::size_t FileOutStream::write(const DataRef& data)
{
    return fwrite(data.begin(), 1, data.end() - data.begin(), mFile);
}
