#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#pragma pack(push, 1)
typedef struct
{
    uint16_t bfType;
    uint32_t  bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t  bfOffBits;
    uint32_t  biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    int16_t biPlanes;
    int16_t biBitCount;
    uint32_t  biCompression;
    uint32_t  biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t  biClrUsed;
    uint32_t  biClrImportant;
    uint32_t  RGBQuad_0;
    uint32_t  RGBQuad_1;
} bmpHdr;
#pragma pack(pop)

typedef struct
{
    int width, height;        // szerokosc i wysokosc obrazu
    unsigned char* pImg;    // wskazanie na początek danych pikselowych
    int cX, cY;                // "aktualne współrzędne"
    int col;                // "aktualny kolor"
} imgInfo;

void* freeResources(FILE* pFile, void* pFirst, void* pSnd)
{
    if (pFile != 0)
        fclose(pFile);
    if (pFirst != 0)
        free(pFirst);
    if (pSnd !=0)
        free(pSnd);
    return 0;
}

imgInfo* readBMP(const char* fname)
{
    imgInfo* pInfo = 0;
    FILE* fbmp = 0;
    bmpHdr bmpHead;
    int lineBytes, y;
    unsigned long imageSize = 0;
    unsigned char* ptr;

    pInfo = 0;
    fbmp = fopen(fname, "rb");
    if (fbmp == 0)
        return 0;

    fread((void *) &bmpHead, sizeof(bmpHead), 1, fbmp);
    // some basic checks
    if (bmpHead.bfType != 0x4D42 || bmpHead.biPlanes != 1 ||
        bmpHead.biBitCount != 1 || bmpHead.biClrUsed != 2 ||
        (pInfo = (imgInfo *) malloc(sizeof(imgInfo))) == 0)
        return (imgInfo*) freeResources(fbmp, pInfo->pImg, pInfo);

    pInfo->width = bmpHead.biWidth;
    pInfo->height = bmpHead.biHeight;
    imageSize = (((pInfo->width + 31) >> 5) << 2) * pInfo->height;

    if ((pInfo->pImg = (unsigned char*) malloc(imageSize)) == 0)
        return (imgInfo*) freeResources(fbmp, pInfo->pImg, pInfo);

    // process height (it can be negative)
    ptr = pInfo->pImg;
    lineBytes = ((pInfo->width + 31) >> 5) << 2; // line size in bytes
    if (pInfo->height > 0)
    {
        // "upside down", bottom of the image first
        ptr += lineBytes * (pInfo->height - 1);
        lineBytes = -lineBytes;
    }
    else
        pInfo->height = -pInfo->height;

    // reading image
    // moving to the proper position in the file
    if (fseek(fbmp, bmpHead.bfOffBits, SEEK_SET) != 0)
        return (imgInfo*) freeResources(fbmp, pInfo->pImg, pInfo);

    for (y=0; y<pInfo->height; ++y)
    {
        fread(ptr, 1, abs(lineBytes), fbmp);
        ptr += lineBytes;
    }
    fclose(fbmp);
    return pInfo;
}

int saveBMP(const imgInfo* pInfo, const char* fname)
{
    int lineBytes = ((pInfo->width + 31) >> 5)<<2;
    bmpHdr bmpHead =
    {
    0x4D42,                // unsigned short bfType;
    sizeof(bmpHdr),        // unsigned long  bfSize;
    0, 0,                // unsigned short bfReserved1, bfReserved2;
    sizeof(bmpHdr),        // unsigned long  bfOffBits;
    40,                    // unsigned long  biSize;
    pInfo->width,        // long  biWidth;
    pInfo->height,        // long  biHeight;
    1,                    // short biPlanes;
    1,                    // short biBitCount;
    0,                    // unsigned long  biCompression;
    lineBytes * pInfo->height,    // unsigned long  biSizeImage;
    11811,                // long biXPelsPerMeter; = 300 dpi
    11811,                // long biYPelsPerMeter;
    2,                    // unsigned long  biClrUsed;
    0,                    // unsigned long  biClrImportant;
    0x00000000,            // unsigned long  RGBQuad_0;
    0x00FFFFFF            // unsigned long  RGBQuad_1;
    };

    FILE * fbmp;
    unsigned char *ptr;
    int y;

    if ((fbmp = fopen(fname, "wb")) == 0)
        return -1;
    if (fwrite(&bmpHead, sizeof(bmpHdr), 1, fbmp) != 1)
    {
        fclose(fbmp);
        return -2;
    }

    ptr = pInfo->pImg + lineBytes * (pInfo->height - 1);
    for (y=pInfo->height; y > 0; --y, ptr -= lineBytes)
        if (fwrite(ptr, sizeof(unsigned char), lineBytes, fbmp) != lineBytes)
        {
            fclose(fbmp);
            return -3;
        }
    fclose(fbmp);
    return 0;
}

