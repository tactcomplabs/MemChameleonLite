/* _MCL_PACKET_C_
 *
 * MemChameleonLite Packet Handler Routines
 *
 * Copyright 2017, Tactical Computing Laboratories, LLC
 * contact@tactcomplabs.com
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "plic/plic_driver.h"
#include "platform.h"
#include "encoding.h"
#include "stdatomic.h"
#include "mcl_const.h"

/* ---------------------------------------------------- GLOBALS */
const mcl_packet __PACKET[] = {
  /*  NAME : OPC : DATA_LEN : RQST_LEN : RSP_LEN : RSP : CTR_IDX : PKT */
  { "WR16",        8,   16,  2,  1,    WR_RS,     MCL_WR16_COUNTER,   PKT_W },
  { "WR32",        9,   32,  3,  1,    WR_RS,     MCL_WR32_COUNTER,   PKT_W },
  { "WR48",       10,   48,  4,  1,    WR_RS,     MCL_WR48_COUNTER,   PKT_W },
  { "WR64",       11,   64,  5,  1,    WR_RS,     MCL_WR64_COUNTER,   PKT_W },
  { "WR80",       12,   80,  6,  1,    WR_RS,     MCL_WR80_COUNTER,   PKT_W },
  { "WR96",       13,   96,  7,  1,    WR_RS,     MCL_WR96_COUNTER,   PKT_W },
  { "WR112",      14,  112,  8,  1,    WR_RS,     MCL_WR112_COUNTER,  PKT_W },
  { "WR128",      15,  128,  9,  1,    WR_RS,     MCL_WR128_COUNTER,  PKT_W },
  { "WR256",      79,  256, 17,  1,    WR_RS,     MCL_WR256_COUNTER,  PKT_W },
  { "RD16",       48,   16,  1,  2,    RD_RS,     MCL_RD16_COUNTER,   PKT_R },
  { "RD32",       49,   32,  1,  3,    RD_RS,     MCL_RD32_COUNTER,   PKT_R },
  { "RD48",       50,   48,  1,  4,    RD_RS,     MCL_RD48_COUNTER,   PKT_R },
  { "RD64",       51,   64,  1,  5,    RD_RS,     MCL_RD64_COUNTER,   PKT_R },
  { "RD80",       52,   80,  1,  6,    RD_RS,     MCL_RD80_COUNTER,   PKT_R },
  { "RD96",       53,   96,  1,  7,    RD_RS,     MCL_RD96_COUNTER,   PKT_R },
  { "RD112",      54,  112,  1,  8,    RD_RS,     MCL_RD112_COUNTER,  PKT_R },
  { "RD128",      55,  128,  1,  9,    RD_RS,     MCL_RD128_COUNTER,  PKT_R },
  { "RD256",     119,  256,  1, 17,    RD_RS,     MCL_RD256_COUNTER,  PKT_R },
  { "PWR16",      24,   16,  2,  0,   NO_RSP,     MCL_PWR16_COUNTER,  PKT_W },
  { "PWR32",      25,   32,  3,  0,   NO_RSP,     MCL_PWR32_COUNTER,  PKT_W },
  { "PWR48",      26,   48,  4,  0,   NO_RSP,     MCL_PWR48_COUNTER,  PKT_W },
  { "PWR64",      27,   64,  5,  0,   NO_RSP,     MCL_PWR64_COUNTER,  PKT_W },
  { "PWR80",      28,   80,  6,  0,   NO_RSP,     MCL_PWR80_COUNTER,  PKT_W },
  { "PWR96",      29,   96,  7,  0,   NO_RSP,     MCL_PWR96_COUNTER,  PKT_W },
  { "PWR112",     30,  112,  8,  0,   NO_RSP,     MCL_PWR112_COUNTER, PKT_W },
  { "PWR128",     31,  128,  9,  0,   NO_RSP,     MCL_PWR128_COUNTER, PKT_W },
  { "PWR256",     95,  256, 17,  0,   NO_RSP,     MCL_PWR256_COUNTER, PKT_W },
  { "2ADD8",      18,   16,  2,  1,    WR_RS,     MCL_2ADD8_COUNTER,  PKT_A },
  { "ADD16",      19,   16,  2,  1,    WR_RS,     MCL_ADD16_COUNTER,  PKT_A },
  { "P2ADD8",     34,   16,  2,  0,   NO_RSP,     MCL_P2ADD8_COUNTER, PKT_A },
  { "PADD16",     35,   16,  2,  0,   NO_RSP,     MCL_PADD16_COUNTER, PKT_A },
  { "2ADDS8R",    82,   16,  2,  2,    RD_RS,     MCL_2ADDS8R_COUNTER,PKT_A },
  { "ADDS16R",    83,   16,  2,  2,    RD_RS,     MCL_ADDS16R_COUNTER,PKT_A },
  { "INC8",       80,    0,  1,  1,    WR_RS,     MCL_INC8_COUNTER,   PKT_A },
  { "PINC8",      84,    0,  1,  0,   NO_RSP,     MCL_PINC8_COUNTER,  PKT_A },
  { "XOR16",      64,   16,  2,  2,    RD_RS,     MCL_XOR16_COUNTER,  PKT_A },
  { "OR16",       65,   16,  2,  2,    RD_RS,     MCL_OR16_COUNTER,   PKT_A },
  { "NOR16",      66,   16,  2,  2,    RD_RS,     MCL_NOR16_COUNTER,  PKT_A },
  { "AND16",      67,   16,  2,  2,    RD_RS,     MCL_AND16_COUNTER,  PKT_A },
  { "NAND16",     68,   16,  2,  2,    RD_RS,     MCL_NAND16_COUNTER, PKT_A },
  { "CASGT8",     96,   16,  2,  2,    RD_RS,     MCL_CASGT8_COUNTER, PKT_A },
  { "CASGT16",    98,   16,  2,  2,    RD_RS,     MCL_CASGT16_COUNTER,PKT_A },
  { "CASLT8",     97,   16,  2,  2,    RD_RS,     MCL_CASLT8_COUNTER, PKT_A },
  { "CASLT16",    99,   16,  2,  2,    RD_RS,     MCL_CASLT16_COUNTER,PKT_A },
  { "CASEQ8",    100,   16,  2,  2,    RD_RS,     MCL_CASEQ8_COUNTER, PKT_A },
  { "CASZERO16", 101,   16,  2,  2,    RD_RS,     MCL_CASZ16_COUNTER, PKT_A },
  { "EQ8",       105,   16,  2,  1,    WR_RS,     MCL_EQ8_COUNTER,    PKT_A },
  { "EQ16",      104,   16,  2,  1,    WR_RS,     MCL_EQ16_COUNTER,   PKT_A },
  { "BWR",        17,   16,  2,  1,    WR_RS,     MCL_BWR_COUNTER,    PKT_A },
  { "PBWR",       33,   16,  2,  0,   NO_RSP,     MCL_PBWR_COUNTER,   PKT_A },
  { "BWR8R",      81,   16,  2,  2,    RD_RS,     MCL_BWR8R_COUNTER,  PKT_A },
  { "SWAP16",    106,   16,  2,  2,    RD_RS,     MCL_SWAP16_COUNTER, PKT_A },
  { "MD_WR",      16,   16,  2,  1, MD_WR_RS,     MCL_MDWR_COUNTER,   PKT_B },
  { "MD_RD",      40,    8,  1,  2, MD_RD_RS,     MCL_MDRD_COUNTER,   PKT_B },
  { NULL,          0,    0,  0,  0,   NO_RSP,     MCL_NULL_COUNTER,   PKT_NONE }
};

