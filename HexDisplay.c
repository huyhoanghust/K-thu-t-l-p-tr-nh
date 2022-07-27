/**
 * @file HexDisplay.c
 * @author Nguyễn Huy Hoàng 20191855
 * @brief (hoangnh191855@sis.hust.edu.vn)
 * @version 0.1
 * @date 2022-07-25
 *
 * @copyright Copyright (c) 2022
 * - Yêu cầu:
 * Mở file
 * Check xem file đó phải file hex không
 * Đọc từng dòng của file
 * Hiển thị file xử lí lên trên màn hình
 *
 * - Kỹ thuật
 * Chương trình chạy bằng Command line với tên file chạy là HexDisplay <input file>
 * Print tối đa 25 dòng, hỏi xem có tiếp tục hiển thị hay không
 *
 * - Xử lí lỗi
 * Nếu file không mở được hiển thị "FILENAME could not be opened"
 * Nếu dòng nào không đúng format của intel hex thì chương trình kết thúc và thông báo lỗi tại dòng đó
 *
 * - Thêm
 * Cần có Comment tại các hàm, input, output là gì
 *
 * Intel Hex
 * start bytecount address recordtype data checksum
 * checksum = ~(bytecount + address + recordtype + data) + 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <windows.h>

#define MAXMEMORY 65536
#define DATA 0
#define BASE_ADDRESS 4
#define END_FILE 1

char buffer[MAXMEMORY][2];
char start;          // 1 character
char byte_count[3];  // 2 characters
char address[5];     // 4 characters
char record_type[3]; // 2 characters
char data[33];       // max 32 characters
char checksum[3];    // 2 character

char arr[100] = {0};

int line = 0;
int indexx = 0;
int addr = 0;
int type_current = 0;
int type_previous = 0;
int addr_current = 0;
int addr_previous = 0;
int addr_previous_2 = 0;

// Xóa dữ liệu các ô nhớ về FF
void clear_buffer(void)
{
    int i;
    for (i = 0; i < MAXMEMORY; i++)
    {
        buffer[i][0] = 'F';
        buffer[i][1] = 'F';
    }
}

/*tính tổng từng byte một trong trường dữ liệu các mảng được trích xuất từ file hex
input là các mảng đọc kí tự từ file hex
output là số tổng các byte đượcc chuyển từ hex sang dec từng byte một
ví dụ: addres[] = "AB1C" chuyển thành dec(AB) + dec(1C)
*/
int dec2char(char str[])
{
    int a;
    int i;
    int sum_dec = 0;
    int j;
    char arr2byte[3];
    for (j = 0; j < strlen(str); j = j + 2)
    {
        // trích xuất 2 kí tự một vào mảng 2 byte ví dụ "AB"
        arr2byte[0] = str[j];
        arr2byte[1] = str[j + 1];
        // convert sang DEC
        for (i = 0; i < strlen(arr2byte); i++)
        {
            switch (arr2byte[i])
            {
            case '0':
                a = 0;
                break;
            case '1':
                a = 1;
                break;
            case '2':
                a = 2;
                break;
            case '3':
                a = 3;
                break;
            case '4':
                a = 4;
                break;
            case '5':
                a = 5;
                break;
            case '6':
                a = 6;
                break;
            case '7':
                a = 7;
                break;
            case '8':
                a = 8;
                break;
            case '9':
                a = 9;
                break;
            case 'A':
            case 'a':
                a = 10;
                break;
            case 'B':
            case 'b':
                a = 11;
                break;
            case 'C':
            case 'c':
                a = 12;
                break;
            case 'D':
            case 'd':
                a = 13;
                break;
            case 'E':
            case 'e':
                a = 14;
                break;
            case 'F':
            case 'f':
                a = 15;
                break;
            }
            sum_dec = sum_dec + a * pow(16, strlen(arr2byte) - i - 1);
        }
        memset(arr2byte, '\0', sizeof(arr2byte));
    }
    return sum_dec;
}

/*
convert hex to dec
*/
int hex_to_dec(char str[])
{
    int a;
    int i;
    int dec = 0;
    for (i = 0; i < strlen(str); i++)
    {
        switch (str[i])
        {
        case '0':
            a = 0;
            break;
        case '1':
            a = 1;
            break;
        case '2':
            a = 2;
            break;
        case '3':
            a = 3;
            break;
        case '4':
            a = 4;
            break;
        case '5':
            a = 5;
            break;
        case '6':
            a = 6;
            break;
        case '7':
            a = 7;
            break;
        case '8':
            a = 8;
            break;
        case '9':
            a = 9;
            break;
        case 'A':
        case 'a':
            a = 10;
            break;
        case 'B':
        case 'b':
            a = 11;
            break;
        case 'C':
        case 'c':
            a = 12;
            break;
        case 'D':
        case 'd':
            a = 13;
            break;
        case 'E':
        case 'e':
            a = 14;
            break;
        case 'F':
        case 'f':
            a = 15;
            break;
        }
        dec = dec + a * pow(16, strlen(str) - i - 1);
    }
    return dec;
}

