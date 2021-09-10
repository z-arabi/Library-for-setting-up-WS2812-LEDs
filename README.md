<h1> Library-for-setting-up-WS2812-LEDs </h1>

<h2> Introduction <\h2>
One of the most widely used parts in electronic devices is RGB LEDs, which are produced in different sizes and different types. One of its types is ws2812.
These LEDs consist of an input data line and an output data line which are cascaded. For the size of each LED, a number of pulses must be given to each LED to get the desired color.
In this project, we want to develop a library in which we can simply quantify n LEDs to get the desired color.

<h2>
  Implementation
</h2>
The library is based on ARM Programming Language and written in Keil uVision, a window-based software development platform for ARM-based microcontrollers. 
The microcontroller used is STM32F103 - Arm Cortex-M3 Microcontrollers.
There are multiple libraries for writing the program to implement on ARM microcontrollers, and generally, I would prefer to use the HAL library for STM32 ARM Cortex.
