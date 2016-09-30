
#ifndef __PLUGIN_H__
#define __PLUGIN_H__

class plugin_ 
{
    protected:
        unsigned sampleRate_;
        unsigned numChansIn_;
        unsigned numChansOut_;

    public:
        plugin_()
            : sampleRate_(0), numChansIn_(0), numChansOut_(0) {}

        virtual ~plugin_() {}

        void SetSampleRate(unsigned sampleRate) 
        {
            sampleRate_ = sampleRate;
        }

        unsigned GetSampleRate() const
        {
            return sampleRate_;
        }

        void SetNumChansOut(unsigned numChansOut) 
        {
            numChansOut_ = numChansOut;
        }

        unsigned GetNumChansOut() const
        {
            return numChansOut_;
        }
        
        void SetNumChansIn(unsigned numChansIn) 
        {
            numChansIn_ = numChansIn;
        }
        
        unsigned GetNumChansIn() const
        {
            return numChansIn_;
        }

        virtual int HandleSampleBuffer(int framesPerBuffer, int *in, int * out) const = 0;
        virtual int Init() const = 0;
};

/* The types of the class factories */
typedef plugin_* create_t();
typedef void destroy_t(plugin_*);





#endif 

