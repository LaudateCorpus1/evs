/*====================================================================================
    EVS Codec 3GPP TS26.442 Sep 15, 2014. Version 12.0.0
  ====================================================================================*/

#include "stl.h"
#include "cnst_fx.h"       /* Common constants                       */
#include "basop_util.h"

/* 125 375 750 1250 2000 4000 8000 16000 24000 */
const Word16 gw_fx[Lgw_max] =    {  1,  3,  6,  10,  16,  32,  64,  128, 192 };                /* frequency group start bins for transient analysis */

const Word16 gw_len_inv_fx[Lgw_max-1] =    /* Q15 */
{
    16384,   10923,   8192,   5461,
    2048,   1024,   512,   512,
};

/* 31.25 343.75 718.75 1218.75 1968.75 4000 8000 16000 24000 */
const Word16 gwlpr_fx[Lgw_max] = {  1,   3*Quot_Lpr_Ltr-1, 6*Quot_Lpr_Ltr-1, 10*Quot_Lpr_Ltr-1, 16*Quot_Lpr_Ltr-1, 32*Quot_Lpr_Ltr, 64*Quot_Lpr_Ltr, 128*Quot_Lpr_Ltr, 192*Quot_Lpr_Ltr }; /* frequency group start bins for transient analysis */


const Word16 hestable_fx[15]=
{
    0X11,  0X21, -0X04, 0X21, -0X03, 0X21, -0X02, -0X05, 0X11, 0X21, -0X01, -0X06,
    0X11, -0X07, -0X00
};
/*-------------------------------------------------------------------*
 *  num x 32768 + 0.5
 * -------------------
 *    PIT_MAX x Fact
 * Tables fill at compiled time
 *-------------------------------------------------------------------*/

#define PIT_MAX_DIV(num, fact) ((num*32768L+(fact)*PIT_MAX/2) / ((fact)*PIT_MAX))
#define PIT_MAX_DIV16k(num, fact) ((num*32768L+(fact)*PIT16k_MAX/2) / ((fact)*PIT16k_MAX))

/*----------------------------------------------------------------------------------*
 * hp filter for filtering random part of excitation in frame error concealment
 * Used in lib_dec\syn_bfi.c
 *----------------------------------------------------------------------------------*/

const Word16 h_high_fx[5] = { -410, -3572, 25602, -3572, -410 };
const Word16 inv_sqi[15] =
{
    8192,   /* 1/4 */
    3641,   /* 1/9 */
    2048,   /* 1/16 */
    1311,   /* 1/25 */
    910,    /* 1/36 */
    669,    /* 1/49 */
    512,    /* 1/64 */
    405,    /* 1/81  */
    328,    /* 1/100 */
    271,    /* 1/121 */
    228,    /* 1/144 */
    194,    /* 1/169 */
    167,    /* 1/196 */
    146,    /* 1/225 */
    128     /* 1/256 */
};
/*-------------------------------------------------------------------*
 * index square used in fec_adapt_codebook
 *-------------------------------------------------------------------*/
const Word16 sqi[15] =
{
    4,    /* 2 */
    9,    /* 3 */
    16,   /* 4 */
    25,   /* 5 */
    36,   /* 6 */
    49,   /* 7 */
    64,   /* 8 */
    81,   /* 9 */
    100,  /* 10 */
    121,  /* 11 */
    144,  /* 12 */
    169,  /* 13 */
    196,  /* 14 */
    225,  /* 15 */
    256   /* 16 */
};
/*------------------------------------------------------------------------------*
 * RE8 related tables rom
 *------------------------------------------------------------------------------*/

/* NOTE: Many values here could fit in shorts instead of ints */

const Word16 mult_avq_tab_fx[7] = {1,1,1,342,1,1,171};
const Word16 shift_avq_tab_fx[7] = {0,0,1, 10,2,0, 10};

/*----------------------------------------------------------------------------------*
 * HR SWB BWE parameters
 *----------------------------------------------------------------------------------*/
const Word16 swb_hr_inv_frm_len[4] = /* in Q19 */
{
    26214, /* 1/(160/8) */
    13107, /* 1/(320/8) */
    6554, /* 1/(640/8) */
    4369  /* 1/(960/8) */
};
const Word16 inv_tbl_2n_minus1[] = { -1, -1, /* Bits = 0 and 1 are not used */
                                     21845/*Q16*/,
                                     18725/*Q17*/,
                                     17476/*Q18*/,
                                     16913/*Q19*/,
                                     16644/*Q20*/,
                                     16513/*Q21*/
                                   }; /* in Qx */


const Word16 hntable_fx[55]=
{
    0X13,  -0X10,  -0X0F,  -0X11,   0X51,   0X61,  -0X0E,  -0X12,   0X51,   0X61,
    -0X13,  -0X0D,   0X51,   0X62,  -0X14,   0X81,  -0X0C,   0X81,  -0X15,  -0X17,
    0X71,   0X81,  -0X18,  -0X16,   0X71,  -0X0B,   0X71,  -0X1A,   0X71,  -0X09,
    -0X1B,  -0X0A,  -0X19,   0X41,  -0X1C,  -0X08,   0X31,   0X41,  -0X1D,  -0X06,
    0X31,   0X41,  -0X07,   0X41,  -0X05,  -0X1E,  -0X04,  -0X1F,   0X11,  -0X03,
    0X11,  -0X02,   0X11,  -0X01,   0X00
};

