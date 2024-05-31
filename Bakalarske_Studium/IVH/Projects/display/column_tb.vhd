LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE work.effects_pack.ALL;
 
 
ENTITY column_tb IS
END column_tb;
 
ARCHITECTURE behavior OF column_tb IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT stlpec
    PORT(
         CLK : IN  std_logic;
         RESET : IN  std_logic;
         STATE : OUT  std_logic_vector(7 downto 0);
         INIT_STATE : IN  std_logic_vector(7 downto 0);
         NEIGH_LEFT : IN  std_logic_vector(7 downto 0);
         NEIGH_RIGHT : IN  std_logic_vector(7 downto 0);
         DIRECTION : IN  DIRECTION_T;
         EN : IN  std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal CLK : std_logic := '0';
   signal RESET : std_logic := '0';
   signal INIT_STATE : std_logic_vector(7 downto 0) := (others => '0');
   signal NEIGH_LEFT : std_logic_vector(7 downto 0) := (others => '0');
   signal NEIGH_RIGHT : std_logic_vector(7 downto 0) := (others => '0');
   signal DIRECTION : DIRECTION_T := DIR_LEFT;
   signal EN : std_logic := '0';

 	--Outputs
   signal STATE : std_logic_vector(7 downto 0);

   -- Clock period definitions
   constant CLK_period : time := 1 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: stlpec PORT MAP (
          CLK => CLK,
          RESET => RESET,
          STATE => STATE,
          INIT_STATE => INIT_STATE,
          NEIGH_LEFT => NEIGH_LEFT,
          NEIGH_RIGHT => NEIGH_RIGHT,
          DIRECTION => DIRECTION,
          EN => EN
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
		RESET <= '1';
		INIT_STATE <= "00110011";
		NEIGH_RIGHT <= "11111111";
		NEIGH_LEFT <= "00000000";
      wait for 10 ns;	
		RESET <= '0';
		EN <= '1';
      wait for CLK_period*2;
		DIRECTION <= DIR_RIGHT;
		wait for CLK_period*2;
		DIRECTION <= DIR_LEFT;
		wait for CLK_period*2;
		DIRECTION <= DIR_RIGHT;
		wait for CLK_period*2;
		DIRECTION <= DIR_TOP;
		wait for CLK_period*2;
		EN <= '0';
		RESET <= '1';

      wait;
   end process;

END;
