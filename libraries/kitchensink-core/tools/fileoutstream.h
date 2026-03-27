#ifndef INCLUDED_FILEOUTSTREAM_H
#define INCLUDED_FILEOUTSTREAM_H

#include "types/outstream.h"

#include <cstdio>

class DataRef;

class FileOutStream : public OutStream
{
public:
    explicit FileOutStream(FILE* file);

public:
    virtual std::size_t write(const DataRef& data) override;

private:
    FILE* mFile;
};

#endif
