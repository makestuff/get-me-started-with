#!/bin/bash

# Settings...
export TESTBENCH=dpram_tb
export ALTERA=/opt/altera/11.0sp1/quartus
export SIGNALS="\
	dispClk \
	--- \
	writeAddr \
	writeData \
	writeEnable \
	--- \
	readAddr \
	readData \
"

# Build the Altera megafunction library if necessary
if [ ! -e altera_mf ]; then
	vlib altera_mf
	vmap altera_mf altera_mf
	vcom -93 -work altera_mf ${ALTERA}/eda/sim_lib/altera_mf_components.vhd
	vcom -93 -work altera_mf ${ALTERA}/eda/sim_lib/altera_mf.vhd
fi

# Compile all our code
rm -rf work
vlib work
if [ ! -e hex_util.vhdl ]; then
	wget -q https://raw.github.com/makestuff/sim_utils/master/vhdl/hex_util.vhdl
fi
vcom -93 -work work hex_util.vhdl
vcom -93 -work work dpram.vhdl
vcom -93 -work work dpram_tb.vhdl

# Create startup files and vsim command-line
rm -f startup.tcl startup.do
echo "vcd file results.vcd" > startup.do
export VOPTARGS="+acc"
for i in ${SIGNALS}; do
	if [ "${i}" = "---" ]; then
		echo "gtkwave::/Edit/Insert_Blank" >> startup.tcl
	else
		export VOPTARGS="${VOPTARGS}+${TESTBENCH}/${i}"
		echo "gtkwave::addSignalsFromList ${i}" >> startup.tcl
		echo "vcd add ${i}" >> startup.do
	fi
done
echo 'for { set i 0 } { $i <= [ gtkwave::getVisibleNumTraces ] } { incr i } { gtkwave::setTraceHighlightFromIndex $i off }' >> startup.tcl
echo 'gtkwave::setLeftJustifySigs on' >> startup.tcl
echo 'gtkwave::setZoomFactor -7' >> startup.tcl
echo 'gtkwave::setMarker 120ns' >> startup.tcl
echo 'run 1000ns' >> startup.do
echo 'quit -f' >> startup.do

# Run simulation
vsim dpram_tb -c -do startup.do -voptargs="$VOPTARGS"

# Show results
cat results.sim
gtkwave -T startup.tcl results.vcd
