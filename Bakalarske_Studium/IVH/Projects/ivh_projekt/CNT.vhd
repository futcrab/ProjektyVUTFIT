library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;
library work;

entity CNT is
    GENERIC (n : integer := 10;
				 stop: integer := 2000);
	 PORT ( clk : in  STD_LOGIC;
           enable : in STD_LOGIC;
			  cycle : out  STD_LOGIC);
end CNT;

architecture Behavioral of CNT is
	
signal counter : std_logic_vector (N-1 downto 0) := (others => '0');
signal end_point : std_logic_vector (N-1 downto 0) := (others => '1');
	
begin
	CNT : process (clk)
	begin
		if rising_edge(clk) then
			if enable = '1' then
				end_point <= std_logic_vector(to_unsigned(stop-1, N));
				counter <= counter + 1;
				if counter = end_point then
					cycle <= '1';
					counter <= (others => '0');
				else
					cycle <= '0';
				end if;
			else
				cycle <= '0';
			end if;
		end if;
	end process;

end Behavioral;
