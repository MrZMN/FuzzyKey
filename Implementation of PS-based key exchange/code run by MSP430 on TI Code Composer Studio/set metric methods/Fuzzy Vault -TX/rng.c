/* --COPYRIGHT--,FRAM-Utilities
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * This source code is part of FRAM Utilities for MSP430 FRAM Microcontrollers.
 * Visit http://www.ti.com/tool/msp-fram-utilities for software information and
 * download.
 * --/COPYRIGHT--*/
#include <msp430.h>

#include <stdint.h>
#include <stdbool.h>

#include "rng.h"

#if (!defined(__MSP430FR5XX_6XX_FAMILY__) || !defined(__MSP430_HAS_AES256__))
#error "RNG only supported by MSP430FR5xx and MSP430FR6xx devices with AES256."
#endif

//******************************************************************************
//
// Helper definitions
//
//******************************************************************************
#define NULL                            0
#define NUM_ELEMENTS(x)                 (sizeof(x)/sizeof(x[0]))

//******************************************************************************
//
// CTR-DRBG definitions
//
//******************************************************************************
#define CTR_DRBG_KEYLEN                 RNG_KEYLEN
#define CTR_DRBG_NONCELEN               8
#define CTR_DRBG_SEEDLEN                (CTR_DRBG_KEYLEN + CTR_DRBG_KEYLEN)
#define CTR_DRBG_SEED_OFFSET            0x28
#define CTR_DRBG_NONCE_OFFSET           0x02
#define CTR_DRBG_SEED_ADDRESS           ((uint8_t *)(TLV_START + CTR_DRBG_SEED_OFFSET))
#define CTR_DRBG_NONCE_ADDRESS          ((uint8_t *)(TLV_START + CTR_DRBG_NONCE_OFFSET))
#define CTR_DRBG_INST_FLAG              0xAA

//******************************************************************************
//
// CTR-DRBG generator working state
//
//******************************************************************************
typedef struct{
    uint8_t key[CTR_DRBG_KEYLEN];
    uint8_t v[CTR_DRBG_KEYLEN];
    uint8_t instantiatedFlag;
} ctr_drbg_state_t;

#if defined (__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(ctr_drbg_state, ".infoD")
#elif defined (__IAR_SYSTEMS_ICC__)
#pragma location="INFOD"
__no_init
#endif
ctr_drbg_state_t ctr_drbg_state;

//******************************************************************************
//
// Helper function to check that the MPU settings for info memory are read and
// write.
//
//******************************************************************************
static bool infoIsRW(void)
{
    // Check MPU setting for INFO memory is read and write
    if ((MPUSAM & (MPUSEGIWE + MPUSEGIRE)) == (MPUSEGIWE + MPUSEGIRE)) {
        return true;
    }
    else {
        return false;
    }
}

//******************************************************************************
//
// Helper function to increment elements of v by one with carry while ensuring
// a constant number of cycles (i.e. regardless of the number of carries in the
// operation).
//
//******************************************************************************
static void incrementByOne(uint8_t *v, uint16_t n)
{
    int16_t i;
    uint16_t c;
    uint16_t x;

    // Increment elements of v by one with carry.
    for (i = n-1, c = 1; i >= 0; i--) {
        x = (uint16_t)v[i] + c;
        v[i] = (uint8_t)(x & 0xff);
        c = x >> 8;
    }
}

//******************************************************************************
//
// Helper function for setting the 128-bit cipher key of the AES256
// accelerator.
//
//******************************************************************************
static void setCipherKey(const uint8_t *cipherKey)
{
    uint16_t i;
    uint16_t sCipherKey;

    AESACTL0 &= (~(AESKL_1 + AESKL_2));

    // Set to AES-128 Encryption
    AESACTL0 |= AESKL__128;

    for(i = 0; i < CTR_DRBG_KEYLEN; i = i + 2) {
        sCipherKey = (uint16_t)(cipherKey[i]);
        sCipherKey = sCipherKey | ((uint16_t)(cipherKey[i + 1]) << 8);
        AESAKEY = sCipherKey;
    }

    // Wait until key is written
    while(0x00 == (AESASTAT & AESKEYWR));

    // Clear the data allocated on the stack
    sCipherKey = 0;
}

