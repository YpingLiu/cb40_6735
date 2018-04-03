/*
 * Copyright © 2010,2012  Google, Inc.
 *
 *  This is part of HarfBuzz, a text shaping library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Google Author(s): Behdad Esfahbod
 */


#include "hb-ot-shape-complex-private.hh"


/* Thai / Lao shaper */


/* PUA shaping */
enum thai_consonant_type_t
{
  NC,
  AC,
  RC,
  DC,
  NOT_CONSONANT,
  NUM_CONSONANT_TYPES = NOT_CONSONANT
};

enum {
    CH_SPACE        = 0x0020,
    CH_YAMAKKAN     = 0x0E4E,
    CH_MAI_HANAKAT  = 0x0E31,
    CH_SARA_AA      = 0x0E32,
    CH_SARA_AM      = 0x0E33,
    CH_SARA_UEE     = 0x0E37,
    CH_MAITAIKHU    = 0x0E47,
    CH_NIKHAHIT     = 0x0E4D,
    CH_SARA_U       = 0x0E38,
    CH_PHINTHU      = 0x0E3A,
    CH_YO_YING      = 0x0E0D,
    CH_THO_THAN     = 0x0E10,
    CH_DOTTED_CIRCLE = 0x25CC
};

enum {
    // Character classes
    NON =  0,
    CON =  1,
    COA =  2,
    COD =  3,
    LVO =  4,
    FV1 =  5,
    FV2 =  6,
    FV3 =  7,
    BV1 =  8,
    BV2 =  9,
    BDI = 10,
    TON = 11,
    AD1 = 12,
    AD2 = 13,
    AD3 = 14,
    NIK = 15,
    AV1 = 16,
    AV2 = 17,
    AV3 = 18,
    classCount = 19,

    // State Transition actions
    tA  =  0,
    tC  =  1,
    tD  =  2,
    tE  =  3,
    tF  =  4,
    tG  =  5,
    tH  =  6,
    tR  =  7,
    tS  =  8
};

typedef struct ThaiStateTransition_ {
    uint8_t nextState;
    uint8_t action;
} ThaiStateTransition, *PThaiStateTransition;

typedef struct ThaiClusterState_ {
    unsigned int preCluster;
    unsigned int ClusterCount;
    bool isClusterStart;
} ThaiClusterSate;

static const uint8_t thaiClassTable[] = {
    //       0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    //       -------------------------------------------------------------------------------
    /*0E00*/ NON, CON, CON, CON, CON, CON, CON, CON, CON, CON, CON, CON, CON, COD, COD, COD,
    /*0E10*/ COD, CON, CON, CON, CON, CON, CON, CON, CON, CON, CON, COA, CON, COA, CON, COA,
    /*0E20*/ CON, CON, CON, CON, FV3, CON, FV3, CON, CON, CON, CON, CON, CON, CON, CON, NON,
    /*0E30*/ FV1, AV2, FV1, FV1, AV1, AV3, AV2, AV3, BV1, BV2, BDI, NON, NON, NON, NON, NON,
    /*0E40*/ LVO, LVO, LVO, LVO, LVO, FV2, NON, AD2, TON, TON, TON, TON, AD1, NIK, AD3, NON,
    /*0E50*/ NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON
};

