# Synchronous Mini Router (VHDL)

This repository contains the VHDL implementation of a synchronous mini-router designed to multiplex two data sources into a single output channel. The design targets high-speed performance and minimal resource utilization on FPGA architectures.

## Architecture Overview
The router operates using a fully combinatorial decision logic rather than a Finite State Machine (FSM). This architectural choice drastically reduces complexity and hardware footprint. It implements a robust Quality of Service (QoS) mechanism through a dual-tier conflict resolution strategy:
1. **Strict Priority:** Each data packet includes 2 priority bits (0 to 3). The router always serves the link with the higher priority first.
2. **Round Robin Fairness:** If both links request transmission simultaneously with identical priorities, a Round Robin algorithm alternates between the two sources to prevent starvation.

## Technical Specifications
*   **Data Width:** 8-bit data + 2-bit priority per link.
*   **Flow Control:** Handshake mechanism utilizing `req`, `grant`, and `valid` signals to prevent data loss.
*   **Target Hardware:** Zybo Development Board (xc7z010clg400-1) synthesized via Xilinx Vivado.
*   **Max Clock Frequency:** ~334.4 MHz.
*   **Resource Utilization:** Extremely lightweight (12 LUTs, 46 Flip-Flops).
*   **Dynamic Power:** ~0.001 W.

## Verification & Synthesis
The design has been validated through a ModelSim testbench covering edge cases such as rapid link switching, equal-priority conflicts, and zero-input transparency. Full synthesis reports, critical path analysis, and RTL schematics are included in the project documentation.


## Author

This project was made by myself for the Electronics class

Grade: 30/30