//******************************************************************************
//
// Helper function for encrypting data using the AES256 accelerator.
//
//******************************************************************************
static void encryptData(const uint8_t *data, uint8_t *encryptedData)
{
    uint16_t i;
    uint16_t tempData = 0;
    uint16_t tempVariable = 0;

    // Set module to encrypt mode
    AESACTL0 &= ~AESOP_3;

    // Write data to encrypt to module
    for(i = 0; i < 16; i = i + 2) {
        tempVariable = (uint16_t)(data[i]);
        tempVariable = tempVariable | ((uint16_t)(data[i + 1]) << 8);
        AESADIN = tempVariable;
    }

    // Key that is already written shall be used
    AESASTAT |= AESKEYWR;

    // Wait unit finished ~167 MCLK
    while(AESBUSY == (AESASTAT & AESBUSY));

    // Write encrypted data back to variable
    for(i = 0; i < 16; i = i + 2) {
        tempData = AESADOUT;
        *(encryptedData + i) = (uint8_t)tempData;
        *(encryptedData + i + 1) = (uint8_t)(tempData >> 8);
    }

    // Clear the data allocated on the stack
    tempData = 0;
    tempVariable = 0;
}

//******************************************************************************
//
// Updates the CTR-DRBG internal state using AES-128 block algorithm based on
// section 10.2.1.2 of NIST SP 800-90Ar1
//
// This function updates the internal states v and key for the CTR-DRBG.
// If not being used for instantiation, provided_data should be zero.
//
// Note: This function does not support a personalization string, reseed,
// or additional input.
//
//******************************************************************************
static void ctr_drbg_update(const uint8_t *providedData)
{
    uint16_t i;
    uint8_t temp[CTR_DRBG_SEEDLEN];

    // Zero initialize temp array
    for(i = 0; i < NUM_ELEMENTS(temp); i++) {
        temp[i] = 0;
    }

    // Load the cipher key
    setCipherKey(ctr_drbg_state.key);

    // Build new Key and V with AES-128 Encryption
    for (i = 0; i < 2; i++) {
        // Increment V by one
        incrementByOne(ctr_drbg_state.v, CTR_DRBG_KEYLEN);

        // Encrypt V and store in temp encryptedData
        encryptData(ctr_drbg_state.v, &temp[i*CTR_DRBG_KEYLEN]);
    }

    // If not NULL pointer, XOR encryptedData with the entropy input(providedData)
    if (providedData != NULL) {
        for (i = 0; i < CTR_DRBG_SEEDLEN; i++) {
            temp[i] ^= providedData[i];
        }
    }

    // Break temp apart into Key and V
    for (i = 0; i < CTR_DRBG_KEYLEN; i++) {
        ctr_drbg_state.key[i] = temp[i];
        ctr_drbg_state.v[i] = temp[i + CTR_DRBG_KEYLEN];
    }

    // Clear the data allocated on the stack
    for(i = 0; i < NUM_ELEMENTS(temp); i++) {
        temp[i] = 0;
    }
}

//******************************************************************************
//
// Helper function used by the derivation_function as described in section
// 10.3.3 of NIST SP 800-90Ar1.
//
//******************************************************************************
static void ctr_drbg_bcc(const uint8_t *key, const uint8_t *data, uint8_t *outputBlock)
{
    uint16_t i;
    uint16_t j;
    uint8_t temp[CTR_DRBG_KEYLEN];

    // Load the cipher key
    setCipherKey(key);

    // Loop (byteLen(data)/KEYLEN) = 4
    for(i = 0; i < 4; i++) {
        // Construct the inputBlock from data and the outputBlock
        for(j = 0; j < CTR_DRBG_KEYLEN; j++) {
            temp[j] = (outputBlock[j] ^ data[i*CTR_DRBG_KEYLEN + j]);
        }

        // Encrypt the inputBlock and store in outputBlock
        encryptData(temp, outputBlock);
    }

    // Clear the data allocated on the stack
    for(i = 0; i < NUM_ELEMENTS(temp); i++) {
        temp[i] = 0;
    }
}

