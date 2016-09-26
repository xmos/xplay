
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
    
    private:
        unsigned chanCount;
        unsigned sampRate;
};


class FileInputChan : public InputChan 
{
    public:
        FileInputChan(char * filename, int chanCount, int sampRate);
        ~FileInputChan();
        void consumeSample(int sample);

    private:
        WrFileBuffer *wrFileBuffer;
        std::thread *wrFileThread;
        unsigned count;
        unsigned bufSize;
        int *buf;
};

#endif


