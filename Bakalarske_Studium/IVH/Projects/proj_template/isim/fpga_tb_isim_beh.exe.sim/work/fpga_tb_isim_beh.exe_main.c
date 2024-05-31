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

#include "xsi.h"

struct XSI_INFO xsi_info;

char *IEEE_P_2592010699;
char *IEEE_P_3620187407;
char *IEEE_P_3499444699;
char *STD_STANDARD;
char *WORK_P_0012275476;
char *IEEE_P_1242562249;
char *WORK_P_0327194142;
char *UNISIM_P_0947159679;
char *STD_TEXTIO;
char *IEEE_P_1367372525;
char *IEEE_P_2717149903;
char *UNISIM_P_3222816464;


int main(int argc, char **argv)
{
    xsi_init_design(argc, argv);
    xsi_register_info(&xsi_info);

    xsi_register_min_prec_unit(-12);
    ieee_p_2592010699_init();
    ieee_p_3499444699_init();
    work_p_0327194142_init();
    ieee_p_1242562249_init();
    unisim_p_0947159679_init();
    std_textio_init();
    ieee_p_2717149903_init();
    ieee_p_1367372525_init();
    unisim_p_3222816464_init();
    unisim_a_1490675510_1976025627_init();
    unisim_a_0780662263_2014779070_init();
    unisim_a_4002211264_2683070183_init();
    unisim_a_1893280375_2982649196_init();
    unisim_a_2575347804_2982649196_init();
    unisim_a_0402065255_2679555531_init();
    unisim_a_2188764497_1233898481_init();
    work_a_3537674494_3212880686_init();
    ieee_p_3620187407_init();
    work_p_0012275476_init();
    work_a_1421273832_3212880686_init();
    work_a_3135133961_3212880686_init();
    work_a_3758293254_3212880686_init();
    work_a_2779505667_3212880686_init();
    work_a_3135090559_3212880686_init();
    work_a_0017495711_3212880686_init();
    work_a_1291828878_3212880686_init();
    work_a_3392787015_3212880686_init();
    work_a_3643194780_3212880686_init();
    work_a_0605910504_3212880686_init();
    work_a_3058721471_0753780893_init();
    work_a_0023372960_2372691052_init();


    xsi_register_tops("work_a_0023372960_2372691052");

    IEEE_P_2592010699 = xsi_get_engine_memory("ieee_p_2592010699");
    xsi_register_ieee_std_logic_1164(IEEE_P_2592010699);
    IEEE_P_3620187407 = xsi_get_engine_memory("ieee_p_3620187407");
    IEEE_P_3499444699 = xsi_get_engine_memory("ieee_p_3499444699");
    STD_STANDARD = xsi_get_engine_memory("std_standard");
    WORK_P_0012275476 = xsi_get_engine_memory("work_p_0012275476");
    IEEE_P_1242562249 = xsi_get_engine_memory("ieee_p_1242562249");
    WORK_P_0327194142 = xsi_get_engine_memory("work_p_0327194142");
    UNISIM_P_0947159679 = xsi_get_engine_memory("unisim_p_0947159679");
    STD_TEXTIO = xsi_get_engine_memory("std_textio");
    IEEE_P_1367372525 = xsi_get_engine_memory("ieee_p_1367372525");
    IEEE_P_2717149903 = xsi_get_engine_memory("ieee_p_2717149903");
    UNISIM_P_3222816464 = xsi_get_engine_memory("unisim_p_3222816464");

    return xsi_run_simulation(argc, argv);

}
