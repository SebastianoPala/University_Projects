library IEEE;
use IEEE.std_logic_1164.all;

entity MiniRouter is
    generic(
        DATA_SIZE : positive;
        PRIORITY_SIZE : positive
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
end entity;

architecture MR of MiniRouter is
    
    component Comparator
    generic(
        SIZE : positive
        );
        port(
            A : in std_logic_vector (SIZE-1 downto 0);
            B : in std_logic_vector (SIZE-1 downto 0);
            
            equals : out std_logic;
            B_is_less : out std_logic
        );
    end component;

    signal forwarded_data : std_logic_vector (DATA_SIZE -1 downto 0); -- contains the data forwarded after the decision took place

    signal both_links_active : std_logic;

    -- priority signals
    signal same_priority : std_logic;
    signal L2_lower_priority : std_logic;

    -- round robin signals
    signal round_robin_result : std_logic; -- used as a decision variable when both links are active and with the same priority
    signal next_round_robin : std_logic; -- changes value of "round_robin_result" when the latter has been used
    
    signal link_decision_maker : std_logic; -- determines which output will be displayed. 0 -> Data of Link 1 , 1 -> Data of Link 2

begin
    
    CMP: Comparator
        generic map( SIZE => PRIORITY_SIZE)
        port map(
            A => data1(PRIORITY_SIZE - 1 downto 0),
            B => data2(PRIORITY_SIZE - 1 downto 0),

            equals => same_priority,
            B_is_less => L2_lower_priority
        );
    -- the signal "L2_lower_priority" will only be used when "same_priority" = '0', so weak inequality in the comparator logic is allowed

    both_links_active <= req1 and req2;

    next_round_robin <= not round_robin_result when ( both_links_active and same_priority) = '1' else round_robin_result;

    
    -- when only one link is active, "link_decision_maker" depends on req2
    --          req2 = '0' -> only Link1 is requesting a transmission -> link_decision_maker = '0' forwards Data of Link1, and viceversa;

    -- when both links use the same priority, data is forwarded using round robin;

    -- if the priority is different, L2_lower_priority = '1' -> Link1 is more important -> link_decision_maker = '0'
    link_decision_maker <= req2 when both_links_active = '0' 
                        else round_robin_result 
                            when same_priority = '1' 
                                else (not L2_lower_priority);

    forwarded_data <= data1(DATA_SIZE + PRIORITY_SIZE - 1 downto PRIORITY_SIZE) 
                    when link_decision_maker = '0' 
                        else data2(DATA_SIZE + PRIORITY_SIZE - 1 downto PRIORITY_SIZE);
    
    UPDATE_REG: process(reset,clk)
    begin
        if(reset = '1') then -- async active high reset
            data_out <= (others => '0');
            valid <= '0';
            round_robin_result <= '0';
            grant1 <= '0';
            grant2 <= '0';
        elsif rising_edge(clk) then
            if (req1 = '1' or req2 = '1') then 
            
                grant1 <= not link_decision_maker; -- link_decision_maker = '0' -> we forwarded Link1
                grant2 <= link_decision_maker;     -- link_decision_maker = '1' -> we forwarded Link2 

                valid <= '1';

                round_robin_result <= next_round_robin;
                
                data_out <= forwarded_data;
            
            else -- if no link is active, set all outputs to '0'
                grant1 <= '0';
                grant2 <= '0';
                valid <= '0';
            end if;
        end if;
    end process;

end architecture;