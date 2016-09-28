
#include "xplay.h"

/* xPlay : InputChan */

void FileWriter(WrFileBuffer &wrFileBuffer)
{
    int *buf = wrFileBuffer.writeBuffer;

    size_t bufSize = wrFileBuffer.getBufferSize();

    wrFileBuffer.signalFileWriterInitialized();

    while (1)     
    {
        buf = wrFileBuffer.getReadBuffer();
        
        /* Note, libsnd file will do the converestion for use e.g. from wav 16 */
        if (sf_write_int (wrFileBuffer.outfile, buf, bufSize) != bufSize)
        {
            puts(sf_strerror (wrFileBuffer.outfile));
        }
    }

    /* Note, currently we only write out full buffers, we could write out a partial buffer on die */
}

void FileInputChan::consumeSample(int inputSample)
{
    this->buf[count] = inputSample;
    this->count++;
    if (this->count == this->bufSize) 
    {
        this->buf = wrFileBuffer->swapWriteBuffers();
        this->count = 0;
    }
}

InputChan::InputChan(int chanCount, int sampRate) 
{ 
    this->chanCount = chanCount;
    this->sampRate = sampRate;
}

FileInputChan::FileInputChan(char *filename, int chanCount, int sampleRate) : InputChan(chanCount, sampleRate) 
{
    int bufferSize = BUFFER_LENGTH * chanCount;
    wrFileBuffer = new WrFileBuffer(bufferSize, filename, chanCount, sampleRate);
    wrFileThread = new std::thread(FileWriter, std::ref(*this->wrFileBuffer) /* sampleRate, freq, chanId*/);

    /* Note, this will wait until FileReader thread is ready to go.. */
    buf = wrFileBuffer->getInitialWriteBuffer();

    this->bufSize = bufferSize;
    this->count= 0;
}

FileInputChan::~FileInputChan()
{
    delete wrFileBuffer;
    delete wrFileThread;
}

