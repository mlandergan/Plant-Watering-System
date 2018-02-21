# Plant-Watering-System
Waters indoor plants using an arduino 

This self-watering plant system checks the moisture levels of the 
soil and activates a water pump if it meets a set threshold. 
Since the sensor is a cheap one from china, it can become oxidized if
continuously powered, so instead I connected VCC of the sensor to a digital pin. 
Currently, my approach is to activate the sensor, and  sample the sensor using an
interrupt service routine. I have it set up so it can only water twice a day and 
implemented a second check that if doesn't get watered at least once during a 
24 hr period then it will water it regardless of the soil level.
