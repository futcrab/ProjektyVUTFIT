LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE ieee.numeric_std.ALL;
USE ieee.std_logic_unsigned.ALL;
USE work.effects_pack.ALL;
LIBRARY work;

-- FSM ENTITY DECLARATION
ENTITY fsm IS
	PORT (
		CLK : IN STD_LOGIC;
		RST : IN STD_LOGIC;
		COLUMN : OUT STD_LOGIC_VECTOR(3 DOWNTO 0);
		COL_VECTOR : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
	);
END fsm;

ARCHITECTURE Behavioral OF fsm IS
	--Counters
	--Refresh of screen (allways up)
	SIGNAL CNT_EN_REFRESH : STD_LOGIC := '1';
	SIGNAL CNT_DONE_REFRESH : STD_LOGIC;
	-- CNT for when to change pos on screen
	SIGNAL CNT_EN_POSCHANGE : STD_LOGIC;
	SIGNAL CNT_DONE_POSCHANGE : STD_LOGIC;
	
	-- CNT for moving sideways 
	-- Counts 3*cols on screen
	SIGNAL CNT_EN_TOSIDE : STD_LOGIC;
	SIGNAL CNT_DONE_TOSIDE : STD_LOGIC := '0';
	-- CNT for moving up
	-- Counts rows on screen
	SIGNAL CNT_EN_UP : STD_LOGIC;
	SIGNAL CNT_DONE_UP : STD_LOGIC := '0';	
	--Signal is up if image is moving, no matter DIR
	SIGNAL MOVE_IN_DIR : STD_LOGIC := CNT_EN_TOSIDE OR CNT_EN_UP;
	
	-- CNT for loading animation
	SIGNAL CNT_EN_LOAD_ANIM : STD_LOGIC;
	SIGNAL CNT_DONE_LOAD_ANIM : STD_LOGIC;
	-- CNT for animation on time
	-- when done cycle will repeat
	SIGNAL CNT_EN_ANIM : STD_LOGIC;
	SIGNAL CNT_DONE_ANIM : STD_LOGIC;
	
	-- MEMORY SIGNALS
	SIGNAL INDEX : INTEGER := 0;
	SIGNAL COL_VALUE : STD_LOGIC_VECTOR (7 DOWNTO 0);
	SIGNAL IMAGE : INTEGER := 0;

	-- SIGNALS FOR DISPLAY
	SIGNAL DISPLAY_INIT : STD_LOGIC_VECTOR (127 DOWNTO 0);
	SIGNAL DISPLAY_OUTPUT : STD_LOGIC_VECTOR (127 DOWNTO 0);

	-- SIGNALS FOR COLS
	SIGNAL COL_RESET : STD_LOGIC := '1';
	SIGNAL INIT_STATE : STD_LOGIC_VECTOR(7 DOWNTO 0) := (OTHERS => '0');
	SIGNAL STATE : STD_LOGIC_VECTOR(7 DOWNTO 0) := (OTHERS => '0');
	SIGNAL DIRECTION : DIRECTION_T := DIR_RIGHT;
	SIGNAL DISPLAY_COL_POS : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";	

	-- STATES IN FSM and boolean to load memory
	TYPE STATE_TYPE IS (RESET, LOAD_IMAGE, RIGHT, LEFT, TOP, ANIMATION);
	SIGNAL FSM_STATE : STATE_TYPE;
	SIGNAL LOAD_FROM_MEMORY : STD_LOGIC := '0';
	
	-- COL Declaration
	COMPONENT stlpec IS
		PORT (
			CLK : IN STD_LOGIC;
			RESET : IN STD_LOGIC;
			STATE : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
			INIT_STATE : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
			NEIGH_LEFT : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
			NEIGH_RIGHT : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
			DIRECTION : IN DIRECTION_T;
			EN : IN STD_LOGIC
		);
	END COMPONENT;

	-- MEMORY Declaration
	COMPONENT MEMORY IS
		PORT (
			INDEX : IN INTEGER;
			COL_VALUE : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
		);
	END COMPONENT;


	-- CNT Declaration
	COMPONENT CNT IS
		GENERIC (n : IN INTEGER;
					stop : IN INTEGER);
		PORT (
			clk : IN STD_LOGIC;
			enable : IN STD_LOGIC;
			cycle : OUT STD_LOGIC
		);
	END COMPONENT;

