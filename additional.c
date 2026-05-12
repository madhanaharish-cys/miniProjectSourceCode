#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// clientData structure definition
struct clientData
{
unsigned int acctNum; // account number
char lastName[15];    // account last name
char firstName[10];   // account first name
double balance;       // account balance
};

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);

int main(int argc, char *argv[])
{
FILE *cfPtr;
unsigned int choice;

    /* PASSWORD LOGIN FEATURE */
    char password[20];

    printf("Enter Admin Password: ");
    scanf("%s", password);

    if (strcmp(password, "admin123") != 0)
    {
        printf("Wrong Password!\n");
        exit(1);
    }

    // open file
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("%s: File could not be opened.\n", argv[0]);
        exit(-1);
    }

    // menu
    while ((choice = enterChoice()) != 5)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;

        case 2:
            updateRecord(cfPtr);
            break;

        case 3:
            newRecord(cfPtr);
            break;

        case 4:
            deleteRecord(cfPtr);
            break;

        default:
            puts("Incorrect choice");
            break;
        }
    }

    fclose(cfPtr);
    return 0;
}

// create formatted text file
void textFile(FILE *readPtr)
{
    FILE *writePtr;
    int result;

    struct clientData client = {0, "", "", 0.0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
    }
    else
    {
        rewind(readPtr);

        fprintf(writePtr,
                "%-6s%-16s%-11s%10s\n",
                "Acct",
                "Last Name",
                "First Name",
                "Balance");

        while (!feof(readPtr))
        {
            result = fread(&client,
                           sizeof(struct clientData),
                           1,
                           readPtr);

            if (result != 0 && client.acctNum != 0)
            {
                fprintf(writePtr,
                        "%-6d%-16s%-11s%10.2f\n",
                        client.acctNum,
                        client.lastName,
                        client.firstName,
                        client.balance);
            }
        }

        fclose(writePtr);

        printf("accounts.txt created successfully.\n");
    }
}

// update balance
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;

    struct clientData client = {0, "", "", 0.0};

    printf("Enter account to update (1 - 100): ");
    scanf("%d", &account);

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account #%d has no information.\n",
               account);
    }
    else
    {
        printf("%-6d%-16s%-11s%10.2f\n\n",
               client.acctNum,
               client.lastName,
               client.firstName,
               client.balance);

        printf("Enter charge (+) or payment (-): ");
        scanf("%lf", &transaction);

        client.balance += transaction;

        printf("%-6d%-16s%-11s%10.2f\n",
               client.acctNum,
               client.lastName,
               client.firstName,
               client.balance);

        fseek(fPtr,
              -sizeof(struct clientData),
              SEEK_CUR);

        fwrite(&client,
               sizeof(struct clientData),
               1,
               fPtr);

        /* TRANSACTION HISTORY + DATE/TIME */
        FILE *logPtr;

        time_t t;
        time(&t);

        logPtr = fopen("transactions.txt", "a");

        fprintf(logPtr,
                "UPDATED -> Account No: %u Balance: %.2f Date: %s",
                client.acctNum,
                client.balance,
                ctime(&t));

        fclose(logPtr);
    }
}

// delete record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0};

    unsigned int accountNum;

    printf("Enter account number to delete (1 - 100): ");
    scanf("%d", &accountNum);

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account %d does not exist.\n",
               accountNum);
    }
    else
    {
        fseek(fPtr,
              (accountNum - 1) * sizeof(struct clientData),
              SEEK_SET);

        fwrite(&blankClient,
               sizeof(struct clientData),
               1,
               fPtr);

        printf("Account deleted successfully.\n");

        /* TRANSACTION HISTORY + DATE/TIME */
        FILE *logPtr;

        time_t t;
        time(&t);

        logPtr = fopen("transactions.txt", "a");

        fprintf(logPtr,
                "DELETED -> Account No: %u Date: %s",
                accountNum,
                ctime(&t));

        fclose(logPtr);
    }
}

// create new record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};

    unsigned int accountNum;

    printf("Enter new account number (1 - 100): ");
    scanf("%d", &accountNum);

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum != 0)
    {
        printf("Account #%d already contains information.\n",
               client.acctNum);
    }
    else
    {
        printf("Enter lastname, firstname, balance\n? ");

        scanf("%14s%9s%lf",
              client.lastName,
              client.firstName,
              &client.balance);

        client.acctNum = accountNum;

        fseek(fPtr,
              (client.acctNum - 1) * sizeof(struct clientData),
              SEEK_SET);

        fwrite(&client,
               sizeof(struct clientData),
               1,
               fPtr);

        printf("New account created successfully.\n");

        /* TRANSACTION HISTORY + DATE/TIME */
        FILE *logPtr;

        time_t t;
        time(&t);

        logPtr = fopen("transactions.txt", "a");

        fprintf(logPtr,
                "NEW ACCOUNT -> Account No: %u Name: %s %s Balance: %.2f Date: %s",
                client.acctNum,
                client.firstName,
                client.lastName,
                client.balance,
                ctime(&t));

        fclose(logPtr);
    }
}

// menu
unsigned int enterChoice(void)
{
    unsigned int menuChoice;

    printf("%s",
           "\nEnter your choice\n"
           "1 - store a formatted text file of accounts called\n"
           "    \"accounts.txt\" for printing\n"
           "2 - update an account\n"
           "3 - add a new account\n"
           "4 - delete an account\n"
           "5 - end program\n? ");

    scanf("%u", &menuChoice);

    return menuChoice;
}