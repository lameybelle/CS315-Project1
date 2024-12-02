#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCOUNTS 1000
#define MAX_TRANSACTION_LEN 256

typedef struct {
    char account_number[100];
    char password[100];
    double balance;
    double reward_rate;
    int transaction_tracter;
} account;

account* read(char* filename, int* account_count) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    fscanf(file, "%d", account_count);
    account* accounts = malloc(sizeof(account) * (*account_count));

    for (int i = 0; i < *account_count; i++) {
        int index;
        fscanf(file, "%d", &index);  // Read index (not used but aligns with input format)
        fscanf(file, "%s", accounts[i].account_number);
        fscanf(file, "%s", accounts[i].password);
        fscanf(file, "%lf", &accounts[i].balance);
        fscanf(file, "%lf", &accounts[i].reward_rate);
        accounts[i].transaction_tracter = 0;
    }

    fclose(file);
    return accounts;
}

account* find_account(account* accounts, int account_count, char* account_number) {
    for (int i = 0; i < account_count; i++) {
        if (strcmp(accounts[i].account_number, account_number) == 0) {
            return &accounts[i];
        }
    }
    return NULL;
}

void process_transaction(account* accounts, int account_count, char* transaction) {
    char type;
    char account_number[100], password[100], dest_account[100] = {0};
    double amount = 0;
    account* dest_account_ptr = NULL;

    // Extract type, account number, and password
    sscanf(transaction, "%c %s %s", &type, account_number, password);
    printf("Transaction type: %c, Account: %s\n", type, account_number);

    account* acc = find_account(accounts, account_count, account_number);

    if (!acc) {
        printf("Error: Account %s not found\n", account_number);
        return;
    }

    if (strcmp(acc->password, password) != 0) {
        printf("Error: Invalid password for account %s\n", account_number);
        return;
    }

    // Process transaction based on type
    switch (type) {
        case 'T':  // Transfer
            sscanf(transaction, "T %s %s %s %lf", account_number, password, dest_account, &amount);
            dest_account_ptr = find_account(accounts, account_count, dest_account);
            if (!dest_account_ptr) {
                printf("Error: Destination account %s not found\n", dest_account);
                return;
            }
            if (acc->balance >= amount) {
                acc->balance -= amount;
                dest_account_ptr->balance += amount;
                acc->transaction_tracter++;
                dest_account_ptr->transaction_tracter++;
                printf("Transferred %.2lf from %s to %s\n", amount, account_number, dest_account);
            } else {
                printf("Error: Insufficient funds in account %s\n", account_number);
            }
            break;

        case 'D':  // Deposit
            sscanf(transaction, "D %s %s %lf", account_number, password, &amount);
            acc->balance += amount;
            acc->transaction_tracter++;
            printf("Deposited %.2lf into account %s\n", amount, account_number);
            break;

        case 'W':  // Withdraw
            sscanf(transaction, "W %s %s %lf", account_number, password, &amount);
            if (acc->balance >= amount) {
                acc->balance -= amount;
                acc->transaction_tracter++;
                printf("Withdrew %.2lf from account %s\n", amount, account_number);
            } else {
                printf("Error: Insufficient funds in account %s\n", account_number);
            }
            break;

        case 'C':  // Check balance
            printf("Account %s balance: %.2lf\n", account_number, acc->balance);
            break;

        default:
            printf("Error: Invalid transaction type %c\n", type);
    }
}

void print_balances(account* accounts, int account_count) {
    for (int i = 0; i < account_count; i++) {
        printf("Account %s balance: %.2lf\n", accounts[i].account_number, accounts[i].balance);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    int account_count;
    account* accounts = read(argv[1], &account_count);

    char transaction[MAX_TRANSACTION_LEN];
    printf("Enter transactions (Ctrl+D to end):\n");
    while (fgets(transaction, MAX_TRANSACTION_LEN, stdin) != NULL) {
        // Remove trailing newline, if any
        transaction[strcspn(transaction, "\n")] = 0;
        if (strlen(transaction) == 0) continue;  // Skip empty lines
        process_transaction(accounts, account_count, transaction);
    }

    free(accounts);
    return 0;
}
