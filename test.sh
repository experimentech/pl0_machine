#!/bin/bash
iverilog -o pl0_machine_tb.vvp pl0_machine.v tb_pl0_machine.v
vvp pl0_machine_tb.vvp

