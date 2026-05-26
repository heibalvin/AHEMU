/**
 * @file CH8DSK.hpp
 * @brief Declaration of the CH8DSK class, which emulates the disk system (ROM loader) of a CHIP-8 system.
 * @details This class handles loading ROM files into memory and provides access to the loaded ROM data.
 */

#pragma once
#include <SDL3/SDL.h>
#include "CH8COM.hpp"

/**
 * @brief The CH8DSK class emulates the disk system of a CHIP-8 system.
 *        It handles loading ROM files into memory and provides read-only access to the ROM data.
 *        The ROM is typically loaded at memory address 0x200 in the CHIP-8 memory space.
 */
class CH8DSK : public CH8COM {
private:
    /**
     * @brief Pointer to the allocated memory storing the ROM data.
     */
    Uint8* romStorage;
    /**
     * @brief Size of the loaded ROM in bytes.
     */
    size_t romStorageSize;

public:
    /**
     * @brief Constructs a CH8DSK instance.
     * @param parentEmu Pointer to the parent CHIP-8 emulator instance.
     */
    CH8DSK(CH8EMU* parentEmu);
    /**
     * @brief Destructor - frees the allocated ROM storage memory.
     */
    ~CH8DSK();

    /**
     * @brief Powers on the disk system (initializes to no ROM loaded).
     */
    void powerOn()  override;
    /**
     * @brief Powers off the disk system (frees any loaded ROM).
     */
    void powerOff() override;
    /**
     * @brief Resets the disk system to its initial powered-on state (no ROM loaded).
     */
    void reset()    override;

    /**
     * @brief Loads a ROM into memory.
     *        This function allocates memory and copies the provided ROM data.
     *        @param datas Pointer to the ROM data to load.
     *        @param size Size of the ROM data in bytes.
     *        @return True if the ROM was successfully loaded, false otherwise.
     */
    bool insertRom(const Uint8* datas, size_t size);

    /**
     * @brief Gets a pointer to the loaded ROM data.
     * @return Pointer to the ROM data, or nullptr if no ROM is loaded.
     */
    const Uint8* getRomData() const { return romStorage; }
    /**
     * @brief Gets the size of the loaded ROM in bytes.
     * @return Size of the ROM data, or 0 if no ROM is loaded.
     */
    size_t       getRomSize() const { return romStorageSize; }
    /**
     * @brief Checks if a ROM is currently loaded.
     * @return True if a ROM is loaded, false otherwise.
     */
    bool         hasRom()     const { return (romStorage != nullptr && romStorageSize > 0); }
};
