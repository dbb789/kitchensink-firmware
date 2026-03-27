#include "crypto/cryptooutstream.h"
#include "crypto/cryptotypes.h"
#include "fileoutstream.h"
#include "types/dataref.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

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

    FileOutStream    fileOut(outFile);
    CryptoOutStream  cryptOut(fileOut, StrRef(password), StrRef(""), iv, dataIv, dataKey);

    if (cryptOut.state() != CryptoOutStream::State::kWriting)
    {
        fprintf(stderr, "Encryption initialization failed\n");
        return 1;
    }

    uint8_t     buf[4096];
    std::size_t n;

    while ((n = fread(buf, 1, sizeof(buf), inFile)) > 0)
    {
        if (cryptOut.write(DataRef(buf, buf + n)) != n)
        {
            fprintf(stderr, "Encryption error\n");
            return 1;
        }
    }

    if (ferror(inFile))
    {
        fprintf(stderr, "Error reading input file\n");
        return 1;
    }

    return 0;
}
