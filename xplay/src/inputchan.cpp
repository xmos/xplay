
#include "xplay.h"

/* xPlay : InputChan */

void FileInputChan::consumeSample(int inputSample)
{

}

InputChan::InputChan(int x) 
{ 
    this->chanCount = x;
}

FileInputChan::FileInputChan(char *filename, int x) : InputChan(x) 
{
   // fileBuffer = new FileBuffer(OUT_BLOCK_SIZE, filename);
    //fileThread = new std::thread(FileReader, std::ref(*this->fileBuffer) /* sampleRate, freq, chanId*/);

    /* Note, this will wait until FileReader thread is ready to go.. */
    //buf = fileBuffer->getInitialReadBuffer();

    this->bufSize = OUT_BLOCK_SIZE;
    //this->count= 0;
}

FileInputChan::~FileInputChan()
{
}

