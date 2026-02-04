library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity MiniRouter_tb is
    generic(
        DATA_SIZE : positive := 8;
        PRIORITY_SIZE : positive := 2;

        CLK_PERIOD : time := 10 ns
    );
end entity;

architecture MRTB of MiniRouter_tb is
    component MiniRouter
        generic(
            DATA_SIZE : positive := 8;
            PRIORITY_SIZE : positive := 2
        );
        port(
            reset : in std_logic;
            clk : in std_logic;
        
            data1 : in std_logic_vector ((DATA_SIZE + PRIORITY_SIZE)-1 downto 0);
            req1 : in std_logic;
            grant1 : out std_logic;

            data2 : in std_logic_vector ((DATA_SIZE + PRIORITY_SIZE)-1 downto 0);
            req2 : in std_logic;
            grant2 : out std_logic;

            data_out : out std_logic_vector (DATA_SIZE-1 downto 0);
            valid : out std_logic
        );
    end component;

    signal clk : std_logic := '0';
    signal reset : std_logic := '1';

    -- signals for readability (they will be combined in the test_data inputs)
    signal value1 : std_logic_vector (DATA_SIZE-1 downto 0) := std_logic_vector(to_unsigned(111,DATA_SIZE));
    signal prio1 : std_logic_vector (PRIORITY_SIZE-1 downto 0) := std_logic_vector(to_unsigned(2,PRIORITY_SIZE));
    
    signal value2 : std_logic_vector (DATA_SIZE-1 downto 0) := std_logic_vector(to_unsigned(222,DATA_SIZE));
    signal prio2 : std_logic_vector (PRIORITY_SIZE-1 downto 0) := std_logic_vector(to_unsigned(1,PRIORITY_SIZE));

    -- link 1
    signal test_data1 : std_logic_vector ((DATA_SIZE + PRIORITY_SIZE)-1 downto 0);
    signal test_req1 : std_logic := '0';
    signal test_grant1 : std_logic;

    -- link 2
    signal test_data2 : std_logic_vector ((DATA_SIZE + PRIORITY_SIZE)-1 downto 0);
    signal test_req2 : std_logic := '0';
    signal test_grant2 : std_logic;

    -- output
    signal test_data_out : std_logic_vector (DATA_SIZE-1 downto 0);
    signal test_valid : std_logic;

    -- round robin test variable
    signal toggle : std_logic := '0'; 

    signal endsim : boolean := false;
