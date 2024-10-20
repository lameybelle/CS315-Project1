#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>


void listDir() {
	//ls command
	DIR *dir = opendir(".");

	struct dirent *read_dir;

	if (dir != NULL) {
		while ((read_dir = readdir(dir)) != NULL) {
			write(1, read_dir->d_name, strlen(read_dir->d_name));
			write(1, " ", strlen(" "));
		}
		write(1, "\n", strlen("\n"));
	} else {
		write(1, "Error in opening directory\n", 27);
		return;
	}
	closedir(dir);
}

void showCurrentDir() {
	//pwd command
	long size;
	char* ptr = NULL;

	size = pathconf(".", _PC_PATH_MAX);
	char buff[(size_t) size];

	ptr = getcwd(buff, (size_t)size);

	write(1, ptr, strlen(ptr));
	write(1, "\n", strlen("\n"));
}

void makeDir(char *dirName) {
	//mkdir command
	int check;
	
	check = mkdir(dirName, 0777);

	if (check == -1) {
		write(1, "Directory Already Exists\n", 26);
		return;
	}
}

void changeDir(char *dirName) {
	//cd command
	int check;
	check = chdir((const char*)dirName);

	if (check == -1) {
		write(1, "Error Dir couldn't be changed\n", 30);
		return;
	}
}

void copyFile(char *sourcePath, char *destinationPath) {
	//cp command
	int srcCheck, destCheck, standardCheck;
	char* buff[1024];

	srcCheck = open(sourcePath, O_RDONLY);
	if (srcCheck < 0) {
		write(1, "Error with opening source\n", strlen("Error with opening source\n"));
		return;
	}

	destCheck = open(destinationPath, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

	if (destCheck < 0) {
		close(srcCheck);
		write(1, "Error something went wrong with dest\n", strlen("Error something went wrong with dest\n"));
		return;
	}


	while ((standardCheck = read(srcCheck, buff, BUFSIZ)) != 0) {
		write(destCheck, buff, standardCheck);
	}

	if (standardCheck == -1) {
		write(1, "Error reading\n", strlen("Error reading\n"));
		return;
	}

	if (close(srcCheck) == -1) {
		write(1, "Error closing source\n", strlen("Error closing source\n"));
		return;
	}

	if(close(destCheck) == -1) {
		write(1, "Error closing dest\n", strlen("Error closing dest\n"));
		return;
	}

}

void moveFile(char *sourcePath, char *destinationPath) {
	//mv command
	int check = 0;

	DIR* dir = opendir(destinationPath);

	if (dir != NULL) {
		int newPath = strlen(sourcePath) + strlen(destinationPath);
		char newDest[newPath];

		strcpy(newDest, destinationPath);
		strcat(newDest, "/");
		strcat(newDest, sourcePath);
		check = rename(sourcePath, newDest);
	} else {
		check = rename(sourcePath, destinationPath);
	}
	
	if (check == -1) {
		write(1, "Error moving file\n", strlen("Error moving file\n"));
		return;
	}

	closedir(dir);

}

void deleteFile(char *filename) {
	//rm command
	int check = 0;

	check = remove(filename);

	if (check == -1) {
		write(1, "Error deleting file\n", strlen("Error deleting file\n"));
		return;
	}

}

void displayFile(char *filename) {
	//cat command
	char buff[BUFSIZ];

	for (int i = 0; i < BUFSIZ; i++) {
		buff[i] = (unsigned long int) NULL;
	}

	int check = open(filename, O_RDONLY);

	if (read(check, buff, BUFSIZ) < 0) {
		write(1, "Error reading file\n", strlen("Error reading file\n"));
		return;
	}

	write(1, buff, strlen(buff));

	close(check);
}
