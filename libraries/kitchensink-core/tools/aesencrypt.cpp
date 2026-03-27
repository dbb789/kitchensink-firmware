#include "crypto/cryptooutstream.h"
#include "crypto/cryptotypes.h"
#include "crypto/cryptoutil.h"
#include "types/dataref.h"
#include "types/outstream.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

// Simple OutStream wrapping a stdio FILE for writing encrypted output.
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

static bool randomFill(uint8_t* buf, std::size_t len)
{
    FILE* f = fopen("/dev/urandom", "rb");
    if (!f)
    {
        return false;
    }

    bool ok = fread(buf, 1, len, f) == len;
    fclose(f);
    return ok;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: aesencrypt <password> <inputfile>\n");
        fprintf(stderr, "  Writes encrypted output to <inputfile>.aes\n");
        return 1;
    }

    const char* password   = argv[1];
    const char* inputPath  = argv[2];
    std::string outputPath = std::string(inputPath) + ".aes";

    if (access(outputPath.c_str(), F_OK) == 0)
    {
        fprintf(stderr, "Output file already exists: %s\n", outputPath.c_str());
        return 1;
    }

    Crypto::IV  iv, dataIv;
    Crypto::Key dataKey;

    if (!randomFill(iv.data(),      iv.size())      ||
        !randomFill(dataIv.data(),  dataIv.size())  ||
        !randomFill(dataKey.data(), dataKey.size()))
    {
        fprintf(stderr, "Failed to generate random bytes\n");
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

    int exitCode = 0;

    {
        FileOutStream    fileOut(outFile);
        CryptoOutStream  cryptOut(fileOut, StrRef(password), StrRef(""), iv, dataIv, dataKey);

        uint8_t    buf[4096];
        std::size_t n;

        while ((n = fread(buf, 1, sizeof(buf), inFile)) > 0)
        {
            if (cryptOut.write(DataRef(buf, buf + n)) != n)
            {
                fprintf(stderr, "Encryption error\n");
                exitCode = 1;
                break;
            }
        }

        if (exitCode == 0 && ferror(inFile))
        {
            fprintf(stderr, "Error reading input file\n");
            exitCode = 1;
        }

        // CryptoOutStream destructor flushes and finalises the encrypted stream.
    }

    fclose(inFile);
    fclose(outFile);

    if (exitCode != 0)
    {
        remove(outputPath.c_str());
    }

    return exitCode;
}
