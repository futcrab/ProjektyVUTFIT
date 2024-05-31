-- Citac s volitelnou frekvenci
-- IVH projekt - ukol2
-- autor: Peter Durica

LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
-- v pripade nutnosti muzete nacist dalsi knihovny

ENTITY counter IS
	GENERIC (
		OUT1_PERIOD : POSITIVE := 1;
		OUT2_PERIOD : POSITIVE := 1;
		OUT3_PERIOD : POSITIVE := 1);
	PORT (
		CLK : IN STD_LOGIC;
		RESET : IN STD_LOGIC;
		EN1 : OUT STD_LOGIC;
		EN2 : OUT STD_LOGIC;
		EN3 : OUT STD_LOGIC
	);
END counter;

ARCHITECTURE Behavioral OF counter IS

BEGIN
	
	process(CLK, RESET)
	variable cnt1: integer := 0;
	variable cnt2: integer := 0;
	variable cnt3: integer := 0;
	
	begin
		if RESET = '1' then
			cnt1 := 0;
			cnt2 := 0;
			cnt3 := 0;
			EN1 <= '0';
			EN2 <= '0';
			EN3 <= '0';
		elsif (CLK'event and CLK = '1') then
			EN1 <= '0';
			EN2 <= '0';
			EN3 <= '0';

			cnt1 := cnt1 + 1;
			cnt2 := cnt2 + 1;
			cnt3 := cnt3 + 1;
			
			if (cnt1 = OUT1_PERIOD) then
				EN1 <= '1';
				cnt1 := 0;
			end if;
			
			if (cnt2 = OUT2_PERIOD) then
				EN2 <= '1';
				cnt2 := 0;
			end if;
			
			if (cnt3 = OUT3_PERIOD) then
				EN3 <= '1';
				cnt3 := 0;
			end if;
		end if;
	end process;
END Behavioral;