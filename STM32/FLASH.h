#ifndef __FLASH_H
#define __FLASH_H

#ifdef STM32F103xB
#define FLASH_BLOCK_SIZE_KB 1
#else
#error MCU series undefined
#endif

#define FLASH_PAGE_ADDR(PageNum) (PageNum << 10 * FLASH_BLOCK_SIZE_KB) + FLASH_BASE

static inline void FLASH_Unlock(void) {
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
}

static inline void FLASH_Lock(void) {
    FLASH->CR |= FLASH_CR_LOCK;
}

static inline unsigned char FLASH_Program(const unsigned int addr, unsigned short val) {
    FLASH->AR = addr;
    FLASH->CR |= FLASH_CR_PG;

    while (FLASH->SR & FLASH_SR_BSY)
        ;

    *(unsigned short *)addr = val;

    while (FLASH->SR & FLASH_SR_BSY)
        ;

    FLASH->CR &= ~FLASH_CR_PG;

    if (*(unsigned int *)addr == val)
        return 0;

    return 1;
}

static inline unsigned char FLASH_ErasePage(unsigned int addr) {

    if ((addr - FLASH_BASE) >> 10 * FLASH_BLOCK_SIZE_KB >= *(unsigned short *)FLASHSIZE_BASE / FLASH_BLOCK_SIZE_KB)
        return 1;

    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = addr;
    FLASH->CR |= FLASH_CR_STRT;

    while (FLASH->SR & FLASH_SR_BSY)
        ;

    FLASH->CR &= ~FLASH_CR_PER;

    return 0;
}

static inline void FLASH_EraseMass(void) {
    FLASH->CR |= FLASH_CR_MER;
    FLASH->CR |= FLASH_CR_STRT;

    while (FLASH->SR & FLASH_SR_BSY)
        ;

    FLASH->CR &= ~FLASH_CR_MER;
}

#endif