#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>  // For Windows functions
#include <sys/stat.h>  // For Linux/macOS: mkdir
#include <errno.h>  // For checking error

#ifdef _WIN32
    #define CREATE_FOLDER(name) CreateDirectory(name, NULL)
    #define SET_HIDDEN_ATTRIBUTE(name) SetFileAttributes(name, FILE_ATTRIBUTE_HIDDEN)
#else
    #define CREATE_FOLDER(name) mkdir(name, 0777)
    #define SET_HIDDEN_ATTRIBUTE(name)  // No need for extra hidden attribute on Linux
#endif

// Function to generate a random string of given length
void generate_random_string(char *str, size_t length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (size_t i = 0; i < length; i++) {
        int key = rand() % (int)(sizeof(charset) - 1);
        str[i] = charset[key];
    }
    str[length] = '\0';
}

// Function to check if a folder exists
int folder_exists(const char *folder_name) {
#ifdef _WIN32
    DWORD fileAttr = GetFileAttributes(folder_name);
    return (fileAttr != INVALID_FILE_ATTRIBUTES && (fileAttr & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat buffer;
    return (stat(folder_name, &buffer) == 0 && S_ISDIR(buffer.st_mode));
#endif
}

// Function to create a hidden folder
void create_hidden_folder(const char *folder_name) {
    char hidden_folder_name[100];  // Adjust size for long folder name

#ifdef _WIN32
    strcpy(hidden_folder_name, folder_name);  // Use name directly on Windows
#else
    snprintf(hidden_folder_name, sizeof(hidden_folder_name), ".%s", folder_name);  // Add dot on Linux
#endif

    // Check if the folder already exists, if so generate a new name
    if (folder_exists(hidden_folder_name)) {
        // Avoid printing "folder exists" if we're already handling uniqueness.
        return;
    }

    // Create the folder
    if (CREATE_FOLDER(hidden_folder_name) == 0) {
        perror("Error creating folder");
    } else {
#ifdef _WIN32
        // Set the folder as hidden on Windows
        SET_HIDDEN_ATTRIBUTE(hidden_folder_name);  
#endif
        printf("Created folder: %s\n", hidden_folder_name);
    }
}

int main() {
    const int batch_size = 10000;  // Create exactly 10,000 folders per batch
    size_t name_length = 1;  // Start with 1 character folder names
    char folder_name[100];  // Buffer for folder names (size 100 should be enough for long folder names)

    srand((unsigned int)time(NULL));  // Seed random number generator

    while (1) {  // Infinite loop to continuously create batches of folders
        int folder_count = 0;  // Reset folder count for the batch

        while (folder_count < batch_size) {  // Create 10,000 folders in each batch
            generate_random_string(folder_name, name_length);  // Generate random folder name

            // Ensure the folder name is unique before creating it
            create_hidden_folder(folder_name);

            folder_count++;  // Increment folder count
        }

        printf("Created %d folders in this batch.\n", batch_size);  // Inform user when a batch is done

        // After every batch, increase the folder name length by 1 character
        name_length++; 
    }

    return 0;
}