/* ---------------------------------------------------- PacketFlitsInBytes */
extern uint32_t PacketFlitsInBytes(uint8_t byte){
  int cur = 0;
  uint8_t cmd = (byte&0x7F);

  while( __PACKET[cur].name != NULL ){
    if( __PACKET[cur].opc == cmd ){
      return (uint32_t)(__PACKET[cur].rqst_len)*16;
    }
    cur++;
  }

  /* error */
  return (uint32_t)(0);
}

/* ---------------------------------------------------- ProcessModePacket */
/*
 * Process incoming MODE requests.
 * Packets must be defined in the __PACKET[]
 * table above as mode packets (PKT_M)
 *
 * There are currently 2 mode registers defined
 *
 * - MCL_CUB_REG = ReadOnly register holding the CUB field
 * - MCL_BLOCK_REG = ReadOnly register holding the max block size
 *
 * You can add additional registers by modifying the MCL_NUM_REG
 * constant in mcl_const.h and adding additional register index
 * macros.  Make sure to intiliaze their state in the reset_driver()
 * function of mcl_driver.c.
 *
 * Return values:
 * -1   : Error Occurred
 *  0   : No Error Occurred
 *
 */
static int ProcessModePacket( uint8_t cmd, uint64_t addr ){

  uint32_t reg = ((uint32_t)(addr)&0x3FFFFF);

  switch( cmd ){
  case 40:
    /* MD_RD */
    switch( reg ){
    case MCL_CUB_REG:
    case MCL_BLOCK_REG:
      __RAW[1] = __REG[reg];
      break;
    default:
      /* register doesn't exist */
      return -1;
      break;
    }
    break;
  case 16:
    /* MD_WR */
    /* error, no registers are enabled for R/W access */
  default:
    return -1;
    break;
  }
  return 0;
}

