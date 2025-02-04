# Efficient Circular Buffer

A simple and efficient implementation of a circular buffer. Implemented in JUCE as as simple delay plugin.
Buffer size must be a power of 2.
Based on bitwise AND instead of modulo or branching. Ref. https://homepage.cs.uiowa.edu/~jones/bcd/mod.shtml#exmod2 


Interpolation methods for reading values out of the buffer:

- no interpolation
- linear
- cubic
- Hermite

Ref.  https://www.musicdsp.org/en/latest/Other/93-hermite-interpollation.html 

https://paulbourke.net/miscellaneous/interpolation/
