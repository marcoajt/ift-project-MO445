------------------------------------------------------------------------------------------------------------------------
-- Testbench for the dco.
------------------------------------------------------------------------------------------------------------------------
-- IEEE standard library
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
-- Base library
library base_lib;
use base_lib.base_lib_pkg.all;

entity dco_TB is
end dco_TB;

architecture behavioral of dco_TB is

-- number of dcos on the testbench.
    constant dco_number_c   : integer := 4;

    signal mclk_i           : std_logic := '0';
    signal resync_s         : std_logic_vector(dco_number_c-1 downto 0);
    signal prescale_s       : std_logic_vector(dco_number_c-1 downto 0) := (others => '0');
    signal dco_clk_s        : std_logic_vector(dco_number_c-1 downto 0);
-- reference clock to be compared to the clock generated by the dco.
    signal ref_clk_s        : std_logic_vector(dco_number_c-1 downto 0) := (others => '0');
    type int_vec_t is array (natural range <>) of integer;
    constant k_c            : int_vec_t(dco_number_c-1 downto 0) := (
        3 => 14,
        2 => 15,
        1 => 16,
        0 => 17
    );
-- Formulas for N:
-- Without prescale: N = Fout*2**(K)/Fmclk
-- With prescale: N = Fout*2**(K)/Fprescale
    constant n_c            : int_vec_t(dco_number_c-1 downto 0) := (
        3 => (2**(k_c(3)))/50,
        2 => 333*(2**(k_c(2)))/100000,
        1 => 23*(2**(k_c(1)))/100000,
        0 => 17*(2**(k_c(0)))/100
    );

begin

    mclk_i <= not mclk_i after 5 ns; -- 100 MHz

    dco_gen: for j in dco_number_c-1 downto 0 generate

        dco_u: dco
            generic map(
                k_c         => k_c(j)
            )
            port map(
                clk_i       => mclk_i,
                resync_i    => resync_s(j),
                prescale_i  => prescale_s(j),
                n_i         => conv_std_logic_vector(n_c(j),k_c(j)),
                clk_o       => dco_clk_s(j)
            );

    end generate;

    ref_clk_s(3) <= not ref_clk_s(3) after 500 ns; -- 1 MHz
    ref_clk_s(2) <= not ref_clk_s(2) after 1.501502 us; -- 333 kHz
    ref_clk_s(1) <= not ref_clk_s(1) after 21.739130 us; -- 23 KHz
    ref_clk_s(0) <= not ref_clk_s(0) after 29.412 ns; -- 17 MHz

    prescale_p: process(mclk_i)
        begin
            if mclk_i'event and mclk_i = '1' then
                prescale_s(3) <= not prescale_s(3);
            end if;
        end process;

    prescale_s(2 downto 0) <= (others => '1');

    resync_s <= (others => '0');

end behavioral;