--
-- Copyright (C) 2012 Chris McClelland
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU Lesser General Public License for more details.
--
-- You should have received a copy of the GNU Lesser General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.
--
library ieee;

use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_textio.all;
use std.textio.all;
use work.hex_util.all;

entity dpram_tb is
	--port(
	--	dispClk_out : out std_logic
	--);
end entity;

architecture behavioural of dpram_tb is
	-- Clocks
	signal sysClk      : std_logic;  -- main system clock
	signal dispClk     : std_logic;  -- display version of sysClk, which transitions 4ns before it
	signal fooClk      : std_logic;  -- display version of sysClk, which transitions 4ns before it

	-- Write port
	signal writeAddr   : std_logic_vector(9 downto 0);
	signal writeData   : std_logic_vector(3 downto 0);
	signal writeEnable : std_logic;

	-- Read port
	signal readAddr    : std_logic_vector(9 downto 0);
	signal readData    : std_logic_vector(3 downto 0);
begin
	--dispClk_out <= dispClk;
	
	-- Instantiate the memory controller for testing
	uut: entity work.dpram
		port map(
			-- Write port
			wrclock   => sysClk,
			wraddress => writeAddr,
			data      => writeData,
			wren      => writeEnable,

			-- Read port
			rdclock   => sysClk,
			rdaddress => readAddr,
			q         => readData
		);

	-- Drive the clocks. In simulation, sysClk lags 4ns behind dispClk, to give a visual hold time
	-- for signals in GTKWave.
	process
	begin
		sysClk <= '0';
		dispClk <= '0';
		wait for 20 ns;
		loop
			dispClk <= not(dispClk);  -- first dispClk transitions
			wait for 4 ns;
			sysClk <= not(sysClk);  -- then sysClk transitions, 4ns later
			wait for 3 ns;
			fooClk <= sysClk;
			wait for 3 ns;
		end loop;
	end process;

	-- Drive the unit under test. Read stimulus from stimulus.sim and write results to results.sim
	process
		variable inLine  : line;
		variable outLine : line;
		file inFile      : text open read_mode is "stimulus.sim";
		file outFile     : text open write_mode is "results.sim";
	begin
		writeAddr <= (others => '0');
		writeData <= (others => '0');
		writeEnable <= '0';
		readAddr <= (others => '1');
		while ( not endfile(inFile) ) loop
			readline(inFile, inLine);
			while ( inLine.all'length = 0 or inLine.all(1) = '#' or inLine.all(1) = ht or inLine.all(1) = ' ' ) loop
				readline(inFile, inLine);
			end loop;
			wait until rising_edge(sysClk);
			writeAddr <= to_2(inLine.all(1)) & to_4(inLine.all(2)) & to_4(inLine.all(3));
			writeData <= to_4(inLine.all(5));
			writeEnable <= to_1(inLine.all(7));
			readAddr <= to_2(inLine.all(9)) & to_4(inLine.all(10)) & to_4(inLine.all(11));
			write(outLine, from_4("00" & writeAddr(9 downto 8)) & from_4(writeAddr(7 downto 4)) & from_4(writeAddr(3 downto 0)));
			write(outLine, ' ');
			write(outLine, from_4(writeData));
			write(outLine, ' ');
			write(outLine, writeEnable);
			write(outLine, ' ');
			write(outLine, '|');
			write(outLine, ' ');
			write(outLine, from_4("00" & readAddr(9 downto 8)) & from_4(readAddr(7 downto 4)) & from_4(readAddr(3 downto 0)));
			write(outLine, ' ');
			write(outLine, from_4(readData));
			writeline(outFile, outLine);
		end loop;
		writeAddr <= (others => '0');
		writeData <= (others => '0');
		writeEnable <= '0';
		readAddr <= (others => '1');
		wait;
	end process;
end architecture;