begin

    test_unit: MiniRouter
        generic map(
            DATA_SIZE => DATA_SIZE,
            PRIORITY_SIZE => PRIORITY_SIZE
        )
        port map(
            reset => reset,
            clk => clk,

            data1 => test_data1,
            req1 => test_req1,
            grant1 => test_grant1,
        
            data2 => test_data2,
            req2 => test_req2,
            grant2 => test_grant2,

            data_out => test_data_out,
            valid => test_valid
        );

    clk_gen: process
        begin
            while(endsim = false) loop
                clk <= '1';
                wait for CLK_PERIOD /2;
                clk <= '0';
                wait for CLK_PERIOD /2;
            end loop;
            wait;
        end process;

    test_data1 <= value1 & prio1;
    test_data2 <= value2 & prio2;


    -- stim_proc structure:
        -- setting inputs
        -- wait one clock
        -- check outputs

    -- inputs have an offset of half a clock

    stim_proc: process
        variable loop_counter : integer;
        variable aux_output : unsigned (DATA_SIZE -1 downto 0);
    begin
        -- starting inputs, already assigned
        -- value1 is 111
        -- prio1 is 2
        
        -- value2 is 222
        -- prio2 is 1

        reset <= '1'; -- start reset phase

        wait for CLK_PERIOD/2; -- initial input offset

        -- TEST 1: RESET 
        assert (unsigned(test_data_out) = to_unsigned(0, test_data_out'length) and
            test_grant1 = '0' and test_grant2 = '0' and test_valid = '0')
        report "Test 1 FAILED: reset not working"
        severity failure;

        report " Test 1 PASSED: reset";


        -- TEST 2: SINGLE LINK FORWARDING

        reset <= '0'; -- exit reset phase

        test_req1 <= '1'; -- link1 requests data forwarding (link2 is off)

        wait for CLK_PERIOD;

        assert (unsigned(test_data_out) = unsigned(value1) and
            test_grant1 = '1' and test_grant2 = '0' and test_valid = '1')
        report "Test 2 FAILED: single link forwarding not working"
        severity failure;

        report " Test 2 PASSED: single link forwarding";



        -- TEST 3: LINK SWITCH

        test_req1 <= '0'; -- turning off link1
        test_req2 <= '1'; -- now link2 is up

        wait for CLK_PERIOD;

        assert (unsigned(test_data_out) = unsigned(value2) and
            test_grant1 = '0' and test_grant2 = '1' and test_valid = '1')
        report "Test 3 FAILED: switching links not working"
        severity failure;

        report " Test 3 PASSED: switching links";



        -- TEST 4: ROUND ROBIN
        prio1 <= std_logic_vector(to_unsigned(2,PRIORITY_SIZE)); -- setting same priority for both links
        prio2 <= std_logic_vector(to_unsigned(2,PRIORITY_SIZE));

        test_req1 <= '1'; -- both links are up
        test_req2 <= '1';

        wait for CLK_PERIOD;

        -- round robin: N tests
        loop_counter := 5;
        while loop_counter > 0 loop
            loop_counter := loop_counter - 1;

            aux_output := unsigned(value2) when toggle = '1' else unsigned(value1);
            
            -- toggle starts as '0' -> link1 goes first
            assert (unsigned(test_data_out) = aux_output and
                test_grant1 = not toggle and test_grant2 = toggle and test_valid = '1')
            report "Test 4 FAILED: round robin not working"
            severity failure;

            toggle <= not toggle; -- the output changes at every clock

            if loop_counter > 0 then -- avoids last clock
                wait for CLK_PERIOD;
            end if;
        end loop;

        report " Test 4 PASSED: round robin";
        

        -- TEST 5: HIGHEST PRIORITY
        
        -- both links are now active with different priorities
        prio1 <= std_logic_vector(to_unsigned(1,PRIORITY_SIZE));
        prio2 <= std_logic_vector(to_unsigned(3,PRIORITY_SIZE));

        wait for CLK_PERIOD;

        -- to verify that round robin is not active, check the output for 2 clocks
        loop_counter := 2;
        while loop_counter > 0 loop
            loop_counter := loop_counter - 1;
            
            assert (unsigned(test_data_out) = unsigned(value2) and
                test_grant1 = '0' and test_grant2 = '1' and test_valid = '1')
            report "Test 5 FAILED: highest priority not working"
            severity failure;

            if loop_counter > 0 then --avoids last clock
                wait for CLK_PERIOD;
            end if;
        end loop;

        report " Test 5 PASSED: highest priority";

        -- TEST 6: NO INPUTS

        test_req1 <= '0'; -- both links are now off
        test_req2 <= '0'; 
        wait for CLK_PERIOD;

        assert (test_grant1 = '0' and test_grant2 = '0' and test_valid = '0')
        report "Test 6 FAILED: no inputs not working"
        severity failure;

        report " Test 6 PASSED: no inputs";

        -- TEST 7: NO TRANSPARENCY
        
        -- both links are still off
        value1 <= std_logic_vector(to_unsigned(100,DATA_SIZE));
        prio1 <= std_logic_vector(to_unsigned(0,PRIORITY_SIZE));
        
        value2 <= std_logic_vector(to_unsigned(200,DATA_SIZE));
        prio2 <= std_logic_vector(to_unsigned(0,PRIORITY_SIZE));

        aux_output := unsigned(test_data_out);

        wait for CLK_PERIOD;

        loop_counter := 2;
        while loop_counter > 0 loop
            loop_counter := loop_counter - 1;
            -- check for variations in output
            assert (unsigned(test_data_out) = aux_output and
            test_grant1 = '0' and test_grant2 = '0' and test_valid = '0')
            report "Test 7 FAILED: no transparency not working"
            severity failure;

            if loop_counter > 0 then --avoids last clock
                wait for CLK_PERIOD;
            end if;
        end loop;

        report " Test 7 PASSED: no transparency";

        report "ALL TESTS PASSED - Ending Simulation...";

        endsim <= true;
        wait;
    end process;
end architecture;