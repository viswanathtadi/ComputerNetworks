# Instructions to setup environment-

### Files in this folder.
- `a2.cpp`
- `visualise.py`
- `Makefile`
- `script.py`
- `README.md`

### Generating required executables.
- Run `make` in terminal to build essential executables
- This will the `cw` object.

### Running the simulation for a given parameters.
- Run `./cw -i Ki -m Km -n Kn -f Kf -s Ps -T T -o outfile`
- This will generate outfile with cwnd values.

### Visualising the Results.
- Run `python3 visualise.py outfile` to generate the plot corresponding to outfile.
- The created plot will have same name as outfile with `.png` extension. 

### Running the script file.
- run `python3 script.py` to generate all outfiles and plots for some predefined configuration of parameters.

### Cleaning instructions.
- Run `make clean` to remove executables.
- Run `make cleane` to remove executables and all simulation data. 	