BEGIN
	--CNT pos_change only when in right states
	CNT_EN_POSCHANGE <= '1' WHEN FSM_STATE = RIGHT OR FSM_STATE = LEFT OR FSM_STATE = TOP ELSE '0';
	--CNT animation load and animation visible on screen only when in right state
	CNT_EN_LOAD_ANIM <= '1' WHEN FSM_STATE = ANIMATION ELSE '0';
	CNT_EN_ANIM <= '1' WHEN FSM_STATE = ANIMATION ELSE '0';
	
	--GEN FOR COLS
	CREATE_COLS :
	FOR i IN 0 TO 15 GENERATE
	SIGNAL STATE_INIT : std_logic_vector(7 downto 0);
	SIGNAL STATE_RIGHT: std_logic_vector(7 downto 0);
	SIGNAL STATE_LEFT: std_logic_vector(7 downto 0);
	BEGIN
		STATE_INIT <= GETCOLUMN(DISPLAY_INIT, i, 8);
		STATE_RIGHT <= GETCOLUMN(DISPLAY_INIT, i-1, 8);
		STATE_LEFT <= GETCOLUMN(DISPLAY_INIT, i+1, 8);
		COLS : stlpec
		PORT MAP(
			CLK => CLK,
			RESET => COL_RESET,
			STATE => DISPLAY_OUTPUT((i + 1) * 8 - 1 DOWNTO i * 8),
			INIT_STATE => STATE_INIT,
			NEIGH_LEFT => STATE_LEFT,
			NEIGH_RIGHT => STATE_RIGHT,
			DIRECTION => DIRECTION,
			EN => MOVE_IN_DIR
		);
	END GENERATE CREATE_COLS;
	
	-- counters for movement
	-- Counts 3 moves to side (display_width = 16, 16 * 3 = 48)
	CNT_TOSIDE : ENTITY work.CNT
		GENERIC MAP(
			n => 6,
			stop => 48)
		PORT MAP(
			clk => CLK,
			enable => CNT_EN_TOSIDE,
			cycle => CNT_DONE_TOSIDE
		);
	--Counts slide to top (display_height = 8, 8 + 1(Show one empty frame) = 9)
	CNT_UP : ENTITY work.CNT
		GENERIC MAP(
			n => 4,
			stop => 9)
		PORT MAP(
			clk => CLK,
			enable => CNT_EN_UP,
			cycle => CNT_DONE_UP
		);


	--Counters for refreshing display
	-- 400 times a second to draw one col on display to let it seem like one image
	-- 25 Mhz / (16 cols) /400 = 3906 rounded
	CNT_REFRESH : ENTITY work.CNT
		GENERIC MAP(N => 12,
						stop => 3906)
		PORT MAP(
			clk => CLK,
			enable => CNT_EN_REFRESH,
			cycle => CNT_DONE_REFRESH
		);
	--Counts when to change pos of image
	-- Current circa 6Hz rate of change
	-- 25Mhz / 4166667 = 6
	CNT_POSCHANGE : ENTITY work.CNT
		GENERIC MAP(N => 22,
						stop => 4166667)
		PORT MAP(
			clk => CLK,
			enable => CNT_EN_POSCHANGE,
			cycle => CNT_DONE_POSCHANGE
		);
		--Loads animation
		--16 for screen width
	CNT_LOAD_ANIM : ENTITY work.CNT
		GENERIC MAP(N => 5,
						stop => 16)
		PORT MAP(
			clk => CLK,
			enable => CNT_EN_LOAD_ANIM,
			cycle => CNT_DONE_LOAD_ANIM
		);
	-- Time on screen for animation
	-- 25Mhz / 25000000 = 1 sec on screen
	CNT_ANIM : ENTITY work.CNT
		GENERIC MAP(N => 25,
						stop => 25000000)
		PORT MAP(
			clk => CLK,
			enable => CNT_EN_ANIM,
			cycle => CNT_DONE_ANIM
		);
	
	--MEMORY CHIP WITH IMAGES
	ROM_CHIP : MEMORY
	PORT MAP(
		INDEX => INDEX,
		COL_VALUE => COL_VALUE
	);
		
	-- RETURN VECTORS FOR DISPLAY
	COLUMN <= DISPLAY_COL_POS;
	COL_VECTOR <= DISPLAY_OUTPUT((to_integer(unsigned(DISPLAY_COL_POS)) + 1) * 8 - 1 DOWNTO to_integer(unsigned(DISPLAY_COL_POS)) * 8);
	
	-- REFRESH DISPLAY
	DIS_REFRESH : PROCESS (CLK)
	BEGIN
		IF rising_edge(CLK) THEN
			IF CNT_DONE_REFRESH = '1' THEN
				DISPLAY_COL_POS <= DISPLAY_COL_POS + 1;
			END IF;
		END IF;
	END PROCESS;

-- Controller process (does all logic)
DISPLAY_CONT : PROCESS (CLK)
    VARIABLE DIS_COL : INTEGER := 0;
