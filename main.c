#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILE_NAME "accounts.txt"
#define FILE_TEMP "temp.txt"
#define MIN_NAME_LEN 3
#define MAX_NAME_LEN 30
#define MIN_ADDRESS_LEN 8
#define MAX_ADDRESS_LEN 50
#define PESEL_LEN 11
#define VERT_LINE '|'
#define SPACE ' '

typedef struct Account {
    int acc_number;
    char name[MAX_NAME_LEN + 1];
    char surname[MAX_NAME_LEN + 1];
    char address[MAX_ADDRESS_LEN + 1];
    char pesel[PESEL_LEN + 1];
    float balance;
} Account;

void print_title();
void main_menu();
void print_accounts();
void find_acc();
void make_transfer();
void make_deposit();
void make_withdraw();
void create_account();
int get_num_of_acc(FILE* file);
int save_account(char* name, char* surname, char* address, char* pesel, float balance);
Account load_Account(FILE *file);
char* to_lowercase(char* str);
int want_to_quit();
int acc_exists(int acc_num);
int change_balance(int acc_num, float value);
int check_name(char* name);
int check_address(char* address);
int check_pesel(char* PESEL);


int get_num_of_acc(FILE *file) {
    int len = 0;
    int next;

    while ((next = fgetc(file)) != EOF)
        if (next == '\n') len++;

    return len;
}


int save_account(char* name, char* surname, char* address, char* pesel, float balance) {
    FILE* file = fopen(FILE_NAME, "a+");

    int acc_num = 100 + get_num_of_acc(file);
    fprintf(file, "%d|%s|%s|%s|%s|%.2f|\n", acc_num, name, surname, address, pesel, balance);

    fclose(file);
    return acc_num;
}


Account load_Account(FILE *file) {
    Account account;
    fscanf(file, "%d|%30[^|]|%30[^|]|%50[^|]|%11[^|]|%f|\n", &account.acc_number, account.name, account.surname, account.address, account.pesel, &account.balance);

    return account;
}


int acc_exists(int acc_num) {
    Account account;
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) return 0;

    while (!feof(file)) {
        account = load_Account(file);
        if (account.acc_number == acc_num) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}


void print_accounts() {
    Account account;
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        printf("No accounts found.\n");
        return;
    }

    printf("\n-----------------------------\n\n");
    while (!feof(file)) {
        account = load_Account(file);
        printf("Account number: %d\nName: %s %s\nPesel: %s\nAddress: %s\nBalance: $%.2f\n\n", account.acc_number, account.name, account.surname, account.pesel, account.address, account.balance);
    }
    printf("-----------------------------\n\n");
    fclose(file);
}


int change_balance(int acc_num, float value) {
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) return 0;
    FILE *temp = fopen(FILE_TEMP, "w");

    Account account;
    int success = -1;

    while (!feof(file)) {
        account = load_Account(file);

        if (account.acc_number == acc_num) {
            if (account.balance + value >= 0) {
                account.balance += value;
                success = 1;
            }else {
                success = 0;
            }
        }

        fprintf(temp, "%d|%s|%s|%s|%s|%.2f|\n", account.acc_number, account.name, account.surname, account.address, account.pesel, account.balance);
    }
    fclose(file);
    fclose(temp);

    if (success != 1) {
        remove(FILE_TEMP);
        return success;
    }

    remove(FILE_NAME);
    rename(FILE_TEMP, FILE_NAME);
    return 1;
}


char* to_lowercase(char* str) {
    int index = 0;

    while (str[index] != '\0') {
        str[index] = (char) tolower(str[index]);
        index++;
    }

    return str;
}


int check_fields(Account account, int field, char* phrase) {
    switch (field) {
        case 1:
            if (account.acc_number == atoi(phrase))
                return 1;
            break;
        case 2:
            if (strstr(to_lowercase(account.name), phrase))
                return 1;
            break;
        case 3:
            if (strstr(to_lowercase(account.surname), phrase))
                return 1;
            break;
        case 4:
            if (strstr(to_lowercase(account.address), phrase))
                return 1;
            break;
        case 5:
            if (strstr(to_lowercase(account.pesel), phrase))
                return 1;
            break;
        default:
            return 0;
    }
    return 0;
}


void search_for_accounts(int field, char* phrase) {
    FILE *file = fopen(FILE_NAME, "r");
    int max_size = get_num_of_acc(file);
    rewind(file);

    Account found[max_size];
    int size = 0;

    while (!feof(file)) {
        Account account = load_Account(file);

        if (check_fields(account, field, to_lowercase(phrase))) {
            found[size] = account;
            size++;
        }
    }
    fclose(file);

    for (int ct = 0; ct < size; ct++) {
        printf("Account number: %d\nName: %s %s\nPesel: %s\nAddress: %s\nBalance: $%.2f\n\n", found[ct].acc_number, found[ct].name, found[ct].surname, found[ct].pesel, found[ct].address, found[ct].balance);
    }
}

