#ifndef __CMT2300A_PARAMS_H
#define __CMT2300A_PARAMS_H

/* [CMT Bank] */
const u8 g_cmt2300aCmtBank[CMT2300A_CMT_BANK_SIZE] = {
    0x00,
    0x66,
    0xEC,
    0x1C,
    0xF0,
    0x80,
    0x14,
    0x08,
    0x91,
    0x02,
    0x02,
    0xD0,
};

/* [System Bank] */
const u8 g_cmt2300aSystemBank[CMT2300A_SYSTEM_BANK_SIZE] = {
    0xAE,
    0xE0,
    0x35,
    0x00,
    0x00,
    0xF4,
    0x10,
    0xE2,
    0x42,
    0x20,
    0x00,
    0x81,
};

/* [Frequency Bank] */
const u8 g_cmt2300aFrequencyBank[CMT2300A_FREQUENCY_BANK_SIZE] = {
    0x42,
    0xB3,
    0x8A,
    0x1A,
    0x42,
    0x9D,
    0xD8,
    0x19,
};

/* [Data Rate Bank] */
const u8 g_cmt2300aDataRateBank[CMT2300A_DATA_RATE_BANK_SIZE] = {
    0x2A,
    0x83,
    0x11,
    0x10,
    0xA2,
    0x13,
    0x0C,
    0x0A,
    0x9F,
    0x5A,
    0x29,
    0x29,
    0xC0,
    0xA5,
    0x02,
    0x53,
    0x18,
    0x00,
    0xB4,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
};

/* [Baseband Bank] */
const u8 g_cmt2300aBasebandBank[CMT2300A_BASEBAND_BANK_SIZE] = {
    0x12,   //#define CMT2300A_CUS_PKT1                0x38
    0x08,   //#define CMT2300A_CUS_PKT2                0x39
    0x00,   //#define CMT2300A_CUS_PKT3                0x3A
    0xAA,   //#define CMT2300A_CUS_PKT4                0x3B
    0x06,   //#define CMT2300A_CUS_PKT5                0x3C
    0x00,   //#define CMT2300A_CUS_PKT6                0x3D
    0x00,   //#define CMT2300A_CUS_PKT7                0x3E
    0x00,   //#define CMT2300A_CUS_PKT8                0x3F
    0x00,   //#define CMT2300A_CUS_PKT9                0x40
    'N',    //#define CMT2300A_CUS_PKT10               0x41
    'U',    //#define CMT2300A_CUS_PKT11
    'L',    //#define CMT2300A_CUS_PKT12
    'L',    //#define CMT2300A_CUS_PKT13
	
    0x01,   //#define CMT2300A_CUS_PKT14  
    0x1F,   //#define CMT2300A_CUS_PKT15  
    0x00,   //#define CMT2300A_CUS_PKT16  
    0x00,   //#define CMT2300A_CUS_PKT17  
    0x00,   //#define CMT2300A_CUS_PKT18  
    0x00,   //#define CMT2300A_CUS_PKT19  
    0x00,   //#define CMT2300A_CUS_PKT20  
    0x01,   //#define CMT2300A_CUS_PKT21  
    0x00,
    0x00,
    0x6C,
    0xFF,
    0x00,
    0x00,
    0x1F,
    0x10,
};

/* [Tx Bank] */
const u8 g_cmt2300aTxBank[CMT2300A_TX_BANK_SIZE] = {
    0x70,
    0x9A,
    0x0C,
    0x00,
    0x03,
    0x50,
    0x00,
    0x46,
    0x0D,
    0x3F,
    0xAF,
};

#endif
