--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   14:27:44 05/07/2023
-- Design Name:   
-- Module Name:   /mnt/c/Users/duric/OneDrive/Dokumenty/skola/ivh/Projects/proj_template/CNT_tb.vhd
-- Project Name:  proj
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: CNT
-- 
-- Dependencies:
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
-- Notes: 
-- This testbench has been automatically generated using types std_logic and
-- std_logic_vector for the ports of the unit under test.  Xilinx recommends
-- that these types always be used for the top-level I/O of a design in order
-- to guarantee that the testbench will bind correctly to the post-implementation 
-- simulation model.
--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY CNT_tb IS
END CNT_tb;
 
ARCHITECTURE behavior OF CNT_tb IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
	COMPONENT CNT
	PORT(
		  clk : IN  std_logic;
		  enable : IN  std_logic;
		  cycle : OUT  std_logic
		 );
	END COMPONENT;
    

   --Inputs
   signal clk : std_logic := '0';
   signal enable : std_logic := '0';

 	--Outputs
   signal cycle : std_logic;

   -- Clock period definitions
   constant clk_period : time := 10 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: ENTITY work.cnt 
	GENERIC MAP (
		  n => 6,
		  stop => 48
		  )
	PORT MAP (
          clk => clk,
          enable => enable,
          cycle => cycle
        );

   -- Clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      wait for 100 ns;	

      wait for clk_period*10;
		
		enable <= '1';
      -- insert stimulus here 

      wait;
   end process;

END;
