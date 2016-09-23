
/* xPlay: FileBuffer */

#include "file.h"
#include <iostream>
#include <cstring>
#include "logging.h"

/* Gets called when buffer has been played via portaudio */
int *FileBuffer::swapFillBuffers(void) 
{
    std::unique_lock<std::mutex> l(lock);
    if (!writeFull) 
    {
        /* Error, the file read is too slow */
        std::cerr << "ERROR: host file read is too slow for output" << std::endl;
    }

    /* Perform the buffer swap */ 
    int *tmp;
    tmp = writeBuffer;
    writeBuffer = readBuffer;
    readBuffer = tmp;
    readFull = true;
    writeFull = false;
    cvDoSwap.notify_one();
    return readBuffer;
}

FileBuffer::~FileBuffer(void)
{
    delete writeBuffer;
    delete readBuffer;
    sf_close (infile) ;
}


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
    cvAnalyzerInit.wait(l, [this](){return this->fileReaderInitialized;});
    return readBuffer;
}

void FileBuffer::signalFileReaderInitialized(void)
{
  std::unique_lock<std::mutex> l(lock);
  fileReaderInitialized = true;
  cvAnalyzerInit.notify_one();
}

FileBuffer::FileBuffer(size_t bufSize, char * filename)
{
    writeBuffer = new int[bufSize];
    readBuffer = new int[bufSize];
    this->bufSize = bufSize;

    this->filename = filename;

    this->writeFull = false;
    this->readFull = false;

    infile = NULL;

    memset (&sfinfo, 0, sizeof (sfinfo)) ;

    if ((infile = sf_open (filename, SFM_READ, &sfinfo)) == NULL)
    {  
         log ("Unable to open input file %s.\n", filename) ;
         puts (sf_strerror (NULL)) ;
    };

    log("# Reading from file %s.\n", filename) ;
    log("# Channels %d, Sample rate %d\n", sfinfo.channels, sfinfo.samplerate) ;

    this->filechannels = sfinfo.channels; 
}


size_t FileBuffer::getBufferSize(void) { return bufSize; }
