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
extern char *STD_STANDARD;
extern char *IEEE_P_2592010699;



char *work_p_0012275476_sub_12585583252958868170_1822264302(char *t1, char *t2, char *t3, unsigned int t4, unsigned int t5, char *t6, int t7, int t8)
{
    char t9[368];
    char t10[16];
    char t14[8];
    char t20[8];
    char t26[16];
    char *t0;
    char *t11;
    char *t12;
    char *t13;
    char *t15;
    char *t16;
    char *t17;
    char *t18;
    char *t19;
    char *t21;
    char *t22;
    int t23;
    int t24;
    unsigned int t25;
    int t27;
    char *t28;
    char *t29;
    int t30;
    unsigned int t31;
    char *t32;
    char *t33;
    char *t34;
    char *t35;
    char *t36;
    char *t37;
    char *t38;
    char *t39;
    char *t40;
    int t41;
    char *t42;
    char *t43;
    unsigned char t44;
    int t45;
    int t46;
    int t47;
    int t48;
    unsigned int t49;
    int t50;
    int t51;
    int t52;
    int t53;
    int t54;
    int t55;
    int t56;
    unsigned int t57;
    unsigned int t58;

LAB0:    t11 = (t9 + 4U);
    t12 = ((STD_STANDARD) + 384);
    t13 = (t11 + 88U);
    *((char **)t13) = t12;
    t15 = (t11 + 56U);
    *((char **)t15) = t14;
    xsi_type_set_default_value(t12, t14, 0);
    t16 = (t11 + 80U);
    *((unsigned int *)t16) = 4U;
    t17 = (t9 + 124U);
    t18 = ((STD_STANDARD) + 384);
    t19 = (t17 + 88U);
    *((char **)t19) = t18;
    t21 = (t17 + 56U);
    *((char **)t21) = t20;
    xsi_type_set_default_value(t18, t20, 0);
    t22 = (t17 + 80U);
    *((unsigned int *)t22) = 4U;
    t23 = (t8 - 1);
    t24 = (t23 - 0);
    t25 = (t24 * 1);
    t25 = (t25 + 1);
    t25 = (t25 * 1U);
    t27 = (t8 - 1);
    t28 = (t26 + 0U);
    t29 = (t28 + 0U);
    *((int *)t29) = 0;
    t29 = (t28 + 4U);
    *((int *)t29) = t27;
    t29 = (t28 + 8U);
    *((int *)t29) = 1;
    t30 = (t27 - 0);
    t31 = (t30 * 1);
    t31 = (t31 + 1);
    t29 = (t28 + 12U);
    *((unsigned int *)t29) = t31;
    t29 = (t9 + 244U);
    t32 = ((IEEE_P_2592010699) + 4000);
    t33 = (t29 + 88U);
    *((char **)t33) = t32;
    t34 = (char *)alloca(t25);
    t35 = (t29 + 56U);
    *((char **)t35) = t34;
    xsi_type_set_default_value(t32, t34, t26);
    t36 = (t29 + 64U);
    *((char **)t36) = t26;
    t37 = (t29 + 80U);
    *((unsigned int *)t37) = t25;
    t38 = (t10 + 4U);
    *((int *)t38) = t7;
    t39 = (t10 + 8U);
    *((int *)t39) = t8;
    t40 = (t6 + 12U);
    t31 = *((unsigned int *)t40);
    t41 = (t31 / t8);
    t42 = (t11 + 56U);
    t43 = *((char **)t42);
    t42 = (t43 + 0);
    *((int *)t42) = t41;
    t44 = (t7 < 0);
    if (t44 != 0)
        goto LAB2;

LAB4:    t12 = (t11 + 56U);
    t13 = *((char **)t12);
    t23 = *((int *)t13);
    t44 = (t7 >= t23);
    if (t44 != 0)
        goto LAB5;

LAB6:    t12 = (t17 + 56U);
    t13 = *((char **)t12);
    t12 = (t13 + 0);
    *((int *)t12) = t7;

LAB3:    t12 = (t3 + 40U);
    t13 = *((char **)t12);
    t12 = (t13 + t5);
    t13 = (t6 + 0U);
    t23 = *((int *)t13);
    t15 = (t17 + 56U);
    t16 = *((char **)t15);
    t24 = *((int *)t16);
    t27 = (t24 * t8);
    t30 = (t27 + t8);
    t41 = (t30 - 1);
    t25 = (t23 - t41);
    t15 = (t17 + 56U);
    t18 = *((char **)t15);
    t45 = *((int *)t18);
    t46 = (t45 * t8);
    t15 = (t6 + 4U);
    t47 = *((int *)t15);
    t19 = (t6 + 8U);
    t48 = *((int *)t19);
    xsi_vhdl_check_range_of_slice(t23, t47, t48, t41, t46, -1);
    t31 = (t25 * 1U);
    t49 = (0 + t31);
    t21 = (t12 + t49);
    t22 = (t29 + 56U);
    t28 = *((char **)t22);
    t22 = (t28 + 0);
    t32 = (t17 + 56U);
    t33 = *((char **)t32);
    t50 = *((int *)t33);
    t51 = (t50 * t8);
    t52 = (t51 + t8);
    t53 = (t52 - 1);
    t32 = (t17 + 56U);
    t35 = *((char **)t32);
    t54 = *((int *)t35);
    t55 = (t54 * t8);
    t56 = (t55 - t53);
    t57 = (t56 * -1);
    t57 = (t57 + 1);
    t58 = (1U * t57);
    memcpy(t22, t21, t58);
    t12 = (t29 + 56U);
    t13 = *((char **)t12);
    t12 = (t26 + 12U);
    t25 = *((unsigned int *)t12);
    t25 = (t25 * 1U);
    t0 = xsi_get_transient_memory(t25);
    memcpy(t0, t13, t25);
    t15 = (t26 + 0U);
    t23 = *((int *)t15);
    t16 = (t26 + 4U);
    t24 = *((int *)t16);
    t18 = (t26 + 8U);
    t27 = *((int *)t18);
    t19 = (t2 + 0U);
    t21 = (t19 + 0U);
    *((int *)t21) = t23;
    t21 = (t19 + 4U);
    *((int *)t21) = t24;
    t21 = (t19 + 8U);
    *((int *)t21) = t27;
    t30 = (t24 - t23);
    t31 = (t30 * t27);
    t31 = (t31 + 1);
    t21 = (t19 + 12U);
    *((unsigned int *)t21) = t31;

LAB1:    return t0;
LAB2:    t12 = (t11 + 56U);
    t13 = *((char **)t12);
    t23 = *((int *)t13);
    t24 = (t23 - 1);
    t12 = (t17 + 56U);
    t15 = *((char **)t12);
    t12 = (t15 + 0);
    *((int *)t12) = t24;
    goto LAB3;

LAB5:    t12 = (t17 + 56U);
    t15 = *((char **)t12);
    t12 = (t15 + 0);
    *((int *)t12) = 0;
    goto LAB3;

LAB7:;
}

