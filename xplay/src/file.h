
#ifndef __FILE_H__
#define __FILE_H__

#include "stdio.h"
#include "portaudio.h"
#include "sndfile.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class FileBuffer 
{
    public:
        FileBuffer(size_t bufSize, char* filename);
        ~FileBuffer(void);
        size_t getBufferSize();
        int *getWriteBuffer(void);
        int *swapFillBuffers(void);
        SNDFILE *infile;            //TODO make private
        void signalFileReaderInitialized(void);
        int *getInitialReadBuffer(void);
        int *writeBuffer;
        int *readBuffer;

    private:
        bool analyzerReady, fileReaderInitialized; 
        bool readFull, writeFull;
        size_t bufSize;
        char * filename;
        SF_INFO sfinfo;     // libsndfile
        int filechannels;
        std::condition_variable cvDoSwap, cvAnalyzerInit; 
        std::mutex lock;
};

#endif
