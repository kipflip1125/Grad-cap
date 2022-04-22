# Grad-cap
arduino controlled led matrix for graduation

The main file of the program has the drawing functions needed for the led matrix. In the setup function we establish a bluetooth signal and begin advertising
to connect with a phone to be controlled. Within the loop we check if a connection is made and while it is connected check the value that is written in the
bluetooth characteristic.

The images shown are formatted into an array of 16 bit hex values for GRB. 6 bits for green 5 bits for blue and red. These get saved into the aruino nano's
internal memory.
