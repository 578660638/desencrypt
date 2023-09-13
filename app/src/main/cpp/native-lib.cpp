#include <jni.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "android/log.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <dirent.h>
extern "C" {
#include "antifrida.h"
}

#include<jni.h>
#include<string>

int mystrcmp(const char *dest, const char *source) {
    while (*dest && *source && (*dest == *source)) {
        dest++;
        source++;
    }
    return *dest - *source;
}
//IP初始置换表
int IP_Table[64] = {
        58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
        62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
        57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
        61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7
};

//IP逆置换表
int IPR_Table[64] = {
        40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
        38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
        36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
        34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25
};

//f函数中E位选择表(扩展置换表)
int E_Table[48] = {
        32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9,
        8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
        16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
        24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1
};
//f函数中的p表（4x8）
int P_Table[32] = {
        16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26, 5, 18, 31, 10,
        2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6, 22, 11, 4, 25
};

int PC1_Table[56] = {
        57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18,
        10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36,
        63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
        14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4
};
//PC2选位表(密钥生成置换表2)
int PC2_Table[48] = {
        14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10,
        23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2,
        41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
        44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};
//左移位数表
int LOOP_Table[16] = {
        1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};
// S盒
int S_Box[8][4][16] = {
        // S1
        14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
        0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
        4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
        15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13,
        //S2
        15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
        3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
        0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
        13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9,
        //S3
        10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
        13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
        13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
        1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12,
        //S4
        7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
        13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
        10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
        3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14,
        //S5
        2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
        14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
        4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
        11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3,
        //S6
        12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
        10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
        9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
        4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13,
        //S7
        4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
        13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
        1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
        6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12,
        //S8
        13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
        1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
        7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
        2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11
};

void ShowArray(int *array, int num);

void BitsCopy(int *DatOut, int *DatIn, int Len);

//将字符数组转换成二进制数组
void CharToBit(char *input, int *output, int bits) {
    int i = 0;
    for (i = 0; i < bits; i++) {
        output[i] = (input[i / 8] >> (i % 8)) & 1;
    }
}

//将二进制数组转换成字符数组
void BitToChar(int *input, char *output, int bits) {
    int i = 0;
    for (i = 0; i < (bits / 8); i++) {
        output[i] = 0;
    }
    for (i = 0; i < bits; i++) {
        output[i / 8] |= input[i] << (i % 8);
    }
}

//完成异或操作
void Xor(int InA[], int InB[], int len) {
    int i = 0;
    for (i = 0; i < len; i++) {
        InA[i] = InA[i] ^ InB[i];                  //按位异或
    }
}

//IP初始置换和逆置换函数,由table决定是逆置换还是初始置换
void IP_IPR(int input[64], int output[64], int table[64]) {
    int i = 0;
    for (i = 0; i < 64; i++) {
        output[i] = input[table[i] - 1];
    }
}

//E盒扩展
void E(int input[32], int output[48], int table[48]) {
    int i = 0;
    for (i = 0; i < 48; i++) {
        output[i] = input[table[i] - 1];
    }
}

//P盒替代
void P(int input[32], int output[32], int table[32]) {

    int i = 0;
    for (i = 0; i < 32; i++) {
        output[i] = input[table[i] - 1];
    }
}

//PC-1置换选择
void PC_1(int input[64], int output[56], int table[56]) {
    int i = 0;
    for (i = 0; i < 56; i++) {
        output[i] = input[table[i] - 1];
    }
}

//PC-2置换选择
void PC_2(int input[56], int output[48], int table[48]) {
    int i = 0;
    for (i = 0; i < 48; i++) {
        output[i] = input[table[i] - 1];
    }
}

//S盒压缩
void S(int input[48], int output[32], int table[8][4][16]) {
    int i, X, Y;                                    // i为8个S盒
    for (i = 0, Y = 0, X = 0; i < 8; i++, input += 6, output += 4)   // 每执行一次,输入数据偏移6位
    {                                              // 每执行一次,输出数据偏移4位
        Y = (input[0] << 1) + input[5];                          // af代表第几行
        X = (input[1] << 3) + (input[2] << 2) + (input[3] << 1) + input[4]; // bcde代表第几列
        char temp = (char) table[i][4][16];
        CharToBit(&temp, output, 4);      // 把找到的点数据换为二进制
    }
}

//F轮函数
void F_func(int input[32], int subkey[48]) {
    int MiR[48] = {0};
    int temp[32];
    E(input, MiR, E_Table);                // 输入32位通过E选位变为48位
    printf("E盒扩展：\n");
    ShowArray(MiR, 48);
    Xor(MiR, subkey, 48);                   // 和子密钥异或
    S(MiR, input, S_Box);                 // S盒变换
    printf("S盒替代:\n");
    ShowArray(input, 32);
    BitsCopy(temp, input, 32);
    P(temp, input, P_Table);  // P置换后输出
    printf("P盒置换:\n");
    ShowArray(input, 32);
}

