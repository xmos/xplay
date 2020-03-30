
/* xPlay: FileBuffer */

#include "wrfile.h"
#include <iostream>
#include <cstring>
#include "logging.h"

/* Gets called when buffer has been played via portaudio */
int *WrFileBuffer::swapWriteBuffers(void) 
{
    std::unique_lock<std::mutex> l(lock);

    /* Check that buffer has been written to the file */
    if(readFull) 
    {
        std::cerr << "ERROR: Host file write is too slow for input" << std::endl;
    }

    /* Perform the buffer swap */ 
    int *tmp;
    tmp = writeBuffer;
    writeBuffer = readBuffer;
    readBuffer = tmp;
    writeFull = true;
    cvDoSwap.notify_one();
    return writeBuffer;
}

WrFileBuffer::~WrFileBuffer(void)
{
    delete writeBuffer;
    delete readBuffer;

    /* Force OS into writing of all file cache buffers to disk */
    sf_write_sync(outfile);

    /* Close the write file */
    sf_close(outfile) ;
}

int *WrFileBuffer::getReadBuffer(void)
{   
    std::unique_lock<std::mutex> l(lock);
    
    /* We have finished with current read buffer */
    readFull = false;
  
    /* Wait until write buffer is filled */
    cvDoSwap.wait(l, [this](){return (this->writeFull == true);});

    readFull = true;
    writeFull = false;

    return readBuffer;
}

int *WrFileBuffer::getInitialWriteBuffer(void)
{
    std::unique_lock<std::mutex> l(lock);
    cvFileWriterInit.wait(l, [this](){return this->fileWriterInitialized;});
    return writeBuffer;
}

void WrFileBuffer::signalFileWriterInitialized(void)
{
  std::unique_lock<std::mutex> l(lock);
  fileWriterInitialized = true;
  cvFileWriterInit.notify_one();
}

WrFileBuffer::WrFileBuffer(size_t bufSize, char * filename, unsigned chanCount, unsigned sampRate, unsigned bitRes)
{
    writeBuffer = new int[bufSize];
    readBuffer = new int[bufSize];
    this->bufSize = bufSize;
    this->filename = filename;
    this->writeFull = false;
    this->readFull = false;
    this->outfile = NULL;

    this->stopping = false;
    this->stopped = false;

    memset (&sfinfo, 0, sizeof (sfinfo)) ;

    /* Setup output file */
    sfinfo.channels = chanCount;
    sfinfo.samplerate = sampRate;

    sfinfo.format = SF_FORMAT_WAV | bitRes;

    if ((outfile = sf_open (filename, SFM_WRITE, &sfinfo)) == NULL)
    {  
         log ("Unable to open output file %s.\n", filename) ;
         puts (sf_strerror (NULL)) ;
    };

    log("# Writing to file %s.\n", filename) ;
    log("# Channels %d, Sample rate %d\n", sfinfo.channels, sfinfo.samplerate) ;

    this->filechannels = sfinfo.channels; 
}

size_t WrFileBuffer::getBufferSize(void) { return bufSize; }

void WrFileBuffer::setStopping(void)
{
    std::unique_lock<std::mutex> l(lock);
    stopping = true;
}

void WrFileBuffer::setStopped(void)
{
    std::unique_lock<std::mutex> l(lock);
    stopped = true;
}

bool WrFileBuffer::isStopping(void)
{
    std::unique_lock<std::mutex> l(lock);
    return stopping;
}

bool WrFileBuffer::isStopped(void)
{
    std::unique_lock<std::mutex> l(lock);
    return stopped;
}
