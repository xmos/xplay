
#ifndef __OUTCHAN_H__
#define __OUTCHAN_H__

/* xPlay : OutputChan */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class OutputChan
{
    public:
        OutputChan(int x);
        virtual ~OutputChan() {};
        virtual int getNextSample(void) = 0;
        int getChanCount(){return chanCount;};

    private:
        unsigned chanCount;
};


class FileOutputChan : public OutputChan 
{
    public:
        FileOutputChan(char * filename, int x);
        ~FileOutputChan();
        int getNextSample(void);

    private:
        FileBuffer *fileBuffer;
        std::thread *fileThread;
        unsigned count;
        unsigned bufSize;
        int *buf;
            
};

class SineOutputChan : public OutputChan 
{
    public:
        SineOutputChan(unsigned sampleRate, unsigned freq, int x);
        int getNextSample(void);
    private:
        unsigned sampleRate;
        unsigned freq;
        int count;
        int period;
        int initialDelayCount;
        int *table;
};

#endif


