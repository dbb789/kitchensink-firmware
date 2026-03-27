#ifndef INCLUDED_FILEINSTREAM_H
#define INCLUDED_FILEINSTREAM_H

#include "types/instream.h"

#include <cstdio>

class OutStream;

class FileInStream : public InStream
{
public:
    explicit FileInStream(FILE* file);

public:
    virtual std::size_t read(OutStream& os, std::size_t len) override;

private:
    FILE* mFile;
};

#endif