//******************************************************************************
//
// Used to distribute entropy throughout a 256-bit bit-string according to
// section 10.3.2 of NIST SP 800-90Ar1.
//
// This function is used during the instantiation process to distribute the
// 128-bit true random seed across 256-bits while taking advantage of the AES256
// accelerator in hardware.
//
//******************************************************************************
static void ctr_drbg_derivationFunction(uint8_t *seedMaterial)
{
    uint16_t i;
    uint16_t k;
    uint8_t *x;
    uint8_t inputLen;
    uint8_t *encryptedData;
    uint8_t key[CTR_DRBG_KEYLEN];
    uint8_t data[CTR_DRBG_SEEDLEN + CTR_DRBG_SEEDLEN];

    // Initialize local variables
    x = data;
    inputLen = 24;
    encryptedData = seedMaterial;

    // Zero initialize data
    for(i = 0; i < NUM_ELEMENTS(data); i++) {
        data[i] = 0;
    }

    // Initialize key as defined by NIST
    for (i = 0; i < NUM_ELEMENTS(key); i++) {
        key[i] = i;
    }

    /*
     * Construct data according to section 10.3.2 of NIST SP 800-90Ar1:
     *
     *           0x00(padding)    0x80     seed_material[24]       N[4]            L[4]           IV[16]
     * data[64] = {63 --> 49}  +  {48}   +   {47 --> 24}    +   {23 --> 20}  +  {19 --> 16}  +  {15 --> 0}
     */
    data[19] = 24;
    data[23] = 32;

    // Copy the seed_material into the data[47]-->data[24]
    for(i = 0; i < inputLen; i++) {
        data[i + CTR_DRBG_KEYLEN + CTR_DRBG_NONCELEN] = seedMaterial[i];
    }

    // End with 0x80 and 0x00 padding
    data[48] = 0x80;

    // Clear the contents of encryptedData as required by bcc*/
    for(i = 0; i < CTR_DRBG_SEEDLEN; i++) {
        encryptedData[i] = 0;
    }

    // Loop (num_bytes_to_return/KEYLEN) = 2
    for(i = 0; i < 2; i++) {
        // Use the bcc helper function (updates encryptedData)
        ctr_drbg_bcc(key, data, &encryptedData[i*CTR_DRBG_KEYLEN]);
        data[3]++;
    }

    // Reconstruct key and x from the updated encryptedData
    for(i = 0; i < CTR_DRBG_KEYLEN; i++) {
        key[i] = encryptedData[i];
        x[i] = encryptedData[i + CTR_DRBG_KEYLEN];
    }

    // Load the cipher key
    setCipherKey(key);

    // Loop (num_bytes_to_return/KEYLEN bits) =  2
    for(i = 0; i < 2; i++) {
        // Encrypt x and store in encryptedData
        encryptData(x, &encryptedData[i*CTR_DRBG_KEYLEN]);

        // Update x with the encryptedData
        for(k = 0; k < CTR_DRBG_KEYLEN; k++)
        {
            x[k] = encryptedData[i*CTR_DRBG_KEYLEN + k];
        }
    }

    // Clear the data allocated on the stack
    for(i = 0; i < NUM_ELEMENTS(data); i++) {
        data[i] = 0;
    }
    for(i = 0; i < NUM_ELEMENTS(key); i++) {
        key[i] = 0;
    }
}

