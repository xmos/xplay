
#include "plugin.h"
#include "stdio.h"

class plugin : public plugin_ 
{
     public:
   
        virtual int HandleSampleBuffer(int framesPerBuffer, int* in, int* out) const 
        {
            for(int i = 0; i <  framesPerBuffer; i++)
                for(int j = 0; j< GetNumChansOut(); j++)
                {
                    *out++ = *in++;
                }

            return 0;
        }
        
        virtual int Init() const 
        {
            printf("Plugin Init: In chans: %d, Out chans: %d, Samplerate: %d\n", 
                    GetNumChansIn(), GetNumChansOut(), GetSampleRate());

            if(GetNumChansIn() != GetNumChansOut())
            {
                printf("Simple monitor plugin cannot handle case of different channel counts!\n");
            }
            return 0;
        }
};


/*  Class factories */
extern "C" 
{
    plugin_* create() 
    {
        return new plugin();
    }

    void destroy(plugin_ *p) 
    {
        delete p;
    }
}
