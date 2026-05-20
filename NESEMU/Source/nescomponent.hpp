#ifndef NES_COMPONENT_HPP
#define NES_COMPONENT_HPP

#include <SDL3/SDL.h>

// Macros for mask manipulation
#define MASK_SET(var, mask)    ((var) |= (mask))
#define MASK_CLEAR(var, mask)  ((var) &= ~(mask))
#define MASK_TOGGLE(var, mask) ((var) ^= (mask))
#define MASK_CHECK_SET(var, mask) (((var) & (mask)) == (mask))
#define MASK_CHECK_CLEAR(var, mask) (((var) & (mask)) == 0)
#define MASK_ASSIGN(var, mask, condition) ((var) = ((var) & ~(mask)) | ((condition) ? (mask) : 0))

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

    // Central Hardware Memory Interface
    virtual Uint8 read(Uint16 address) { return 0x00; }
    virtual void write(Uint16 address, Uint8 value) {}

protected:
    NESEMU* emu;
};

#endif // NES_COMPONENT_HPP