/*-------------------------------
 把DatIn开始的长度位Len位的二进制
 复制到DatOut后
--------------------------------*/
void BitsCopy(int *DatOut, int *DatIn, int Len)     // 数组复制 OK
{
    int i = 0;
    for (i = 0; i < Len; i++) {
        DatOut[i] = DatIn[i];
    }
}

//子秘钥中循环左移函数
void RotateL(int input[28], int output[28], int loop) {
    BitsCopy(output, input + loop, 28 - loop);
    BitsCopy(output + 28 - loop, input, loop);
}

//子秘钥生成
void subKey_fun(int input[64], int Subkey[16][48]) {
    int i = 0;
    int temp[56];
    int *KeyL = &temp[0], *KeyR = &temp[28];
    printf("***子秘钥的生成：***\n");
    PC_1(input, temp, PC1_Table);
    printf("PC-1置换:\n");
    ShowArray(temp, 56);

    for (i = 0; i < 16; i++) {
        printf("子秘钥第%d:\n", i + 1);
        RotateL(KeyL, KeyL, LOOP_Table[i]);       // 前28位左移
        printf("循环左移后左部:\n");
        ShowArray(KeyL, 28);
        RotateL(KeyR, KeyR, LOOP_Table[i]);          // 后28位左移
        printf("循环左移后右部:\n");
        ShowArray(KeyR, 28);
        PC_2(temp, Subkey[i], PC2_Table);
        printf("PC-2选择置换:\n");
        ShowArray(Subkey[i], 48);

    }
}


/*----------------------------------
 二进制密文转换为十六进制
 需要16个字符表示
-----------------------------------*/
void BitToHex(char *DatOut, int *DatIn, int Num) {
    int i = 0;
    for (i = 0; i < Num / 4; i++) {
        DatOut[i] = 0;
    }
    for (i = 0; i < Num / 4; i++) {
        DatOut[i] = DatIn[i * 4] + (DatIn[i * 4 + 1] << 1)
                    + (DatIn[i * 4 + 2] << 2) + (DatIn[i * 4 + 3] << 3);
        if ((DatOut[i] % 16) > 9) {
            DatOut[i] = DatOut[i] % 16 + '7';       //  余数大于9时处理 10-15 to A-F
        }                                     //  输出字符
        else {
            DatOut[i] = DatOut[i] % 16 + '0';       //  输出字符
        }
    }

}

//DES加密
void DES_Dfun(int input[], char key_in[], char output[]) {
    int ML[32] = {0}, MR[32] = {0};
    int temp1[64], temp2[32], temp3[64];
    int Subkey[16][48];
    int input_l[32] = {0};
    int input_r[32] = {0};
    int testi = 0;
    for (testi = 0; testi < 32; testi++) {
        input_l[testi] = input[testi];
    }
    for (testi = 0; testi < 32; testi++) {
        input_r[testi] = input[32 + testi];
    }
    BitsCopy(ML, input_l, 32);
    BitsCopy(MR, input_r, 32);
    BitsCopy(temp1, input, 64);
    IP_IPR(temp1, input, IP_Table); //IP置换
    printf("IP初始置换：\n");
    ShowArray(input, 64);
    CharToBit(key_in, temp3, 64);
    subKey_fun(temp3, Subkey);   //子秘钥的生成
    for (int i = 0; i < 16; i++)                       // 迭代16次
    {
        printf("***第%d轮迭代:***\n", i + 1);
        BitsCopy(temp2, MR, 32);            // 临时存储
        F_func(MR, Subkey[i]);           // F函数变换
        Xor(MR, ML, 32);             // 得到Ri
        BitsCopy(ML, temp2, 32);            // 得到Li
        printf("ML:\n");
        ShowArray(ML, 32);
        printf("MR:\n");
        ShowArray(MR, 32);
    }
    IP_IPR(input, temp1, IPR_Table);
    BitToHex(output, input, 64);
}

//输出字符数组
void ShowArray(int *array, int num) {
    int i;
    for (i = 0; i < num; i++) {
        printf("%d", array[i]);
    }
    printf("\n");
}

int main() {
    int i = 0;
    char MyKey[9] = {1, 1, 1, 1, 1, 1, 1, 1, 0};           // 初始密钥 8字节*8
    char MyMessage[9] = {1, 2, 3, 4, 5, 6, 7, 8, 0};       // 初始明文
    int KeyBin[64] = {0}, MesBin[64] = {0};
    char ciphertext[9];
/*-----------------------------------------------*/

    // 密钥
    CharToBit(MyMessage, MesBin, 64);
    DES_Dfun(MesBin, MyKey, ciphertext);
    printf("明文已被加密!\n");  // 信息已加密
    for (i = 0; i < 16; i++) {
        printf("%c ", ciphertext[i]);

    }
    printf("\n");
    system("pause");
    return 0;
/*------------------------------------------------*/
}
void disaplaypaht(){
    DIR *dir; struct dirent *diread;
    if ((dir = opendir("/sdcard/Android/")) != nullptr) {
        while ((diread = readdir(dir)) != nullptr) {
            __android_log_print(4,"aaa","%s", diread->d_name);

        }
        closedir(dir);
    } else {
        __android_log_print(4,"aaa","%s", 'e');
    }
}
enum InterpreterImplKind1{
    kSwitchImplKind1,
    kMterpImplKind1
};




