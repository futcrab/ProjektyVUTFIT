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
static const char *ng0 = "/mnt/c/Users/duric/OneDrive/Dokumenty/skola/ivh/Projects/proj_template/CNT.vhd";
extern char *IEEE_P_2592010699;
extern char *IEEE_P_1242562249;
extern char *IEEE_P_3620187407;

char *ieee_p_1242562249_sub_10420449594411817395_1035706684(char *, char *, int , int );
unsigned char ieee_p_2592010699_sub_2763492388968962707_503743352(char *, char *, unsigned int , unsigned int );
char *ieee_p_3620187407_sub_2255506239096166994_3965413181(char *, char *, char *, char *, int );


static void work_a_3135090559_3212880686_p_0(char *t0)
{
    char t7[16];
    char *t1;
    unsigned char t2;
    char *t3;
    char *t4;
    unsigned char t5;
    unsigned char t6;
    int t8;
    char *t9;
    char *t10;
    char *t11;
    char *t12;
    char *t13;
    unsigned int t14;
    unsigned int t15;
    char *t16;

LAB0:    xsi_set_current_line(23, ng0);
    t1 = (t0 + 992U);
    t2 = ieee_p_2592010699_sub_2763492388968962707_503743352(IEEE_P_2592010699, t1, 0U, 0U);
    if (t2 != 0)
        goto LAB2;

LAB4:
LAB3:    t1 = (t0 + 3384);
    *((int *)t1) = 1;

LAB1:    return;
LAB2:    xsi_set_current_line(24, ng0);
    t3 = (t0 + 1192U);
    t4 = *((char **)t3);
    t5 = *((unsigned char *)t4);
    t6 = (t5 == (unsigned char)3);
    if (t6 != 0)
        goto LAB5;

LAB7:    xsi_set_current_line(34, ng0);
    t1 = (t0 + 3592);
    t3 = (t1 + 56U);
    t4 = *((char **)t3);
    t9 = (t4 + 56U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)2;
    xsi_driver_first_trans_fast_port(t1);

LAB6:    goto LAB3;

LAB5:    xsi_set_current_line(25, ng0);
    t8 = (4166667 - 1);
    t3 = ieee_p_1242562249_sub_10420449594411817395_1035706684(IEEE_P_1242562249, t7, t8, 22);
    t9 = (t0 + 3464);
    t10 = (t9 + 56U);
    t11 = *((char **)t10);
    t12 = (t11 + 56U);
    t13 = *((char **)t12);
    memcpy(t13, t3, 22U);
    xsi_driver_first_trans_fast(t9);
    xsi_set_current_line(26, ng0);
    t1 = (t0 + 1512U);
    t3 = *((char **)t1);
    t1 = (t0 + 5728U);
    t4 = ieee_p_3620187407_sub_2255506239096166994_3965413181(IEEE_P_3620187407, t7, t3, t1, 1);
    t9 = (t7 + 12U);
    t14 = *((unsigned int *)t9);
    t15 = (1U * t14);
    t2 = (22U != t15);
    if (t2 == 1)
        goto LAB8;

LAB9:    t10 = (t0 + 3528);
    t11 = (t10 + 56U);
    t12 = *((char **)t11);
    t13 = (t12 + 56U);
    t16 = *((char **)t13);
    memcpy(t16, t4, 22U);
    xsi_driver_first_trans_fast(t10);
    xsi_set_current_line(27, ng0);
    t1 = (t0 + 1512U);
    t3 = *((char **)t1);
    t1 = (t0 + 5728U);
    t4 = (t0 + 1672U);
    t9 = *((char **)t4);
    t4 = (t0 + 5744U);
    t2 = ieee_std_logic_unsigned_equal_stdv_stdv(IEEE_P_3620187407, t3, t1, t9, t4);
    if (t2 != 0)
        goto LAB10;

LAB12:    xsi_set_current_line(31, ng0);
    t1 = (t0 + 3592);
    t3 = (t1 + 56U);
    t4 = *((char **)t3);
    t9 = (t4 + 56U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)2;
    xsi_driver_first_trans_fast_port(t1);

LAB11:    goto LAB6;

LAB8:    xsi_size_not_matching(22U, t15, 0);
    goto LAB9;

LAB10:    xsi_set_current_line(28, ng0);
    t10 = (t0 + 3592);
    t11 = (t10 + 56U);
    t12 = *((char **)t11);
    t13 = (t12 + 56U);
    t16 = *((char **)t13);
    *((unsigned char *)t16) = (unsigned char)3;
    xsi_driver_first_trans_fast_port(t10);
    xsi_set_current_line(29, ng0);
    t1 = xsi_get_transient_memory(22U);
    memset(t1, 0, 22U);
    t3 = t1;
    memset(t3, (unsigned char)2, 22U);
    t4 = (t0 + 3528);
    t9 = (t4 + 56U);
    t10 = *((char **)t9);
    t11 = (t10 + 56U);
    t12 = *((char **)t11);
    memcpy(t12, t1, 22U);
    xsi_driver_first_trans_fast(t4);
    goto LAB11;

}


extern void work_a_3135090559_3212880686_init()
{
	static char *pe[] = {(void *)work_a_3135090559_3212880686_p_0};
	xsi_register_didat("work_a_3135090559_3212880686", "isim/fsm_tb_isim_beh.exe.sim/work/a_3135090559_3212880686.didat");
	xsi_register_executes(pe);
}