//******************************************************************************
//
// Instantiates the CTR-DRBG according to Section 9.1 and 10.2.1.3.1 of NIST SP
// 800-90Ar1.
//
// This function generates an initial working state for the CTR-DRBG and only
// needs to be called once during the CTR-DRBG's lifetime.
//
// Note: This function does not support a personalization string, does not
// support prediction resistance, and has a fixed security strength of 128-bits.
//
//******************************************************************************
static void ctr_drbg_instantiate(void)
{
    uint16_t i;
    uint8_t seedMaterial[CTR_DRBG_SEEDLEN];

    /*
     * Construct seed_material according to section 10.2.1.3.2 of NIST SP 800-90Ar1:
     *
     *                      nonce[8]      rand_seed[16]
     * seed_material[24] = {23 --> 16}  +  {15 --> 0}
     */
    for(i = 0; i < CTR_DRBG_KEYLEN; i++) {
        seedMaterial[i] = CTR_DRBG_SEED_ADDRESS[i];
    }
    for(i = 0; i < CTR_DRBG_NONCELEN; i++) {
        seedMaterial[i+CTR_DRBG_KEYLEN] = CTR_DRBG_NONCE_ADDRESS[i];
        seedMaterial[i+CTR_DRBG_KEYLEN+CTR_DRBG_NONCELEN] = 0;
    }

    // Use the derivation function to distribute 128-bit entropy through 256-bits
    ctr_drbg_derivationFunction(seedMaterial);

    // Initialize the working state to 0
    for (i = 0; i < CTR_DRBG_KEYLEN ; i++) {
        ctr_drbg_state.key[i] = 0;
        ctr_drbg_state.v[i] = 0;
    }

    // Update the working state with the provided entropy
    ctr_drbg_update(seedMaterial);

    // Signal that the working state has been instantiated
    ctr_drbg_state.instantiatedFlag = CTR_DRBG_INST_FLAG;

    // Clear the data allocated on the stack
    for(i = 0; i < CTR_DRBG_SEEDLEN; i++) {
        seedMaterial[i] = 0;
    }
}

//******************************************************************************
//
// Generates the requested number of random bytes using the AES-128 block cipher
// algorithm according to Section 9.3.1 and 10.2.1.5.1 of NIST SP 800-90Ar1.
//
// This function generates the requested number of random bytes using the
// CTR-DRBG methodology.
//
// Note: Reseed, prediction resistance and additional inputs are not supported.
// The security strength is fixed at 128-bit.
//
//******************************************************************************
static uint16_t ctr_drbg_generate(uint8_t *dst, uint16_t length)
{
    uint16_t i;
    uint16_t numEncLoops;

    // Check MPU write permissions of info section
    if (infoIsRW() == false) {
        // Clear input data and return length of zero
        for (i = 0; i < length; i++) {
            dst[i] = 0;
        }
        return 0;
    }

    // Check for instantiation and instantiate working state if needed
    if(ctr_drbg_state.instantiatedFlag != CTR_DRBG_INST_FLAG) {
        ctr_drbg_instantiate();
    }

    // Calculate the number of encryption passes necessary to fill destination
    numEncLoops = length/CTR_DRBG_KEYLEN;

    // Run the encryption routine to generate random bytes
    if (numEncLoops > 0) {
        // Load the cipher key
        setCipherKey(ctr_drbg_state.key);

        // Encrypt data and fill rand_bytes
        for (i = 0; i < numEncLoops; i++) {
            incrementByOne(ctr_drbg_state.v, CTR_DRBG_KEYLEN);
            encryptData(ctr_drbg_state.v, &dst[i*CTR_DRBG_KEYLEN]);
        }

        // Update the working state
        ctr_drbg_update(NULL);
    }

    return numEncLoops*CTR_DRBG_KEYLEN;
}

//******************************************************************************
//
// Generates random bytes using CTR-DRBG methodology.
//
//******************************************************************************
uint16_t rng_generateBytes(uint8_t *dst, uint16_t length)
{
    // Check for NULL pointer and return length of zero
    if (dst == NULL) {
        return 0;
    }

    // Generate random bytes and return length
    return ctr_drbg_generate(dst, length);
}
