LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE ieee.numeric_std.ALL;
USE ieee.std_logic_unsigned.ALL;
USE work.effects_pack.ALL;
LIBRARY work;
 
ENTITY fsm_tb IS
END fsm_tb;
 
ARCHITECTURE behavior OF fsm_tb IS 
 
 
    COMPONENT fsm
    PORT(
         CLK : IN  std_logic;
         RST : IN  std_logic;
         COLUMN : OUT  std_logic_vector(3 downto 0);
         COL_VECTOR : OUT  std_logic_vector(7 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal CLK : std_logic := '0';
   signal RST : std_logic := '0';

 	--Outputs
   signal COLUMN : std_logic_vector(3 downto 0);
   signal COL_VECTOR : std_logic_vector(7 downto 0);

   -- Clock period definitions
   constant CLK_period : time := 1 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: fsm PORT MAP (
          CLK => CLK,
          RST => RST,
          COLUMN => COLUMN,
          COL_VECTOR => COL_VECTOR
        );

   -- Clock process definitions
   CLK_process :process
   begin
		CLK <= '0';
		wait for CLK_period/2;
		CLK <= '1';
		wait for CLK_period/2;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin		
			RST <= '1';
			wait for 10 ns;
			RST <= '0';
			wait for 100 ns;
			RST <= '1';
			wait for 100 ns;
			RST <= '0';

      -- insert stimulus here 
   end process;

END;
