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

account* read_accounts_and_process_transactions(char* filename, int* account_count) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    // Read total number of accounts
    fscanf(file, "%d", account_count);
    account* accounts = malloc(sizeof(account) * (*account_count));

    // Read accounts
    for (int i = 0; i < *account_count; i++) {
        int index;
        fscanf(file, "%d", &index);  // Index of the account
        fscanf(file, "%s", accounts[i].account_number);
        fscanf(file, "%s", accounts[i].password);
        fscanf(file, "%lf", &accounts[i].balance);
        fscanf(file, "%lf", &accounts[i].reward_rate);
        accounts[i].transaction_tracter = 0;
    }

    // Process transactions
    char transaction[MAX_TRANSACTION_LEN];
    while (fgets(transaction, MAX_TRANSACTION_LEN, file) != NULL) {
        // Remove trailing newline, if any
        transaction[strcspn(transaction, "\n")] = 0;

        if (strlen(transaction) == 0) continue;  // Skip empty lines

        // Process each transaction
        char type;
        char src_account[100], dest_account[100], password[100];
        double amount = 0;

        sscanf(transaction, "%c", &type);

        if (type == 'D') {  // Deposit
            sscanf(transaction, "D %s %s %lf", src_account, password, &amount);
            account* acc = find_account(accounts, *account_count, src_account);
            if (acc && strcmp(acc->password, password) == 0) {
                acc->balance += amount;
                acc->transaction_tracter++;
                printf("Deposited %.2lf into account %s\n", amount, src_account);
            } else {
                printf("Error: Invalid account or password for deposit.\n");
            }
        } else if (type == 'W') {  // Withdraw
            sscanf(transaction, "W %s %s %lf", src_account, password, &amount);
            account* acc = find_account(accounts, *account_count, src_account);
            if (acc && strcmp(acc->password, password) == 0) {
                if (acc->balance >= amount) {
                    acc->balance -= amount;
                    acc->transaction_tracter++;
                    printf("Withdrew %.2lf from account %s\n", amount, src_account);
                } else {
                    printf("Error: Insufficient funds in account %s\n", src_account);
                }
            } else {
                printf("Error: Invalid account or password for withdrawal.\n");
            }
        } else if (type == 'T') {  // Transfer
            sscanf(transaction, "T %s %s %s %lf", src_account, password, dest_account, &amount);
            account* src_acc = find_account(accounts, *account_count, src_account);
            account* dest_acc = find_account(accounts, *account_count, dest_account);
            if (src_acc && dest_acc && strcmp(src_acc->password, password) == 0) {
                if (src_acc->balance >= amount) {
                    src_acc->balance -= amount;
                    dest_acc->balance += amount;
                    src_acc->transaction_tracter++;
                    dest_acc->transaction_tracter++;
                    printf("Transferred %.2lf from %s to %s\n", amount, src_account, dest_account);
                } else {
                    printf("Error: Insufficient funds in account %s\n", src_account);
                }
            } else {
                printf("Error: Invalid account(s) or password for transfer.\n");
            }
        } else if (type == 'C') {  // Check balance
            sscanf(transaction, "C %s %s", src_account, password);
            account* acc = find_account(accounts, *account_count, src_account);
            if (acc && strcmp(acc->password, password) == 0) {
                printf("Account %s balance: %.2lf\n", src_account, acc->balance);
            } else {
                printf("Error: Invalid account or password for balance check.\n");
            }
        } else {
            printf("Error: Unknown transaction type %c\n", type);
        }
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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    int account_count;
    account* accounts = read_accounts_and_process_transactions(argv[1], &account_count);

    // Free memory
    free(accounts);

    return 0;
}
