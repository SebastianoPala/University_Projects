library IEEE;
use IEEE.std_logic_1164.all;

entity MR_Wrapper is
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
end entity;

architecture structural of MR_Wrapper is

    component MiniRouter is
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
    end component;

    -- input registers
    signal data1_reg : std_logic_vector ((DATA_SIZE + PRIORITY_SIZE)-1 downto 0);
    signal data2_reg : std_logic_vector ((DATA_SIZE + PRIORITY_SIZE)-1 downto 0);
    signal req1_reg, req2_reg : std_logic;
    signal reset_reg : std_logic;

    -- output registers
    signal data_out_reg : std_logic_vector (DATA_SIZE -1 downto 0);
    signal grant1_reg : std_logic;
    signal grant2_reg : std_logic;
    signal valid_reg : std_logic;

    -- output signals
    signal data_out_aux : std_logic_vector (DATA_SIZE -1 downto 0);
    signal grant1_aux : std_logic;
    signal grant2_aux : std_logic;
    signal valid_aux : std_logic;

begin

    data_out <= data_out_reg;
    grant1 <= grant1_reg;
    grant2 <= grant2_reg;
    valid <= valid_reg;

    MR_core: MiniRouter
        generic map(
            DATA_SIZE => DATA_SIZE,
            PRIORITY_SIZE => PRIORITY_SIZE
        )
        port map(
            reset => reset_reg,
            clk => clk,
            data1 => data1_reg,
            req1 => req1_reg,
            grant1 => grant1_aux,
            data2 => data2_reg,
            req2 => req2_reg,
            grant2 => grant2_aux,
            data_out => data_out_aux,
            valid => valid_aux
        );

    process(clk)
    begin
        if rising_edge(clk) then
            reset_reg <= reset;
            data1_reg <= data1;
            data2_reg <= data2;
            req1_reg <= req1;
            req2_reg <= req2;

            data_out_reg <= data_out_aux;
            grant1_reg <= grant1_aux;
            grant2_reg <= grant2_aux;
            valid_reg <= valid_aux;
        end if;
    end process;

end architecture;