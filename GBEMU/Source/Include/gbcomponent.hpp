#ifndef GB_COMPONENT_HPP
#define GB_COMPONENT_HPP

#include <SDL3/SDL.h>

class GBEMU;

class GBComponent {
public:
    explicit GBComponent(GBEMU* emu);
    virtual ~GBComponent() = default;
    
    // Power management functions
    virtual void powerOn() {}
    virtual void powerOff() {}
    virtual void reset() {}
    
    // Main execution function
    virtual void step() {}

protected:
    GBEMU* emu;
};

#endif // GB_COMPONENT_HPP