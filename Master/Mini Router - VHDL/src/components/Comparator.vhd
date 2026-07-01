library IEEE;
use IEEE.std_logic_1164.all;

entity Comparator is
    generic(
        SIZE : positive
    );
    port(
        A : in std_logic_vector (SIZE-1 downto 0);
        B : in std_logic_vector (SIZE-1 downto 0);

        equals : out std_logic;
        B_is_less : out std_logic
    );
end entity;

architecture RCA of Comparator is
    component RippleCarryAdder
        generic(
            N : positive
        );
        port(
            A : in std_logic_vector (SIZE-1 downto 0);
            B : in std_logic_vector (SIZE-1 downto 0);
            cin : in std_logic;
    
            S : out std_logic_vector (SIZE-1 downto 0);
            cout : out std_logic
        );
    end component;
    
    signal result : std_logic_vector (SIZE-1 downto 0);

    signal not_B : std_logic_vector (SIZE-1 downto 0);

begin

    not_B <= not B;

    RCA: RippleCarryAdder
        generic map(N => SIZE)
        port map(
            A => A,
            B => not_B,
            cin => '1',
            
            S => result,
            cout => B_is_less
        );
        
    equals <= nor result;

end architecture;
 

-- architecture NO_RCA_2bit of Comparator is
--     begin
--         equals <= (A(1) xnor B(1)) and (A(0) xnor B(0));
--         B_is_less <= ((xor A) and (not B(1))) or (and A);
--     end architecture;