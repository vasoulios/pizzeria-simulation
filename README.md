# Pizzeria Simulation

A multithreaded C program simulating a pizzeria using pthreads. It manages pizza orders from placement to delivery, handling limited resources (2 cooks, 15 ovens, 2 packers, 10 deliverers). Each order has a random number of plain or special pizzas, with a 10% payment failure chance. The program tracks income, order success/failure, and calculates average/max delivery and cooling times.

## Usage
Compile and run:
```bash
gcc p3200105-p3200181-pizzeria.c -pthread
./a.exe <number_of_orders> <random_seed>