# ADVISE Car Attack Modeling

This folder contains an ADVISE (Adversary View Security Evaluation) model developed within the Mobius modeling environment. The project analyzes cyber threats targeting the communication channels of connected vehicles, specifically evaluating the probability of an attacker successfully exfiltrating private data.

## Overview

The model assesses vehicle security by simulating different attack vectors and adversary capabilities. 

### Key Vulnerabilities Modeled
*   **V2X Spoofing:** Crafting and sending malicious V2X messages via proximity network access.
*   **CAN Bus Injection:** Injecting malicious traffic directly into the internal Controller Area Network.
*   **Gateway Compromise:** Reflashing or rewriting the gateway firmware chip.
*   **Remote Code Injection:** Injecting tampered software binaries through remote network access.
*   **OEM Impersonation:** Sending malicious proprietary messages by impersonating legitimate entities.

### Adversary Profiles
The project simulates three distinct attacker mindsets to observe how varying constraints influence the attack execution path:
*   **Payoff-Based:** Prioritizes success rate and maximum reward. Defaults to direct remote extraction, but shifts to internal CAN bus compromise if possessing expert hardware skills.
*   **Cost-Based:** Focuses on minimizing financial investment. Consistently targets proximity-based V2X interfaces due to the extremely low barrier to entry.
*   **Detection-Based (Stealthy):** Prioritizes avoiding detection. Abandons noisy methods like V2X manipulation in favor of OEM impersonation or highly discrete hardware-level compromises.

## Authors
This project was made for the Dependable and Distributed Systems exams, together with my colleague Eleonora Sgorbini ( [@ele239](https://github.com/ele239) )
Grade: 30
