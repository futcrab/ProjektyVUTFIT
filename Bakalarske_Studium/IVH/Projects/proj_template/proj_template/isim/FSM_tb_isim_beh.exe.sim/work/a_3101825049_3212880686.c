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
    char t7[16];
    char *t1;
    unsigned char t2;
    char *t3;
    char *t4;
    unsigned char t5;
    unsigned char t6;
    char *t8;
    char *t9;
    char *t10;
    char *t11;
    char *t12;
    char *t13;
    char *t14;

LAB0:    xsi_set_current_line(24, ng0);
    t1 = (t0 + 568U);
    t2 = ieee_p_2592010699_sub_1744673427_503743352(IEEE_P_2592010699, t1, 0U, 0U);
    if (t2 != 0)
        goto LAB2;

LAB4:
LAB3:    t1 = (t0 + 1884);
    *((int *)t1) = 1;

LAB1:    return;
LAB2:    xsi_set_current_line(25, ng0);
    t3 = (t0 + 776U);
    t4 = *((char **)t3);
    t5 = *((unsigned char *)t4);
    t6 = (t5 == (unsigned char)3);
    if (t6 != 0)
        goto LAB5;

LAB7:    xsi_set_current_line(34, ng0);
    t1 = (t0 + 1964);
    t3 = (t1 + 32U);
    t4 = *((char **)t3);
    t8 = (t4 + 40U);
    t9 = *((char **)t8);
    *((unsigned char *)t9) = (unsigned char)2;
    xsi_driver_first_trans_fast_port(t1);
    xsi_set_current_line(35, ng0);
    t1 = xsi_get_transient_memory(13U);
    memset(t1, 0, 13U);
    t3 = t1;
    memset(t3, (unsigned char)2, 13U);
    t4 = (t0 + 1928);
    t8 = (t4 + 32U);
    t9 = *((char **)t8);
    t10 = (t9 + 40U);
    t11 = *((char **)t10);
    memcpy(t11, t1, 13U);
    xsi_driver_first_trans_fast(t4);

LAB6:    goto LAB3;

LAB5:    xsi_set_current_line(26, ng0);
    t3 = (t0 + 868U);
    t8 = *((char **)t3);
    t3 = (t0 + 3400U);
    t9 = ieee_p_3620187407_sub_436279890_3965413181(IEEE_P_3620187407, t7, t8, t3, 1);
    t10 = (t0 + 1928);
    t11 = (t10 + 32U);
    t12 = *((char **)t11);
    t13 = (t12 + 40U);
    t14 = *((char **)t13);
    memcpy(t14, t9, 13U);
    xsi_driver_first_trans_fast(t10);
    xsi_set_current_line(27, ng0);
    t1 = (t0 + 868U);
    t3 = *((char **)t1);
    t1 = (t0 + 3400U);
    t4 = (t0 + 960U);
    t8 = *((char **)t4);
    t4 = (t0 + 3416U);
    t2 = ieee_std_logic_unsigned_equal_stdv_stdv(IEEE_P_3620187407, t3, t1, t8, t4);
    if (t2 != 0)
        goto LAB8;

LAB10:    xsi_set_current_line(31, ng0);
    t1 = (t0 + 1964);
    t3 = (t1 + 32U);
    t4 = *((char **)t3);
    t8 = (t4 + 40U);
    t9 = *((char **)t8);
    *((unsigned char *)t9) = (unsigned char)2;
    xsi_driver_first_trans_fast_port(t1);

LAB9:    goto LAB6;

LAB8:    xsi_set_current_line(28, ng0);
    t9 = (t0 + 1964);
    t10 = (t9 + 32U);
    t11 = *((char **)t10);
    t12 = (t11 + 40U);
    t13 = *((char **)t12);
    *((unsigned char *)t13) = (unsigned char)3;
    xsi_driver_first_trans_fast_port(t9);
    xsi_set_current_line(29, ng0);
    t1 = xsi_get_transient_memory(13U);
    memset(t1, 0, 13U);
    t3 = t1;
    memset(t3, (unsigned char)2, 13U);
    t4 = (t0 + 1928);
    t8 = (t4 + 32U);
    t9 = *((char **)t8);
    t10 = (t9 + 40U);
    t11 = *((char **)t10);
    memcpy(t11, t1, 13U);
    xsi_driver_first_trans_fast(t4);
    goto LAB9;

}


extern void work_a_3101825049_3212880686_init()
{
	static char *pe[] = {(void *)work_a_3101825049_3212880686_p_0};
	xsi_register_didat("work_a_3101825049_3212880686", "isim/FSM_tb_isim_beh.exe.sim/work/a_3101825049_3212880686.didat");
	xsi_register_executes(pe);
}
