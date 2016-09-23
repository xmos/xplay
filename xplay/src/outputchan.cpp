#include "xplay.h"

#include "math.h"


OutputChan::OutputChan(int x) 
{ 
    this->chanCount = x;
}

void FileReader(FileBuffer &fileBuffer)
{
    int *buf = fileBuffer.writeBuffer;

    size_t bufSize = fileBuffer.getBufferSize();

    printf("FileReader:: using buffer size %d\n", (int) bufSize);

    /* Fill up our first buffer */
    /* TODO handle readcount < bufSize */
    int readcount = sf_read_int(fileBuffer.infile, buf, bufSize);

    /* Signal that we are ready to go! */
    fileBuffer.signalFileReaderInitialized();

    while (1)     
    {
        buf = fileBuffer.getWriteBuffer();
        
        /* Note, libsnd file will do the converestion for use e.g. from wav 16 */
        int readcount = sf_read_int(fileBuffer.infile, buf, bufSize);

        if(readcount == 0)
        {
            /* TODO handle EOF case */
            printf("EOF");
            while(1);
        }
    }
}


FileOutputChan::FileOutputChan(char *filename, int x) : OutputChan(x) 
{
    fileBuffer = new FileBuffer(OUT_BLOCK_SIZE, filename);
    fileThread = new std::thread(FileReader, std::ref(*this->fileBuffer) /* sampleRate, freq, chanId*/);

    /* Note, this will wait until FileReader thread is ready to go.. */
    buf = fileBuffer->getInitialReadBuffer();

    this->bufSize = OUT_BLOCK_SIZE;
    this->count= 0;
    
}

/****** FileOutputChan ********/

FileOutputChan::~FileOutputChan()
{
}

int FileOutputChan::getNextSample(void) 
{
    if(this->count == 0)
    {
        int *old = buf;
        buf = fileBuffer->swapFillBuffers();
        count = this->bufSize;
    }
    
    int sample = buf[bufSize-count];
    count--;

    return sample;
}

static unsigned gcd(unsigned u, unsigned v) 
{
    while ( v != 0) 
    {
        unsigned r = u % v;
        u = v;
        v = r;
    }
    return u;
}

SineOutputChan::SineOutputChan(unsigned sampleRate, unsigned freq, int x) : OutputChan(x) 
{
    unsigned d = gcd(sampleRate, freq);
    period = sampleRate/d * freq/d;
    table = new int[period];
    for (int i = 0; i < period; i++) 
    {
        float ratio = (double) sampleRate / (double) freq;
        float ii = i;
        float x = sinf(ii * 2 * M_PI / ratio);
        x = x * ldexp(2,27);
        table[i] = (int) x;
    }
    count = 0;
    this->sampleRate = sampleRate;
    this->freq = freq;
    initialDelayCount = 48000;
}

int SineOutputChan::getNextSample(void) 
{
    if (initialDelayCount) 
    {
        initialDelayCount--;
        return 0;
    }
  
    int sample = table[count];
    count++;
    if (count >= period)
    {
        count = 0;
    }
  
    return sample;
}