imgInfo* InitImage (int w, int h)
{
    imgInfo *pImg;
    if ( (pImg = (imgInfo *) malloc(sizeof(imgInfo))) == 0)
        return 0;
    pImg->height = h;
    pImg->width = w;
    pImg->pImg = (unsigned char*) malloc((((w + 31) >> 5) << 2) * h);
    if (pImg->pImg == 0)
    {
        free(pImg);
        return 0;
    }
    memset(pImg->pImg, 0xFF, (((w + 31) >> 5) << 2) * h);
    pImg->cX = 0;
    pImg->cY = 0;
    pImg->col = 0;
    return pImg;
}

void FreeImage(imgInfo* pInfo)
{
    if (pInfo && pInfo->pImg)
        free(pInfo->pImg);
    if (pInfo)
        free(pInfo);
}

imgInfo* SetColor(imgInfo* pImg, int col)
{
    pImg->col = col != 0;
    return pImg;
}

imgInfo* MoveTo(imgInfo* pImg, int x, int y)
{
    if (x >= 0 && x < pImg->width)
        pImg->cX = x;
    if (y >= 0 && y < pImg->height)
        pImg->cY = y;
    return pImg;
}

extern void encodeASM(char text[], char binar[]);

void encode(char text[], char binar[]) {
    encodeASM(text, binar);
    printf("Binary result: %s\n", binar);
}

extern void paintASM(unsigned char *pImg, unsigned short module_width, char text[], unsigned short offset);

void paintC(imgInfo *pInfo, unsigned short rowsize, unsigned short offset) {
    int i;
    unsigned char *first = pInfo->pImg + offset * rowsize;
    unsigned char *current = first;
    unsigned long temp_height = pInfo->height - offset;
    while (pInfo->cY < temp_height)
    {
        current = pInfo->pImg + pInfo->cY * rowsize;
        for (i=0; i<rowsize; ++i) {
            *current = *first;
            current++;
            first++;
        }
        pInfo->cY += 1;
        first = pInfo->pImg + offset * rowsize;
    }
}

void paint(imgInfo *pInfo, char text[]) {
    unsigned short module_width = pInfo->width / 67;
    unsigned long rowsize = ((pInfo->width + 31) >> 5) << 2;
    unsigned short offset = (pInfo->width - (module_width * 67)) / 2;
    SetColor(pInfo, 0);
    paintASM(pInfo->pImg+rowsize*offset, module_width, text, offset);
    MoveTo(pInfo, 0, offset+1);
    paintC(pInfo, rowsize, offset);
    
}

int convertChars(char number[]) {
    int i = 0; int result = 0;
    while(number[i] != 0) {
        result *= 10;
        result += (number[i] - '0');
        i++;
    }
    return result;
}

int main(int argc, char* argv[])
{
    imgInfo* pInfo;
    char* text = "12345671";
    char binar[] = "101AAAAAAAAAAAAAAAAAAAAAAAAAAAA01010AAAAAAAAAAAAAAAAAAAAAAAAAAAA101";
    unsigned short height = 60;
    unsigned short width = 300;
    // minimal 67x1; maximal 65535x65535
    if (argc == 2) {
        text = argv[1];
    }
    if (argc == 3) {
        width = convertChars(argv[1]);
        height = convertChars(argv[2]);
    }
    if (argc == 4) {
        text = argv[1];
        width = convertChars(argv[2]);
        height = convertChars(argv[3]);
    }
    
    if (sizeof(bmpHdr) != 62)
    {
        printf("Change compilation options so as bmpHdr struct size is 62 bytes instead of %li.\n", sizeof(bmpHdr));
        return 1;
    }
    if ((pInfo = InitImage (width, height)) == 0)
        return 2;

    encode(text, binar);
    paint(pInfo, binar);

    saveBMP(pInfo, "result.bmp");
    FreeImage(pInfo);
    return 0;
}

