library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;

entity counter_48 is
    Port ( CLK : in  STD_LOGIC;
           CNT_EN : in  STD_LOGIC;
           CNT_DONE : out  STD_LOGIC);
end counter_48;

architecture Behavioral of counter_48 is

	signal cnt : std_logic_vector (5 downto 0) := "000000";

begin

	counter : process (CLK)
	begin
		if rising_edge(CLK) then
			if CNT_EN = '1' then
				cnt <= cnt + 1;
				if cnt = "101111" then -- counts to 47 === <47-0>
					CNT_DONE <= '1';
					cnt <= "000000";
				else
					CNT_DONE <= '0';
				end if;
			else
				CNT_DONE <= '0';
			end if;
		end if;
	end process;

end Behavioral;
