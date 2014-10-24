Modification on the Toradex Evalboard to run the CanLib on PXA310 module:
**************************************************************************
The CAN interrupt signal on the Toradex Colibri EvalBoard is connected to Colibri SODIMM pin 73 which isn't a real GPIO on the PXA310 module.
To use the EvalBoard CAN in combination with the Colibri PXA310 module you have to connect two pins of the EvalBoard jumper area:
X12-31  <--> X12-47
This connects the interrupt signal from the CAN controller with valid (interrupt capable) GPIO of the PXA310 module.

For all other modules (PXA270, PXA300, PXA320) no modification is needed.


Please contact colibri@toradex.com for more information.

Toradex AG