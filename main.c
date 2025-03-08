#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct
{
    int length;
    short type;
    char *data;
    char *crc;
} PNG_chunk;

PNG_chunk parseChunk()
{
}

int fourBytesToInt(int bytes[], int start)
{
    int convertedValue = 0, byteValue;
    for (int i = start, j = 3; i < start + 4, j >= 0; i++, j--)
    {
        byteValue = bytes[i];
        convertedValue += (byteValue * pow(16, j));
    }
    return convertedValue;
}

int crcValidatePerByte(int dataByte, int crcByte)
{

    int polynomialHEX = 0xEDB88320;
    // XOR CRCBYTE WITH DATABYTE
    printf("XOR-ing crcByte %02x with dataByte %02x:\t", crcByte, dataByte);
    crcByte = crcByte ^ dataByte;
    printf("%02x\n", crcByte);
    // FOR EACH BIT IN DATABYTE, EXTRACT LEAST SIGNIFICANT BIT, AND SHIFT ALL BITS TO RIGHT
    int lsb;
    for (int i = 0; i < 8; i++)
    {
        lsb = dataByte & 1;
        printf("LSB of dataByte %d is %d\n", dataByte, lsb);
        dataByte = dataByte >> 1;
        crcByte = crcByte >> 1;
        // ONLY XOR CRCBYTE WITH POLYNOMIAL HEX IF LSB IS 1
        if (lsb == 1)
        {
            crcByte = crcByte ^ polynomialHEX;
        }
    }
    printf("\nAt end of byte and crc processing, the CRC value is now %02x\n", crcByte);
    return crcByte;
}

void readFile(char *imageFileName)
{
    FILE *file = fopen(imageFileName, "rb");
    if (!file)
    {
        perror("Error opening image file");
        return;
    }

    int byte;
    int byteValue;
    int bytesArray[100], bytesIter = 0;
    int chunkLength = 0;
    int readOffset = 0;
    int totalChunks = 0;
    int i;
    /*
    READ CONTENT OF IMAGE SIGNATURE FIELD (8 BYTES)
    */
    for (i = 0; i < 8; i++)
    { // SKIP INITIAL 8 BYTES OF FILE SIGNATURE
        byteValue = fgetc(file);
        // bytesArray[bytesIter++] = byteValue;
        readOffset++;
        // printf("%02X\t", fgetc(file)); //LOADING 8 BYTES LONG SIGNATURE CORRECTLY
    }
    printf("\n");
    /*
    READ IMAGE HEADER CHUNK
    */
    int imageHeaderLength = 0;
    for (int j = 3; j >= 0; j--)
    {
        byteValue = fgetc(file);
        // bytesArray[bytesIter++] = byteValue;
        imageHeaderLength += (byteValue * pow(256, j));
    }
    printf("\nLength of image header is %d\n", imageHeaderLength);
    printf("Image Header Chunk Type is: ");
    for (i = 0; i < 4; i++)
    { // I-H-D-R CHARS
        byteValue = fgetc(file);
        bytesArray[bytesIter++] = byteValue;
        printf("%d -> %c\t", byteValue, byteValue);
    }
    printf("\n");

    int imageWidth = 0;
    for (int j = 3; j >= 0; j--)
    {
        byteValue = fgetc(file);
        bytesArray[bytesIter++] = byteValue;
        if (byteValue == 0)
        {
            continue;
        }
        imageWidth += (byteValue * pow(256, j));
    }
    printf("\nWidth of image is %d\n", imageWidth);

    int imageHeight = 0;
    for (int j = 3; j >= 0; j--)
    {
        byteValue = fgetc(file);
        bytesArray[bytesIter++] = byteValue;
        if (byteValue == 0)
        {
            continue;
        }
        imageHeight += (byteValue * pow(256, j));
    }
    printf("\nHeight of image is %d\n", imageHeight);
    int bitDepth = fgetc(file), colorType = fgetc(file), compression = fgetc(file), filter = fgetc(file), interlace = fgetc(file);
    bytesArray[bytesIter++] = bitDepth;
    bytesArray[bytesIter++] = colorType;
    bytesArray[bytesIter++] = compression;
    bytesArray[bytesIter++] = filter;
    bytesArray[bytesIter++] = interlace;
    printf("Bit Depth: %d\n", bitDepth);
    printf("Color Type: %d\n", colorType);
    printf("Compression: %d\n", compression);
    printf("Filter: %d\n", filter);
    printf("Interlace: %d\n", interlace);
    /*
    RUN A CRC CHECK ON THE 17 BYTES IN THE BYTES_ARRAY
    */
    int crcValue = 0xFFFFFFFF;
    for (i = 0; i < bytesIter; i++)
    {
        printf("%02x\t", bytesArray[i]);
        crcValue = crcValidatePerByte(bytesArray[i], crcValue);
    }
    // for (i = 0; i < 5; i++)
    // { // SKIP IMAGE_HEADER ihdr FIELD
    //     byteValue = fgetc(file);
    //     // fgetc(file);
    //     // printf("%02X\t", byteValue);
    // }
    printf("\n");
    readOffset = 33;

    while ((byte = fgetc(file)) != EOF)
    {
        // printf("Length of chunk is %02X in HEX and %d in Base-10\n", byte, byte);
        // printf("Now printing content of image\n");
        totalChunks++;
        if (readOffset == 0)
        {
            for (i = 0; i < byte; i++)
            {
                fgetc(file);
                printf("%02X\t", fgetc(file));
            }
        }
        readOffset++;
    }
    printf("\nTotal chunks were %d\n", totalChunks);
    fclose(file);
}

int main()
{
    printf("Hello, World!\n");
    readFile("img.png");
    // printf("\nValidating via CRC check...");
    // int k = 7;
    // k = k >> 1;
    // printf("%d\n", k);
    // k = k >> 1;
    // printf("%d\n", k);
    int exitWait;
    scanf("%d", &exitWait);
    return EXIT_SUCCESS;
}