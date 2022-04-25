// Author: Trever Hall
// Dates: start - 4/21/2022 & last update - 4/24/2022
// Description: generates a .gv file of a directory tree.

#include <stdlib.h>
#include <stdio.h>         // FILE.
#include <sys/stat.h>      // lstat() - gets filepaths status, struct stat.
#include <stdint.h>        // uintmax_t.
#include <time.h>          // ctime()
#include <dirent.h>        // DIR, struct dirent.
#include <string.h>        // sprintf().
#include <unistd.h>        // getcwd().

/// Globals used to simplify function parameters, and overall code. ///
// Used to give each node a unique number in the .gv file.
int GV_NODE_COUNT = 0;
// Struct used to store the current nodes information.
struct stat NODE;


/// ======================================================================= ///
// HELPER FUNCTIONS //

// Adds a node to the .gv file for the given path.
void add_node(FILE *fp, char *path) {
  fprintf(fp, "\t%i [label = <%s <br/> \n\tFile size: %jd <br/> \n\tBlocks Allocated: %jd <br/> \n\tI-node Number: %ju <br/> \n\tLink Count: %ju <br/>>];\n",
    GV_NODE_COUNT, path, (intmax_t) NODE.st_size,  (intmax_t) NODE.st_blocks,
    (intmax_t) NODE.st_ino, (intmax_t) NODE.st_nlink);
}



// Recursively writes all the directory's contents as nodes
// and relationships to a .gv file. 
void write_directory_to_file(FILE *fp, char *directory)
{
  // Log number representing this directory in the .gv file. 
  int current_dir_id = GV_NODE_COUNT;
  // Open the directory for reading.
  DIR *d = opendir(directory);
  // Structure used to store current directory entry's information.
  struct dirent *dentry; 

  // If not empty, read an entry from the directory into dentry.
  while ((dentry = readdir(d)) != NULL) {
    // Increment count to give it a unique node number.
    GV_NODE_COUNT++;

    // Get name of new path.
    char new_path[255];
    sprintf(new_path, "%s/%s", directory, dentry->d_name);

    // Load the nodes stats, into global variable NODE.
    if (lstat(new_path, &NODE) == -1) {
      perror("lstat");
      exit(EXIT_FAILURE);
    }

    // Add node and add directory->entry relationship to .gv file. 
    add_node(fp, new_path);
    fprintf(fp, "\t%i -> %i\n\n", current_dir_id, GV_NODE_COUNT);

    // If dentry is a directory, 
    // then recursively write its contents (excluding . and ..).
    if(dentry->d_type == DT_DIR 
      && strcmp(dentry->d_name,".") != 0 
      && strcmp(dentry->d_name,"..") != 0) {
      write_directory_to_file(fp, new_path);
    }
  }

  // Close the directory.
  closedir(d);
}



// Writes a DOT-readable GraphViz file (graph.gv) of a given directory.
void write_GV(char *directory)
{
  // Verify lstat works for (in this context) the root directory.
  if (lstat(directory, &NODE) == -1)
  {
    perror("lstat");
    exit(EXIT_FAILURE);
  }

  // Init a FILE pointer.
  FILE *fp;
  // Open the file for re-writing.
  fp = fopen("graph.gv", "w+");

  // Print beginning of the .gv file
  fprintf(fp, "digraph {\n");
  fprintf(fp, "\trankdir=LR\n\n");
  fprintf(fp, "\t/* Relationships */\n");

  // Add (in this context) root directory's information to .gv file.
  add_node(fp, directory);
  // Add a new line to improve human-readability.
  fprintf(fp, "\n");

  // Recursively write all contents of the directory.
  write_directory_to_file(fp, directory);

  // Print the closing bracket of the file.
  fprintf(fp, "}");

  // Close the file.
  fclose(fp);
}



/// ======================================================================= ///
// MAIN FUNCTION - ENRTY POINT //

int main(int argc, char *argv[])
{
  // Handle arguments to decide directory used. //
  char *directory;
  if(argc == 1){
    //Used directory the executable is called from.
    directory = getcwd(NULL, 0);
  }
  else if(argc == 2){
    //Use the directory given as a single argument.
    directory = argv[1];
  }
  else{
    perror("Too many argments.");
    exit(EXIT_FAILURE);
  }
  
  // Call function that writes the i-node graph to "graph.gv".
  write_GV(directory);
  // Clean up allocated memory.
  free(directory);
}
