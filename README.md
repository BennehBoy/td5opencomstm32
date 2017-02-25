# td5opencomstm32

Port of Luca Veronasi's (Luca72) td5opencom to STM32 (Maple Mini) http://luca72.xoom.it/td5mapsuiteweb/archive/td5opencom/

This port also includes Paul Jacobson's TD% Keygen code -> https://github.com/pajacobson/td5keygen

This version replaces the resistive button inputs with micro switches, and utilises an SSD1306 OLED display instead of an LCD display.

# WORK IN PROGRESS

This should be considered a work in progress, conversion to STM32 has required extensive modifications to how PID & string data is stored & accessed (no PROGMEM on ARM), so there are bugs.

Serial output is performed using STM32Duino Serial bit banging to an interfaced L9637D K-line driver