const Word16 hetable_fx[57]=
{
    0X12,   0X41,  -0X0F,   0X41,  -0X10,  -0X0E,   0X31,  -0X11,   0X31,   0X41,
    -0X0D,   0X41,  -0X12,   0X42,  -0x0C,   0X61,  -0X13,   0X61,   0X71,  -0X0A,
    0X71,  -0X0B,  -0X14,  -0X15,   0X51,   0X61,  -0X16,  -0X09,   0X51,   0X61,
    -0X17,   0X62,  -0X08,   0X81,  -0X07,   0X81,  -0X18,  -0X05,  -0X04,  -0X03,
    0X51,  -0X06,   0X51,  -0X19,   0X51,  -0X01,  -0X1C,  -0X02,  -0X1B,  -0X1A,
    0X11,   0X00,   0X11,  -0X1D,   0X11,  -0X1E,  -0X1F
};

const Word16 lsf_tab_fx[LPC_SHB_ORDER] =
{
    2315, 3920, 5546, 6759, 7978,
    10254,  11336,	12476, 13437, 14590
};


/*-----------------------------------------------------------------------------*
 * lp filter for filtering periodic part of excitation in artificial onset
 * construction after frame error concealment
 * Used in lib_dec\onset_fx.c
 *-----------------------------------------------------------------------------*/

const Word16 H_low[5] =
{
    -410, 3572, 25602, 3572, -410
};

const Word16 w_hamm48k_2_fx[Ltrana48k/2] =
{
    2621,     2623,     2630,     2640,     2654,     2672,     2694,     2721,
    2751,     2785,     2824,     2866,     2913,     2963,     3017,     3076,
    3138,     3204,     3274,     3348,     3426,     3507,     3593,     3682,
    3775,     3871,     3972,     4076,     4184,     4295,     4410,     4529,
    4651,     4777,     4906,     5039,     5175,     5314,     5457,     5603,
    5752,     5904,     6060,     6219,     6381,     6546,     6714,     6884,
    7058,     7235,     7414,     7596,     7781,     7969,     8159,     8352,
    8547,     8745,     8945,     9148,     9353,     9560,     9769,     9980,
    10194,    10409,    10627,    10846,    11067,    11290,    11515,    11741,
    11969,    12199,    12430,    12662,    12896,    13131,    13367,    13605,
    13843,    14083,    14323,    14565,    14807,    15050,    15294,    15538,
    15783,    16029,    16275,    16521,    16768,    17015,    17262,    17509,
    17757,    18004,    18251,    18498,    18745,    18991,    19238,    19483,
    19729,    19973,    20217,    20461,    20704,    20945,    21186,    21427,
    21666,    21904,    22140,    22376,    22611,    22844,    23075,    23306,
    23534,    23762,    23987,    24211,    24433,    24653,    24872,    25088,
    25303,    25515,    25725,    25934,    26140,    26343,    26545,    26744,
    26940,    27134,    27326,    27515,    27701,    27884,    28065,    28243,
    28419,    28591,    28760,    28927,    29090,    29250,    29408,    29562,
    29712,    29860,    30004,    30146,    30283,    30418,    30548,    30676,
    30800,    30920,    31037,    31150,    31260,    31366,    31468,    31567,
    31662,    31753,    31840,    31924,    32003,    32079,    32151,    32219,
    32283,    32344,    32400,    32452,    32501,    32545,    32585,    32622,
    32654,    32682,    32707,    32727,    32743,    32755,    32763,    32767
};

const Word16 w_hamm32k_2_fx[Ltrana32k/2]=
{
    2621,     2626,     2640,     2663,     2695,     2736,     2786,     2845,
    2913,     2991,     3077,     3172,     3276,     3388,     3509,     3639,
    3778,     3925,     4080,     4243,     4415,     4595,     4782,     4978,
    5181,     5392,     5610,     5836,     6069,     6309,     6555,     6809,
    7069,     7336,     7609,     7888,     8173,     8464,     8760,     9062,
    9369,     9681,     9998,    10319,    10646,    10976,    11310,    11649,
    11991,    12336,    12685,    13037,    13391,    13749,    14108,    14470,
    14834,    15199,    15566,    15935,    16304,    16674,    17045,    17416,
    17788,    18159,    18530,    18900,    19270,    19639,    20007,    20373,
    20738,    21101,    21461,    21820,    22176,    22529,    22879,    23226,
    23570,    23910,    24247,    24579,    24907,    25231,    25551,    25865,
    26175,    26479,    26778,    27072,    27360,    27642,    27918,    28188,
    28451,    28708,    28958,    29202,    29438,    29667,    29889,    30104,
    30311,    30510,    30702,    30886,    31061,    31229,    31388,    31539,
    31682,    31816,    31942,    32059,    32167,    32266,    32357,    32439,
    32511,    32575,    32630,    32675,    32712,    32739,    32758,    32767
};

const Word16 w_hamm16k_2_fx[Ltrana16k/2] =
{
    2621,     2640,     2695,     2787,     2916,     3080,     3281,     3516,
    3787,     4091,     4429,     4799,     5201,     5633,     6095,     6585,
    7102,     7646,     8214,     8805,     9418,    10051,    10703,    11371,
    12056,    12754,    13464,    14185,    14914,    15650,    16391,    17136,
    17881,    18626,    19369,    20108,    20841,    21566,    22282,    22986,
    23678,    24354,    25015,    25658,    26281,    26883,    27463,    28019,
    28549,    29053,    29529,    29976,    30393,    30780,    31134,    31455,
    31742,    31995,    32213,    32396,    32543,    32653,    32727,    32763
};

