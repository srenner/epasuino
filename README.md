# epasuino

**EPAS** + ard**UINO** = epasuino. Pronounced "eh-puh-sween-oh." Or however you want. I don't care. Speed sensitive power steering for automobiles.

## Overview
Typical aftermarket electric power assist steering (EPAS) kits for automobiles use an adjustment knob to tailor how much power assist to provide to the steering system. The user turns the knob up in parking lots, and turns it down on the highway, or selects a middle ground that's ok for every scenario, but rarely ideal. epasuino uses the speed of the vehicle to adjust the amount of assist.

This project has a secondary purpose of taking the speed sensor info and sending it on the CAN bus to a MegaSquirt ECU and my [carfuino performance computer](https://github.com/srenner/carfuino).

## Hardware details
* [SparkFun AST-CAN485 Dev Board](https://www.sparkfun.com/products/14483)
* [Ford 8,000 pulse per mile speed sensor](https://lmr.com/item/LRS-9731B/83-93-Mustang-Speed-Sensor)
* [VR Conditioner Board](http://jbperf.com/dual_VR/v2_1.html)
* On/off switch
* 100k ohm digital potentiometer
* 100k ohm analog potentiometer
