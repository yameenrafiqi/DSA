#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------------- Structure Definitions -------------------------

// Structure to represent a File
typedef struct File {
  char *filename;
  struct File *next;
} File;

// Structure to represent a Directory
typedef struct Directory {
  char *directoryName;
  struct Directory *subdirectories; // Head of subdirectories linked list
  struct Directory *next;           // Next sibling directory
  File *files;                      // Head of files linked list
  struct Directory *parent;         // Pointer to parent directory
} Directory;

// Structure to represent the File System
typedef struct FileSystem {
  Directory *rootDirectory;
} FileSystem;

// ------------------------- Function Prototypes ------------------------

Directory *getDirectSubdirectory(Directory *parent, const char *dirName);
Directory *add_subdirectory(Directory *dir, const char *directoryName);
int add_file(Directory *dir, const char *filename);
void print_directory_contents(Directory *dir);
void print_filesystem(FileSystem *fs);
void print_directory_recursive(Directory *currentDir, int depth);

// ------------------------- Function Implementations ---------------------

// Function to get a direct subdirectory by name
Directory *getDirectSubdirectory(Directory *parent, const char *dirName) {
  Directory *current = parent->subdirectories;
  while (current) {
    if (strcmp(current->directoryName, dirName) == 0) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

// Function to add a subdirectory to a directory
Directory *add_subdirectory(Directory *dir, const char *directoryName) {
  // Check if the subdirectory already exists
  if (getDirectSubdirectory(dir, directoryName)) {
    return NULL;
  }

  // Create new subdirectory
  Directory *newDir = (Directory *)malloc(sizeof(Directory));
  newDir->directoryName = strdup(directoryName);
  newDir->subdirectories = NULL;
  newDir->files = NULL;
  newDir->parent = dir;
  newDir->next = dir->subdirectories;
  dir->subdirectories = newDir;

  return newDir;
}

// Function to add a file to a directory
int add_file(Directory *dir, const char *filename) {
  File *current = dir->files;
  while (current) {
    if (strcmp(current->filename, filename) == 0) {
      return 0; // File already exists
    }
    current = current->next;
  }

  // Create new file
  File *newFile = (File *)malloc(sizeof(File));
  newFile->filename = strdup(filename);
  newFile->next = dir->files;
  dir->files = newFile;

  return 1;
}

// Function to print the contents of a directory
void print_directory_contents(Directory *dir) {
  Directory *subDir = dir->subdirectories;
  File *file = dir->files;

  printf("Directories:\n");
  while (subDir) {
    printf("  %s/\n", subDir->directoryName);
    subDir = subDir->next;
  }

  printf("Files:\n");
  while (file) {
    printf("  %s\n", file->filename);
    file = file->next;
  }
}

// Recursive helper function to print the entire filesystem
void print_directory_recursive(Directory *currentDir, int depth) {
  Directory *subDir = currentDir->subdirectories;
  while (subDir) {
    for (int i = 0; i < depth; ++i)
      printf("  ");
    printf("Directory: %s\n", subDir->directoryName);
    print_directory_recursive(subDir, depth + 1);
    subDir = subDir->next;
  }

  File *file = currentDir->files;
  while (file) {
    for (int i = 0; i < depth; ++i)
      printf("  ");
    printf("File: %s\n", file->filename);
    file = file->next;
  }
}

// Function to print the entire filesystem
void print_filesystem(FileSystem *fs) {
  Directory *dir = fs->rootDirectory;
  printf("Root Directory: %s\n", dir->directoryName);
  print_directory_recursive(dir, 1);
}

// Function to create a new directory
Directory *create_directory(const char *name, Directory *parent) {
  Directory *newDir = (Directory *)malloc(sizeof(Directory));
  if (!newDir) {
    perror("Failed to allocate memory for directory");
    exit(EXIT_FAILURE);
  }
  newDir->directoryName = strdup(name);
  newDir->subdirectories = NULL;
  newDir->files = NULL;
  newDir->parent = parent;
  newDir->next = NULL;
  return newDir;
}

// Function to create a file system with a root directory
FileSystem *create_filesystem(const char *rootName) {
  FileSystem *fs = (FileSystem *)malloc(sizeof(FileSystem));
  if (!fs) {
    perror("Failed to allocate memory for file system");
    exit(EXIT_FAILURE);
  }
  fs->rootDirectory = create_directory(rootName, NULL);
  return fs;
}

// Utility function to remove trailing newline
void trim_newline(char *str) {
  if (!str)
    return;
  size_t len = strlen(str);
  if (len == 0)
    return;
  if (str[len - 1] == '\n') {
    str[len - 1] = '\0';
  }
}

// ------------------------- Main Function ------------------------------

int main() {
  FileSystem *fs = create_filesystem("root");
  Directory *head = fs->rootDirectory;
  Directory *dir = NULL;

  // Variable to store user input
  char input[256];

  printf("Welcome to the In-Memory File System!\n");
  printf("Available commands:\n");
  printf("  cd <directory_name>\n");
  printf("  mkdir <directory_name>\n");
  printf("  touch <file_name>\n");
  printf("  ls\n");
  printf("  exit\n\n");

  while (1) {
    printf(">> "); // Prompt

    // Read input from the user
    if (!fgets(input, sizeof(input), stdin)) {
      printf("Error reading input. Exiting.\n");
      break;
    }

    // Remove the trailing newline character
    trim_newline(input);

    // Check for empty input
    if (strlen(input) == 0) {
      continue;
    }

    // Tokenize the input into command and argument
    char *command = strtok(input, " ");
    char *argument = strtok(NULL, " ");

    // Handle commands
    if (strcmp(command, "cd") == 0) {
      if (argument == NULL) {
        printf("Error: 'cd' command requires a directory name.\n");
        continue;
      }

      if (strcmp(argument, "..") == 0) {
        if (head != fs->rootDirectory) {
          head = head->parent;
          printf("Switched to parent directory %s\n", head->directoryName);
        } else {
          printf("You are already at root directory\n");
        }
        continue;
      }

      dir = getDirectSubdirectory(head, argument);
      if (dir) {
        printf("Switched to directory %s\n", argument);
        head = dir;
      } else {
        printf("No such subdirectory here\n");
      }

    } else if (strcmp(command, "mkdir") == 0) {
      if (argument == NULL) {
        printf("Error: 'mkdir' command requires a directory name.\n");
        continue;
      }

      dir = add_subdirectory(head, argument);
      if (dir) {
        printf("Created SubDirectory %s\n", argument);
      } else {
        printf("SubDirectory with name %s already exists here\n", argument);
      }

    } else if (strcmp(command, "touch") == 0) {
      if (argument == NULL) {
        printf("Error: 'touch' command requires a file name.\n");
        continue;
      }

      int x = add_file(head, argument);
      if (x) {
        printf("Created file %s\n", argument);
      } else {
        printf("File with name %s already exists here\n", argument);
      }

    } else if (strcmp(command, "ls") == 0) {
      print_directory_contents(head);

    } else if (strcmp(command, "print") == 0) {
      print_filesystem(fs);

    } else if (strcmp(command, "exit") == 0) {
      printf("Exiting the File System. Goodbye!\n");
      break;

    } else {
      printf("Error: Unknown command '%s'.\n", command);
    }
  }

  return 0;
}