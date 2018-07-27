
#ifndef __WRFILE_H__
#define __WRFILE_H__

#include "stdio.h"
#include "portaudio.h"
#include "sndfile.h"
#include <thread>
#include <mutex>
#include <condition_variable>

/* Note, Wr and Rd could extend a FileBuffer Class */

class WrFileBuffer 
{
    public:
        WrFileBuffer(size_t bufSize, char * filename, unsigned chanCount, unsigned sampRate);
        ~WrFileBuffer(void);
        size_t getBufferSize();
        int *getReadBuffer(void);
        int *swapWriteBuffers(void);
        void setStopping(void);
        void setStopped(void);
        bool isStopping(void);
        bool isStopped(void);
        SNDFILE *outfile;            // TODO make private
        int *writeBuffer;            // "
        int *readBuffer;             // "
        void signalFileWriterInitialized(void);
        int *getInitialWriteBuffer(void);

    private:
        bool fileWriterInitialized; 
        bool readFull, writeFull;
        bool stopping, stopped;
        size_t bufSize;
        char * filename;
        SF_INFO sfinfo;     // libsndfile
        int filechannels;
        std::condition_variable cvDoSwap, cvFileWriterInit; 
        std::mutex lock;
};

#endif