const Word16 w_hamm8k_2_fx[Ltrana8k/2] =
{
    2621,     2696,     2920,     3291,     3805,     4457,     5241,     6148,
    7170,     8297,     9517,    10818,    12188,    13612,    15077,    16568,
    18071,    19569,    21049,    22495,    23894,    25231,    26494,    27668,
    28744,    29710,    30557,    31275,    31859,    32302,    32600,    32749
};

const Word16 w_hamm_sana48k_2_fx[288] =
{
    2621,     2622,     2625,     2630,     2636,     2644,     2654,     2666,
    2679,     2694,     2711,     2730,     2751,     2773,     2797,     2823,
    2851,     2881,     2912,     2945,     2980,     3017,     3055,     3095,
    3137,     3180,     3226,     3273,     3321,     3372,     3424,     3478,
    3534,     3591,     3650,     3710,     3773,     3837,     3902,     3970,
    4039,     4109,     4181,     4255,     4330,     4407,     4486,     4566,
    4648,     4731,     4816,     4902,     4990,     5079,     5170,     5263,
    5357,     5452,     5549,     5647,     5747,     5848,     5950,     6054,
    6160,     6266,     6374,     6484,     6595,     6707,     6820,     6935,
    7051,     7168,     7287,     7406,     7527,     7650,     7773,     7898,
    8023,     8150,     8278,     8407,     8538,     8669,     8802,     8935,
    9070,     9205,     9342,     9479,     9618,     9758,     9898,    10040,
    10182,    10325,    10469,    10614,    10760,    10907,    11054,    11202,
    11351,    11501,    11652,    11803,    11955,    12108,    12261,    12415,
    12569,    12725,    12880,    13037,    13194,    13351,    13509,    13668,
    13827,    13986,    14146,    14306,    14467,    14628,    14789,    14951,
    15113,    15276,    15438,    15601,    15765,    15928,    16092,    16256,
    16420,    16584,    16748,    16913,    17077,    17242,    17406,    17571,
    17736,    17901,    18065,    18230,    18394,    18559,    18723,    18888,
    19052,    19216,    19379,    19543,    19706,    19870,    20032,    20195,
    20357,    20519,    20681,    20842,    21003,    21163,    21324,    21483,
    21642,    21801,    21959,    22117,    22274,    22431,    22587,    22742,
    22897,    23052,    23205,    23358,    23511,    23662,    23813,    23963,
    24113,    24261,    24409,    24556,    24702,    24848,    24992,    25136,
    25279,    25421,    25562,    25702,    25841,    25979,    26116,    26252,
    26387,    26521,    26654,    26786,    26917,    27047,    27175,    27303,
    27429,    27554,    27678,    27801,    27923,    28043,    28162,    28280,
    28397,    28512,    28626,    28739,    28850,    28960,    29069,    29177,
    29283,    29387,    29490,    29592,    29693,    29792,    29889,    29985,
    30080,    30173,    30265,    30355,    30444,    30531,    30616,    30700,
    30783,    30864,    30943,    31021,    31097,    31172,    31244,    31316,
    31386,    31454,    31520,    31585,    31648,    31710,    31769,    31827,
    31884,    31939,    31992,    32043,    32093,    32140,    32187,    32231,
    32274,    32315,    32354,    32391,    32427,    32461,    32493,    32524,
    32552,    32579,    32604,    32628,    32649,    32669,    32687,    32703,
    32717,    32730,    32741,    32750,    32757,    32762,    32766,    32767
};

const Word16 POW_ATT_TABLE0[OFF_FRAMES_LIMIT+1]=    /* in Q15-- 10^((-i*4)/20), for i = 0, 1, ..., 15.*/
{
    32767,   31293,   29885,   28540,   27255,   26029,   24857,   23738,
    22670,   21650,   20675,   19745,   18856,   18007,   17197,   16423
};
const Word16 POW_ATT_TABLE1[OFF_FRAMES_LIMIT+1]=    /*= in Q15-- 10^((-i*0.3)/20), for i = 0, 1, ..., 15.*/
{
    32767,   31656,   30581,   29543,   28540,   27571,   26635,   25731,
    24857,   24013,   23198,   22410,   21650,   20915,   20205,   19519
};
const Word16 GR_POW_HEADROOM[Lgw_max] =   /* in Q0-- Number of bits down-shifting to prevent from saturation in group power accumulation.*/
{
    0, 0, 1, 2, 2, 3, 4, 4, 4
};

/*---------------------------------------------------------------------*
 * NB post-filter tables
 *---------------------------------------------------------------------*/

const Word16 Tab_hup_s[SIZ_TAB_HUP_S] =
{
    -188,  2873, 31650, -1597,  -484,  7041, 28469, -2147,  -933, 12266,
    23705, -1992, -1492, 18050, 18050, -1492, -1992, 23705, 12266,  -933,
    -2147, 28469,  7041,  -484, -1597, 31650,  2873,  -188
};

