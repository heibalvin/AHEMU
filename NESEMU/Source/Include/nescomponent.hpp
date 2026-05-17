#ifndef NES_COMPONENT_HPP
#define NES_COMPONENT_HPP

#include <SDL3/SDL.h>

class NESEMU;

class NESComponent {
public:
    explicit NESComponent(NESEMU* emu);
    virtual ~NESComponent() = default;
    
    // Power management functions
    virtual void powerOn() {}
    virtual void reset() {}
    
    // Main execution function
    virtual void step() {}

protected:
    NESEMU* emu;
};

#endif // NES_COMPONENT_HPP