int check_name(char* name) {
    if (name == NULL) return 0;

    int index = 0;
    char sign = name[index];

    while (sign != '\0') {
        if (!isalpha(sign) || index >= MAX_NAME_LEN || sign == VERT_LINE) {
            return 0;
        }
        index++;
        sign = name[index];
    }

    if (index < MIN_NAME_LEN) {
        return 0;
    }

    return 1;
}


int check_address(char* address) {
    if (address == NULL) return 0;

    int index = 0;
    char sign = address[index];

    while (sign != '\0') {
        if ((!isalnum(sign) && sign != SPACE) || index >= MAX_ADDRESS_LEN || sign == VERT_LINE) {
            return 0;
        }

        index++;
        sign = address[index];
    }

    if (index < MIN_ADDRESS_LEN) {
        return 0;
    }

    return 1;
}


int check_pesel(char* PESEL) {
    int index = 0;
    char sign = PESEL[index];

    while (sign != '\0') {
        if (!isdigit(sign) || index > PESEL_LEN || sign == VERT_LINE) {
            return 0;
        }
        index++;
        sign = PESEL[index];
    }

    if (index != PESEL_LEN) return 0;
    return 1;
}

void find_acc() {
    int check_c, check_p;
    int choice;
    char phrase[51];

    printf("\n-----------------------------\n\n");
    printf("Choose field you want to search by:\n1 - account number\n2 - name\n3 - surname\n4 - address\n5 - PESEL\nYour choice:");
    check_c = scanf("%10d", &choice);
    while (getchar()!='\n');
    while (check_c == 0 || choice < 1 || choice > 5) {
        printf("Wrong choice:\n");
        check_c = scanf("%10d", &choice);
        while (getchar()!='\n');
    }

    printf("\nEnter phrase you want to find:");
    check_p = scanf("%50[^\n]", phrase);
    while (check_p == 0) {
        printf("Wrong phrase: ");
    }
    printf("\n");
    search_for_accounts(choice, phrase);
    printf("\n-----------------------------\n\n");
}

void make_transfer() {
    int from, to;
    float transfer;
    int approve;
    int check_n1, check_n2, check_t, check_ap;

    printf("\n-----------------------------\n\n");
    printf("Enter your account number: ");
    check_n1 = scanf("%d", &from);
    while (check_n1 == 0 || acc_exists(from) != 1) {
        printf("Wrong account number: ");
        check_n1 = scanf("%d", &from);
        while(getchar() != '\n');
    }

    printf("\nEnter account number you want to transfer money to: ");
    check_n2 = scanf("%d", &to);
    while (check_n2 == 0 || acc_exists(to) != 1 || to == from) {
        printf("Wrong account number: ");
        check_n2 = scanf("%d", &to);
        while(getchar() != '\n');
    }

    do {
        printf("\nEnter amount of money you want to transfer: ");
        scanf("%f", &transfer);
        while (check_t == 0 || transfer < 0 || transfer > 1000000) {
            printf("Wrong amount of money: ");
            check_t = scanf("%f", &transfer);
            while(getchar() != '\n');
        }

        printf("\nAre you sure?\n1 - Yes\n2 - No\nYour choice: ");
        check_ap = scanf("%d", &approve);
        while (getchar()!='\n');
        while (check_ap == 0) {
            printf("Wrong choice: ");
            check_ap = scanf("%d", &approve);
            while (getchar()!='\n');
        }
    } while (approve != 1);

    transfer = ((int) (transfer * 100)) / 100.0;
    if (!change_balance(from, -transfer)) {
        printf("\nYou do not have enough money.\n");
        printf("\n-----------------------------\n\n");
        return;
    }
    change_balance(to, transfer);
    printf("\nTransfer successful.\n");
    printf("\n-----------------------------\n\n");
}

void make_deposit() {
    int acc_num;
    int check_n, check_d, check_ap;
    int approve;;
    float deposit;

    printf("\n-----------------------------\n\n");
    printf("Enter your account number: ");
    check_n = scanf("%d", &acc_num);
    while (check_n == 0 || acc_exists(acc_num) != 1) {
        printf("Wrong account number: ");
        check_n = scanf("%d", &acc_num);
        while(getchar() != '\n');
    }

    do {
        printf("\nEnter amount of money you want to deposit: ");
        scanf("%f", &deposit);
        while (check_d == 0 || deposit < 0 || deposit > 1000000) {
            printf("Wrong amount of money: ");
            check_d = scanf("%f", &deposit);
            while(getchar() != '\n');
        }

        printf("\nAre you sure?\n1 - Yes\n2 - No\nYour choice: ");
        check_ap = scanf("%d", &approve);
        while (getchar()!='\n');
        while (check_ap == 0) {
            printf("Wrong choice: ");
            check_ap = scanf("%d", &approve);
            while (getchar()!='\n');
        }
    } while (approve != 1);

    deposit = ((int) (deposit * 100)) / 100.0;
    if (change_balance(acc_num, deposit)) {
        printf("\nTransfer successful.\n");
        printf("\n-----------------------------\n\n");
    }
}

