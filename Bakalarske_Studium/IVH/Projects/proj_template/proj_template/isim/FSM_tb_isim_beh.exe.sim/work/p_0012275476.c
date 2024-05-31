/**********************************************************************/
/*   ____  ____                                                       */
/*  /   /\/   /                                                       */
/* /___/  \  /                                                        */
/* \   \   \/                                                       */
/*  \   \        Copyright (c) 2003-2009 Xilinx, Inc.                */
/*  /   /          All Right Reserved.                                 */
/* /---/   /\                                                         */
/* \   \  /  \                                                      */
/*  \___\/\___\                                                    */
/***********************************************************************/

/* This file is designed for use with ISim build 0x79f3f3a8 */

#define XSI_HIDE_SYMBOL_SPEC true
#include "xsi.h"
#include <memory.h>
#ifdef __GNUC__
#include <stdlib.h>
#else
#include <malloc.h>
#define alloca _alloca
#endif
extern char *STD_STANDARD;



int work_p_0012275476_sub_2062509486_1822264302(char *t1, int t2, int t3)
{
    char t4[72];
    char t5[16];
    char t9[8];
    int t0;
    char *t6;
    char *t7;
    char *t8;
    char *t10;
    char *t11;
    char *t12;
    char *t13;
    unsigned char t14;
    char *t15;
    char *t16;
    int t17;

LAB0:    t6 = (t4 + 4U);
    t7 = ((STD_STANDARD) + 240);
    t8 = (t6 + 52U);
    *((char **)t8) = t7;
    t10 = (t6 + 36U);
    *((char **)t10) = t9;
    xsi_type_set_default_value(t7, t9, 0);
    t11 = (t6 + 48U);
    *((unsigned int *)t11) = 4U;
    t12 = (t5 + 4U);
    *((int *)t12) = t2;
    t13 = (t5 + 8U);
    *((int *)t13) = t3;
    t14 = (t2 < 0);
    if (t14 != 0)
        goto LAB2;

LAB4:    t14 = (t2 >= t3);
    if (t14 != 0)
        goto LAB5;

LAB6:    t17 = (t2 + 1);
    t7 = (t6 + 36U);
    t8 = *((char **)t7);
    t7 = (t8 + 0);
    *((int *)t7) = t17;

LAB3:    t7 = (t6 + 36U);
    t8 = *((char **)t7);
    t17 = *((int *)t8);
    t0 = t17;

LAB1:    return t0;
LAB2:    t15 = (t6 + 36U);
    t16 = *((char **)t15);
    t15 = (t16 + 0);
    *((int *)t15) = t3;
    goto LAB3;

LAB5:    t7 = (t6 + 36U);
    t8 = *((char **)t7);
    t7 = (t8 + 0);
    *((int *)t7) = 1;
    goto LAB3;

LAB7:;
}

int work_p_0012275476_sub_320051366_1822264302(char *t1, int t2)
{
    char t3[72];
    char t4[8];
    char t8[8];
    int t0;
    char *t5;
    char *t6;
    char *t7;
    char *t9;
    char *t10;
    char *t11;
    char *t12;
    char *t13;
    int t14;
    unsigned char t15;
    char *t16;
    int t17;
    int t18;
    char *t19;

LAB0:    t5 = (t3 + 4U);
    t6 = ((STD_STANDARD) + 552);
    t7 = (t5 + 52U);
    *((char **)t7) = t6;
    t9 = (t5 + 36U);
    *((char **)t9) = t8;
    *((int *)t8) = 1;
    t10 = (t5 + 48U);
    *((unsigned int *)t10) = 4U;
    t11 = (t4 + 4U);
    *((int *)t11) = t2;

LAB2:    t12 = (t5 + 36U);
    t13 = *((char **)t12);
    t14 = *((int *)t13);
    t15 = (t14 < t2);
    if (t15 != 0)
        goto LAB3;

LAB5:    t6 = (t5 + 36U);
    t7 = *((char **)t6);
    t14 = *((int *)t7);
    t0 = t14;

LAB1:    return t0;
LAB3:    t12 = (t5 + 36U);
    t16 = *((char **)t12);
    t17 = *((int *)t16);
    t18 = (t17 * 2);
    t12 = (t5 + 36U);
    t19 = *((char **)t12);
    t12 = (t19 + 0);
    *((int *)t12) = t18;
    goto LAB2;

LAB4:;
LAB6:;
}


extern void work_p_0012275476_init()
{
	static char *se[] = {(void *)work_p_0012275476_sub_2062509486_1822264302,(void *)work_p_0012275476_sub_320051366_1822264302};
	xsi_register_didat("work_p_0012275476", "isim/FSM_tb_isim_beh.exe.sim/work/p_0012275476.didat");
	xsi_register_subprogram_executes(se);
}
