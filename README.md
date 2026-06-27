## Graph Theory Project: Influence Blocking Maximization Problem


This repository contains the implementation of different heuristics regarding the aforementioned IBM problem, and aims to compare their time consumption and amount of negatively activated nodes.

### Greedy (Budak et al.)
The more intuitive approach, this heuristic runs a number of monte-carlo simulations in order to greedily select the seeds infected the most times as the initial k positive seeds

### CMIA-H (Wu et al.)
In order to avoid large time consumption, this heuristic approximately computes the negative influence spread through Maximum Influence Arborescences (MIAs) in order to greedily select the initial k positive seeds

### How it works
For accurate comparisons, each algorithm must be executed with the same dataset as input, namely the NetHEPT dataset inside the datasets folder. The outputs will then be generated on the 'output_meio' folder, indicating the positive seeds chosen by said heuristic. Lastly, statistics.cpp must then be ran (you choose which heuristic to compare while running), and CMICM simulations will be executed in order to get data on the choice of the positive seed set in each heuristic. The final results are inside the 'output_final' folder.

(sorry if that was confusing, i'm genuinely exhausted)


Carolina Nunes de Carvalho, Ian Fernandes França