static const ThaiStateTransition thaiStateTable[][classCount] = {
    //+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    //|         N         C         C         C         L         F         F         F         B         B         B         T         A         A         A         N         A         A         A    |
    //|         O         O         O         O         V         V         V         V         V         V         D         O         D         D         D         I         V         V         V    |
    //|         N         N         A         D         O         1         2         3         1         2         I         N         1         2         3         K         1         2         3    |
    //+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    /*00*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*01*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 2, tC}, { 6, tC}, { 0, tC}, { 8, tE}, { 0, tE}, { 0, tE}, { 0, tC}, { 9, tE}, {11, tC}, {14, tC}, {16, tC}},
    /*02*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 3, tE}, { 0, tE}, { 0, tR}, { 0, tR}, { 4, tE}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*03*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*04*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 5, tC}, { 0, tC}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*05*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*06*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 7, tE}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*07*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*08*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tA}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*09*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {10, tC}, { 0, tC}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*10*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*11*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {12, tC}, { 0, tC}, { 0, tR}, { 0, tR}, {13, tC}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*12*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*13*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*14*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {15, tC}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*15*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*16*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {17, tC}, { 0, tR}, { 0, tC}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*17*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*18*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tA}, { 0, tS}, { 0, tA}, {19, tC}, {23, tC}, { 0, tC}, {25, tF}, { 0, tF}, { 0, tF}, { 0, tD}, {26, tF}, {28, tD}, {31, tD}, {33, tD}},
    /*19*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {20, tF}, { 0, tF}, { 0, tR}, { 0, tR}, {21, tF}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*20*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*21*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {22, tC}, { 0, tC}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*22*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*23*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {24, tF}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*24*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*25*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tA}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*26*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {27, tG}, { 0, tG}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*27*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*28*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {29, tG}, { 0, tG}, { 0, tR}, { 0, tR}, {30, tG}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*29*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*30*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*31*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {32, tG}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*32*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*33*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {34, tG}, { 0, tR}, { 0, tG}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*34*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*35*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tA}, { 0, tS}, { 0, tA}, {36, tH}, {40, tH}, { 0, tH}, {42, tE}, { 0, tE}, { 0, tE}, { 0, tC}, {43, tE}, {45, tC}, {48, tC}, {50, tC}},
    /*36*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {37, tE}, { 0, tE}, { 0, tR}, { 0, tR}, {38, tE}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*37*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*38*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {39, tC}, { 0, tC}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*39*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*40*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {41, tE}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*41*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*42*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tA}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*43*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {44, tC}, { 0, tC}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*44*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*45*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {46, tC}, { 0, tC}, { 0, tR}, { 0, tR}, {47, tC}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*46*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*47*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*48*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {49, tC}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*49*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*50*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tS}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, {51, tC}, { 0, tR}, { 0, tC}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}},
    /*51*/ {{ 0, tA}, { 1, tA}, {18, tA}, {35, tA}, { 0, tA}, { 0, tS}, { 0, tA}, { 0, tA}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}, { 0, tR}}
};

static thai_consonant_type_t
get_consonant_type (hb_codepoint_t u)
{
  if (u == 0x0E1Bu || u == 0x0E1Du || u == 0x0E1Fu/* || u == 0x0E2Cu*/)
    return AC;
  if (u == 0x0E0Du || u == 0x0E10u)
    return RC;
  if (u == 0x0E0Eu || u == 0x0E0Fu)
    return DC;
  if (hb_in_range (u, 0x0E01u, 0x0E2Eu))
    return NC;
  return NOT_CONSONANT;
}


enum thai_mark_type_t
{
  AV,
  BV,
  T,
  NOT_MARK,
  NUM_MARK_TYPES = NOT_MARK
};

static thai_mark_type_t
get_mark_type (hb_codepoint_t u)
{
  if (u == 0x0E31u || hb_in_range (u, 0x0E34u, 0x0E37u) ||
      u == 0x0E47u || hb_in_range (u, 0x0E4Du, 0x0E4Eu))
    return AV;
  if (hb_in_range (u, 0x0E38u, 0x0E3Au))
    return BV;
  if (hb_in_range (u, 0x0E48u, 0x0E4Cu))
    return T;
  return NOT_MARK;
}


enum thai_action_t
{
  NOP,
  SD,  /* Shift combining-mark down */
  SL,  /* Shift combining-mark left */
  SDL, /* Shift combining-mark down-left */
  RD   /* Remove descender from base */
};

