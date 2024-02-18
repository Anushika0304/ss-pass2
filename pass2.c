#include<stdio.h>
#include<string.h>
#include<stdlib.h>

void main() {
    char a[10], ad[10], label[10], opcode[10], operand[10], symbol[10], ch,ch1[100],cher;
    int st, diff, i, address, add, len, actual_len, finaddr, prevaddr, j = 0,counter=0,st1,k=0,n=0;
    char mnemonic[15][15] = { "LDA","STA","LDCH","STCH" };
    char code[15][15] = { "33","44","53","57" };
    FILE* fp1, * fp2, * fp3, * fp4,*fp5;

    fp1 = fopen("ASSMLIST.txt", "w");
    fp2 = fopen("SYMTAB.txt", "r");
    fp3 = fopen("INTERMED.txt", "r");
    fp4 = fopen("OBJCODE.txt", "w");
    fp5 = fopen("experiment.txt","w+");

    fscanf(fp3, "\t%s\t%s\t%s", label, opcode, operand); //Read first line from intermediate.txt START 5000

    while (strcmp(opcode, "END") != 0) {
        prevaddr = address;
        fscanf(fp3, "%X%s%s%s", &address, label, opcode, operand); // Read address in hexadecimal format. Keep reading lines until END is encountered.
    }
    finaddr = address;
    fclose(fp3);

    fp3 = fopen("INTERMED.txt", "r");

    fscanf(fp3, "\t%s\t%s\t%s", label, opcode, operand); //Read next line from intermediate.txt
    if (strcmp(opcode, "START") == 0) {
        int op;
        sscanf(operand, "%X", &op); // Read starting address in hexadecimal format
        int len = finaddr - op;

        fprintf(fp1, "\t%s\t%s\t%s\n", label, opcode, operand); //print in ASSMLIST.txt
        fprintf(fp4, "H^%s^00%s^00%X\n", label, operand, len); // Output length in hexadecimal format. Print the header record.
        fscanf(fp3, "%X%s%s%s", &address, label, opcode, operand); // Read address in hexadecimal format. Read next line from intermediate.txt
        st = address;
        st1=address;
        //diff = prevaddr - st;
        //printf("Prevaddr is %d",prevaddr);
        fprintf(fp4, "T^00%X", address); // Output address in hexadecimal format
    }

    while (strcmp(opcode, "END") != 0) {
        if (strcmp(opcode, "BYTE") == 0) {
            fprintf(fp1, "%X\t%s\t%s\t%s\t", address, label, opcode, operand); // Output address in hexadecimal format
            len = strlen(operand);
            actual_len = len - 3;
            fprintf(fp5, "^");
            for (i = 2; i < (actual_len + 2); i++) {
                itoa(operand[i], ad, 16);
                fprintf(fp1, "%s", ad);
                fprintf(fp5, "%s", ad);

                counter++;
            }

            fprintf(fp1, "\n");
        }
        else if (strcmp(opcode, "WORD") == 0) {
            len = strlen(operand);
            itoa(atoi(operand), a, 10);
            fprintf(fp1, "%X\t%s\t%s\t%s\t00000%s\n", address, label, opcode, operand, a); // Output address in hexadecimal format
            fprintf(fp5, "^00000%s", a);
            counter=counter+3;
        }
        else if ((strcmp(opcode, "RESB") == 0) || (strcmp(opcode, "RESW") == 0))
            fprintf(fp1, "%X\t%s\t%s\t%s\n", address, label, opcode, operand); // Output address in hexadecimal format
        else {
                counter=counter+3;
            while (strcmp(opcode, mnemonic[j]) != 0) {
                j++;
            }

            if (strcmp(operand, "COPY") == 0) {
                fprintf(fp3, "%X\t%s\t%s\t%s\t%s0000\n", address, label, opcode, operand, code[j]); // Output address in hexadecimal format
            }
            else {
                // Check if operand contains ",X"
                printf("%s\n", operand);

                if (strstr(operand, ",X") != 0) {
                    char operand_copy[10];
                    strcpy(operand_copy, operand);
                    char* symbol_without_index = strtok(operand_copy, ",");
                    printf("I am exec");

                    // Operand contains ",X", add 8 to the first digit of the symbol
                    rewind(fp2);
                    fscanf(fp2, "%s\t%X", symbol, &add);
                    while (strcmp(symbol_without_index, symbol) != 0) {
                        fscanf(fp2, "%s\t%X", symbol, &add);
                    }
                   int leftmost_digit = (add / 4096); // Extracting the leftmost digit (4096 = 16^3)
leftmost_digit = (leftmost_digit + 8) % 16; // Adding 8 and taking modulo 16 to ensure it remains a hexadecimal digit
add = (leftmost_digit * 4096) + (add % 4096); // Reconstructing the updated hexadecimal address

                    // Convert code[j] to string
                    char code_string[10];
                    sprintf(code_string, "%s", code[j]);

                    fprintf(fp1, "%X\t%s\t%s\t%s\t%s%X\n", address, label, opcode, operand, code_string, add); // Output address in hexadecimal format
                    fprintf(fp5, "^%s%X", code_string, add);
                }
                else {
                    // Operand doesn't contain ",X", add the symbol value directly to the instruction
                    rewind(fp2);
                    fscanf(fp2, "%s\t%X", symbol, &add);
                    while (strcmp(operand, symbol) != 0) {
                        fscanf(fp2, "%s\t%X", symbol, &add);
                    }

                    fprintf(fp1, "%X\t%s\t%s\t%s\t%s%X\n", address, label, opcode, operand, code[j], add); // Output address in hexadecimal format
                    fprintf(fp5, "^%s%X", code[j], add);
                }
            }
        }
        if((strcmp(opcode,"RESB")!=0) && (strcmp(opcode,"RESW")!=0) && (strcmp(opcode,"END")!=0) )
            prevaddr=address;
//printf("Diff is %X",address-st);
fscanf(fp3, "%X%s%s%s", &address, label, opcode, operand); // Read address in hexadecimal format

        if(prevaddr-st>0x1E && (strcmp(opcode,"RESB")!=0) && (strcmp(opcode,"RESW")!=0))
        {
            fprintf(fp4,"^%X",counter);
            rewind(fp5);
            fseek(fp5,k,SEEK_SET);
            cher=fgetc(fp5);
            while(cher!=EOF)
            {
               fprintf(fp4,"%c",cher);
                k++;
               cher=fgetc(fp5);
            }
             //n = k;
            fprintf(fp4,"\nT^00%X",prevaddr);
            st=prevaddr;
            printf("ST is %X",st);
            //printf("St = %X",st);
            counter=0;
        }


    }

    //fprintf(fp4,"^%X",counter);
    //fseek(fp5,k,SEEK_SET);
    cher=fgetc(fp5);
    //printf("%c",cher);

            while(cher!=EOF){
               fprintf(fp4,"%c",cher);
               k++;
               cher=fgetc(fp5);
            }


    fprintf(fp1, "%X\t%s\t%s\t%s\n", address, label, opcode, operand); // Output address in hexadecimal format
    fprintf(fp4, "\nE^00%X", st1); // Output address in hexadecimal format
    printf("\n Intermediate file is converted into object code");
    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
    fclose(fp5);
    printf("\n\nThe contents of Intermediate file:\n\n\t");
    fp3 = fopen("INTERMED.txt", "r");
    ch = fgetc(fp3);
    while (ch != EOF) {
        printf("%c", ch);
        ch = fgetc(fp3);
    }
    printf("\n\nThe contents of Symbol Table :\n\n");
    fp2 = fopen("SYMTAB.txt", "r");
    ch = fgetc(fp2);
    while (ch != EOF) {
        printf("%c", ch);
        ch = fgetc(fp2);
    }
    printf("\n\nThe contents of Output file :\n\n");
    fp1 = fopen("ASSMLIST.txt", "r");
    ch = fgetc(fp1);
    while (ch != EOF) {
        printf("%c", ch);
        ch = fgetc(fp1);
    }
    printf("\n\nThe contents of Object code file :\n\n");
    fp4 = fopen("OBJCODE.txt", "r");
    ch = fgetc(fp4);
    while (ch != EOF) {
        printf("%c", ch);
        ch = fgetc(fp4);
    }
    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
    getchar();
}
