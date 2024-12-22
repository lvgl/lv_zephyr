# Zephyr base project for the LVGL project creator

This repository is used along with [LVGL project creator](https://lvgl.io/tools/project-creator) to
help user to quick setup a [Zephyr RTOS](https://zephyrproject.org/) application that uses
[LVGL](https://lvgl.io/) on top of the Zephyr Display subsystem.

## Why this is needed?

Zephyr RTOS already has the LVGL samples but they are
located inside of the Zephyr kernel source tree, and in general
the applications are intended to be out from it, the project
creator helps users with that.

## How to use it:

* Before use the project creator make sure you have [Zephyr installed](https://docs.zephyrproject.org/latest/develop/getting_started/index.htm) on the current folder;
* After that launch the project creator;
* After the creation just build the application as regular Zephyr project using `west build`;
