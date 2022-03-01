# -Embedded-Railway-control-system

The project simulates a seminar that is a traffic light in the following way: a main button that switches between modes:

1. Drive - Main Green Secondary Green 
2. Drive at Limited Speed - Main Yellow Secondary Yellow and Green  
3. Stop - Main Red Secondary Yellow 

In addition, there is an emergency button that, when pressed, flashes a red emergency light and enters the stop mode, and it is not possible to switch positions on the main button only when a long press on the emergency button is pressed.

In addition there is a screen that shows the speed allowed for driving, you can change the speed when in mode (2) and long press. In this mode, short presses will change the speed and a long press will determine that it is the new speed.

If the system temperature exceeds 30 degrees the emergency button will turn on and off (half a second on and half a second off) and the speed limit displayed on the screen will decrease to 8 if it was higher than 8 and otherwise the speed will remain as it is.

When in mode (1), the logo of the Israel Railways will be displayed on the screen.

The speeds are kept in memory so when you turn off the system the last speed is kept.