/*
trích xuất dữ liệu bao gồm: start, bytecount, address, recordtype, data, checksum
*/
void extract_element(char arr[])
{
    int l = strlen(arr) - 1;
    // lấy kí tự đầu tiên
    start = arr[0];
    // Trích xuất số lượng Byte dữ liệu
    byte_count[0] = arr[1];
    byte_count[1] = arr[2];
    // Trích xuất địa chỉ addres
    int i = 0;
    while (i < 4)
    {
        address[i] = arr[i + 3];
        i++;
    }
    // Trích xuất record type
    record_type[0] = arr[7];
    record_type[1] = arr[8];
    // Trích xuất data
    int k = 0;
    while (k < (l - 11))
    {
        data[k] = arr[9 + k];
        k++;
    }
    // Trích xuất checksum
    checksum[0] = arr[l - 2];
    checksum[1] = arr[l - 1];
}

/*
Kiểm tra format của từng dòng xem có đúng chuẩn của Intel Hex không
nếu không thì dừng lại và in ra số dòng xảy ra lỗi
Cách kiểm tra: lấy đảo của tổng giá trị byte được giải mã trích xuất ra 2 byte một dưới dạng DEC
sau đó cộng với 1 xem có bằng checksum không
*/
void check_format(char arr[], int line)
{
    // Check xem kí tự đầu tiên có phải ':' hay không nếu không thì không phải file hex và thoát
    if (start != ':')
    {
        printf("This not a Hex File\nLine error at: %d", line);
        exit(1);
    }

    // printf("%c%s%s%s%s%s\n", start, byte_count, address, record_type, data, checksum);

    // tính toán checksum
    int cal_checksum;
    cal_checksum = ~(dec2char(byte_count) + dec2char(address) + dec2char(record_type) + dec2char(data)) + 1;
    //ép kiểu từ int 4 byte về kiểu dữ liệu byte vì checksum chỉ có 1 byte
    // printf("%d\n", (byte)cal_checksum);
    if ((byte)cal_checksum != dec2char(checksum))
    {
        printf("This not a Hex File\nLine error at: %d", line);
        exit(1);
    }

    // Vì độ dài data có thể thay đổi, nên sau mỗi lần trích xuất xong thì giải phóng các phần tử
    // memset(data, '\0', sizeof(data));
}
/*
print dữ liệu data theo mẫu
giới hạn 1 dòng là 16 ô tương ứng với 16 bytes
*/
void print_data(char arr[])
{
    int i = 0;
    // gán từng byte data vào ô nhớ trong buffer
    if (dec2char(record_type) != END_FILE)
    {
        for (indexx; indexx < MAXMEMORY; indexx++)
        {
            buffer[indexx][0] = data[i];
            buffer[indexx][1] = data[i + 1];
            i = i + 2;
            if (i == strlen(data))
            {
                break;
            }
        }
    }

    // Nếu đọc đến kí tự kết thúc file recordtype = 01 thì bắt đầu in
    if (dec2char(record_type) == END_FILE)
    {
        char arr_ascii[3] = {0};
        int j, ascii;
        for (j = 0; j < MAXMEMORY; j++)
        {
            if (j == 0)
            {
                printf("%08x\t", addr);
                addr = addr + 16;
            }
            // Nếu in đủ 16 bytes thì in ASCII
            else if ((j % 16) == 0)
            {
                printf("\t");
                // print ra mã ascii
                int m;
                for (m = j - 16; m < j; m++)
                {
                    arr_ascii[0] = buffer[m][0];
                    arr_ascii[1] = buffer[m][1];
                    ascii = dec2char(arr_ascii);
                    // printf("%d", ascii);
                    // Nếu data là kí tự đặc biệt thì in ra dấu "."
                    if (ascii < 32 || (ascii >= 127 && ascii < 255))
                    {
                        printf(".");
                    }
                    // Nếu data là FF thì in ra khoảng trắng
                    if (ascii == 255)
                    {
                        printf(" ");
                    }
                    // Còn lại là in ra kí tự ASCII
                    if (ascii >= 32 && ascii <= 126)
                    {
                        printf("%c", (char)ascii);
                    }
                }
                // dòng tăng 1
                line++;
                // kiểm tra xem đã đủ 25 dòng chưa
                if (line == 25)
                {
                    // Nếu in hết 25 dòng thì hỏi người dùng in tiếp hay không. Nếu có chọn 1 nếu ko chọn 2
                    int choice;
                    printf("\nDo you want to see more lines?\nEnter 1 to continue Or Enter any key to quit.\n");
                    scanf("%d", &choice);
                    if (choice == 1)
                    {
                        line = 0;
                        goto resume;
                    }
                    else
                    {
                        exit(0);
                    }
                }
            resume:
                printf("\n");
                printf("%08x\t", addr);
                addr = addr + 16;
            }
            printf("%c%c ", buffer[j][0], buffer[j][1]);
        }
    }
}

//Mở file bằng command
void open_by_cmd(char *filename, FILE *file)
{
    if (file == NULL)
    {
        printf("Error: %s could not be opened.", filename);
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    FILE *hex_file;
    if (argc > 1)
        hex_file = fopen(argv[1], "r");
    else
    {
        printf("No input file");
    }
    open_by_cmd(argv[1], hex_file);
    //Clear ô nhớ về FF
    clear_buffer();
    //handle
    while (fgets(arr, 100, hex_file) != NULL)
    {
        extract_element(arr);
        check_format(arr, line);
        print_data(arr);
        memset(arr, '\0', sizeof(arr));
        memset(data, '\0', sizeof(data));
    }
    fclose(hex_file);
}
