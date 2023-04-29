#include <stdio.h>
#include <stdbool.h>
# include <string.h>
#include <stdlib.h>
# include <ctype.h>
# include <math.h>


char *getProcessednum(const char buffer[], int start, int last);

char *cleanSpaces(char buffer[]);

bool IsContainNonValid(const char buffer[]);

bool isHexaDigit(char ch);

int getDigitLength(const char *string);

char *startBinaryConverting(char *hexa_num, char ordering, int data_size);

char *convertToBinary(char ch);

int getDecimalValue(char ch);

void addToArray(char *binaryform, int index, const char *current_binary);

char *littleEndian(const char *num, int length);

void convert_and_write_decimal(int index, char *binarynumber, char const datatype[], int datasize, FILE *output);

int getExponentSize(int datasize);

char *getExponent(int size, const char *binarynumber);

char *getFraction(char *binary, int fractionSize);

bool all_exp_1(char *binary, int exp_size);

bool all_frac_0(char *binary, int frac_size);

bool isDeNormalized(const char *exponent, int size);

int getBias(int size);


int convertSignToDecimal(char *binary);


double getFloatingPointtoDecimal(char sign, double mantissa, int value);

int convertUnsignToDecimal(char *binarynumber);


bool isNanCondition(char *binaryNumber, int exponent_size, int fraction_size);

bool isInfinity(char *binaryNumber, int exponent_size, int fraction_size);


double convertBinToFloating(char *binaryNumber, int fraction_size);

char *roundBinary(char *binaryNumber, int datasize, int exponent_size);

double convertToFloat(char *binary, int datasize);

int getFractionSize(char *binary, int exponent_size);

void printUnsignedResult(int index, unsigned int decimal, int datasize, FILE *output);

void printSignedResult(int index, const char *binarynumber, signed int decimal, int datasize, FILE *output);

void printFloatingResult(int index, double floating_number, char *binarynumber, int datasize, FILE *output);

void processInput(int size, char ordering, char datatype[], FILE *input, FILE *output);

bool isHalfWayDown(char *fraction);

bool isHalfWay(char *fraction);

int main() {
    // Takes input file name.
    char fileName[100];
    printf("Enter the name of the file(ex:input.txt): ");
    scanf("%s", fileName);
    FILE *input = fopen(fileName, "r");
    if (input == NULL) {
        printf("The file is empty check the directory of the file or the name convention");
        return -1;
    }
    FILE *output = fopen("output.txt", "w");

    // Taking inputs for byte ordering, data type, data size.
    char byte_ordering;
    char datatype[20];
    int data_size;
    printf("Byte Ordering(l or b): ");
    scanf(" %c", &byte_ordering);
    printf("Data Type(float or int or unsigned)?: ");
    scanf("%s", datatype);
    printf("Datatype size:(1-2-3-4): ");
    scanf("%d", &data_size);
    printf("byte order %c\n", byte_ordering);
    printf("data type %s\n", datatype);
    printf("data size %d\n", data_size);


    processInput(data_size, byte_ordering, datatype, input, output);
    fclose(input);
    fclose(output);
}

// This function processes input, finds total number for each line, checkes for non-valid values etc.
void processInput(int data_size, char byte_ordering, char datatype[], FILE *input, FILE *output) {
    int num_len = 12 / data_size; // the total number for each line
    int each_len = 2 * data_size; // every byte includes 2*n digits in hexadecimal.
    char buffer[40];
    // that array will be used to store the current number that we are processing at that time
    while (fgets(buffer, sizeof(buffer), input) !=
           NULL) {
        // printf("%s", buffer);// read line by line and store the current line characters into the buffer array
        if (IsContainNonValid(buffer)) { // check if the hexadecimal number is a valid input or not
            printf("Check the characters of inputs file");
            break;
        }
        // clean and splitting is working now
        char *cleaned_buffer = cleanSpaces(buffer); // remove the white spaces from the buffer.
        int init_pos, last_pos;
        for (int i = 0; i <
                        num_len; ++i) { // calculate the initial and last position of each number according the datatype and convert this to binary
            init_pos = i * each_len;
            last_pos = init_pos + each_len;
            char *curr_processing = getProcessednum(cleaned_buffer, init_pos, last_pos);
            char *binary_number = startBinaryConverting(curr_processing, byte_ordering, data_size);
            convert_and_write_decimal(i, binary_number, datatype, data_size, output);
        }
        if (!feof(input))
            fprintf(output, "\n");
    }


}

