This is a program to evaluate the TI's sensor hub boards.Due to the tight
schedule,not all the functions of sensors are developed.For example,there
are several modes in ISL29023 but it's only set to detect the ambient
light.However,the main difference of each mode is sending the different 
commands to different registers.
  
A simple user interface is used for selecting different sensors to
evaluate.Two buttons on the MSP430 are used to select sensors and to switch
between low power mode and working mode.All the data are sent to PC and
plotted in SerialChart.
  
Note:All the functions with prefix 'read' return the raw data of a sensor,
and all the functions with prefix 'get' return the converted data.

For more detailed desription,please go to this page:

 (Sensor Data Acquisition,Filtering,and Fusion Based on MSP430F5529 and Sensor Hub)[http://lichaoma.com/2016/01/03/sensor-data-acquisitionfilteringand-fusion-based-on-msp430f5529-and-sensor-hub/]