static hb_codepoint_t
thai_pua_shape (hb_codepoint_t u, thai_action_t action, hb_font_t *font)
{
  struct thai_pua_mapping_t {
    hb_codepoint_t u;
    hb_codepoint_t win_pua;
    hb_codepoint_t mac_pua;
  } const *pua_mappings = NULL;
  static const thai_pua_mapping_t SD_mappings[] = {
    {0x0E48u, 0xF70Au, 0xF88Bu}, /* MAI EK */
    {0x0E49u, 0xF70Bu, 0xF88Eu}, /* MAI THO */
    {0x0E4Au, 0xF70Cu, 0xF891u}, /* MAI TRI */
    {0x0E4Bu, 0xF70Du, 0xF894u}, /* MAI CHATTAWA */
    {0x0E4Cu, 0xF70Eu, 0xF897u}, /* THANTHAKHAT */
    {0x0E38u, 0xF718u, 0xF89Bu}, /* SARA U */
    {0x0E39u, 0xF719u, 0xF89Cu}, /* SARA UU */
    {0x0E3Au, 0xF71Au, 0xF89Du}, /* PHINTHU */
    {0x0000u, 0x0000u, 0x0000u}
  };
  static const thai_pua_mapping_t SDL_mappings[] = {
    {0x0E48u, 0xF705u, 0xF88Cu}, /* MAI EK */
    {0x0E49u, 0xF706u, 0xF88Fu}, /* MAI THO */
    {0x0E4Au, 0xF707u, 0xF892u}, /* MAI TRI */
    {0x0E4Bu, 0xF708u, 0xF895u}, /* MAI CHATTAWA */
    {0x0E4Cu, 0xF709u, 0xF898u}, /* THANTHAKHAT */
    {0x0000u, 0x0000u, 0x0000u}
  };
  static const thai_pua_mapping_t SL_mappings[] = {
    {0x0E48u, 0xF713u, 0xF88Au}, /* MAI EK */
    {0x0E49u, 0xF714u, 0xF88Du}, /* MAI THO */
    {0x0E4Au, 0xF715u, 0xF890u}, /* MAI TRI */
    {0x0E4Bu, 0xF716u, 0xF893u}, /* MAI CHATTAWA */
    {0x0E4Cu, 0xF717u, 0xF896u}, /* THANTHAKHAT */
    {0x0E31u, 0xF710u, 0xF884u}, /* MAI HAN-AKAT */
    {0x0E34u, 0xF701u, 0xF885u}, /* SARA I */
    {0x0E35u, 0xF702u, 0xF886u}, /* SARA II */
    {0x0E36u, 0xF703u, 0xF887u}, /* SARA UE */
    {0x0E37u, 0xF704u, 0xF888u}, /* SARA UEE */
    {0x0E47u, 0xF712u, 0xF889u}, /* MAITAIKHU */
    {0x0E4Du, 0xF711u, 0xF899u}, /* NIKHAHIT */
    {0x0000u, 0x0000u, 0x0000u}
  };
  static const thai_pua_mapping_t RD_mappings[] = {
    {0x0E0Du, 0xF70Fu, 0xF89Au}, /* YO YING */
    {0x0E10u, 0xF700u, 0xF89Eu}, /* THO THAN */
    {0x0000u, 0x0000u, 0x0000u}
  };

  switch (action) {
    default: assert (false); /* Fallthrough */
    case NOP: return u;
    case SD:  pua_mappings = SD_mappings; break;
    case SDL: pua_mappings = SDL_mappings; break;
    case SL:  pua_mappings = SL_mappings; break;
    case RD:  pua_mappings = RD_mappings; break;
  }
  for (; pua_mappings->u; pua_mappings++)
    if (pua_mappings->u == u)
    {
      hb_codepoint_t glyph;
      if (hb_font_get_glyph (font, pua_mappings->win_pua, 0, &glyph))
	return pua_mappings->win_pua;
      if (hb_font_get_glyph (font, pua_mappings->mac_pua, 0, &glyph))
	return pua_mappings->mac_pua;
      break;
    }
  return u;
}


static enum thai_above_state_t
{     /* Cluster above looks like: */
  T0, /*  ⣤                      */
  T1, /*     ⣼                   */
  T2, /*        ⣾                */
  T3, /*           ⣿             */
  NUM_ABOVE_STATES
} thai_above_start_state[NUM_CONSONANT_TYPES + 1/* For NOT_CONSONANT */] =
{
  T0, /* NC */
  T1, /* AC */
  T0, /* RC */
  T0, /* DC */
  T3, /* NOT_CONSONANT */
};

static const struct thai_above_state_machine_edge_t {
  thai_action_t action;
  thai_above_state_t next_state;
} thai_above_state_machine[NUM_ABOVE_STATES][NUM_MARK_TYPES] =
{        /*AV*/    /*BV*/    /*T*/
/*T0*/ {{NOP,T3}, {NOP,T0}, {SD, T3}},
/*T1*/ {{SL, T2}, {NOP,T1}, {SDL,T2}},
/*T2*/ {{NOP,T3}, {NOP,T2}, {SL, T3}},
/*T3*/ {{NOP,T3}, {NOP,T3}, {NOP,T3}},
};


static enum thai_below_state_t
{
  B0, /* No descender */
  B1, /* Removable descender */
  B2, /* Strict descender */
  NUM_BELOW_STATES
} thai_below_start_state[NUM_CONSONANT_TYPES + 1/* For NOT_CONSONANT */] =
{
  B0, /* NC */
  B0, /* AC */
  B1, /* RC */
  B2, /* DC */
  B2, /* NOT_CONSONANT */
};