BEGIN
    IF rising_edge(CLK) THEN
		-- Loading Image from memory
        IF LOAD_FROM_MEMORY = '1' THEN
				-- Address of Col in memory
				--  + index of image * num of vec in memory
            INDEX <= DIS_COL + (IMAGE * 16);
            DISPLAY_INIT(DIS_COL * 8 - 1 DOWNTO (DIS_COL - 1) * 8) <= COL_VALUE;
            -- When done reset and next state
				IF DIS_COL = 17 THEN
                COL_RESET <= '0';
                DIS_COL := 0;
					 -- Next image index
					 IF IMAGE = 3 THEN
						IMAGE <= -1;
					 END IF;
					 IMAGE <= IMAGE + 1;
            ELSE
                DIS_COL := DIS_COL + 1;
            END IF;
        END IF;

        IF FSM_STATE = RIGHT OR FSM_STATE = LEFT OR FSM_STATE = TOP THEN
            IF CNT_EN_TOSIDE = '1' OR CNT_EN_UP = '1' THEN
                IF DIRECTION = DIR_RIGHT THEN 
                    DISPLAY_INIT <= DISPLAY_INIT(119 DOWNTO 0) & DISPLAY_INIT(127 DOWNTO 120);
                ELSIF DIRECTION = DIR_LEFT THEN 
                    DISPLAY_INIT <= DISPLAY_INIT(7 DOWNTO 0) & DISPLAY_INIT(127 DOWNTO 8);
                ELSE
						FOR i IN 0 TO 15 LOOP
							-- every col add zero at end
							DISPLAY_INIT((i + 1) * 8 - 1 DOWNTO i * 8) <= '0' & DISPLAY_INIT((i + 1) * 8 - 1 DOWNTO i * 8 + 1);
						END LOOP;
					 END IF;
					 COL_RESET <= '1';
            ELSE
                COL_RESET <= '0';
            END IF;
        END IF;

        IF FSM_STATE = ANIMATION THEN
				-- load stripes to every col
            DISPLAY_INIT <= DISPLAY_INIT(119 DOWNTO 0) & "00110011";
            IF CNT_DONE_LOAD_ANIM = '1' THEN
                COL_RESET <= '1';
            END IF;
        END IF;
    END IF;
END PROCESS;
 
	-- Cycle infinitely trough states
	STATE_MACHINE : PROCESS (CLK)
	BEGIN
		IF rising_edge(CLK) THEN
			IF RST = '1' THEN
				FSM_STATE <= RESET;
			ELSE
				CASE FSM_STATE IS
						-- RST signal up, wait until down
					WHEN RESET =>
						IF RST = '0' THEN
							LOAD_FROM_MEMORY <= '1';
							FSM_STATE <= LOAD_IMAGE;
						END IF;

						-- Load image from memory
					WHEN LOAD_IMAGE =>
						-- COL_RESET is set to zero when image is loaded
						-- Thats why it is checked
						IF COL_RESET = '0' THEN
							LOAD_FROM_MEMORY <= '0';
							FSM_STATE <= RIGHT;
						END IF;

						-- Move image to right 3 times
					WHEN RIGHT =>
						DIRECTION <= DIR_RIGHT;
						IF CNT_DONE_POSCHANGE = '1' THEN
							CNT_EN_TOSIDE <= '1';
						ELSE
							CNT_EN_TOSIDE <= '0';
						END IF;
						IF CNT_DONE_TOSIDE = '1' THEN
							DIRECTION <= DIR_LEFT;
							FSM_STATE <= LEFT;
						END IF;

						-- Move image to left 3 times
					WHEN LEFT =>
						IF CNT_DONE_POSCHANGE = '1' THEN
							CNT_EN_TOSIDE <= '1';
						ELSE
							CNT_EN_TOSIDE <= '0';
						END IF;
						IF CNT_DONE_TOSIDE = '1' THEN
							DIRECTION <= DIR_TOP;
							FSM_STATE <= TOP;
						END IF;

						-- Move image up out of frame 
					WHEN TOP =>
						IF CNT_DONE_POSCHANGE = '1' THEN
							CNT_EN_UP <= '1';
						ELSE
							CNT_EN_UP <= '0';
						END IF;
						IF CNT_DONE_UP = '1' THEN
							FSM_STATE <= ANIMATION;
						END IF;
	

						-- Loading animation
					WHEN ANIMATION =>
					IF CNT_DONE_ANIM = '1' THEN
						-- After animation is done come back to first state
						FSM_STATE <= RESET;
					END IF;
			END CASE;
		END IF;
	END IF;
END PROCESS;

END Behavioral;