const Word16 Tab_hup_l[SIZ_TAB_HUP_L] =
{
    -40,      72,    -156,     315,    -579,    1023,   -1874,    4439,
    31915,   -3390,    1595,    -887,     501,    -266,     130,     -59,
    -77,     147,    -317,     631,   -1150,    2030,   -3773,    9639,
    29436,   -5579,    2727,   -1527,     859,    -453,     218,    -101,
    -106,     212,    -455,     892,   -1614,    2850,   -5392,   15206,
    25569,   -6549,    3303,   -1860,    1041,    -543,     258,    -122,
    -123,     253,    -538,    1044,   -1876,    3319,   -6414,   20676,
    20676,   -6414,    3319,   -1876,    1044,    -538,     253,    -123,
    -122,     258,    -543,    1041,   -1860,    3303,   -6549,   25569,
    15206,   -5392,    2850,   -1614,     892,    -455,     212,    -106,
    -101,     218,    -453,     859,   -1527,    2727,   -5579,   29436,
    9639,   -3773,    2030,   -1150,     631,    -317,     147,     -77,
    -59,     130,    -266,     501,    -887,    1595,   -3390,   31915,
    4439,   -1874,    1023,    -579,     315,    -156,      72,     -40
};


/*Table 256 / (L_frame*2) , needed in sig_classifier.c and er_dec_acelp.c*/
const Word16 T_256DIV_L_Frame[]=
{
    6554 /*L_Frame 640*/
    ,8192 /*L_Frame 512*/
    ,8738 /*L_Frame 480*/
    ,13107/*L_Frame 320*/
    ,16384 /*L_Frame 256*/
    ,26214 /*L_Frame 160*/
};

const Word16 T_DIV_L_Frame[] =   /*0Q15 * 2^-7 */
{
    26214  /*160*/
    ,16384 /*256*/
    ,13107 /*320*/
    ,8738  /*480*/
    ,8192  /*512*/
    ,6553  /*640*/
};


/*er_dec_tcx.c*/
const Word16 h_high3_32[L_FIR_FER2]   = {FL2WORD16(-0.0517f), FL2WORD16(-0.0587f), FL2WORD16(-0.0820f), FL2WORD16(-0.1024f), FL2WORD16(-0.1164f),  FL2WORD16(0.8786f), FL2WORD16(-0.1164f), FL2WORD16(-0.1024f), FL2WORD16(-0.0820f), FL2WORD16(-0.0587f), FL2WORD16(-0.0517f)};
const Word16 h_high3_25_6[L_FIR_FER2] = {FL2WORD16(-0.0300f), FL2WORD16(-0.0521f), FL2WORD16(-0.0837f), FL2WORD16(-0.1142f), FL2WORD16(-0.1362f),  FL2WORD16(0.8557f), FL2WORD16(-0.1362f), FL2WORD16(-0.1142f), FL2WORD16(-0.0837f), FL2WORD16(-0.0521f), FL2WORD16(-0.0300f)};
const Word16 h_high3_16[L_FIR_FER2]   = {FL2WORD16( 0.f),     FL2WORD16(-0.0205f), FL2WORD16(-0.0651f), FL2WORD16(-0.1256f), FL2WORD16(-0.1792f),  FL2WORD16(0.8028f), FL2WORD16(-0.1792f), FL2WORD16(-0.1256f), FL2WORD16(-0.0651f), FL2WORD16(-0.0205f), FL2WORD16( 0.f    )};

const Word16 pow2tab[15] = { 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080, 0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000 };

/* CLDFB-BPF */
/**
  \brief   Bass Post Filter weights
   First bpf_weights coefficient is scaled by 0.5
*/
const Word16 bpf_weights_16_Fx[16] =
{
    FL2WORD16(0.500020f), FL2WORD16(0.702561f), FL2WORD16(0.208838f), FL2WORD16(0.001553f),
    FL2WORD16(0.001777f), FL2WORD16(0.001370f), FL2WORD16(0.000926f), FL2WORD16(0.000554f),
    FL2WORD16(0.000270f), FL2WORD16(0.000080f), FL2WORD16(0.000030f), FL2WORD16(0.000082f),
    FL2WORD16(0.000081f), FL2WORD16(0.000052f), FL2WORD16(0.000017f), FL2WORD16(0.000016f)
};