static const struct thai_below_state_machine_edge_t {
  thai_action_t action;
  thai_below_state_t next_state;
} thai_below_state_machine[NUM_BELOW_STATES][NUM_MARK_TYPES] =
{        /*AV*/    /*BV*/    /*T*/
/*B0*/ {{NOP,B0}, {NOP,B2}, {NOP, B0}},
/*B1*/ {{NOP,B1}, {RD, B2}, {NOP, B1}},
/*B2*/ {{NOP,B2}, {SD, B2}, {NOP, B2}},
};


static void
do_thai_pua_shaping (const hb_ot_shape_plan_t *plan HB_UNUSED,
		     hb_buffer_t              *buffer,
		     hb_font_t                *font)
{
  thai_above_state_t above_state = thai_above_start_state[NOT_CONSONANT];
  thai_below_state_t below_state = thai_below_start_state[NOT_CONSONANT];
  unsigned int base = 0;

  hb_glyph_info_t *info = buffer->info;
  unsigned int count = buffer->len;
  for (unsigned int i = 0; i < count; i++)
  {
    thai_mark_type_t mt = get_mark_type (info[i].codepoint);

    if (mt == NOT_MARK) {
      thai_consonant_type_t ct = get_consonant_type (info[i].codepoint);
      above_state = thai_above_start_state[ct];
      below_state = thai_below_start_state[ct];
      base = i;
      continue;
    }

    const thai_above_state_machine_edge_t &above_edge = thai_above_state_machine[above_state][mt];
    const thai_below_state_machine_edge_t &below_edge = thai_below_state_machine[below_state][mt];
    above_state = above_edge.next_state;
    below_state = below_edge.next_state;

    /* At least one of the above/below actions is NOP. */
    thai_action_t action = above_edge.action != NOP ? above_edge.action : below_edge.action;

    if (action == RD)
      info[base].codepoint = thai_pua_shape (info[base].codepoint, action, font);
    else
      info[i].codepoint = thai_pua_shape (info[i].codepoint, action, font);
  }
}

static void insert_dotted_circle (hb_codepoint_t u, ThaiClusterSate *clusterState, hb_buffer_t *buffer, hb_font_t *font)
{
    hb_codepoint_t dottedcircle_glyph;
    if (!font->get_glyph (u, 0, &dottedcircle_glyph))
    return;

    hb_glyph_info_t dottedcircle;
    dottedcircle.codepoint = u;
    _hb_glyph_info_set_unicode_props (&dottedcircle, buffer->unicode);

    hb_glyph_info_t info = dottedcircle;

    clusterState->preCluster= clusterState->ClusterCount;
    info.cluster = clusterState->ClusterCount;

    clusterState->ClusterCount++;
    info.mask = buffer->cur().mask;
    buffer->output_info (info);
    buffer->cur().cluster = info.cluster;
    buffer->next_glyph();

}

inline ThaiStateTransition thaiGetTransition(uint8_t state, uint8_t currClass)
{
    return thaiStateTable[state][currClass];
}

static uint8_t thaiGetCharClass(hb_codepoint_t ch) {
    uint8_t charClass = NON;

    if (ch >= 0x0E00 && ch <= 0x0E5B) {
        charClass = thaiClassTable[ch - 0x0E00];
    }

    return charClass;
}

static bool thaiIsLegalHere(hb_codepoint_t ch, uint8_t prevState)
{
    uint8_t charClass = thaiGetCharClass(ch);
    ThaiStateTransition transition = thaiGetTransition(prevState, charClass);

    switch (transition.action) {
    case tA:
    case tC:
    case tD:
    case tE:
    case tF:
    case tG:
    case tH:
        return true;

    case tR:
    case tS:
        return false;

    default:
        // FIXME: if we get here, there's an error
        // in the state table!
        return false;
    }
}

static uint8_t thaiDoTransition (
        ThaiStateTransition transition,
        hb_codepoint_t currChar,
        ThaiClusterSate *clusterState,
        hb_buffer_t *buffer,
        hb_font_t   *font)
{

    switch (transition.action) {
    case tR:
        insert_dotted_circle(CH_DOTTED_CIRCLE, clusterState, buffer, font);

        break;

    case tS:
        if (currChar == CH_SARA_AM) {
            insert_dotted_circle(CH_DOTTED_CIRCLE, clusterState, buffer, font);
            break;
        }

    default:
        if (!clusterState->isClusterStart) {
            buffer->cur().cluster = clusterState->preCluster;
        } else {
            clusterState->preCluster = clusterState->ClusterCount;
            buffer->cur().cluster = clusterState->ClusterCount;
        }
        clusterState->ClusterCount++;
        buffer->next_glyph();
        break;
     }

     return transition.nextState;
}

