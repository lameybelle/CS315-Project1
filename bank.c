#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "account.h"

#define MAX_ACCOUNTS 1000
#define MAX_TRANSACTION_LEN 256

account* read(char* filename, int* account_count) {
	FILE* file = fopen(filename, "r");
	if (!file) {
		perror("Error opening file\n");
		exit(1);
	}

	fscanf(file, "%d", account_count);
	account* accounts = malloc(sizeof(account) * (*account_count));

	for (int i = 0; i < *account_count; i++) {
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
	for (int i=0; i < account_count; i++) {
		if (strcmp(accounts[i].account_number, account_number) == 0) {
			return &accounts[i];
		}
	}
	return NULL;
}

void process_transaction(account* accounts, int account_count, char* transaction) {
	char type;
	char account_number[100], password[100], dest_account[100];
	double amount;

	sscanf(transaction, "%c %s %s", &type, account_number, password);

	account* account = find_account(accounts, account_count, account_number);

	if (!account) {
		printf("Account not found\n");
		exit(1);
	}

	if (strcmp(account->password, password) != 0) {
		printf("Invalid password");
		exit(1);
	}

	switch (type) {
	    case 'T':  // Transfer
	    {
	        if (sscanf(transaction + strlen(transaction), "%99s %lf", dest_account, &amount) != 2) {
	            fprintf(st:wderr, "Invalid transfer transaction: %s\n", transaction);
	            return;
	        }
	        account* dest_account_ptr = find_account(accounts, account_count, dest_account);
	        if (!dest_account_ptr) {
	            fprintf(stderr, "Destination account not found: %s\n", dest_account);
	            return;
	        }
	        if (account->balance < amount) {
	            fprintf(stderr, "Insufficient balance for transfer: %s\n", account_number);
	            return;
	        }
	        account->balance -= amount;
	        dest_account_ptr->balance += amount;
	        break;
	    }
	    case 'D':  // Deposit
	        if (amount <= 0) {
	            fprintf(stderr, "Invalid deposit amount\n");
	            return;
	        }
	        account->balance += amount;
	        break;
	    case 'W':  // Withdrawal
	        if (amount <= 0 || account->balance < amount) {
	            fprintf(stderr, "Invalid withdrawal attempt\n");
	            return;
	        }
	        account->balance -= amount;
	        break;
	    case 'C':  // Check balance
	        printf("%s balance: %.2lf\n", account_number, account->balance);
	        return;
	    default:
	        fprintf(stderr, "Unknown transaction type: %c\n", type);
	        return;
	}
}

void print_balances(account* accounts, int account_count) {
	for (int i = 0; i < account_count; i++) {
		printf("%d balance: %.2lf\n", i, accounts[i].balance);
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
	while (fgets(transaction, MAX_TRANSACTION_LEN, stdin) != NULL) {
		process_transaction(accounts, account_count, transaction);
	}

	//print_balances(accounts, account_count);

	free(accounts);

	return 0;
}
