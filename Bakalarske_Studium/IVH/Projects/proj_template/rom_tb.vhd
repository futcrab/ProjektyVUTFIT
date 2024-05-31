LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
ENTITY rom_tb IS
END rom_tb;
 
ARCHITECTURE behavior OF rom_tb IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT MEMORY
    PORT(
         INDEX : IN INTEGER;
         COL_VALUE : OUT  std_logic_vector(7 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal INDEX : INTEGER := 0;

 	--Outputs
   signal COL_VALUE : std_logic_vector(7 downto 0);
   -- No clocks detected in port list. Replace <clock> below with 
   -- appropriate port name 
 
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: MEMORY PORT MAP (
          INDEX => INDEX,
          COL_VALUE => COL_VALUE
        );
 
   -- Stimulus process
   stim_proc: process
   begin		
		wait for 10 ns;
		FOR i IN 0 TO 7 LOOP
		INDEX <= i;
		wait for 1 ns;
		END LOOP;
		
		FOR i IN 36 TO 43 LOOP
		INDEX <= i;
		wait for 1 ns;
		END LOOP;

      wait;
   end process;

END;
