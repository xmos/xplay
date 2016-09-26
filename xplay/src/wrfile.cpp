
/* xPlay: FileBuffer */

#include "wrfile.h"
#include <iostream>
#include <cstring>
#include "logging.h"

/* Gets called when buffer has been played via portaudio */
int *WrFileBuffer::swapFillBuffers(void) 
{
    std::unique_lock<std::mutex> l(lock);

    /* Check that buffer has been written to the file */
    if (readFull) 
    {
        std::cerr << "ERROR: Host file write is too slow for intput" << std::endl;
    }

    /* Perform the buffer swap */ 
    int *tmp;
    tmp = writeBuffer;
    writeBuffer = readBuffer;
    readBuffer = tmp;
    readFull = true;
    writeFull = false;
    cvDoSwap.notify_one();
    return writeBuffer;
}

WrFileBuffer::~WrFileBuffer(void)
{
    delete writeBuffer;
    delete readBuffer;
    sf_close(outfile) ;
}

#if 0
int *FileBuffer::getWriteBuffer(void)
{   
    std::unique_lock<std::mutex> l(lock);
    writeFull = true;
   
    /* Wait until our write buffer has been taken */
    cvDoSwap.wait(l, [this](){return (this->writeFull == false);});
    return writeBuffer;
}

int *FileBuffer::getInitialReadBuffer(void)
{
    std::unique_lock<std::mutex> l(lock);
    cvFileBufferInit.wait(l, [this](){return this->fileReaderInitialized;});
    return readBuffer;
}

void FileBuffer::signalFileReaderInitialized(void)
{
  std::unique_lock<std::mutex> l(lock);
  fileReaderInitialized = true;
  cvFileBufferInit.notify_one();
}
#endif

WrFileBuffer::WrFileBuffer(size_t bufSize, char * filename, unsigned chanCount, unsigned sampRate)
{
    writeBuffer = new int[bufSize];
    readBuffer = new int[bufSize];
    this->bufSize = bufSize;
    this->filename = filename;
    this->writeFull = false;
    this->readFull = false;
    this->outfile = NULL;

    printf("creating new WrFileBuffer");
    memset (&sfinfo, 0, sizeof (sfinfo)) ;

    /* Setup output file */
    sfinfo.channels = chanCount;
    sfinfo.samplerate = sampRate;

    /* TODO ideally format is specified */
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

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
