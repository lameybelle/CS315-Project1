#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "account.h"

#define MAX_ACCOUNTS 1000
#define MAX_TRANSACTION_LEN 256
#define MAX_TRANSACTIONS 1000

// Structure to hold transaction data
typedef struct {
    char type;
    char src_account[17];
    char password[9];
    char dest_account[17]; // Only needed for transfer type 'T'
    double amount;          // Amount for 'T', 'D', 'W' types
} transaction;

account* read(char* filename, int* account_count, transaction* transactions, int* transaction_count) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file\n");
        exit(1);
    }

    fscanf(file, "%d", account_count);
    account* accounts = malloc(sizeof(account) * (*account_count));

    // Read account information
    for (int i = 0; i < *account_count; i++) {
        fscanf(file, "%d", &i); // Skip index line
        fscanf(file, "%s", accounts[i].account_number);
        fscanf(file, "%s", accounts[i].password);
        fscanf(file, "%lf", &accounts[i].balance);
        fscanf(file, "%lf", &accounts[i].reward_rate);
        accounts[i].transaction_tracter = 0;
        // Initialize mutex
        pthread_mutex_init(&accounts[i].ac_lock, NULL);
    }

    // Read transaction data
    *transaction_count = 0;
    char line[MAX_TRANSACTION_LEN];
    while (fgets(line, sizeof(line), file)) {
        // Ignore empty lines or invalid format
        if (line[0] == '\n' || line[0] == '#') continue;
        
        transaction t;
        sscanf(line, "%c %s %s", &t.type, t.src_account, t.password);
        if (t.type == 'T') {
            sscanf(line + strlen(line), "%s %lf", t.dest_account, &t.amount);
        } else if (t.type == 'D' || t.type == 'W') {
            sscanf(line + strlen(line), "%lf", &t.amount);
        }
        
        transactions[*transaction_count] = t;
        (*transaction_count)++;
    }

    fclose(file);
    return accounts;
}

void process_transaction(account* accounts, int account_count, transaction* transactions, int transaction_count) {
    for (int i = 0; i < transaction_count; i++) {
        transaction t = transactions[i];
        account* account = find_account(accounts, account_count, t.src_account);

        if (!account) {
            printf("Account %s not found\n", t.src_account);
            continue;
        }

        if (strcmp(account->password, t.password) != 0) {
            printf("Invalid password\n");
            continue;
        }

        if (t.type == 'T') {
            account* dest_account_ptr = find_account(accounts, account_count, t.dest_account);
            if (!dest_account_ptr) {
                printf("Destination account %s not found\n", t.dest_account);
                continue;
            }

            account->balance -= t.amount;
            dest_account_ptr->balance += t.amount;
            account->transaction_tracter++;
            dest_account_ptr->transaction_tracter++;
        } else if (t.type == 'D') {
            account->balance += t.amount;
            account->transaction_tracter++;
        } else if (t.type == 'W') {
            account->balance -= t.amount;
            account->transaction_tracter++;
        } else if (t.type == 'C') {
            printf("%s balance: %.2lf\n", t.src_account, account->balance);
        } else {
            printf("Unknown transaction type %c\n", t.type);
        }
    }
}

void print_balances(account* accounts, int account_count) {
    for (int i = 0; i < account_count; i++) {
        printf("Account %d: %s, Balance: %.2lf\n", i, accounts[i].account_number, accounts[i].balance);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    int account_count, transaction_count;
    transaction transactions[MAX_TRANSACTIONS];
    
    account* accounts = read(argv[1], &account_count, transactions, &transaction_count);

    // Process transactions
    printf("Processing transactions...\n");
    process_transaction(accounts, account_count, transactions, transaction_count);

    // Optionally print balances
    print_balances(accounts, account_count);

    free(accounts);

    return 0;
}
