# Monte Carlo Ising Model simulator

The program simulates the spin orientations of atoms in a ferromagnetic (J>0) or anti-ferromagnetic (J<0) material.
Temperature and external applied magnetic field are adjustible. At lower temperatures (T<2.0) and no external
magnetic field, the spins form domains and at higher temperatures, the spins are erratic (this illustrates the
Curie temperature). Hysteresis effect can also be observed by applying a magnetic field in one direction, getting
all spins aligned and then trying to align it back with opposite magnetic field.

## Demo

## Usage

### Windows
* make sure SDL2.dll is in path or in same folder as the executable

### Linux


## To-do
* make Makefile work with both win and linux
* draw parameters on screen
