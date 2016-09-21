
#ifndef __XPLAY_H__
#define __XPLAY_H__

#include <sndfile.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "file.h"

typedef enum playmode{PLAYMODE_TONE, PLAYMODE_FILE, PLAYMODE_SILENCE} playmode_t;

class OutputChan
{
    public:
        OutputChan();
        virtual ~OutputChan() {};
        virtual int getNextSample(void) = 0;
};


class InputChan 
{
    public:
        InputChan();
        virtual ~InputChan() {};
        virtual void consumeSample(int sample) = 0;
};


class XPlay 
{
	public:
  		XPlay(unsigned sampleRate, OutputChan *oc);
  		~XPlay();
  		int run(unsigned delay_ms);
  		unsigned GetSampleRate();
        unsigned GetNumChansOut();
        unsigned GetNumChansIn();
  		OutputChan* outChans;   // TODO Should be private
  		InputChan* inChans;

	private:
  		unsigned sampleRate;
  		unsigned numIn;
  		unsigned numOut;
        unsigned playmode;
  		//bool loopback;        //TODO future option
  		//bool useWDM;          //TODO future option
};


/* OutChans */

class FileOutputChan : public OutputChan 
{
    public:
        FileOutputChan(char * filename);
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
        SineOutputChan(unsigned sampleRate, unsigned freq);
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




