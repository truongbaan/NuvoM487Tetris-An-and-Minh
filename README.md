# NuvoM487Tetris-An-and-Minh

A Tetris game implementation designed to run on the Nuvoton Nuvo M487 microcontroller. Developed in C as part of the Embedded Systems coursework, this assignment covers two assignment questions: rewriting Tutorial 8 and building the full Tetris gameplay loop.

## Assignment Overview

This repository contains two main subfolders corresponding to the assignment tasks:

* **Question1/**: Reimplementation of Tutorial 8 exercises with enhancements.
* **Question2/**: Full Tetris game loop (`gameplay.c`) running on the Nuvo M487 board. It handles board initialization, piece generation and movement, scoring, level progression, pause/resume, and game over conditions.

The game makes extensive use of:

* 2D array board representation (`uint8_t board[HEIGHT][WIDTH]`)
* Timer interrupts for piece descent
* LCD drawing routines for board, score, level, and time
* Button input handling for movement, rotation, and pause/exit

---

## Features

* Standard Tetris gameplay on embedded hardware
* Score tracking and high‑score display
* Level progression with adjustable fall speed
* Countdown timer display (mm\:ss)
* Pause/Resume functionality
* Single‑use exit (ESC) to quit mid‑game

---

## Software Requirements

* Keil MDK‑ARM 
* Nuvoton M487 firmware libraries


## Assignment Questions

* **Question 1**: Rewriting Tutorial 8 and some write some draw functions.
* **Question 2**: Comprehensive Tetris gameplay loop with scoring, timing, and hardware integration.

---

## Authors

* **An** – Core gameplay logic, timer and input handling
* **Minh** – LCD rendering, scoring, and diagram documentation

---

*This project was developed for the Embedded Systems course at RMIT.*
