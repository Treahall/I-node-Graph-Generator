// Author: Trever Hall
// Dates: start - 4/21/2022 & last update - 4/23/2022
// Description: generates a .gv file of a directory tree.

#include <stdlib.h>
#include <stdio.h>         // FILE.
#include <sys/stat.h>      // lstat() - gets filepaths status, struct stat.
#include <stdint.h>        // uintmax_t.
#include <time.h>          // ctime()
#include <dirent.h>        // DIR, struct dirent.
#include <string.h>        // sprintf().
#include <unistd.h>        // getcwd().

int GV_NODE_COUNT =  0;
struct stat node;

// ========================================================================= //
//HELPER FUNCTIONS
void add_node(FILE *fp, char *path){
  fprintf(fp, "\t%i [label = <%s <br/> \n\tFile size: %jd <br/> \n\tBlocks Allocated: %jd <br/> \n\tI-node Number: %ju <br/> \n\tLink Count: %ju <br/>>];\n",
    GV_NODE_COUNT, path, (intmax_t) node.st_size,  (intmax_t) node.st_blocks,
    (intmax_t) node.st_ino, (intmax_t) node.st_nlink);
}


void write_directory_to_file(FILE *fp, char *directory)
{
  int current_dir_id = GV_NODE_COUNT;
  // Write directory entries to .gv file. //
  DIR *d = opendir(directory); // open the directory.
  struct dirent *dentry; // directory entry data structure.

  while ((dentry = readdir(d)) != NULL) // if there is something in the directory.
  {
    GV_NODE_COUNT++;
    // Append entry name to directory path.
    char new_path[255];
    sprintf(new_path, "%s/%s", directory, dentry->d_name);
    // verify lstat works for new path.
    if (lstat(new_path, &node) == -1)
    {
      perror("lstat");
      exit(EXIT_FAILURE);
    }
    // Add node and directory->entry relationship to .gv file. 
    add_node(fp, new_path);
    fprintf(fp, "\t%i -> %i\n\n", current_dir_id, GV_NODE_COUNT);

    // If directory, recursively write its contents. (excludes . and ..)
    if(dentry->d_type == DT_DIR 
      && strcmp(dentry->d_name,".") != 0 
      && strcmp(dentry->d_name,"..") != 0) 
    {
      write_directory_to_file(fp, new_path);
    }
  }

  closedir(d);
}


void write_GV(char *directory)
{
  FILE *fp;
  // Open the file.
  fp = fopen("graph.gv", "w+");
  // Print beginning of the .gv file
  fprintf(fp, "digraph {\n");
  fprintf(fp, "\trankdir=LR\n\n");
  fprintf(fp, "\t/* Relationships */\n");
  // Add root (in this context) directory information to .gv file.
  add_node(fp, directory);
  // Add a new line for human-readability.
  fprintf(fp, "\n");
  // Recursively write all contents of the directory.
  write_directory_to_file(fp, directory);
  // Print the closing bracket of the file.
  fprintf(fp, "}");
  // Close the file.
  fclose(fp);
}

// ========================================================================= //
// MAIN FUNCTION - ENRTY POINT
int main(int argc, char *argv[])
{
  // Handle arguments.
  char *directory;
  if(argc == 1){
    directory = getcwd(NULL, 0);
  }
  else if(argc == 2){
    directory = argv[1];
  }
  else{
    perror("Too many argments.");
    exit(EXIT_FAILURE);
  }

  // Verify lstat works for root (in this context) directory.
  if (lstat(directory, &node) == -1)
  {
    perror("lstat");
    exit(EXIT_FAILURE);
  }
  
  write_GV(directory);
  // Clean up allocated memory.
  free(directory);
}
