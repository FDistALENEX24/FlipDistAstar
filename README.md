# FlipDistAstar
The Code for the paper "Engineering an Astar Algorithm for the Flip Distance of Plane Triangulations" for review purposes.

# Dependencies
C++:

	- CGAL
	- Gurobi 9.5 (If you want to use gurobi 10.0 you may have to change FindGurobi.cmake)

Python:

 	- pandas
	- seaborn
 	- matplotlib



# Our result files
The data is given with our complete result files. If you want to repeat our experiments you first need to execute purge.py.



# Building our code 
```bash
git clone {link here}
```
Then move into the project folder and:
```bash
mkdir build
cd build
cmake ..
make 
```

Then (if you have purged the result files) you can either execute the given shell scripts, or call the binary with four or five arguments:
```bash
./A_star_for_flipdistance algorithm instances size order 
./A_star_for_flipdistance algorithm instances size order path
```
with:

	1. algorithm = eppstein heuristic ilp bfs heuristic
	2. instances = sealevel random
	3. size = 10 15 20 25 30 for random and 25 30 for sealevel
	4. order = 2,5,25,30 for sealevel and always 1 for random
	5. path = path to the folder that contains "random_experiments_paper" and "sealevel_experiments_paper"

If you do not specify the path it's assumed that the folder "data" is on the same level as build
