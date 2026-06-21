## Graph Theory Project: Influence Blocking Maximization Problem


This repository contains the implementation of different heuristics regarding the aforementioned IBM problem, and aims to compare their time consumption and amount of negatively activated nodes.

### Greedy (Budak et al.)
The more intuitive approach, this heuristic runs a number of monte-carlo simulations in order to greedily select the seeds infected the most times to 

### CMIA-H (Wu et al.)
In order to avoid large time consumption, this heuristic approximately computes the negative influence spread through Maximum Influence Arborescences (MIAs) in order to greedily select the positive seeds

### How it works
For accurate comparisons, each algorithm must be executed with the same dataset as input. The outputs will then be generated on the 'output' folder, and CMICM simulations must then be ran in order to get data on the choice of the positive seed set in each heuristic. At last, the python file 'generate_graphs' must be ran in order to (surprise surprise) generate the graphs that compare the heuristics.

Carolina Nunes de Carvalho, Ian Fernandes França