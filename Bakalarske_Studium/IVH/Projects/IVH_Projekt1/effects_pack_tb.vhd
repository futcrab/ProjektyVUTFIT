library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use work.effects_pack.ALL;


entity effects_pack_tb is
end effects_pack_tb;

architecture Behavioral of effects_pack_tb is
signal DATA : std_logic_vector (0 to 11) := "101001100111";
begin

assert(GETCOLUMN(DATA, 0, 3) = "101");
assert(GETCOLUMN(DATA, 1, 3) = "001");
assert(GETCOLUMN(DATA, 2, 3) = "100");
assert(GETCOLUMN(DATA, 3, 3) = "111");

assert(GETCOLUMN(DATA, 4, 3) = "101");
assert(GETCOLUMN(DATA, 10, 3) = "101");
assert(GETCOLUMN(DATA, -1, 3) = "111");

assert(GETCOLUMN(DATA, 0, 4) = "1010");
assert(GETCOLUMN(DATA, 1, 4) = "0110");
assert(GETCOLUMN(DATA, 2, 4) = "0111");

assert(NEAREST2N(0) = 1);
assert(NEAREST2N(1) = 1);
assert(NEAREST2N(4) = 4);
assert(NEAREST2N(6) = 8);
assert(NEAREST2N(42) = 64);
assert(NEAREST2N(8100) = 8192);

end Behavioral;

