
library IEEE;
use IEEE.STD_LOGIC_1164.all;

package effects_pack is

type DIRECTION_T is (DIR_LEFT, DIR_RIGHT, DIR_TOP);

function GETCOLUMN (signal DATA : in std_logic_vector; COLID : in integer; ROWS : in integer) return std_logic_vector;
function NEAREST2N (DATA : in natural) return natural;

end effects_pack;

package body effects_pack is
 
	function GETCOLUMN (signal DATA : in std_logic_vector; COLID : in integer; ROWS : in integer) return std_logic_vector is
		variable NCols : integer;
		variable trueCol : integer;
		variable result : std_logic_vector(0 to ROWS-1);
		
		begin
			Ncols := DATA'length / ROWS;
			
			if COLID < 0 then 
				trueCol := NCols - 1;
			elsif COLID >= NCols then
				trueCol := 0;
			else
				trueCol := COLID;
			end if;
			
			result := DATA((trueCol*ROWS) to (trueCol*ROWS + ROWS - 1));
			return result;
		end;
	
	function NEAREST2N (DATA : in natural) return natural is
		variable result : natural := 1;
		begin
			while result < DATA loop
				result := result * 2;
			end loop;
			return result;
		end;
 
end effects_pack;