/* ---------------------------------------------------- ProcessAtomicPacket */
/*
 * Process incoming ATOMIC requests.
 * Packets must be defined in the __PACKET[]
 * table above as mode packets (PKT_A)
 *
 * Return values:
 * -1   : Error Occurred
 *  0   : No Error Occurred
 *
 */
static int ProcessAtomicPacket( uint8_t cmd, uint64_t addr, uint8_t *af ){

  uint32_t op32_1 = 0x00;
  uint32_t op32_2 = 0x00;
  uint64_t op64_1 = 0x00ull;
  uint64_t op64_2 = 0x00ull;
  uint64_t low    = 0x00ull;
  int64_t high    = 0x00ll;
  uint64_t mop    = 0x00ull;
  int i           = 0;

  switch( cmd ){
  case 18:
    /* 2ADD8 */
    *af = 0;
    op32_1 = (uint32_t)(__RAW[1] & 0xFFFFFFFF);
    op32_2 = (uint32_t)(__RAW[2] & 0xFFFFFFFF);

    /* -- get the first operand */
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    /* -- perform the first op */
    mop += (uint64_t)(op32_1);

    /* -- write the first operand back */
    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    /* -- get the second operand */
    mop = 0x00ull;
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i+8])<<(i*8));
      i++;
    }

    /* -- perform the second op */
    mop += (uint64_t)(op32_2);

    /* -- write the second operand back */
    i = 0;
    while( i < 8 ){
      __STOR[addr+i+8] = (uint8_t)( (mop>>(i*8)) & 0xFF);
      i++;
    }

    break;
  case 19:
    /* ADD16 */
    *af = 0;
    i = 0;
    while( i < 8 ){
      low |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      high |= ((int64_t)(__STOR[addr+i+8])<<(i*8));
      i++;
    }

    /* raw[1] == least significant 64 bits */
    low += __RAW[1];

    /* raw[2] == most significant 64 bits */
    high += (int64_t)(__RAW[2]);

    if( low < __RAW[1] ){
      high++;
    }

    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)((low>>(i*8)) & 0xFF);
      __STOR[addr+i+8] = (uint8_t)(((uint64_t)(high)>>(i*8)) & 0xFF);
      i++;
    }

    break;
  case 34:
    /* P2ADD8 */
    *af = 0;
    op32_1 = (uint32_t)(__RAW[1] & 0xFFFFFFFF);
    op32_2 = (uint32_t)(__RAW[2] & 0xFFFFFFFF);

    /* -- get the first operand */
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    /* -- perform the first op */
    mop += (uint64_t)(op32_1);

    /* -- write the first operand back */
    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    /* -- get the second operand */
    mop = 0x00ull;
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i+8])<<(i*8));
      i++;
    }

    /* -- perform the second op */
    mop += (uint64_t)(op32_2);

    /* -- write the second operand back */
    i = 0;
    while( i < 8 ){
      __STOR[addr+i+8] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    break;
  case 35:
    /* PADD16 */
    *af = 0;
    i = 0;
    while( i < 8 ){
      low |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      high |= ((int64_t)(__STOR[addr+i+8])<<(i*8));
      i++;
    }

    /* raw[1] == least significant 64 bits */
    low += __RAW[1];

    /* raw[2] == most significant 64 bits */
    high += (int64_t)(__RAW[2]);

    if( low < __RAW[1] ){
      high++;
    }

    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)((low>>(i*8)) & 0xFF);
      __STOR[addr+i+8] = (uint8_t)(((uint64_t)(high)>>(i*8)) & 0xFF);
      i++;
    }

    break;
  case 82:
    /* 2ADDS8R */
    *af = 0;
    op64_1 = __RAW[1];
    op64_2 = __RAW[2];

    /* -- get the first operand */
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    /* -- perform the first op */
    __RAW[1] = mop;
    mop += op64_1;

    /* -- write the first operand back */
    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    /* -- get the second operand */
    mop = 0x00ull;
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i+8])<<(i*8));
      i++;
    }

    /* -- perform the second op */
    __RAW[2] = mop;
    mop += op64_2;

    /* -- write the second operand back */
    i = 0;
    while( i < 8 ){
      __STOR[addr+i+8] = (uint8_t)( (mop>>(i*8)) & 0xFF);
      i++;
    }

    break;
  case 83:
    /* ADDS16R */
    *af = 0;
    i = 0;
    while( i < 8 ){
      low |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      high |= ((int64_t)(__STOR[addr+i+8])<<(i*8));
      i++;
    }

    /* raw[1] == least significant 64 bits */
    op64_1 = low;
    low += __RAW[1];
    __RAW[1] = op64_1;

    /* raw[2] == most significant 64 bits */
    op64_1 = (uint64_t)(high);
    high += (int64_t)(__RAW[2]);
    __RAW[2] = op64_1;

    if( low < __RAW[1] ){
      high++;
    }

    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)((low>>(i*8)) & 0xFF);
      __STOR[addr+i+8] = (uint8_t)(((uint64_t)(high)>>(i*8)) & 0xFF);
      i++;
    }

    break;
  case 80:
    /* INC8 */
    *af = 0;
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    mop++;

    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    break;
  case 84:
    /* PINC8 */
    *af = 0;
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    mop++;

    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    break;
  case 64:
    /* XOR16 */
    *af = 0;
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    op64_1 = mop;
    mop ^= __RAW[1];
    __RAW[1] = op64_1;
    if( mop != __RAW[1] ){
      *af = 1;
    }

    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i+8])<<(i*8));
      i++;
    }

    op64_1 = mop;
    mop ^= __RAW[2];
    __RAW[2] = op64_1;
    if( mop != __RAW[2] ){
      *af = 1;
    }


    i = 0;
    while( i < 8 ){
      __STOR[addr+i+8] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    break;
  case 65:
    /* OR16 */
    *af = 0;
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    op64_1 = mop;
    mop |= __RAW[1];
    __RAW[1] = op64_1;
    if( mop != __RAW[1] ){
      *af = 1;
    }


    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i+8])<<(i*8));
      i++;
    }

    op64_1 = mop;
    mop |= __RAW[2];
    __RAW[2] = op64_1;
    if( mop != __RAW[2] ){
      *af = 1;
    }


    i = 0;
    while( i < 8 ){
      __STOR[addr+i+8] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    break;
  case 66:
    /* NOR16 */
    *af = 0;
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    op64_1 = mop;
    mop = ~(__RAW[1]|mop);
    __RAW[1] = op64_1;
    if( mop != __RAW[1] ){
      *af = 1;
    }


    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i+8])<<(i*8));
      i++;
    }

    op64_1 = mop;
    mop = ~(__RAW[2]|mop);
    __RAW[2] = op64_1;
    if( mop != __RAW[2] ){
      *af = 1;
    }


    i = 0;
    while( i < 8 ){
      __STOR[addr+i+8] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    break;
  case 67:
    /* AND16 */
    *af = 0;
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    op64_1 = mop;
    mop &= __RAW[1];
    __RAW[1] = op64_1;
    if( mop != __RAW[1] ){
      *af = 1;
    }


    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i+8])<<(i*8));
      i++;
    }

    op64_1 = mop;
    mop &= __RAW[2];
    __RAW[2] = op64_1;
    if( mop != __RAW[2] ){
      *af = 1;
    }


    i = 0;
    while( i < 8 ){
      __STOR[addr+i+8] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    break;
  case 68:
    /* NAND16 */
    *af = 0;
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    op64_1 = mop;
    mop = ~(__RAW[1]&mop);
    __RAW[1] = op64_1;
    if( mop != __RAW[1] ){
      *af = 1;
    }


    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i+8])<<(i*8));
      i++;
    }

    op64_1 = mop;
    mop = ~(__RAW[2]&mop);
    __RAW[2] = op64_1;
    if( mop != __RAW[2] ){
      *af = 1;
    }


    i = 0;
    while( i < 8 ){
      __STOR[addr+i+8] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    break;
  case 96:
    /* CASGT8 */
    *af = 0;
    op64_1 = __RAW[1];
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    __RAW[1] = mop;
    if( op64_1 > mop ){
      *af = 1;
      mop = op64_1;

      i = 0;
      while( i < 8 ){
        __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
        i++;
      }
    }

    break;
  case 98:
    /* CASGT16 */
    *af = 0;
    op64_1 = __RAW[2];
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i+8])<<(i*8));
      i++;
    }
    while( i < 8 ){
      op64_2 |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    __RAW[1] = mop;
    if( op64_1 > mop ){
      *af = 1;
      mop = op64_1;

      i = 0;
      while( i < 8 ){
        __STOR[addr+i+8] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
        i++;
      }

      mop = __RAW[2];
      __RAW[2] = op64_2;
      i = 0;
      while( i < 8 ){
        __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
        i++;
      }
    }else if( (op64_1 == mop) && (__RAW[2] > op64_2) ){
      *af = 1;
      mop = op64_1;
      __RAW[2] = op64_2;

      i = 0;
      while( i < 8 ){
        __STOR[addr+i+8] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
        i++;
      }

      mop = __RAW[2];
      __RAW[2] = op64_2;
      i = 0;
      while( i < 8 ){
        __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
        i++;
      }
    }

    break;
  case 97:
    /* CASLT8 */
    *af = 0;
    op64_1 = __RAW[1];
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    __RAW[1] = mop;
    if( op64_1 < mop ){
      *af = 1;
      mop = op64_1;

      i = 0;
      while( i < 8 ){
        __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
        i++;
      }
    }

    break;
  case 99:
    /* CASLT16 */
    *af = 0;
    op64_1 = __RAW[2];
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i+8])<<(i*8));
      i++;
    }
    while( i < 8 ){
      op64_2 |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    __RAW[1] = mop;
    if( op64_1 < mop ){
      *af = 1;
      mop = op64_1;

      i = 0;
      while( i < 8 ){
        __STOR[addr+i+8] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
        i++;
      }

      mop = __RAW[2];
      __RAW[2] = op64_2;
      i = 0;
      while( i < 8 ){
        __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
        i++;
      }
    }else if( (op64_1 == mop) && (__RAW[2] < op64_2) ){
      *af = 1;
      mop = op64_1;
      __RAW[2] = op64_2;

      i = 0;
      while( i < 8 ){
        __STOR[addr+i+8] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
        i++;
      }

      mop = __RAW[2];
      __RAW[2] = op64_2;
      i = 0;
      while( i < 8 ){
        __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
        i++;
      }
    }

    break;
  case 100:
    /* CASEQ8 */
    *af = 0;
    op64_1 = __RAW[1];
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    __RAW[1] = mop;
    __RAW[2] = 0x00ull;
    if( op64_1 == mop ){
      *af = 1;
    }

    break;
  case 101:
    /* CASZERO16 */
    *af = 0;
    i = 0;
    while( i < 8 ){
      op64_1 |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      op64_2 |= ((uint64_t)(__STOR[addr+i+8])<<(i*8));
      i++;
    }

    if( (op64_1 == 0x00ull) && (op64_2 == 0x00ull) ){
      *af = 1;
      mop = op64_1;
      i=0;
      while( i < 8 ){
        __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      }
      __RAW[1] = op64_1;

      mop = op64_2;
      i=0;
      while( i < 8 ){
        __STOR[addr+i+8] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      }
      __RAW[2] = op64_2;
    }

    break;
  case 105:
    /* EQ8 */
    *af = 0;
    op64_1 = __RAW[1];
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    if( op64_1 == mop ){
      *af = 1;
    }

    break;
  case 104:
    /* EQ16 */
    *af = 0;
    op64_1 = __RAW[1];
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    if( op64_1 == mop ){
      op64_2 = __RAW[2];
      i = 0;
      while( i < 8 ){
        mop |= ((uint64_t)(__STOR[addr+i+8])<<(i*8));
        i++;
      }
      if( op64_2 == mop ){
        *af = 1;
      }
    }

    break;
  case 17:
    /* BWR */
    *af = 0;
    op64_1 = __RAW[1];  /* write data */
    op64_2 = (~__RAW[2]);  /* inverted bit mask */
    op64_1 = (op64_1 | op64_2); /* this is our data to write */

    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    mop|=op64_1;

    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    break;
  case 33:
    /* PBWR */
    *af = 0;
    op64_1 = __RAW[1];  /* write data */
    op64_2 = (~__RAW[2]);  /* inverted bit mask */
    op64_1 = (op64_1 | op64_2); /* this is our data to write */

    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    mop|=op64_1;

    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    break;
  case 81:
    /* BWR8R */
    *af = 0;
    op64_1 = __RAW[1];  /* write data */
    op64_2 = (~__RAW[2]);  /* inverted bit mask */
    op64_1 = (op64_1 | op64_2); /* this is our data to write */

    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    op64_2 = mop; /* save the old data */
    mop|=op64_1;

    __RAW[1] = op64_2;
    __RAW[2] =- 0x00ull;

    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    break;
  case 106:
    /* SWAP16 */
    *af = 0;
    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i])<<(i*8));
      i++;
    }

    op64_1 = mop;
    mop = __RAW[1];
    __RAW[1] = op64_1;

    i = 0;
    while( i < 8 ){
      __STOR[addr+i] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    i = 0;
    while( i < 8 ){
      mop |= ((uint64_t)(__STOR[addr+i+8])<<(i*8));
      i++;
    }

    op64_1 = mop;
    mop = __RAW[2];
    __RAW[2] = op64_1;

    i = 0;
    while( i < 8 ){
      __STOR[addr+i+8] = (uint8_t)( (mop>>(i*8)) & 0x1FF);
      i++;
    }

    break;
  default:
    return -1;
    break;
  }
  return 0;
}