/* Word16 (Q15) Hann window ranging from 0 to 32767/32768 (~0.999969) */
Word16 pcmdsp_window_hann_960[960] =
{
    0,    0,    1,    3,    6,    9,    13,   17,   22,   28,   35,   42,   51,   59,   69,
    79,   90,   101,  114,  127,  140,  155,  170,  185,  202,  219,  237,  255,  274,  294,
    315,  336,  358,  381,  404,  428,  453,  478,  504,  531,  558,  586,  615,  645,  675,
    705,  737,  769,  802,  835,  869,  904,  940,  976,  1013, 1050, 1088, 1127, 1166, 1206,
    1247, 1289, 1331, 1373, 1416, 1460, 1505, 1550, 1596, 1643, 1690, 1737, 1786, 1835, 1884,
    1935, 1985, 2037, 2089, 2142, 2195, 2249, 2303, 2359, 2414, 2471, 2528, 2585, 2643, 2702,
    2761, 2821, 2882, 2943, 3004, 3066, 3129, 3192, 3256, 3321, 3386, 3451, 3517, 3584, 3651,
    3719, 3787, 3856, 3926, 3995, 4066, 4137, 4208, 4280, 4353, 4426, 4499, 4574, 4648, 4723,
    4799, 4875, 4951, 5028, 5106, 5184, 5263, 5342, 5421, 5501, 5581, 5662, 5743, 5825, 5907,
    5990, 6073, 6157, 6241, 6325, 6410, 6495, 6581, 6667, 6754, 6841, 6928, 7016, 7104, 7193,
    7282, 7371, 7461, 7551, 7641, 7732, 7823, 7915, 8007, 8099, 8192, 8285, 8378, 8472, 8566,
    8661, 8755, 8850, 8946, 9042, 9138, 9234, 9331, 9427, 9525, 9622, 9720, 9818, 9917,10015,
    10114,10213,10313,10413,10512,10613,10713,10814,10915,11016,11118,11219,11321,11423,11525,
    11628,11731,11834,11937,12040,12144,12247,12351,12455,12559,12664,12768,12873,12978,13083,
    13188,13293,13398,13504,13609,13715,13821,13927,14033,14139,14245,14352,14458,14565,14671,
    14778,14885,14992,15099,15205,15312,15419,15527,15634,15741,15848,15955,16062,16170,16277,
    16384,16491,16598,16706,16813,16920,17027,17134,17241,17349,17456,17563,17669,17776,17883,
    17990,18097,18203,18310,18416,18523,18629,18735,18841,18947,19053,19159,19264,19370,19475,
    19580,19685,19790,19895,20000,20104,20209,20313,20417,20521,20624,20728,20831,20934,21037,
    21140,21243,21345,21447,21549,21650,21752,21853,21954,22055,22155,22256,22355,22455,22555,
    22654,22753,22851,22950,23048,23146,23243,23341,23437,23534,23630,23726,23822,23918,24013,
    24107,24202,24296,24390,24483,24576,24669,24761,24853,24945,25036,25127,25217,25307,25397,
    25486,25575,25664,25752,25840,25927,26014,26101,26187,26273,26358,26443,26527,26611,26695,
    26778,26861,26943,27025,27106,27187,27267,27347,27426,27505,27584,27662,27740,27817,27893,
    27969,28045,28120,28194,28269,28342,28415,28488,28560,28631,28702,28773,28842,28912,28981,
    29049,29117,29184,29251,29317,29382,29447,29512,29576,29639,29702,29764,29825,29886,29947,
    30007,30066,30125,30183,30240,30297,30354,30409,30465,30519,30573,30626,30679,30731,30783,
    30833,30884,30933,30982,31031,31078,31125,31172,31218,31263,31308,31352,31395,31437,31479,
    31521,31562,31602,31641,31680,31718,31755,31792,31828,31864,31899,31933,31966,31999,32031,
    32063,32093,32123,32153,32182,32210,32237,32264,32290,32315,32340,32364,32387,32410,32432,
    32453,32474,32494,32513,32531,32549,32566,32583,32598,32613,32628,32641,32654,32667,32678,
    32689,32699,32709,32717,32726,32733,32740,32746,32751,32755,32759,32762,32765,32767,32767,
    32767,32767,32766,32764,32761,32758,32754,32750,32745,32739,32732,32725,32716,32708,32698,
    32688,32677,32666,32653,32640,32627,32612,32597,32582,32565,32548,32530,32512,32493,32473,
    32452,32431,32409,32386,32363,32339,32314,32289,32263,32236,32209,32181,32152,32122,32092,
    32062,32030,31998,31965,31932,31898,31863,31827,31791,31754,31717,31679,31640,31601,31561,
    31520,31478,31436,31394,31351,31307,31262,31217,31171,31124,31077,31030,30981,30932,30883,
    30832,30782,30730,30678,30625,30572,30518,30464,30408,30353,30296,30239,30182,30124,30065,
    30006,29946,29885,29824,29763,29701,29638,29575,29511,29446,29381,29316,29250,29183,29116,
    29048,28980,28911,28841,28772,28701,28630,28559,28487,28414,28341,28268,28193,28119,28044,
    27968,27892,27816,27739,27661,27583,27504,27425,27346,27266,27186,27105,27024,26942,26860,
    26777,26694,26610,26526,26442,26357,26272,26186,26100,26013,25926,25839,25751,25663,25574,
    25485,25396,25306,25216,25126,25035,24944,24852,24760,24668,24575,24482,24389,24295,24201,
    24106,24012,23917,23821,23725,23629,23533,23436,23340,23242,23145,23047,22949,22850,22752,
    22653,22554,22454,22354,22255,22154,22054,21953,21852,21751,21649,21548,21446,21344,21242,
    21139,21036,20933,20830,20727,20623,20520,20416,20312,20208,20103,19999,19894,19789,19684,
    19579,19474,19369,19263,19158,19052,18946,18840,18734,18628,18522,18415,18309,18202,18096,
    17989,17882,17775,17668,17562,17455,17348,17240,17133,17026,16919,16812,16705,16597,16490,
    16383,16276,16169,16061,15954,15847,15740,15633,15526,15418,15311,15204,15098,14991,14884,
    14777,14670,14564,14457,14351,14244,14138,14032,13926,13820,13714,13608,13503,13397,13292,
    13187,13082,12977,12872,12767,12663,12558,12454,12350,12246,12143,12039,11936,11833,11730,
    11627,11524,11422,11320,11218,11117,11015,10914,10813,10712,10612,10511,10412,10312,10212,
    10113,10014,9916, 9817, 9719, 9621, 9524, 9426, 9330, 9233, 9137, 9041, 8945, 8849, 8754,
    8660, 8565, 8471, 8377, 8284, 8191, 8098, 8006, 7914, 7822, 7731, 7640, 7550, 7460, 7370,
    7281, 7192, 7103, 7015, 6927, 6840, 6753, 6666, 6580, 6494, 6409, 6324, 6240, 6156, 6072,
    5989, 5906, 5824, 5742, 5661, 5580, 5500, 5420, 5341, 5262, 5183, 5105, 5027, 4950, 4874,
    4798, 4722, 4647, 4573, 4498, 4425, 4352, 4279, 4207, 4136, 4065, 3994, 3925, 3855, 3786,
    3718, 3650, 3583, 3516, 3450, 3385, 3320, 3255, 3191, 3128, 3065, 3003, 2942, 2881, 2820,
    2760, 2701, 2642, 2584, 2527, 2470, 2413, 2358, 2302, 2248, 2194, 2141, 2088, 2036, 1984,
    1934, 1883, 1834, 1785, 1736, 1689, 1642, 1595, 1549, 1504, 1459, 1415, 1372, 1330, 1288,
    1246, 1205, 1165, 1126, 1087, 1049, 1012, 975,  939,  903,  868,  834,  801,  768,  736,
    704,  674,  644,  614,  585,  557,  530,  503,  477,  452,  427,  403,  380,  357,  335,
    314,  293,  273,  254,  236,  218,  201,  184,  169,  154,  139,  126,  113,  100,  89,
    78,   68,   58,   50,   41,   34,   27,   21,   16,   12,   8,    5,    2,    0,    0
};