void make_withdraw() {
    int acc_num;
    int approve;
    int check_n, check_w, check_ap;
    float withdraw;

    printf("\n-----------------------------\n\n");
    printf("Enter your account number: ");
    check_n = scanf("%d", &acc_num);
    while (check_n == 0 || acc_exists(acc_num) != 1) {
        printf("Wrong account number: ");
        check_n = scanf("%d", &acc_num);
        while(getchar() != '\n');
    }

    do {
        printf("\nEnter amount of money you want to deposit: ");
        scanf("%f", &withdraw);
        while (check_w == 0 || withdraw < 0 || withdraw > 1000000) {
            printf("Wrong amount of money: ");
            check_w = scanf("%f", &withdraw);
            while(getchar() != '\n');
        }

        printf("\nAre you sure?\n1 - Yes\n2 - No\nYour choice: ");
        check_ap = scanf("%d", &approve);
        while (getchar()!='\n');
        while (check_ap == 0) {
            printf("Wrong choice: ");
            check_ap = scanf("%d", &approve);
            while (getchar()!='\n');
        }
    } while (approve != 1);

    withdraw = ((int) (withdraw * 100)) / 100.0;
    if (change_balance(acc_num, -withdraw) == 1) {
        printf("\nWithdraw successful.\n");
        printf("\n-----------------------------\n\n");
        return;
    }
    else {
        printf("\nYou do not have enough money.\n");
        printf("\n-----------------------------\n\n");
        return;
    }
}


void create_account() {
    int check_n, check_sn, check_a, check_p;
    char name[MAX_NAME_LEN + 1];
    char surname[MAX_NAME_LEN + 1];
    char address[MAX_ADDRESS_LEN + 1];
    char PESEL[PESEL_LEN + 1];
    double balance;
    printf("\n-----------------------------\n\n");
    printf("Enter name: ");
    check_n = scanf("%30s", name);
    while (getchar()!='\n' || check_n == 0);
    while (!check_name(name)) {
        printf("Wrong name: \n");
        check_n = scanf("%30s", name);
        while (getchar()!='\n' || check_n == 0);
    }

    printf("\nEnter surname: ");
    check_sn = scanf("%30s", surname);
    while (getchar()!='\n');
    while (!check_name(surname) || check_sn == 0) {
        printf("Wrong surname: \n");
    }

    printf("\nAddress: ");
    check_a = scanf("%50[^\n]", address);
    while (getchar()!='\n');
    while (!check_address(address) || check_a == 0) {
        printf("Wrong address: \n");
    }

    printf("\nPESEL: ");
    check_p = scanf("%11s", PESEL);
    while (getchar()!='\n');
    while (!check_pesel(PESEL) || check_p == 0) {
        printf("Wrong PESEL: \n");
        check_p = scanf("%11s", PESEL);
        while (getchar()!='\n');
    }


    balance = 0;
    save_account(name, surname, address, PESEL, balance);
    printf("\n-----------------------------\n\n");
}

int want_to_quit() {
    int choice;
    int check;

    printf("\nDo you want to quit?\n1 - yes\n2 - no\nYour choice: ");
    check = scanf("%d", &choice);
    while(getchar() != '\n');
    while (check == 0 || choice < 1 || choice > 2) {
        printf("Wrong choice try again: ");
        check = scanf("%d", &choice);
        while(getchar() != '\n');
    }

    return choice;
}

void main_menu() {
    int choice;
    int check;

    printf("Choose an option: \n");
    printf("\n1 - list accounts\n2 - make transfer\n3 - make deposit\n4 - withdraw money\n5 - create account\n6 - find account\n7 - quit\nYour choice: ");
    check = scanf("%d", &choice);
    while(getchar() != '\n');
    while (check == 0) {
        printf("Wrong choice try again: ");
        check = scanf("%d", &choice);
        while(getchar() != '\n');
    }

    switch (choice)
    {
        case 1: // list
            print_accounts();
            break;
        case 2: // transfer
            make_transfer();
            break;
        case 3: // deposit
            make_deposit();
            break;
        case 4: // withdraw
            make_withdraw();
            break;
        case 5: // create acc
            create_account();
            break;
        case 6: // find acc
            find_acc();
            break;
        case 7:
            if (want_to_quit() == 1)
                return;
            break;
        default :
            break;
    }

    main_menu();
}

void print_title() {
    printf("-----------------------------\n");
    printf("     WELCOME TO THE BANK\n");
    printf("-----------------------------\n\n");
}

int main() {
    print_title();
    main_menu();
    return 0;
}
