
#include "xplay.h"

/* xPlay : InputChan */

void FileWriter(WrFileBuffer &wrFileBuffer)
{
    int *buf = wrFileBuffer.writeBuffer;

    size_t bufSize = wrFileBuffer.getBufferSize();

    printf("FileWriter:: using buffer size %d\n", (int) bufSize);

    while(1);
        
#if 0
    /* Fill up our first buffer */
    /* TODO handle readcount < bufSize */
    int readcount = sf_read_int(fileBuffer.infile, buf, bufSize);

    /* Signal that we are ready to go! */
    fileBuffer.signalFileReaderInitialized();

    while (1)     
    {
        buf = fileBuffer.getWriteBuffer();
        
        /* Note, libsnd file will do the converestion for use e.g. from wav 16 */
        int readcount = sf_read_int(fileBuffer.infile, buf, bufSize);

        if(readcount == 0)
        {
            /* TODO handle EOF case */
            printf("EOF");
            while(1);
        }
    }
#endif
}

void FileInputChan::consumeSample(int inputSample)
{
    printf("%d\n", inputSample);
}

InputChan::InputChan(int chanCount, int sampRate) 
{ 
    this->chanCount = chanCount;
    this->sampRate = sampRate;
}

FileInputChan::FileInputChan(char *filename, int chanCount, int sampleRate) : InputChan(chanCount, sampleRate) 
{
    wrFileBuffer = new WrFileBuffer(OUT_BLOCK_SIZE, filename, chanCount, sampleRate);
    wrFileThread = new std::thread(FileWriter, std::ref(*this->wrFileBuffer) /* sampleRate, freq, chanId*/);

    /* Note, this will wait until FileReader thread is ready to go.. */
    //buf = fileBuffer->getInitialReadBuffer();

    this->bufSize = OUT_BLOCK_SIZE;
    this->count= 0;
}

FileInputChan::~FileInputChan()
{
}