// This function converts to float from given binary.
double convertToFloat(char *binary, int datasize) {
    int exponent_size = getExponentSize(datasize); // finds exponent size
    int fraction_size = getFractionSize(binary, exponent_size); // finds fraction size
    if (isNanCondition(binary, exponent_size, fraction_size)) {// all exponents are one and all fraction is 1
        return NAN;
    }
    binary = roundBinary(binary, datasize, exponent_size);
    char *exponent = getExponent(exponent_size, binary);
    if (isInfinity(binary, exponent_size, fraction_size)) { // if all exponent bits are 1 and fraction is all 0
        return INFINITY;
    }
    double mantissa;
    int bias;
    int value;
    int exponent_decimal;
    fraction_size = getFractionSize(binary, exponent_size);
    if (isDeNormalized(exponent, exponent_size)) { // Checks if number is denormalized if not add 1 to mantissa.
        // calculate mantissa
        mantissa = convertBinToFloating(binary, fraction_size);
        exponent_decimal = 1;
    } else {
        mantissa = 1 + convertBinToFloating(binary, fraction_size);
        exponent_decimal = convertUnsignToDecimal(exponent);
    }
    char sign = binary[0];
    bias = getBias(exponent_size);
    value = exponent_decimal - bias;


    return getFloatingPointtoDecimal(sign, mantissa, value);
}

// This function finds fraction size of the given floating point number
int getFractionSize(char *binary, int exponent_size) {
    int total = getDigitLength(binary);
    return total - exponent_size - 1;
}

// Converts and prints number.
void convert_and_write_decimal(int index, char *binarynumber, char const datatype[], int datasize, FILE *output) {
    int signed_decimal;
    unsigned int unsigned_decimal;
    double floating_number;
    // according to the datatype do the conversion
    switch (datatype[0]) { // This switch checks if data type is signed, unsigned or floating point number.
        case 's':
            signed_decimal = convertSignToDecimal(binarynumber);
            printSignedResult(index, binarynumber, signed_decimal, datasize, output);
            break;
        case 'u':
            unsigned_decimal = convertUnsignToDecimal(binarynumber);
            printUnsignedResult(index, unsigned_decimal, datasize, output);
            break;

        case 'f':
            floating_number = convertToFloat(binarynumber, datasize);
            printFloatingResult(index, floating_number, binarynumber, datasize, output);
            break;
        default:
            printf("Check your datatype!");

    }
}

// This function prints floating point number to output file.
void printFloatingResult(int index, double floating_number, char *binarynumber, int datasize, FILE *output) {
    int exponent_size = getExponentSize(datasize);
    int fraction_size = getFractionSize(binarynumber, exponent_size);
    if (isInfinity(binarynumber, exponent_size, fraction_size)) { // Checking for infinity case.
        char inf[4] = "∞";
        if (binarynumber[0] == '1') {
            fprintf(output, "-");
        }
        fprintf(output, "%s", inf);
    } else if (isNanCondition(binarynumber, exponent_size, fraction_size)) { // Checking for NaN case.
        fprintf(output, "NaN");
    } else {
        if (floating_number == 0) {
            if (binarynumber[0] == '1') { // Check for numbers sign.
                fprintf(output, "-");
            }
            fprintf(output, "0");

        } else if (floor(log10(fabs(floating_number))) == 0) {
            fprintf(output, "%.5lf", floating_number);
        } else {
            fprintf(output, "%.5e", floating_number);
        }
    }
    if ((12 / datasize) - 1 != index) { // Checking if we are at the end if the line, if not we put a white space.
        fprintf(output, " ");
    }
}

// This function prints signed integers to output file.
void printSignedResult(int index, const char *binarynumber, signed int decimal, int datasize, FILE *output) {
    if (decimal == 0) {
        if (binarynumber[0] == '1') {  // If the number is negative we print - before printing number.
            fprintf(output, "-");
        }
    }
    fprintf(output, "%d", decimal);
    int total_num = 12 / datasize;
    if (index != total_num - 1) { // Checking if we are at the end if the line, if not we put a white space.
        fprintf(output, " ");
    }


}

// This function prints unsigned integers to output file.
void printUnsignedResult(int index, unsigned int decimal, int datasize, FILE *output) {
    fprintf(output, "%d", decimal);
    if (index != (12 / datasize - 1)) { // Checking if we are at the end if the line, if not we put a white space.
        fprintf(output, " ");
    }

}

// This function converts unsigned binary integers to decimal.
int convertUnsignToDecimal(char *number) {
    char *binary = number;
    int len = getDigitLength(binary); // finding length of binary number
    int decimal = 0;
    // calculating decimal value from binary number
    for (int i = 0; i < len; i++) {
        if (number[i] == '1') {
            decimal += (int) pow(2, len - i - 1);
        }
    }
    return decimal;
}