/* ---------------------------------------------------- ProcessPacket */
/*
 * Process in incoming packet requests.
 * Packets must be defined in the __PACKET[]
 * table above.
 *
 * Return values:
 * -1   : Error Occurred
 *  0   : No response is required, but packet was successful
 *  > 0 : Success and we need to send a response
 *
 */
extern int ProcessPacket(){
  int cur     = 0;
  int i       = 0;
  int j       = 0;
  int shift   = 0;
  uint8_t af  = 0;    /* atomic flag */
  uint8_t cmd = (uint8_t)(__RAW[0] & 0x7F);
  uint64_t addr = 0x00ull;

  while( __PACKET[cur].name != NULL ){
    if( __PACKET[cur].opc == cmd ){

      /* decode the address */
      addr = ((__RAW[0]>>24)&0x3FFFFFFFF);
      if( (addr > MCL_SIZE) ||
          (addr + (uint64_t)(__PACKET[cur].data_len)) ){
        /* address is out of range */
        return -1;
      }

      /* found a valid request : decode the type */
      switch( __PACKET[cur].pkt ){
      case PKT_R:
        /* read packet */
        i = 0;
        j = 1;
        shift = 0;
        while( i < __PACKET[cur].data_len ){
          __RAW[j] |= ((uint64_t)(__STOR[addr+i])<<shift);
          i++;
          shift+=8;
          if( shift == 64 ){
            shift = 0;
            j++;
          }
        }
        break;
      case PKT_W:
        /* write packet */
        i=0;
        j=1;
        shift = 0;
        while( i < __PACKET[cur].data_len ){
          __STOR[addr+i] = (uint8_t)((__RAW[j]>>shift)&0xFF);
          i++;
          shift+=8;
          if( shift == 64 ){
            shift = 0;
            j++;
          }
        }
        break;
      case PKT_A:
        /* atomic packet */
        if( ProcessAtomicPacket(cmd,addr,&af) != 0 ){
          return -1;
        }
        break;
      case PKT_B:
        /* mode packet */
        if( ProcessModePacket(cmd,addr) != 0 ){
          return -1;
        }
        break;
      case PKT_NONE:
      default:
        /* error */
        return -1;
        break;
      }

      /* -- build the response block */
      /* -- -- packet tail */
      shift = (__PACKET[cur].rsp_len-1)*2;
      __RAW[shift] = __RAW[(__PACKET[cur].rqst_len-1)*2];

      /* -- -- -- clear the DINV and RTC bits */
      for( i=21; i<=28; i++ ){
        __RAW[shift] &= ~(1 << i);
      }

      /* -- -- packet header */
      /* -- -- -- SLID */
      __RAW[0] |= (((uint64_t)(MCL_SLID)<<39)&0x38000000000);

      /* -- -- -- AF */
      if( af == 1 ){
        __RAW[0] |= ((uint64_t)(af)<<33);
      }

      /* -- -- -- Clear the RES bits */
      for( i=23; i<=32; i++ ){
        __RAW[0] &= ~(1 << i);
      }
      for( i=42; i<=60; i++ ){
        __RAW[0] &= ~(1 << i);
      }

      /* -- increment the counter */
      __COUNTER[__PACKET[cur].ctr_idx]++;

      /* return the response length */
      return __PACKET[cur].rsp_len*16;
    }
  }

  /* error occurred */
  return -1;
}

/* EOF */