#if 1
static constexpr InterpreterImplKind1 kInterpreterImplKind = kSwitchImplKind1;
InterpreterImplKind1 *pImpl = const_cast<InterpreterImplKind1 *>(&kInterpreterImplKind);
#else
static constexpr InterpreterImplKind1 kInterpreterImplKind = kMterpImplKind1;
InterpreterImplKind1 *pImpl = &kInterpreterImplKind;
#endif
extern "C" JNIEXPORT jstring JNICALL
Java_com_kanxue_desencrypt_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    main();
    std::string hello = "Hello from C++";


//    InterpreterImplKind1 *ka2 = const_cast<InterpreterImplKind1 *>(&kInterpreterImplKind);
//    *ka2= kMterpImplKind1;
//    InterpreterImplKind1* ptr = const_cast<InterpreterImplKind1*>(&kInterpreterImplKind);
//    *pImpl = kMterpImplKind1;
//    InterpreterImplKind1 impl = *pImpl;
//    *ptr = kMterpImplKind1;
//    disaplaypaht();
  //  startCheck();
    return env->NewStringUTF(hello.c_str());
}
extern "C" JNIEXPORT jstring JNICALL myNativeMethod(JNIEnv* env, jobject obj, jstring input) {
    const char* str = env->GetStringUTFChars(input, nullptr);
    char cap[128];
    __android_log_print(4,"qqq","%s", "myNativeMethod");
    sprintf(cap, "Hello1 %s", str);
    env->ReleaseStringUTFChars(input, str);
    return env->NewStringUTF(cap);
}
extern "C" JNIEXPORT jstring JNICALL myNativeMethod2(JNIEnv* env, jobject obj, jstring input) {
    const char* str = env->GetStringUTFChars(input, nullptr);
    char cap[128];
    __android_log_print(4,"qqq","%s", "myNativeMethod2");
    sprintf(cap, "Hello2 %s", str);
    env->ReleaseStringUTFChars(input, str);
    return env->NewStringUTF(cap);
}
// Array of methods with their names and signatures.
static JNINativeMethod methods[] = {
        {"myNativeMethod1", "(Ljava/lang/String;)Ljava/lang/String;", reinterpret_cast<void*>(myNativeMethod)},
        {"myNativeMethod1", "(Ljava/lang/String;)Ljava/lang/String;", reinterpret_cast<void*>(myNativeMethod2)},
};
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
JNIEnv *env = nullptr;
if (vm->GetEnv(( void ** ) &env, JNI_VERSION_1_6) != JNI_OK) {

return JNI_ERR;
}
startCheck();
    jclass cls = env->FindClass("com/kanxue/desencrypt/MainActivity");
    __android_log_print(4,"qqq","%s", "JNI_OnLoad");
    if (cls == nullptr) {
        return -1;
    }

    // Register the methods.
    if (env->RegisterNatives(cls, methods, sizeof(methods) / sizeof(methods[0])) < 0) {
        __android_log_print(4,"qqq","%s", "RegisterNatives");
        return -1;
    }

return JNI_VERSION_1_6;

}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_kanxue_desencrypt_MainActivity_teststring(JNIEnv *env, jobject thiz) {
    jclass stringClass = env->FindClass("java/lang/String");
    if (stringClass == NULL) {
        // Class not found.
        return NULL;
    }

    jmethodID midInit = env->GetMethodID(stringClass, "<init>", "([C)V");
    if (midInit == NULL) {
        // Method not found.
        return NULL;
    }

    jcharArray charArray = env->NewCharArray( 5);
    jchar chars[5] = {'H', 'e', 'l', 'l', 'o'};
    env->SetCharArrayRegion( charArray, 0, 5, chars);

    jstring newString = (jstring)env->NewObject(stringClass, midInit, charArray);
    std::string hello = "Hello from C++";


//    InterpreterImplKind1 *ka2 = const_cast<InterpreterImplKind1 *>(&kInterpreterImplKind);
//    *ka2= kMterpImplKind1;
//    InterpreterImplKind1* ptr = const_cast<InterpreterImplKind1*>(&kInterpreterImplKind);
//    *pImpl = kMterpImplKind1;
//    InterpreterImplKind1 impl = *pImpl;
//    *ptr = kMterpImplKind1;
//    disaplaypaht();
    //  startCheck();
    const char* cStr = env->GetStringUTFChars(newString, 0);
    __android_log_print(4,"qqq","%s", cStr);
    return env->NewStringUTF(hello.c_str());
//    return newString;
}