// This function converts signed binary integers to decimal.
signed int convertSignToDecimal(char *binary) {
    //INCLUDE MATH.H
    int len = getDigitLength(binary); // Getting digit length so we can find power
    signed int num = 0;
    int power = len - 1;
    if (binary[0] == '1') {
        num = num - (int) pow(2, power);
    }
    power = power - 1;
    for (int i = 1; i < len; i++) { // For loop to find decimal
        if (binary[i] == '1') {
            num = num + (int) pow(2, power);
        }
        power = power - 1;
    }

    // printf("Signed integer: %d\n\n", num);
    return num;
}

// This function round fraction part of floating point number.
char *roundBinary(char *binaryNumber, int datasize, int exponent_size) {
    if (datasize != 3 && datasize != 4) {
        return binaryNumber;
    }
    int last = exponent_size + 13;
    int frac_size = getFractionSize(binaryNumber, exponent_size);
    char *fraction = getFraction(binaryNumber, frac_size);
    binaryNumber[last + 1] = '\0';
    if (isHalfWayDown(fraction)) {
        // flip the bits
        return binaryNumber;
    }
    if (isHalfWay(fraction)) {
        // check the 13th of the fraction
        if (fraction[12] == '0') {
            return binaryNumber;
        }

    }
    // otherwise it s  greater than 10000 or halfway but odd the 13.th bit so rounding is starting
    int start = 1;
    char ch;
    for (int i = last; i >= start; --i) {
        ch = binaryNumber[i];
        if (ch == '0') {
            binaryNumber[i] = '1';
            break;
        }
        binaryNumber[i] = '0';
    }
    binaryNumber[last + 1] = '\0';
    return binaryNumber;
}

// This function finds if fraction part after 13th bit is halfway or not
bool isHalfWay(char *fraction) {
    // for halfway 1000..
    if (fraction[13] == '0') return false;
    int size = getDigitLength(fraction);
    for (int index = 14; index < size; ++index) {
        if (fraction[index] != '0') {
            return false;
        }
    }
    return true;
}

// This function finds if fraction part after 13th bit is halfwayDown or not
bool isHalfWayDown(char *fraction) {
    int fraction_size = getDigitLength(fraction);
    // if the 13h bit is 0 its not otherwise its halfway;
    if (fraction[13] == '0') {
        return true;
    }

    return false;
}

// Converts binary to floating.
double convertBinToFloating(char *binaryNumber, int fraction_size) {
    char *fraction = getFraction(binaryNumber, fraction_size); // Gets fraction part.
    double result = 0;
    char ch;
    double digit_value = 1.f / 2;
    for (int i = 0; i < fraction_size; ++i) {
        ch = fraction[i];
        if (ch == '1') {
            result += digit_value; // Adds digit value to total result
        }
        digit_value /= 2;
    }
    return result;
}

// This function finds bias of the floating number.
int getBias(int exponent_size) {
    return (int) pow(2, exponent_size - 1) - 1;
}

// This function finds if floating number is infinite.
bool isInfinity(char *binaryNumber, int exponent_size, int fraction_size) {
    return all_exp_1(binaryNumber, exponent_size) && all_frac_0(binaryNumber, fraction_size);
}

// This function finds if floating number is NaN.
bool isNanCondition(char *binaryNumber, int exponent_size, int fraction_size) {
    // if all exponents are 1111 and fraction is not 000 its nan condition;
    if (!all_exp_1(binaryNumber, exponent_size)) {
        return false;
    }
    if (all_frac_0(binaryNumber, fraction_size)) {
        return false;
    }
    return true;
}

// This function finds decimal value of a floating point number.
double getFloatingPointtoDecimal(char sign, double mantissa, int value) {
    if (sign == '0') { // If sign bit is 0
        return mantissa * pow(2, value);
    } // If sign bit is not 0 then we should multiply value with -1.
    return mantissa * pow(2, value) * -1;
}

// This function finds if the exponent is denormalized.
bool isDeNormalized(const char *exponent, int size) {
    for (int i = 0; i < size; ++i) {
        if (exponent[i] != '0') {
            return false;
        }
    }

    return true;
}

// This function finds if fraction is all 0s or not.
bool all_frac_0(char *binary, int frac_size) { // all fraction bits are zero
    char *fraction = getFraction(binary, frac_size);
    for (int i = 0; i < frac_size; ++i) { // if fraction is 0000000
        if (fraction[i] != '0') {
            return false;
        }
    }
    return true;
}

// This function finds if exponent is all 1s or not.
bool all_exp_1(char *binary, int exponent_size) {
    char *exponent = getExponent(exponent_size, binary);
    for (int i = 0; i < exponent_size; ++i) {
        if (exponent[i] != '1') {
            return false;
        }
    }
    return true;
}

