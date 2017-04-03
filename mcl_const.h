/* _MCL_CONST_H_
 *
 * MemChameleonLite Constants
 *
 * Copyright 2017, Tactical Computing Laboratories, LLC
 * contact@tactcomplabs.com
 *
 */

#ifndef _MCL_CONST_H_
#define _MCL_CONST_H_

#include <stdint.h>

/* ---------------------------------------------------- MACROS */


/*
 * RTC_FREQUENCY
 *
 * Defines the real-time clock frequency
 *
 */
#define RTC_FREQUENCY 32768

/*
 * MCL_CUB
 *
 * Defines the CUB or "Cube ID"
 * field of the target device.
 * This currently must be hardcoded
 * in order to bootstrap the environment.
 */
#define MCL_CUB         0x01

/*
 * MCL_SLID
 *
 * Defines the SLID or "Source Link ID"
 * for the target device.
 * This currently must be hardcoded in
 * order to bootstrap the environment
 * and correctly send response packets.
 * The value must be between 0 & 8.
 *
 */
#define MCL_SLID        0x0

/*
 * MCL_SIZE
 *
 * Size of the MCL protected data region
 * in bytes.  This is the total backed
 * storage size of the target device
 * instance.
 *
 * Note for the Arty FPGA board, we
 * reserve 200MB of physical space
 * for the HMC-backed memory.
 *
 * Sample Storage:
 *  - device -   : - Sz - : - Sz in hex -
 * Digilent Arty : 200MB  : 0xC800000
 */
#define MCL_SIZE        0xC800000

/*
 * MCL_MAX_PACKET_SIZE
 *
 * Defines the maximum packet size
 * in terms of the number of 8 byte
 * blocks.  NOT FLITS!
 * By default, this is 34 to represent
 * a 256 byte I/O requiring 17 FLITS
 *
 */
#define MCL_MAX_PACKET_SIZE 34

/*
 * MCL_MAX_BLOCK_SZ
 *
 * Defines the maximum block size of
 * the request structure.  May affect
 * certain device addressing modes.
 * Defaults to 256
 *
 */
#define MCL_MAX_BLOCK_SZ  256

/*
 * MCL_NUM_REG
 *
 * Defines the number of internal registers
 * By default, we must have at least two
 * registers as follows:
 *
 * MCL_CUB_REG = defines the cube ID (idx=0)
 * MCL_BLOCK_REG = defines the max block size (idx=1)
 *
 * These indices cannot be changed.  However, ancillary
 * implementations may append to this list
 *
 */
#define MCL_NUM_REG   0x2

/*
 * MCL REGISTER INDEX MACROS
 *
 */
#define MCL_CUB_REG     0x00
#define MCL_BLOCK_REG   0x01

/*
 * MCL_NUM_COUNTER
 *
 * Defines the number of counters
 * in the counter register block.
 *
 */
#define MCL_NUM_COUNTER     0x36

/*
 * MCL COUNTER INDEX MACROS
 *
 */
