#include "crypto/cryptoinstream.h"
#include "crypto/cryptoutil.h"
#include "types/dataref.h"
#include "types/instream.h"
#include "types/outstream.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

// Simple InStream wrapping a stdio FILE for reading encrypted input.
class FileInStream : public InStream
{
public:
    explicit FileInStream(FILE* file)
        : mFile(file)
    { }

public:
    std::size_t read(OutStream& os, std::size_t len) override
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

private:
    FILE* mFile;
};

// Simple OutStream wrapping a stdio FILE for writing plaintext output.
class FileOutStream : public OutStream
{
public:
    explicit FileOutStream(FILE* file)
        : mFile(file)
    { }

public:
    std::size_t write(const DataRef& data) override
    {
        return fwrite(data.begin(), 1, data.end() - data.begin(), mFile);
    }

private:
    FILE* mFile;
};

static std::string stripSuffix(const std::string& path, const std::string& suffix)
{
    if (path.length() > suffix.length() &&
        path.substr(path.length() - suffix.length()) == suffix)
    {
        return path.substr(0, path.length() - suffix.length());
    }
    return "";
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: aesdecrypt <password> <inputfile>.aes\n");
        fprintf(stderr, "  Writes decrypted output to <inputfile>\n");
        return 1;
    }

    const char* password  = argv[1];
    const char* inputPath = argv[2];
    std::string outputPath = stripSuffix(inputPath, ".aes");

    if (outputPath.empty())
    {
        fprintf(stderr, "Input file must have a .aes extension: %s\n", inputPath);
        return 1;
    }

    if (access(outputPath.c_str(), F_OK) == 0)
    {
        fprintf(stderr, "Output file already exists: %s\n", outputPath.c_str());
        return 1;
    }

    FILE* inFile = fopen(inputPath, "rb");
    if (!inFile)
    {
        fprintf(stderr, "Cannot open input file: %s\n", inputPath);
        return 1;
    }

    FILE* outFile = fopen(outputPath.c_str(), "wb");
    if (!outFile)
    {
        fclose(inFile);
        fprintf(stderr, "Cannot create output file: %s\n", outputPath.c_str());
        return 1;
    }

    CryptoUtil::initializeLibrary();

    FileInStream   fileIn(inFile);
    FileOutStream  fileOut(outFile);
    CryptoInStream cryptIn(fileIn, StrRef(password), StrRef(""));

    while (cryptIn.state() == CryptoInStream::State::kReading)
    {
        cryptIn.read(fileOut, 4096);
    }

    fclose(inFile);
    fclose(outFile);

    if (cryptIn.state() != CryptoInStream::State::kValidated)
    {
        fprintf(stderr, "Decryption failed (state=%d) — bad password or corrupted file\n",
                static_cast<int>(cryptIn.state()));
        remove(outputPath.c_str());
        return 1;
    }

    return 0;
}
