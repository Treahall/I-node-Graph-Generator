
# I-node Graph Generator 

### Description

C program that creates a GraphViz formatted file called "graph.gv" that represents the relationships of a directory. This GraphViz file can then be used to create a PDF using DOT. 

### How To Run (in Linux Terminal):

Files Needed (in the same directory):
- generate-graph.c
- makefile

Steps to Make PDF for Directory of the Executable:
1. Run the 'make' command from the Terminal.
    - This generates the executable.
    - Then it runs the executable with no arguments (which defaults the directory to the working directory of the executable).
    - Then it uses DOT to generate a PDF called 'graph.pdf'.

Steps to make PDF of a Directory you Specify:
1. Run the 'make' command from the Terminal.
    - This does all the above.
    - If you have already done this, then you can skip step 1.
1. Run the executable with a single argument, the directory to graph.
    - ./generate-graph \[path].
1. Rerun the 'make' command.
    - This will recognize that the graph.gv file has changed, and use DOT to remake the PDF.
    - Alternatively, you can run 'dot -Tpdf:cairo graph.gv -o graph.pdf'.
