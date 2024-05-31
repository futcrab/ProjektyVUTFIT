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



char *work_p_0012275476_sub_3119652960_1822264302(char *t1, char *t2, char *t3, unsigned int t4, unsigned int t5, char *t6, int t7, int t8)
{
    char t9[144];
    char t10[16];
    char t17[8];
    char t23[8];
    char *t0;
    char *t11;
    unsigned int t12;
    int t13;
    char *t14;
    char *t15;
    char *t16;
    char *t18;
    char *t19;
    char *t20;
    char *t21;
    char *t22;
    char *t24;
    char *t25;
    char *t26;
    char *t27;
    char *t28;
    char *t29;
    int t30;
    unsigned char t31;
    char *t32;
    int t33;
    int t34;
    int t35;
    int t36;
    int t37;
    int t38;
    int t39;
    unsigned int t40;
    unsigned int t41;
    int t42;
    int t43;
    int t44;
    int t45;
    int t46;
    int t47;
    int t48;
    unsigned int t49;
    unsigned int t50;
    int t51;
    int t52;
    int t53;
    int t54;
    int t55;
    int t56;
    char *t57;
    int t58;
    unsigned int t59;

LAB0:    t11 = (t6 + 12U);
    t12 = *((unsigned int *)t11);
    t13 = (t12 / t8);
    t14 = (t9 + 4U);
    t15 = ((STD_STANDARD) + 240);
    t16 = (t14 + 52U);
    *((char **)t16) = t15;
    t18 = (t14 + 36U);
    *((char **)t18) = t17;
    *((int *)t17) = t13;
    t19 = (t14 + 48U);
    *((unsigned int *)t19) = 4U;
    t20 = (t9 + 72U);
    t21 = ((STD_STANDARD) + 240);
    t22 = (t20 + 52U);
    *((char **)t22) = t21;
    t24 = (t20 + 36U);
    *((char **)t24) = t23;
    *((int *)t23) = t7;
    t25 = (t20 + 48U);
    *((unsigned int *)t25) = 4U;
    t26 = (t10 + 4U);
    *((int *)t26) = t7;
    t27 = (t10 + 8U);
    *((int *)t27) = t8;
    t28 = (t14 + 36U);
    t29 = *((char **)t28);
    t30 = *((int *)t29);
    t31 = (t7 >= t30);
    if (t31 != 0)
        goto LAB2;

LAB4:    t31 = (t7 < 0);
    if (t31 != 0)
        goto LAB5;

LAB6:
LAB3:    t11 = (t3 + 24U);
    t15 = *((char **)t11);
    t11 = (t15 + t5);
    t15 = (t6 + 0U);
    t13 = *((int *)t15);
    t16 = (t20 + 36U);
    t18 = *((char **)t16);
    t30 = *((int *)t18);
    t33 = (t30 * t8);
    t12 = (t33 - t13);
    t16 = (t20 + 36U);
    t19 = *((char **)t16);
    t34 = *((int *)t19);
    t35 = (t34 + 1);
    t36 = (t35 * t8);
    t37 = (t36 - 1);
    t16 = (t6 + 4U);
    t38 = *((int *)t16);
    t21 = (t6 + 8U);
    t39 = *((int *)t21);
    xsi_vhdl_check_range_of_slice(t13, t38, t39, t33, t37, 1);
    t40 = (t12 * 1U);
    t41 = (0 + t40);
    t22 = (t11 + t41);
    t24 = (t20 + 36U);
    t25 = *((char **)t24);
    t42 = *((int *)t25);
    t43 = (t42 * t8);
    t24 = (t20 + 36U);
    t28 = *((char **)t24);
    t44 = *((int *)t28);
    t45 = (t44 + 1);
    t46 = (t45 * t8);
    t47 = (t46 - 1);
    t48 = (t47 - t43);
    t49 = (t48 * 1);
    t49 = (t49 + 1);
    t50 = (1U * t49);
    t0 = xsi_get_transient_memory(t50);
    memcpy(t0, t22, t50);
    t24 = (t20 + 36U);
    t29 = *((char **)t24);
    t51 = *((int *)t29);
    t52 = (t51 * t8);
    t24 = (t20 + 36U);
    t32 = *((char **)t24);
    t53 = *((int *)t32);
    t54 = (t53 + 1);
    t55 = (t54 * t8);
    t56 = (t55 - 1);
    t24 = (t2 + 0U);
    t57 = (t24 + 0U);
    *((int *)t57) = t52;
    t57 = (t24 + 4U);
    *((int *)t57) = t56;
    t57 = (t24 + 8U);
    *((int *)t57) = 1;
    t58 = (t56 - t52);
    t59 = (t58 * 1);
    t59 = (t59 + 1);
    t57 = (t24 + 12U);
    *((unsigned int *)t57) = t59;

LAB1:    return t0;
LAB2:    t28 = (t20 + 36U);
    t32 = *((char **)t28);
    t28 = (t32 + 0);
    *((int *)t28) = 0;
    goto LAB3;

LAB5:    t11 = (t14 + 36U);
    t15 = *((char **)t11);
    t13 = *((int *)t15);
    t30 = (t13 - 1);
    t11 = (t20 + 36U);
    t16 = *((char **)t11);
    t11 = (t16 + 0);
    *((int *)t11) = t30;
    goto LAB3;

LAB7:;
}

int work_p_0012275476_sub_384483872_1822264302(char *t1, int t2)
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
	static char *se[] = {(void *)work_p_0012275476_sub_3119652960_1822264302,(void *)work_p_0012275476_sub_384483872_1822264302};
	xsi_register_didat("work_p_0012275476", "isim/tlv_gp_ifc_isim_beh.exe.sim/work/p_0012275476.didat");
	xsi_register_subprogram_executes(se);
}
