# Instructions to setup environment-

### Files in this folder.
- `a3.py`
- `launch.py`
- `Makefile`
- `comments.txt`
- `README.md`
- `REPORT.pdf`
- `infile.txt`

### Running simulation for each router.
- Run `python3 a3.py -i id -f infilename.txt -o outfilename -h hi -a lsai -s spfi`

### Launch simulation for all the routers with single command.
- Run `python3 launch.py routers infilename.txt outfilename hi lsai spfi` to launch the simulations for all the routers simultaneously.
- This command will launch and exit.
- The simulations will run in the background.

### Simulation running time.
- Default running time for the simulation is set to 45 seconds.
- This can be adjusted in the code easily.

### Outputs.
- Each router will produce a output file.
- Specifically, a router with number `id` will produce `outfilename-id.txt` file.
- Each output file contains the router distance and path table at different intervals of time.
- The output format is `destination-id path-to-destination-id distance-to-destination-id`

### Cleaning instructions.
- Run `make cleane` to delete all input and output files. 	