int work_p_0012275476_sub_14377937632486463654_1822264302(char *t1, int t2)
{
    char t3[128];
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
    t6 = ((STD_STANDARD) + 824);
    t7 = (t5 + 88U);
    *((char **)t7) = t6;
    t9 = (t5 + 56U);
    *((char **)t9) = t8;
    *((int *)t8) = 1;
    t10 = (t5 + 80U);
    *((unsigned int *)t10) = 4U;
    t11 = (t4 + 4U);
    *((int *)t11) = t2;

LAB2:    t12 = (t5 + 56U);
    t13 = *((char **)t12);
    t14 = *((int *)t13);
    t15 = (t14 < t2);
    if (t15 != 0)
        goto LAB3;

LAB5:    t6 = (t5 + 56U);
    t7 = *((char **)t6);
    t14 = *((int *)t7);
    t0 = t14;

LAB1:    return t0;
LAB3:    t12 = (t5 + 56U);
    t16 = *((char **)t12);
    t17 = *((int *)t16);
    t18 = (t17 * 2);
    t12 = (t5 + 56U);
    t19 = *((char **)t12);
    t12 = (t19 + 0);
    *((int *)t12) = t18;
    goto LAB2;

LAB4:;
LAB6:;
}


extern void work_p_0012275476_init()
{
	static char *se[] = {(void *)work_p_0012275476_sub_12585583252958868170_1822264302,(void *)work_p_0012275476_sub_14377937632486463654_1822264302};
	xsi_register_didat("work_p_0012275476", "isim/fsm_tb_isim_beh.exe.sim/work/p_0012275476.didat");
	xsi_register_subprogram_executes(se);
}
