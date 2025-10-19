#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LEN 1000

typedef struct {
    int digits[MAX_LEN];
    int len;
    int sign;
} BigInt;

void initBigInt(BigInt* num) {
    memset(num->digits, 0, sizeof(num->digits));
    num->len = 1;
    num->sign = 0;
}

// 从字符串初始化大数
void fromString(BigInt* num, const char* str) {
    initBigInt(num);

    int start = 0;
    if (str[0] == '-') {
        num->sign = 1;
        start = 1;
    }

    num->len = strlen(str) - start;

    // 逆序存储：digits[0]是个位
    for (int i = 0; i < num->len; i++) {
        num->digits[i] = str[strlen(str) - 1 - i] - '0';
    }
}

BigInt addBigInt(BigInt a, BigInt b) {
    BigInt result;
    initBigInt(&result);
    int carry = 0;
    int max_len = a.len > b.len ? a.len : b.len;

    // 修复：正确的循环条件
    for (int i = 0; i < max_len; i++) {
        int sum = a.digits[i] + b.digits[i] + carry;
        result.digits[i] = sum % 10;
        carry = sum / 10;
    }

    if (carry > 0) {
        result.digits[max_len] = carry;
        result.len = max_len + 1;
    }
    else {
        result.len = max_len;
    }

    return result;
}

int compareBigInt(BigInt a, BigInt b) {
    if (a.len != b.len)
        return a.len > b.len ? 1 : -1;  // 修复：返回比较结果，不是长度

    for (int i = a.len - 1; i >= 0; i--) {
        if (a.digits[i] != b.digits[i]) {
            return a.digits[i] > b.digits[i] ? 1 : -1;
        }
    }
    return 0;
}

BigInt subBigInt(BigInt a, BigInt b) {
    BigInt result;
    initBigInt(&result);

    if (compareBigInt(a, b) < 0) {
        result = subBigInt(b, a);
        result.sign = 1;
        return result;
    }

    int borrow = 0;
    for (int i = 0; i < a.len; i++) {
        int diff = a.digits[i] - borrow;  // 修复：先减去借位
        if (i < b.len) {                  // 修复：检查b的长度
            diff -= b.digits[i];
        }

        if (diff < 0) {
            diff += 10;
            borrow = 1;
        }
        else {
            borrow = 0;
        }
        result.digits[i] = diff;
    }

    result.len = a.len;
    while (result.len > 1 && result.digits[result.len - 1] == 0) {
        result.len--;
    }
    return result;
}

BigInt mulBigInt(BigInt a, BigInt b) {
    BigInt result;
    initBigInt(&result);

    for (int i = 0; i < a.len; i++) {
        int carry = 0;
        for (int j = 0; j < b.len; j++) {
            int product = a.digits[i] * b.digits[j] + result.digits[i + j] + carry;
            result.digits[i + j] = product % 10;
            carry = product / 10;
        }
        if (carry > 0) {
            result.digits[i + b.len] += carry;
        }
    }

    result.len = a.len + b.len;
    while (result.len > 1 && result.digits[result.len - 1] == 0) {
        result.len--;
    }
    result.sign = a.sign ^ b.sign;
    return result;
}

// 高精度除以低精度
BigInt divBigInt(const BigInt* a, int b, int* remainder) {
    BigInt result;
    initBigInt(&result);
    if (b == 0) {
        printf("错误，除数为0\n");  // 修复：去掉\0
        return result;
    }

    // 处理符号
    result.sign = a->sign ^ (b < 0 ? 1 : 0);
    if (b < 0)
        b = -b;

    long long rem = 0;
    result.len = a->len;

    for (int i = a->len - 1; i >= 0; i--) {
        rem = rem * 10 + a->digits[i];
        result.digits[i] = rem / b;
        rem = rem % b;
    }

    // 修复：去除前导零，但至少保留1位
    while (result.len > 1 && result.digits[result.len - 1] == 0) {
        result.len--;
    }

    if (remainder != NULL) {
        *remainder = rem;
        if (a->sign)
            *remainder = -(*remainder);
    }
    return result;
}

void printBigInt(BigInt num) {
    if (num.sign)
        printf("-");
    for (int i = num.len - 1; i >= 0; i--) {
        printf("%d", num.digits[i]);
    }
    printf("\n");
}

int main() {
    BigInt a, b;
    char str1[MAX_LEN], str2[MAX_LEN];

    printf("请输入第一个大数: ");
    scanf("%s", str1);
    printf("请输入第二个大数: ");
    scanf("%s", str2);

    fromString(&a, str1);
    fromString(&b, str2);

    printf("\n计算结果:\n");

    BigInt res_add = addBigInt(a, b);
    printf("加法: ");
    printBigInt(res_add);

    BigInt res_sub = subBigInt(a, b);
    printf("减法: ");
    printBigInt(res_sub);

    BigInt res_mul = mulBigInt(a, b);
    printf("乘法: ");
    printBigInt(res_mul);

    int rem;
    BigInt res_div = divBigInt(&a, 123, &rem);  // 除以123作为示例
    printf("除法 (除以123): ");
    printBigInt(res_div);
    printf("余数: %d\n", rem);

    return 0;
}