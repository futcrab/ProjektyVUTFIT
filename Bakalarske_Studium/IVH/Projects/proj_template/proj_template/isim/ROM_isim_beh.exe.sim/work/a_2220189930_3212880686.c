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
static const char *ng0 = "//VBoxSvr/fitkit-vbox-202103/proj_template/ROM.vhd";



static void work_a_2220189930_3212880686_p_0(char *t0)
{
    char *t1;
    char *t2;
    char *t3;
    int t4;
    char *t5;
    char *t6;
    int t7;
    char *t8;
    char *t9;
    int t10;
    char *t11;
    int t13;
    char *t14;
    int t16;
    char *t17;
    int t19;
    char *t20;
    int t22;
    char *t23;
    int t25;
    char *t26;
    int t28;
    char *t29;
    char *t30;
    int t31;
    unsigned int t32;
    unsigned int t33;
    unsigned int t34;
    char *t35;
    char *t36;
    char *t37;
    char *t38;
    char *t39;

LAB0:    xsi_set_current_line(35, ng0);
    t1 = (t0 + 592U);
    t2 = *((char **)t1);
    t1 = (t0 + 4232);
    t4 = xsi_mem_cmp(t1, t2, 4U);
    if (t4 == 1)
        goto LAB3;

LAB13:    t5 = (t0 + 4236);
    t7 = xsi_mem_cmp(t5, t2, 4U);
    if (t7 == 1)
        goto LAB4;

LAB14:    t8 = (t0 + 4240);
    t10 = xsi_mem_cmp(t8, t2, 4U);
    if (t10 == 1)
        goto LAB5;

LAB15:    t11 = (t0 + 4244);
    t13 = xsi_mem_cmp(t11, t2, 4U);
    if (t13 == 1)
        goto LAB6;

LAB16:    t14 = (t0 + 4248);
    t16 = xsi_mem_cmp(t14, t2, 4U);
    if (t16 == 1)
        goto LAB7;

LAB17:    t17 = (t0 + 4252);
    t19 = xsi_mem_cmp(t17, t2, 4U);
    if (t19 == 1)
        goto LAB8;

LAB18:    t20 = (t0 + 4256);
    t22 = xsi_mem_cmp(t20, t2, 4U);
    if (t22 == 1)
        goto LAB9;

LAB19:    t23 = (t0 + 4260);
    t25 = xsi_mem_cmp(t23, t2, 4U);
    if (t25 == 1)
        goto LAB10;

LAB20:    t26 = (t0 + 4264);
    t28 = xsi_mem_cmp(t26, t2, 4U);
    if (t28 == 1)
        goto LAB11;

LAB21:
LAB12:    xsi_set_current_line(45, ng0);
    t1 = (t0 + 856U);
    t2 = *((char **)t1);
    t4 = (3 - 0);
    t32 = (t4 * 1);
    t33 = (128U * t32);
    t34 = (0 + t33);
    t1 = (t2 + t34);
    t3 = (t0 + 1652);
    t5 = (t3 + 32U);
    t6 = *((char **)t5);
    t8 = (t6 + 40U);
    t9 = *((char **)t8);
    memcpy(t9, t1, 128U);
    xsi_driver_first_trans_fast_port(t3);

LAB2:    t1 = (t0 + 1608);
    *((int *)t1) = 1;

LAB1:    return;
LAB3:    xsi_set_current_line(36, ng0);
    t29 = (t0 + 856U);
    t30 = *((char **)t29);
    t31 = (0 - 0);
    t32 = (t31 * 1);
    t33 = (128U * t32);
    t34 = (0 + t33);
    t29 = (t30 + t34);
    t35 = (t0 + 1652);
    t36 = (t35 + 32U);
    t37 = *((char **)t36);
    t38 = (t37 + 40U);
    t39 = *((char **)t38);
    memcpy(t39, t29, 128U);
    xsi_driver_first_trans_fast_port(t35);
    goto LAB2;

LAB4:    xsi_set_current_line(37, ng0);
    t1 = (t0 + 856U);
    t2 = *((char **)t1);
    t4 = (1 - 0);
    t32 = (t4 * 1);
    t33 = (128U * t32);
    t34 = (0 + t33);
    t1 = (t2 + t34);
    t3 = (t0 + 1652);
    t5 = (t3 + 32U);
    t6 = *((char **)t5);
    t8 = (t6 + 40U);
    t9 = *((char **)t8);
    memcpy(t9, t1, 128U);
    xsi_driver_first_trans_fast_port(t3);
    goto LAB2;

LAB5:    xsi_set_current_line(38, ng0);
    t1 = (t0 + 856U);
    t2 = *((char **)t1);
    t4 = (2 - 0);
    t32 = (t4 * 1);
    t33 = (128U * t32);
    t34 = (0 + t33);
    t1 = (t2 + t34);
    t3 = (t0 + 1652);
    t5 = (t3 + 32U);
    t6 = *((char **)t5);
    t8 = (t6 + 40U);
    t9 = *((char **)t8);
    memcpy(t9, t1, 128U);
    xsi_driver_first_trans_fast_port(t3);
    goto LAB2;

LAB6:    xsi_set_current_line(39, ng0);
    t1 = (t0 + 856U);
    t2 = *((char **)t1);
    t4 = (3 - 0);
    t32 = (t4 * 1);
    t33 = (128U * t32);
    t34 = (0 + t33);
    t1 = (t2 + t34);
    t3 = (t0 + 1652);
    t5 = (t3 + 32U);
    t6 = *((char **)t5);
    t8 = (t6 + 40U);
    t9 = *((char **)t8);
    memcpy(t9, t1, 128U);
    xsi_driver_first_trans_fast_port(t3);
    goto LAB2;

LAB7:    xsi_set_current_line(40, ng0);
    t1 = (t0 + 856U);
    t2 = *((char **)t1);
    t4 = (4 - 0);
    t32 = (t4 * 1);
    t33 = (128U * t32);
    t34 = (0 + t33);
    t1 = (t2 + t34);
    t3 = (t0 + 1652);
    t5 = (t3 + 32U);
    t6 = *((char **)t5);
    t8 = (t6 + 40U);
    t9 = *((char **)t8);
    memcpy(t9, t1, 128U);
    xsi_driver_first_trans_fast_port(t3);
    goto LAB2;

LAB8:    xsi_set_current_line(41, ng0);
    t1 = (t0 + 856U);
    t2 = *((char **)t1);
    t4 = (5 - 0);
    t32 = (t4 * 1);
    t33 = (128U * t32);
    t34 = (0 + t33);
    t1 = (t2 + t34);
    t3 = (t0 + 1652);
    t5 = (t3 + 32U);
    t6 = *((char **)t5);
    t8 = (t6 + 40U);
    t9 = *((char **)t8);
    memcpy(t9, t1, 128U);
    xsi_driver_first_trans_fast_port(t3);
    goto LAB2;

LAB9:    xsi_set_current_line(42, ng0);
    t1 = (t0 + 856U);
    t2 = *((char **)t1);
    t4 = (6 - 0);
    t32 = (t4 * 1);
    t33 = (128U * t32);
    t34 = (0 + t33);
    t1 = (t2 + t34);
    t3 = (t0 + 1652);
    t5 = (t3 + 32U);
    t6 = *((char **)t5);
    t8 = (t6 + 40U);
    t9 = *((char **)t8);
    memcpy(t9, t1, 128U);
    xsi_driver_first_trans_fast_port(t3);
    goto LAB2;

LAB10:    xsi_set_current_line(43, ng0);
    t1 = (t0 + 856U);
    t2 = *((char **)t1);
    t4 = (7 - 0);
    t32 = (t4 * 1);
    t33 = (128U * t32);
    t34 = (0 + t33);
    t1 = (t2 + t34);
    t3 = (t0 + 1652);
    t5 = (t3 + 32U);
    t6 = *((char **)t5);
    t8 = (t6 + 40U);
    t9 = *((char **)t8);
    memcpy(t9, t1, 128U);
    xsi_driver_first_trans_fast_port(t3);
    goto LAB2;

LAB11:    xsi_set_current_line(44, ng0);
    t1 = (t0 + 856U);
    t2 = *((char **)t1);
    t4 = (8 - 0);
    t32 = (t4 * 1);
    t33 = (128U * t32);
    t34 = (0 + t33);
    t1 = (t2 + t34);
    t3 = (t0 + 1652);
    t5 = (t3 + 32U);
    t6 = *((char **)t5);
    t8 = (t6 + 40U);
    t9 = *((char **)t8);
    memcpy(t9, t1, 128U);
    xsi_driver_first_trans_fast_port(t3);
    goto LAB2;

LAB22:;
}


extern void work_a_2220189930_3212880686_init()
{
	static char *pe[] = {(void *)work_a_2220189930_3212880686_p_0};
	xsi_register_didat("work_a_2220189930_3212880686", "isim/ROM_isim_beh.exe.sim/work/a_2220189930_3212880686.didat");
	xsi_register_executes(pe);
}
