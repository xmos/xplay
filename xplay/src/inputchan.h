
#ifndef __INPUTCHAN_H__
#define __INPUTCHAN_H__

#include "wrfile.h"

class InputChan 
{
    public:
        InputChan(int chanCount, int sampRate);
        virtual ~InputChan() {};
        virtual void consumeSample(int sample) = 0;
        int getChanCount(){return chanCount;};
        int getSampRate(){return sampRate;};
        virtual bool isDone(void) = 0;
        virtual void stop(void) = 0;
    
    private:
        unsigned chanCount;
        unsigned sampRate;
};


class FileInputChan : public InputChan 
{
    public:
        FileInputChan(char * filename, unsigned chanCount, unsigned sampRate, unsigned bitRes);
        ~FileInputChan();
        void consumeSample(int sample);
        unsigned GetBufSize(void) {return bufSize;};
        bool isDone(void);
        void stop(void);

    private:
        WrFileBuffer *wrFileBuffer;
        std::thread *wrFileThread;
        unsigned count;
        unsigned bufSize;
        int *buf;
};

#endif


