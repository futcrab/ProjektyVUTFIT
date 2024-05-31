-- Testovani counteru, kod je tak, jak je vygenerovan od ISE
-- Autor: Peter Durica

LIBRARY ieee;
USE ieee.std_logic_1164.ALL;

ENTITY counter_tb IS
END counter_tb;

ARCHITECTURE behavior OF counter_tb IS
    --Inputs
    SIGNAL CLK : STD_LOGIC := '0';
    SIGNAL RESET : STD_LOGIC := '0';

    --Outputs
    SIGNAL EN1 : STD_LOGIC;
    SIGNAL EN2 : STD_LOGIC;
    SIGNAL EN3 : STD_LOGIC;

    -- Clock period definitions
    CONSTANT CLK_period : TIME := 10 ns;

BEGIN

    -- Instantiate the Unit Under Test (UUT)
    -- muzete samozrejme nastavit i genericke parametry!
    -- pozor na dobu simulace (nenastavujte moc dlouhe casy nebo zkratte CLK_period)
    -- Pocitejte s tim, ze pri zkouseni pobezi testbench 100 ms
    uut : ENTITY work.counter 
		GENERIC MAP (
		  OUT1_PERIOD => 1,
		  OUT2_PERIOD => 2,
		  OUT3_PERIOD => 3
		  )
		PORT MAP (
        CLK => CLK,
        RESET => RESET,
        EN1 => EN1,
        EN2 => EN2,
        EN3 => EN3
        );

    -- Clock process definitions
    CLK_process : PROCESS
    BEGIN
        CLK <= '0';
        WAIT FOR CLK_period/2;
        CLK <= '1';
        WAIT FOR CLK_period/2;
    END PROCESS;
    -- Stimulus process
    stim_proc : PROCESS
    BEGIN
        -- hold reset state for 100 ns.
        RESET <= '1';
        WAIT FOR 100 ns;
        RESET <= '0';
		  
		  -- OUT1 nastavene na 1, OUT2 na 2, OUT3 na 3

        WAIT FOR CLK_period * 10; -- 10. cyklus (Delitelne 1 a 2)

        assert(EN1 = '1');
		  assert(EN2 = '1');
		  assert(EN3 = '0');
		  
		  WAIT FOR CLK_period * 2; -- 12. cyklus (Delitelne 1,2 a 3)
		  
		  assert(EN1 = '1');
		  assert(EN2 = '1');
		  assert(EN3 = '1');
		  
		  WAIT FOR CLK_period * 3; -- 15. cyklus (Delitelne 1 a 3)
		  
		  assert(EN1 = '1');
		  assert(EN2 = '0');
		  assert(EN3 = '1');
		  
		  WAIT FOR CLK_period * 2; -- 17. cyklus (Delitelne 1)
		  
		  assert(EN1 = '1');
		  assert(EN2 = '0');
		  assert(EN3 = '0');

        WAIT;
    END PROCESS;

END;