// This function finds and returns fraction part of the number.
char *getFraction(char *binary, int fractionSize) {
    int len = getDigitLength(binary); // Finds total length of the number.
    char *fraction = malloc(sizeof(char) * fractionSize);
    int start = len - fractionSize; // Total length of the number - fraction size = starting point of the fraction part.
    int j = 0;
    for (int i = start; i < len; ++i) { // We copy binary number to fraction array from start point.
        fraction[j] = binary[i];
        j++;
    }
    fraction[j] = '\0';

    return fraction;
}

// This function finds and returns exponent part of the number.
char *getExponent(int exp_size, const char *binarynumber) {
    char *exponent = malloc(sizeof(char) * exp_size);
    int j = 0;
    for (int i = 1; i <= exp_size; ++i) { // Since first element is sign bit, starting from
        exponent[j] = binarynumber[i];    // 2nd element until exp_size.
        j++;
    }
    exponent[j] = '\0';
    return exponent;
}

// This function return exponent size as bits from given data size input.
int getExponentSize(int datasize) {
    switch (datasize) {
        case 1:
            return 4;
        case 2:
            return 6;
        case 3:
            return 8;
        case 4:
            return 10;
        default:
            return -1;
    }

}

// This function converts hexadecimal number to binary.
char *startBinaryConverting(char *hexa_num, char ordering, int data_size) {
    int hexa_len = 2 * data_size;
    if (ordering == 'l') { // Checks ordering and changes to little endian if needed.
        hexa_num = littleEndian(hexa_num, hexa_len);
    }
    int len = 8 * data_size;

    char *binaryform = malloc(sizeof(int) * len);
    char ch;
    char *current_binary_num;
    for (int i = 0; i < hexa_len; ++i) {
        ch = hexa_num[i];
        current_binary_num = convertToBinary(ch);
        addToArray(binaryform, i, current_binary_num); // Adds to binary array.
    }
    binaryform[len] = '\0';

    return binaryform;
}

// If given byte ordering is little endian this function reverse ordering of the taken input
char *littleEndian(const char *num, int length) {
    char *converted = malloc(sizeof(int) * length);
    int j = 0;
    for (int i = length - 2; i >= 0; i = i - 2) {
        converted[j++] = num[i];
        converted[j++] = num[i + 1];
    }
    return converted;
}

// This function is used make binary numbers array.
void addToArray(char *binaryform, int index, const char *current_binary) {
    int start = 4 * index;
    int last = start + 4;
    int j = 0;
    for (int i = start; i < last; ++i) {
        binaryform[i] = current_binary[j];
        j++;
    }
}

// This function converts given number to binary.
char *convertToBinary(char ch) {
    int value = getDecimalValue(ch);
    int len = 4;
    char *digits = malloc(sizeof(int) * 4);

    int remain;
    for (int i = len - 1; i >= 0; --i) {
        remain = value % 2;
        if (remain == 1) {
            digits[i] = '1';
        } else {
            digits[i] = '0';
        }
        value = value / 2;
    }
    digits[len] = '\0';
    return digits;
}

// This function returns given char's decimal value.
int getDecimalValue(char ch) {
    if (isdigit(ch)) {
        return ch - '0';
    }
    return ch - 'a' + 10;
}

// Finds if given char is hexadecimal digit.
bool isHexaDigit(char ch) {
    return ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f'));
}

// Finds if input contains any non-valid char.
bool IsContainNonValid(const char buffer[200]) {
    char ch;

    for (int i = 0; i < 8; ++i) {
        ch = buffer[i];
        if (!(isHexaDigit(ch) || ch == ' ')) {
            return true;  // Checks if it contains any white space or non-HexadecimalDigit
        }
    }

    return false;
}

// This function deletes white spaces in the taken input.
char *cleanSpaces(char buffer[]) {
    int digit_len = getDigitLength(buffer);
    int total_len = digit_len + digit_len / 2 - 1;
    char *cleanedBuffer = malloc(sizeof(int) * digit_len);
    int j = 0;
    for (int i = 0; i < total_len; ++i) {
        if (!isspace(buffer[i])) {
            cleanedBuffer[j] = buffer[i];
            j++;
        }

    }
    cleanedBuffer[j] = '\0';
    return cleanedBuffer;
}

// This function finds length of the given string.
int getDigitLength(const char *string) {
    int count = 0;
    int i = 0;
    char ch;
    while ((ch = string[i]) != '\0') {
        if (ch != ' ') {
            count++;
        }
        i++;

    }
    return count;
}

// This function separates numbers from each other.
char *getProcessednum(const char buffer[200], int start, int last) {
    int len = last - start;
    char *num = malloc(sizeof(int) * len);
    int j = 0;
    for (int i = start; i < last; i++) {
        num[j] = buffer[i];
        j++;
    }
    num[j] = '\0';
    return num;
}



