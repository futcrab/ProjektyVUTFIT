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

/* This file is designed for use with ISim build 0xfbc00daa */

#define XSI_HIDE_SYMBOL_SPEC true
#include "xsi.h"
#include <memory.h>
#ifdef __GNUC__
#include <stdlib.h>
#else
#include <malloc.h>
#define alloca _alloca
#endif
static const char *ng0 = "/mnt/c/Users/duric/OneDrive/Dokumenty/skola/ivh/Projects/proj_template/rom_tb.vhd";



static void work_a_3448919454_2372691052_p_0(char *t0)
{
    char *t1;
    char *t2;
    int64 t3;
    char *t4;
    int t5;
    int t6;
    char *t7;
    char *t8;
    char *t9;
    char *t10;
    char *t11;
    char *t12;
    int t13;

LAB0:    t1 = (t0 + 2344U);
    t2 = *((char **)t1);
    if (t2 == 0)
        goto LAB2;

LAB3:    goto *t2;

LAB2:    xsi_set_current_line(39, ng0);
    t3 = (10 * 1000LL);
    t2 = (t0 + 2152);
    xsi_process_wait(t2, t3);

LAB6:    *((char **)t1) = &&LAB7;

LAB1:    return;
LAB4:    xsi_set_current_line(40, ng0);
    t2 = (t0 + 4604);
    *((int *)t2) = 0;
    t4 = (t0 + 4608);
    *((int *)t4) = 7;
    t5 = 0;
    t6 = 7;

LAB8:    if (t5 <= t6)
        goto LAB9;

LAB11:    xsi_set_current_line(45, ng0);
    t2 = (t0 + 4612);
    *((int *)t2) = 36;
    t4 = (t0 + 4616);
    *((int *)t4) = 43;
    t5 = 36;
    t6 = 43;

LAB17:    if (t5 <= t6)
        goto LAB18;

LAB20:    xsi_set_current_line(50, ng0);

LAB28:    *((char **)t1) = &&LAB29;
    goto LAB1;

LAB5:    goto LAB4;

LAB7:    goto LAB5;

LAB9:    xsi_set_current_line(41, ng0);
    t7 = (t0 + 4604);
    t8 = (t0 + 2728);
    t9 = (t8 + 56U);
    t10 = *((char **)t9);
    t11 = (t10 + 56U);
    t12 = *((char **)t11);
    *((int *)t12) = *((int *)t7);
    xsi_driver_first_trans_fast(t8);
    xsi_set_current_line(42, ng0);
    t3 = (1 * 1000LL);
    t2 = (t0 + 2152);
    xsi_process_wait(t2, t3);

LAB14:    *((char **)t1) = &&LAB15;
    goto LAB1;

LAB10:    t2 = (t0 + 4604);
    t5 = *((int *)t2);
    t4 = (t0 + 4608);
    t6 = *((int *)t4);
    if (t5 == t6)
        goto LAB11;

LAB16:    t13 = (t5 + 1);
    t5 = t13;
    t7 = (t0 + 4604);
    *((int *)t7) = t5;
    goto LAB8;

LAB12:    goto LAB10;

LAB13:    goto LAB12;

LAB15:    goto LAB13;

LAB18:    xsi_set_current_line(46, ng0);
    t7 = (t0 + 4612);
    t8 = (t0 + 2728);
    t9 = (t8 + 56U);
    t10 = *((char **)t9);
    t11 = (t10 + 56U);
    t12 = *((char **)t11);
    *((int *)t12) = *((int *)t7);
    xsi_driver_first_trans_fast(t8);
    xsi_set_current_line(47, ng0);
    t3 = (1 * 1000LL);
    t2 = (t0 + 2152);
    xsi_process_wait(t2, t3);

LAB23:    *((char **)t1) = &&LAB24;
    goto LAB1;

LAB19:    t2 = (t0 + 4612);
    t5 = *((int *)t2);
    t4 = (t0 + 4616);
    t6 = *((int *)t4);
    if (t5 == t6)
        goto LAB20;

LAB25:    t13 = (t5 + 1);
    t5 = t13;
    t7 = (t0 + 4612);
    *((int *)t7) = t5;
    goto LAB17;

LAB21:    goto LAB19;

LAB22:    goto LAB21;

LAB24:    goto LAB22;

LAB26:    goto LAB2;

LAB27:    goto LAB26;

LAB29:    goto LAB27;

}


extern void work_a_3448919454_2372691052_init()
{
	static char *pe[] = {(void *)work_a_3448919454_2372691052_p_0};
	xsi_register_didat("work_a_3448919454_2372691052", "isim/rom_tb_isim_beh.exe.sim/work/a_3448919454_2372691052.didat");
	xsi_register_executes(pe);
}