/* Word16 (Q15) Hann window ranging from 0 to 32767/32768 (~0.999969) */
Word16 pcmdsp_window_hann_640[640] =
{
    0,    1,    3,    7,    13,   20,   28,   39,   51,   64,   79,   95,  114,  133,  155,
    177,  202,  228,  255,  284,  315,  347,  381,  416,  453,  491,  531,  572,  615,  660,
    705,  753,  802,  852,  904,  958,  1013, 1069, 1127, 1186, 1247, 1309, 1373, 1438, 1505,
    1573, 1643, 1713, 1786, 1859, 1935, 2011, 2089, 2168, 2249, 2331, 2414, 2499, 2585, 2672,
    2761, 2851, 2943, 3035, 3129, 3224, 3321, 3418, 3517, 3618, 3719, 3822, 3926, 4031, 4137,
    4244, 4353, 4463, 4574, 4686, 4799, 4913, 5028, 5145, 5263, 5381, 5501, 5622, 5743, 5866,
    5990, 6115, 6241, 6368, 6495, 6624, 6754, 6884, 7016, 7148, 7282, 7416, 7551, 7687, 7823,
    7961, 8099, 8238, 8378, 8519, 8661, 8803, 8946, 9089, 9234, 9379, 9525, 9671, 9818, 9966,
    10114,10263,10413,10563,10713,10864,11016,11168,11321,11474,11628,11782,11937,12092,12247,
    12403,12559,12716,12873,13030,13188,13346,13504,13662,13821,13980,14139,14299,14458,14618,
    14778,14938,15099,15259,15419,15580,15741,15902,16062,16223,16384,16545,16706,16866,17027,
    17188,17349,17509,17669,17830,17990,18150,18310,18469,18629,18788,18947,19106,19264,19422,
    19580,19738,19895,20052,20209,20365,20521,20676,20831,20986,21140,21294,21447,21600,21752,
    21904,22055,22205,22355,22505,22654,22802,22950,23097,23243,23389,23534,23679,23822,23965,
    24107,24249,24390,24530,24669,24807,24945,25081,25217,25352,25486,25620,25752,25884,26014,
    26144,26273,26400,26527,26653,26778,26902,27025,27146,27267,27387,27505,27623,27740,27855,
    27969,28082,28194,28305,28415,28524,28631,28737,28842,28946,29049,29150,29251,29350,29447,
    29544,29639,29733,29825,29917,30007,30096,30183,30269,30354,30437,30519,30600,30679,30757,
    30833,30909,30982,31055,31125,31195,31263,31330,31395,31459,31521,31582,31641,31699,31755,
    31810,31864,31916,31966,32015,32063,32108,32153,32196,32237,32277,32315,32352,32387,32421,
    32453,32484,32513,32540,32566,32591,32613,32635,32654,32673,32689,32704,32717,32729,32740,
    32748,32755,32761,32765,32767,32767,32766,32764,32760,32754,32747,32739,32728,32716,32703,
    32688,32672,32653,32634,32612,32590,32565,32539,32512,32483,32452,32420,32386,32351,32314,
    32276,32236,32195,32152,32107,32062,32014,31965,31915,31863,31809,31754,31698,31640,31581,
    31520,31458,31394,31329,31262,31194,31124,31054,30981,30908,30832,30756,30678,30599,30518,
    30436,30353,30268,30182,30095,30006,29916,29824,29732,29638,29543,29446,29349,29250,29149,
    29048,28945,28841,28736,28630,28523,28414,28304,28193,28081,27968,27854,27739,27622,27504,
    27386,27266,27145,27024,26901,26777,26652,26526,26399,26272,26143,26013,25883,25751,25619,
    25485,25351,25216,25080,24944,24806,24668,24529,24389,24248,24106,23964,23821,23678,23533,
    23388,23242,23096,22949,22801,22653,22504,22354,22204,22054,21903,21751,21599,21446,21293,
    21139,20985,20830,20675,20520,20364,20208,20051,19894,19737,19579,19421,19263,19105,18946,
    18787,18628,18468,18309,18149,17989,17829,17668,17508,17348,17187,17026,16865,16705,16544,
    16383,16222,16061,15901,15740,15579,15418,15258,15098,14937,14777,14617,14457,14298,14138,
    13979,13820,13661,13503,13345,13187,13029,12872,12715,12558,12402,12246,12091,11936,11781,
    11627,11473,11320,11167,11015,10863,10712,10562,10412,10262,10113, 9965, 9817, 9670, 9524,
    9378, 9233, 9088, 8945, 8802, 8660, 8518, 8377, 8237, 8098, 7960, 7822, 7686, 7550, 7415,
    7281, 7147, 7015, 6883, 6753, 6623, 6494, 6367, 6240, 6114, 5989, 5865, 5742, 5621, 5500,
    5380, 5262, 5144, 5027, 4912, 4798, 4685, 4573, 4462, 4352, 4243, 4136, 4030, 3925, 3821,
    3718, 3617, 3516, 3417, 3320, 3223, 3128, 3034, 2942, 2850, 2760, 2671, 2584, 2498, 2413,
    2330, 2248, 2167, 2088, 2010, 1934, 1858, 1785, 1712, 1642, 1572, 1504, 1437, 1372, 1308,
    1246, 1185, 1126, 1068, 1012, 957,  903,  851,  801,  752,  704,  659,  614,  571,  530,
    490,  452,  415,  380,  346,  314,  283,  254,  227,  201,  176,  154,  132,  113,  94,
    78,   63,   50,   38,   27,   19,   12,   6,    2,    0
};

