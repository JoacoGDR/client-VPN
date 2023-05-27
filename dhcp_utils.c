
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAX_LINE_LENGTH 500
#define MAX_IP_LEN 16
#define CSV_FILE "dhcp_db.csv"
#define MAX_HOSTS 200

int counter = 0;



char* getIPAddress(const char* publicKey) {
    FILE* file = fopen(CSV_FILE, "r");
    if (file == NULL) {
        printf("Failed to open the CSV file.\n");
        return NULL;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        char * token = strtok(line, ",");
        if (token != NULL && strcmp(token, publicKey) == 0) {
            // Found matching publicKey, retrieve the IP address
            token = strtok(NULL, ",");
            if (token != NULL) {
                char* ipAddress = strdup(token);
                fclose(file);
                return ipAddress;
            }
        }
    }

    fclose(file);
    return NULL;  // PublicKey not found
}

char* generateIPAddress() {
    char* ip = (char*)malloc(MAX_IP_LEN * sizeof(char));

    if(counter == 200){
        printf("no ip to give");
        return NULL;
    }
    // Format the IP address string
    snprintf(ip, MAX_IP_LEN, "10.0.0.%d", counter++);
    return ip;
}



void addEntry(const char* publicKey, const char* ip) {
    FILE* file = fopen(CSV_FILE, "r+");
    if (file == NULL) {
        printf("Failed to open the CSV file.\n");
        return;
    }

    char line[MAX_LINE_LENGTH];
    char newLine[MAX_LINE_LENGTH];
    int entryAdded = 0;
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        char* token = strtok(line, ",");
        if (token != NULL && strcmp(token, publicKey) == 0) {
            // Found existing publicKey, replace reservedIp
            snprintf(newLine, MAX_LINE_LENGTH, "%s, %s\n", publicKey, ip);
            fputs(newLine, file);
            entryAdded = 1;
        }
    }

    if (!entryAdded) {
        // Public key not found, add new entry
        fseek(file, 0, SEEK_END);
        snprintf(newLine, MAX_LINE_LENGTH, "%s, %s\n", publicKey, ip);
        fputs(newLine, file);
    }

    fclose(file);
}


void removeEntry(const char* publicKey) {
    FILE* file = fopen(CSV_FILE, "r");
    if (file == NULL) {
        printf("Failed to open the CSV file.\n");
        return;
    }

    char line[MAX_LINE_LENGTH];
    char newLine[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        char* token = strtok(line, ",");
        if (token != NULL && strcmp(token, publicKey) == 0) {
            // Found matching publicKey, remove the entry
            continue;
        }
        else {
            // Copy the line to the new file
            snprintf(newLine, MAX_LINE_LENGTH, "%s", line);
            fputs(newLine, file);
        }
    }

    fclose(file);
}

int isReserved(const char* ip) {
    FILE* file = fopen(CSV_FILE, "r");
    if (file == NULL) {
        printf("Failed to open the CSV file.\n");
        return 0;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        char* token = strtok(line, ",");
        if (token != NULL) {
            // Skip the first column (publicKey)
            token = strtok(NULL, ",");
            if (token != NULL && strcmp(token, ip) == 0) {
                fclose(file);
                return 1;  // Reserved IP found
            }
        }
    }

    fclose(file);
    return 0;  // Reserved IP not found
}

