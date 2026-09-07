# 5G Connected High-Speed Train Simulation

This repository contains an OMNeT++ simulation model developed to analyze the performance of a 5G connected high-speed train system. The project evaluates the overall system throughput and average response time by varying critical parameters such as passenger load, number of base stations, and handover delay.

## Overview

A high-speed train travels along a 10 km railway at a constant speed of 300 km/h, providing WiFi connectivity to onboard passengers. The train's Access Point (AP) connects to a 5G radio network consisting of multiple Base Stations (BSs) placed alongside the track.

The system's performance is driven by a dynamic transmission rate: as the train moves, the distance between the AP and the nearest BS changes, exponentially affecting the data rate. Furthermore, when the train switches to a closer BS, a handover operation occurs, temporarily suspending data transmission.

## Key Features & Modeling
*   **Dynamic Transmission Rate:** Modeled using an exponential function based on the physical distance between the AP and the closest BS.
*   **Handover Mechanism:** Implements a deterministic delay (modeled as a uniform random variable) whenever the AP connects to a new BS, causing a temporary transmission halt.
*   **Passenger Traffic:** Modeled as independent packet generators following exponential distributions for both packet size and generation time.

## Simulation Insights
Through extensive full factorial analysis across hundreds of configurations, the simulation revealed a dual behavior in the system:
*   **Low Load:** System performance is stable and primarily constrained by the passenger input rate. The number of BSs has minimal impact.
*   **High Load (Saturation):** The system reaches its maximum output capacity. A critical trade-off emerges: increasing the number of Base Stations boosts the minimum data rate but introduces more frequent handover delays. The optimal number of BSs is highly sensitive to the duration of the handover operation.


## Authors

This project was made with Eleonora Sgorbini ( [@ele239](https://github.com/ele239) ) for the Performance Evaluation of Computer Systems and Networks class.

Grade: 30/30
