
#ifndef __INPUTCHAN_H__
#define __INPUTCHAN_H__

class InputChan 
{
    public:
        InputChan(int x);
        virtual ~InputChan() {};
        virtual void consumeSample(int sample) = 0;
        int getChanCount(){return chanCount;};
    
    private:
        unsigned chanCount;
};


class FileInputChan : public InputChan 
{
    public:
        FileInputChan(char * filename, int x);
        ~FileInputChan();
        void consumeSample(int sample);

    private:
        FileBuffer *fileBuffer;
        std::thread *fileThread;
        unsigned count;
        unsigned bufSize;
        int *buf;
};

#endif