Word16 pcmdsp_window_hann_1280[1280] =
	{0,0,1,1,2,3,5,7,10,13,17,20,24,28,34,39,
	45,51,58,64,72,79,87,95,105,114,124,133,144,155,166,177,
	190,202,215,228,242,255,270,284,300,315,331,347,364,381,399,416,
	435,453,472,491,511,531,552,572,594,615,638,660,683,705,729,753,
	778,802,827,852,878,904,931,958,986,1013,1041,1069,1098,1127,1157,1186,
	1217,1247,1278,1309,1341,1373,1406,1438,1472,1505,1539,1573,1608,1643,1678,1713,
	1750,1786,1823,1859,1897,1935,1973,2011,2050,2089,2129,2168,2209,2249,2290,2331,
	2373,2414,2457,2499,2542,2585,2629,2672,2717,2761,2806,2851,2897,2943,2989,3035,
	3082,3129,3177,3224,3273,3321,3370,3418,3468,3517,3568,3618,3669,3719,3771,3822,
	3874,3926,3979,4031,4084,4137,4191,4244,4299,4353,4408,4463,4519,4574,4630,4686,
	4743,4799,4856,4913,4971,5028,5087,5145,5204,5263,5322,5381,5441,5501,5562,5622,
	5683,5743,5805,5866,5928,5990,6053,6115,6178,6241,6305,6368,6432,6495,6560,6624,
	6689,6754,6819,6884,6950,7016,7082,7148,7215,7282,7349,7416,7484,7551,7619,7687,
	7755,7823,7892,7961,8030,8099,8169,8238,8308,8378,8449,8519,8590,8661,8732,8803,
	8875,8946,9018,9089,9162,9234,9307,9379,9452,9525,9598,9671,9745,9818,9892,9966,
	10040,10114,10189,10263,10338,10413,10488,10563,10638,10713,10789,10864,10940,11016,11092,11168,
	11245,11321,11398,11474,11551,11628,11705,11782,11860,11937,12015,12092,12170,12247,12325,12403,
	12481,12559,12638,12716,12795,12873,12952,13030,13109,13188,13267,13346,13425,13504,13583,13662,
	13742,13821,13901,13980,14060,14139,14219,14299,14379,14458,14538,14618,14698,14778,14858,14938,
	15019,15099,15179,15259,15339,15419,15500,15580,15661,15741,15822,15902,15982,16062,16143,16223,
	16304,16384,16465,16545,16626,16706,16786,16866,16947,17027,17108,17188,17269,17349,17429,17509,
	17589,17669,17750,17830,17910,17990,18070,18150,18230,18310,18390,18469,18549,18629,18709,18788,
	18868,18947,19027,19106,19185,19264,19343,19422,19501,19580,19659,19738,19817,19895,19974,20052,
	20131,20209,20287,20365,20443,20521,20599,20676,20754,20831,20909,20986,21063,21140,21217,21294,
	21371,21447,21524,21600,21676,21752,21828,21904,21980,22055,22130,22205,22280,22355,22430,22505,
	22580,22654,22728,22802,22876,22950,23024,23097,23170,23243,23316,23389,23462,23534,23607,23679,
	23751,23822,23894,23965,24036,24107,24178,24249,24320,24390,24460,24530,24600,24669,24738,24807,
	24876,24945,25013,25081,25149,25217,25285,25352,25419,25486,25553,25620,25686,25752,25818,25884,
	25949,26014,26079,26144,26209,26273,26337,26400,26464,26527,26590,26653,26716,26778,26840,26902,
	26964,27025,27086,27146,27207,27267,27327,27387,27446,27505,27564,27623,27682,27740,27798,27855,
	27912,27969,28026,28082,28138,28194,28250,28305,28360,28415,28470,28524,28578,28631,28684,28737,
	28790,28842,28894,28946,28998,29049,29100,29150,29201,29251,29301,29350,29399,29447,29496,29544,
	29592,29639,29686,29733,29779,29825,29871,29917,29962,30007,30052,30096,30140,30183,30226,30269,
	30312,30354,30396,30437,30478,30519,30560,30600,30640,30679,30718,30757,30795,30833,30871,30909,
	30946,30982,31019,31055,31090,31125,31160,31195,31229,31263,31297,31330,31363,31395,31427,31459,
	31490,31521,31552,31582,31612,31641,31670,31699,31727,31755,31783,31810,31837,31864,31890,31916,
	31941,31966,31991,32015,32039,32063,32086,32108,32131,32153,32175,32196,32217,32237,32257,32277,
	32296,32315,32334,32352,32370,32387,32404,32421,32437,32453,32469,32484,32499,32513,32527,32540,
	32553,32566,32579,32591,32602,32613,32624,32635,32645,32654,32664,32673,32681,32689,32697,32704,
	32711,32717,32723,32729,32735,32740,32744,32748,32752,32755,32758,32761,32763,32765,32766,32767,
	32767,32767,32767,32766,32765,32764,32762,32760,32757,32754,32751,32747,32743,32739,32734,32728,
	32722,32716,32710,32703,32696,32688,32680,32672,32663,32653,32644,32634,32623,32612,32601,32590,
	32578,32565,32552,32539,32526,32512,32498,32483,32468,32452,32436,32420,32403,32386,32369,32351,
	32333,32314,32295,32276,32256,32236,32216,32195,32174,32152,32130,32107,32085,32062,32038,32014,
	31990,31965,31940,31915,31889,31863,31836,31809,31782,31754,31726,31698,31669,31640,31611,31581,
	31551,31520,31489,31458,31426,31394,31362,31329,31296,31262,31228,31194,31159,31124,31089,31054,
	31018,30981,30945,30908,30870,30832,30794,30756,30717,30678,30639,30599,30559,30518,30477,30436,
	30395,30353,30311,30268,30225,30182,30139,30095,30051,30006,29961,29916,29870,29824,29778,29732,
	29685,29638,29591,29543,29495,29446,29398,29349,29300,29250,29200,29149,29099,29048,28997,28945,
	28893,28841,28789,28736,28683,28630,28577,28523,28469,28414,28359,28304,28249,28193,28137,28081,
	28025,27968,27911,27854,27797,27739,27681,27622,27563,27504,27445,27386,27326,27266,27206,27145,
	27085,27024,26963,26901,26839,26777,26715,26652,26589,26526,26463,26399,26336,26272,26208,26143,
	26078,26013,25948,25883,25817,25751,25685,25619,25552,25485,25418,25351,25284,25216,25148,25080,
	25012,24944,24875,24806,24737,24668,24599,24529,24459,24389,24319,24248,24177,24106,24035,23964,
	23893,23821,23750,23678,23606,23533,23461,23388,23315,23242,23169,23096,23023,22949,22875,22801,
	22727,22653,22579,22504,22429,22354,22279,22204,22129,22054,21979,21903,21827,21751,21675,21599,
	21523,21446,21370,21293,21216,21139,21062,20985,20908,20830,20753,20675,20598,20520,20442,20364,
	20286,20208,20130,20051,19973,19894,19816,19737,19658,19579,19500,19421,19342,19263,19184,19105,
	19026,18946,18867,18787,18708,18628,18548,18468,18389,18309,18229,18149,18069,17989,17909,17829,
	17749,17668,17588,17508,17428,17348,17268,17187,17107,17026,16946,16865,16785,16705,16625,16544,
	16464,16383,16303,16222,16142,16061,15981,15901,15821,15740,15660,15579,15499,15418,15338,15258,
	15178,15098,15018,14937,14857,14777,14697,14617,14537,14457,14378,14298,14218,14138,14059,13979,
	13900,13820,13741,13661,13582,13503,13424,13345,13266,13187,13108,13029,12951,12872,12794,12715,
	12637,12558,12480,12402,12324,12246,12169,12091,12014,11936,11859,11781,11704,11627,11550,11473,
	11397,11320,11244,11167,11091,11015,10939,10863,10788,10712,10637,10562,10487,10412,10337,10262,
	10188,10113,10039,9965,9891,9817,9744,9670,9597,9524,9451,9378,9306,9233,9161,9088,
	9017,8945,8874,8802,8731,8660,8589,8518,8448,8377,8307,8237,8168,8098,8029,7960,
	7891,7822,7754,7686,7618,7550,7483,7415,7348,7281,7214,7147,7081,7015,6949,6883,
	6818,6753,6688,6623,6559,6494,6431,6367,6304,6240,6177,6114,6052,5989,5927,5865,
	5804,5742,5682,5621,5561,5500,5440,5380,5321,5262,5203,5144,5086,5027,4970,4912,
	4855,4798,4742,4685,4629,4573,4518,4462,4407,4352,4298,4243,4190,4136,4083,4030,
	3978,3925,3873,3821,3770,3718,3668,3617,3567,3516,3467,3417,3369,3320,3272,3223,
	3176,3128,3081,3034,2988,2942,2896,2850,2805,2760,2716,2671,2628,2584,2541,2498,
	2456,2413,2372,2330,2289,2248,2208,2167,2128,2088,2049,2010,1972,1934,1896,1858,
	1822,1785,1749,1712,1677,1642,1607,1572,1538,1504,1471,1437,1405,1372,1340,1308,
	1277,1246,1216,1185,1156,1126,1097,1068,1040,1012,985,957,930,903,877,851,
	826,801,777,752,728,704,682,659,637,614,593,571,551,530,510,490,
	471,452,434,415,398,380,363,346,330,314,299,283,269,254,241,227,
	214,201,189,176,165,154,143,132,123,113,104,94,86,78,71,63,
	57,50,44,38,33,27,23,19,16,12,9,6,4,2,1,0
	};
