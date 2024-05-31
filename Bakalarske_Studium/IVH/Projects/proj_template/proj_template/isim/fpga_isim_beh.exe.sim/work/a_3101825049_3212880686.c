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
static const char *ng0 = "//VBoxSvr/fitkit-vbox-202103/proj_template/counter_N.vhd";
extern char *IEEE_P_2592010699;
extern char *IEEE_P_3620187407;

unsigned char ieee_p_2592010699_sub_1744673427_503743352(char *, char *, unsigned int , unsigned int );
char *ieee_p_3620187407_sub_436279890_3965413181(char *, char *, char *, char *, int );


static void work_a_3101825049_3212880686_p_0(char *t0)
{
    char t8[16];
    char *t1;
    char *t2;
    unsigned char t3;
    char *t4;
    char *t5;
    unsigned char t6;
    unsigned char t7;
    char *t9;
    char *t10;
    char *t11;
    char *t12;
    char *t13;
    char *t14;
    char *t15;

LAB0:    t1 = (t0 + 1688U);
    t2 = *((char **)t1);
    if (t2 == 0)
        goto LAB2;

LAB3:    goto *t2;

LAB2:    xsi_set_current_line(24, ng0);
    t2 = (t0 + 568U);
    t3 = ieee_p_2592010699_sub_1744673427_503743352(IEEE_P_2592010699, t2, 0U, 0U);
    if (t3 != 0)
        goto LAB4;

LAB6:
LAB5:    goto LAB2;

LAB4:    xsi_set_current_line(25, ng0);
    t4 = (t0 + 776U);
    t5 = *((char **)t4);
    t6 = *((unsigned char *)t5);
    t7 = (t6 == (unsigned char)3);
    if (t7 != 0)
        goto LAB7;

LAB9:    xsi_set_current_line(34, ng0);
    t2 = (t0 + 1956);
    t4 = (t2 + 32U);
    t5 = *((char **)t4);
    t9 = (t5 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)2;
    xsi_driver_first_trans_fast_port(t2);
    xsi_set_current_line(35, ng0);
    t2 = xsi_get_transient_memory(13U);
    memset(t2, 0, 13U);
    t4 = t2;
    memset(t4, (unsigned char)2, 13U);
    t5 = (t0 + 1920);
    t9 = (t5 + 32U);
    t10 = *((char **)t9);
    t11 = (t10 + 40U);
    t12 = *((char **)t11);
    memcpy(t12, t2, 13U);
    xsi_driver_first_trans_fast(t5);

LAB8:    goto LAB5;

LAB7:    xsi_set_current_line(26, ng0);
    t4 = (t0 + 868U);
    t9 = *((char **)t4);
    t4 = (t0 + 3392U);
    t10 = ieee_p_3620187407_sub_436279890_3965413181(IEEE_P_3620187407, t8, t9, t4, 1);
    t11 = (t0 + 1920);
    t12 = (t11 + 32U);
    t13 = *((char **)t12);
    t14 = (t13 + 40U);
    t15 = *((char **)t14);
    memcpy(t15, t10, 13U);
    xsi_driver_first_trans_fast(t11);
    xsi_set_current_line(27, ng0);
    t2 = (t0 + 868U);
    t4 = *((char **)t2);
    t2 = (t0 + 3392U);
    t5 = (t0 + 960U);
    t9 = *((char **)t5);
    t5 = (t0 + 3408U);
    t3 = ieee_std_logic_unsigned_equal_stdv_stdv(IEEE_P_3620187407, t4, t2, t9, t5);
    if (t3 != 0)
        goto LAB10;

LAB12:    xsi_set_current_line(31, ng0);
    t2 = (t0 + 1956);
    t4 = (t2 + 32U);
    t5 = *((char **)t4);
    t9 = (t5 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)2;
    xsi_driver_first_trans_fast_port(t2);

LAB11:    goto LAB8;

LAB10:    xsi_set_current_line(28, ng0);
    t10 = (t0 + 1956);
    t11 = (t10 + 32U);
    t12 = *((char **)t11);
    t13 = (t12 + 40U);
    t14 = *((char **)t13);
    *((unsigned char *)t14) = (unsigned char)3;
    xsi_driver_first_trans_fast_port(t10);
    xsi_set_current_line(29, ng0);
    t2 = xsi_get_transient_memory(13U);
    memset(t2, 0, 13U);
    t4 = t2;
    memset(t4, (unsigned char)2, 13U);
    t5 = (t0 + 1920);
    t9 = (t5 + 32U);
    t10 = *((char **)t9);
    t11 = (t10 + 40U);
    t12 = *((char **)t11);
    memcpy(t12, t2, 13U);
    xsi_driver_first_trans_fast(t5);
    goto LAB11;

LAB1:    return;
}


extern void work_a_3101825049_3212880686_init()
{
	static char *pe[] = {(void *)work_a_3101825049_3212880686_p_0};
	xsi_register_didat("work_a_3101825049_3212880686", "isim/fpga_isim_beh.exe.sim/work/a_3101825049_3212880686.didat");
	xsi_register_executes(pe);
}
