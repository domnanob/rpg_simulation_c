# C RPG Simulation (Multithreaded)

This project is a console-based RPG battle simulation written in C, focusing on multithreaded execution and inter-thread communication.
Besides basic RPG mechanics, the application demonstrates the use of semaphores and pipelines to coordinate parallel tasks.
The project was created primarily for educational purposes, combining game simulation with operating system–level concepts.

## Key Features
- Turn-based RPG combat simulation
- Multiple entity types (player characters and enemies) 
- Multithreaded execution
- Semaphore-based synchronization
- Pipeline-based data flow between threads
- Accuracy-based combat and random events
- Console-based visualization of game events

## Concurrency Model
The application runs on multiple threads, where different parts of the simulation (e.g. combat steps, entity actions, or event handling) execute in parallel.
### Semaphores
- Semaphores are used to synchronize threads and protect shared resources.
- They ensure that critical sections (such as combat resolution or state updates) are accessed safely.
- Prevents race conditions between concurrently running game logic.
### Pipelines
- Pipelines are used for communication between threads.
- Game events and results are passed through pipelines to maintain a clear execution order.
- This design separates computation from output handling and improves structure.

## Game Mechanics
- Entities have health, power, accuracy, and type.
- Attacks succeed based on accuracy.
- Ranged and Meele characters receive temporary accuracy buffs.
- Characters can heal up to a maximum of 100% health.
- An entity is alive while health > 0.
