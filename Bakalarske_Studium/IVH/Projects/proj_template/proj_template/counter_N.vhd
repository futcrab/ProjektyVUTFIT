library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;
library work;

entity cnt is
    generic (n : integer := 10;
				 stop: integer := 2000);
	 Port ( clk : in  STD_LOGIC;
           enable : in STD_LOGIC;
			  cycle : out  STD_LOGIC);
end cnt;

architecture Behavioral of cnt_for_n is
	
signal counter : std_logic_vector (N-1 downto 0) := (others => '0');
signal end_point : std_logic_vector (N-1 downto 0) := std_logic_vector(to_unsigned(stop, N));
	
begin
	cnt : process (clk)
	begin
		if rising_edge(clk) then
			if enable = '1' then
				counter <= counter + 1;
				if counter = end_point then
					cycle <= '1';
					counter <= (others => '0');
				else
					cycle <= '0';
				end if;
			else
				cycle <= '0';
				counter <= (others => '0');
			end if;
		end if;
	end process;

end Behavioral;
