------------------------------------------------------------------------------------------------------------------------
-- Edge up detector
-- - Output active during one cycle after the actual edge
------------------------------------------------------------------------------------------------------------------------
--Standard Libraries
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity det_up is
    Port (
        I   : in  std_logic;--Signal input
        O   : out std_logic;--Pulse output
        CK  : in  std_logic --Master clock
    );
end det_up;

architecture rtl of det_up is

    signal tmp_s : std_logic;

begin

    sync_ffd_p : process(CK)
        begin
            if CK'event and CK = '1' then
                tmp_s <= I;
            end if;
    end process;

    O <= (not tmp_s) and I;

end rtl;
