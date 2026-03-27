#include "crypto/cryptoinstream.h"
#include "fileinstream.h"
#include "fileoutstream.h"
#include "types/dataref.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

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

    FileInStream   fileIn(inFile);
    FileOutStream  fileOut(outFile);
    CryptoInStream cryptIn(fileIn, StrRef(password), StrRef(""));

    while (cryptIn.state() == CryptoInStream::State::kReading)
    {
        cryptIn.read(fileOut, 4096);
    }

    if (ferror(outFile))
    {
        fprintf(stderr, "Error writing output file\n");
        remove(outputPath.c_str());
        return 1;
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
