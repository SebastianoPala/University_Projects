library IEEE;
use IEEE.std_logic_1164.all;

entity RippleCarryAdder is
    generic(
        N : positive
    );
    port(
        A : in std_logic_vector (N-1 downto 0);
        B : in std_logic_vector (N-1 downto 0);
        cin : in std_logic;

        S : out std_logic_vector (N-1 downto 0);
        cout : out std_logic
    );
end entity;

architecture RR of RippleCarryAdder is
    component FullAdder
        port(
            a : in std_logic;
            b : in std_logic;
            cin : in std_logic;

            s : out std_logic;
            cout : out std_logic
        );
    end component;
    
    signal sig : std_logic_vector (N-2 downto 0);
begin

    iter_gen:for i in 0 to N-1 generate
        first_gen: if (i = 0) generate
            i_elem: FullAdder
                port map(
                    a => A(i),
                    b => B(i),
                    cin => cin,

                    s => S(i),
                    cout => sig(i)
                );
        end generate;

        mid_gen:if (i > 0 and i < N-1) generate
        i_elem: FullAdder
                port map(
                    a => A(i),
                    b => B(i),
                    cin => sig(i-1),

                    s => S(i),
                    cout => sig(i)
                );
        end generate;

        last_gen:if (i = N-1 and N > 1) generate
        i_elem: FullAdder
                port map(
                    a => A(i),
                    b => B(i),
                    cin => sig(i-1),

                    s => S(i),
                    cout => cout
                );
        end generate;


    end generate;

end architecture;