static uint8_t thaiGetNextState(
        uint16_t ch,
        uint8_t prevState,
        ThaiClusterSate *clusterState,
        hb_buffer_t *buffer,
        hb_font_t   *font)
{
    ThaiStateTransition transition;

    uint8_t charClass = thaiGetCharClass(ch);
    transition = thaiGetTransition(prevState, charClass);
    if (prevState == 0 || charClass == LVO || charClass == CON
                || charClass == COA || charClass == COD) {
        clusterState->isClusterStart = true;
    } else {
        clusterState->isClusterStart = false;
    }

    uint8_t nextState = thaiDoTransition(transition, ch, clusterState, buffer, font);

    return nextState;
}

static void
preprocess_text_thai (const hb_ot_shape_plan_t *plan,
		      hb_buffer_t              *buffer,
		      hb_font_t                *font)
{
#define IS_SARA_AM(x) (((x) & ~0x0080u) == 0x0E33u)
#define NIKHAHIT_FROM_SARA_AM(x) ((x) - 0x0E33u + 0x0E4Du)
#define SARA_AA_FROM_SARA_AM(x) ((x) - 1)
#define IS_TONE_MARK(x) (hb_in_ranges ((x) & ~0x0080u, 0x0E34u, 0x0E37u, 0x0E47u, 0x0E4Eu, 0x0E31u, 0x0E31u))


    uint8_t state = 0;
    unsigned int inputIndex;
    unsigned int outputIndex = 0;
    int j = 0;
    ThaiClusterSate clusterState;
    clusterState.isClusterStart= true;
    clusterState.preCluster = clusterState.ClusterCount = buffer->info[0].cluster; /* cluster number */

    buffer->clear_output ();
    unsigned int count = buffer->len;
    for (buffer->idx = 0; buffer->idx < count;)
    {

        hb_codepoint_t u = buffer->cur().codepoint;
        if (u == CH_SARA_AM && thaiIsLegalHere(u, state)) {
    /* Is SARA AM. Decompose and reorder. */
    hb_codepoint_t decomposed[2] = {hb_codepoint_t (NIKHAHIT_FROM_SARA_AM (u)),
				    hb_codepoint_t (SARA_AA_FROM_SARA_AM (u))};
    buffer->replace_glyphs (1, 2, decomposed);
    if (unlikely (buffer->in_error))
      return;

    /* Make Nikhahit be recognized as a mark when zeroing widths. */
    unsigned int end = buffer->out_len;
    _hb_glyph_info_set_general_category (&buffer->out_info[end - 2], HB_UNICODE_GENERAL_CATEGORY_NON_SPACING_MARK);
    /* Ok, let's see... */
    unsigned int start = end - 2;
    while (start > 0 && IS_TONE_MARK (buffer->out_info[start - 1].codepoint))
      start--;

    if (start + 2 < end)
    {
      /* Move Nikhahit (end-2) to the beginning */
      buffer->merge_out_clusters (start, end);
      hb_glyph_info_t t = buffer->out_info[end - 2];
      memmove (buffer->out_info + start + 1,
	       buffer->out_info + start,
	       sizeof (buffer->out_info[0]) * (end - start - 2));
      buffer->out_info[start] = t;
    }
    else
    {
      /* Since we decomposed, and NIKHAHIT is combining, merge clusters with the
       * previous cluster. */
      if (start)
	buffer->merge_out_clusters (start - 1, end);
            }
            state = 0;
            clusterState.ClusterCount ++;
            continue;
        }

        state = thaiGetNextState(u, state, &clusterState, buffer, font); 
    }
    buffer->swap_buffers();

  /* If font has Thai GSUB, we are done. */
  if (plan->props.script == HB_SCRIPT_THAI && !plan->map.found_script[0])
    do_thai_pua_shaping (plan, buffer, font);
}

const hb_ot_complex_shaper_t _hb_ot_complex_shaper_thai =
{
    "thai",
    NULL, /* collect_features */
    NULL, /* override_features */
    NULL, /* data_create */
    NULL, /* data_destroy */
    preprocess_text_thai,
  HB_OT_SHAPE_NORMALIZATION_MODE_DEFAULT,
    NULL, /* decompose */
    NULL, /* compose */
    NULL, /* setup_masks */
  HB_OT_SHAPE_ZERO_WIDTH_MARKS_DEFAULT,
    false,/* fallback_position */
};