#define MCL_WR16_COUNTER    0x00
#define MCL_WR32_COUNTER    0x01
#define MCL_WR48_COUNTER    0x02
#define MCL_WR64_COUNTER    0x03
#define MCL_WR80_COUNTER    0x04
#define MCL_WR96_COUNTER    0x05
#define MCL_WR112_COUNTER   0x06
#define MCL_WR128_COUNTER   0x07
#define MCL_WR256_COUNTER   0x08
#define MCL_MDWR_COUNTER    0x09
#define MCL_PWR16_COUNTER   0x0A
#define MCL_PWR32_COUNTER   0x0B
#define MCL_PWR48_COUNTER   0x0C
#define MCL_PWR64_COUNTER   0x0D
#define MCL_PWR80_COUNTER   0x0E
#define MCL_PWR96_COUNTER   0x0F
#define MCL_PWR112_COUNTER  0x10
#define MCL_PWR128_COUNTER  0x11
#define MCL_PWR256_COUNTER  0x12
#define MCL_RD16_COUNTER    0x13
#define MCL_RD32_COUNTER    0x14
#define MCL_RD48_COUNTER    0x15
#define MCL_RD64_COUNTER    0x16
#define MCL_RD80_COUNTER    0x17
#define MCL_RD96_COUNTER    0x18
#define MCL_RD112_COUNTER   0x19
#define MCL_RD128_COUNTER   0x1A
#define MCL_RD256_COUNTER   0x1B
#define MCL_MDRD_COUNTER    0x1C
#define MCL_2ADD8_COUNTER   0x1D
#define MCL_ADD16_COUNTER   0x1E
#define MCL_P2ADD8_COUNTER  0x1F
#define MCL_PADD16_COUNTER  0x20
#define MCL_2ADDS8R_COUNTER 0x21
#define MCL_ADDS16R_COUNTER 0x22
#define MCL_INC8_COUNTER    0x23
#define MCL_PINC8_COUNTER   0x24
#define MCL_XOR16_COUNTER   0x25
#define MCL_OR16_COUNTER    0x26
#define MCL_NOR16_COUNTER   0x27
#define MCL_AND16_COUNTER   0x28
#define MCL_NAND16_COUNTER  0x29
#define MCL_CASGT8_COUNTER  0x2A
#define MCL_CASGT16_COUNTER 0x2B
#define MCL_CASLT8_COUNTER  0x2C
#define MCL_CASLT16_COUNTER 0x2D
#define MCL_CASEQ8_COUNTER  0x2E
#define MCL_CASZ16_COUNTER  0x2F
#define MCL_EQ8_COUNTER     0x30
#define MCL_EQ16_COUNTER    0x31
#define MCL_BWR_COUNTER     0x32
#define MCL_PBWR_COUNTER    0x33
#define MCL_BWR8R_COUNTER   0x34
#define MCL_SWAP16_COUNTER  0x35
#define MCL_NULL_COUNTER    -1

/*
 * RSP_TYPE
 *
 * Defines the HMC packet return types
 *
 */
typedef enum{
  WR_RS,      /* -- write response */
  MD_WR_RS,   /* -- mode write response */
  RD_RS,      /* -- read response */
  MD_RD_RS,   /* -- mode read response */
  NO_RSP      /* -- no response */
}RSP_TYPE;

/*
 * PKT_TYPE
 *
 * Defines the HMC packet request types
 *
 */
typedef enum{
  PKT_R,      /* -- read packet */
  PKT_W,      /* -- write packet */
  PKT_A,      /* -- atomic packet */
  PKT_B,      /* -- bit (mode) packet */
  PKT_NONE    /* -- no type */
}PKT_TYPE;

/*
 * MCL_PACKET
 *
 * MCL packet structure that
 * defines each of the individual
 * HMC packets
 * The actual definition of the packet
 * is found in mcl_packet.c
 *
 */
typedef struct{
  char *name;         /* -- name of the packet */
  uint8_t opc;        /* -- opcode */
  uint16_t data_len;  /* -- defines the data size in bytes */
  uint8_t rqst_len;   /* -- request length in flits */
  uint8_t rsp_len;    /* -- response length in flits */
  RSP_TYPE rsp;       /* -- response type */
  uint16_t ctr_idx;   /* -- counter index */
  PKT_TYPE pkt;       /* -- packet type */
} mcl_packet;

/* ---------------------------------------------------- GLOBALS */
//extern uint8_t __STOR[MCL_SIZE];
extern uint8_t *__STOR;
extern uint64_t __REG[MCL_NUM_REG];
extern uint64_t __COUNTER[MCL_NUM_COUNTER];
extern uint64_t __RAW[MCL_MAX_PACKET_SIZE];

#endif /* _MCL_CONST_H_ */
/* EOF */
