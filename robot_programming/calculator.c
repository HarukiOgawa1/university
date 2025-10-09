// 電卓
// コンパイル方法 gcc calculator.c -o calculator -lm

#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main() {
    char operator;
    double num;
    double sum = 0.0; // 計算結果を保存する変数
    int mode = 0;     // 0: 四則演算モード, 1: 関数電卓モード

    printf("電卓\n");
    printf("終了: 'q' | クリア: 'c'\n");
    printf("モード切替 (関数電卓: 'f', 四則演算: 'a')\n");
    printf("----------------------------------------\n");

    while (1) {
        printf("sum = %g\n", sum);

        // モードに応じた案内を表示
        if (mode == 0) {
            printf("【四則演算】演算子 (+, -, *, /) を入力: ");
        } else {
            printf("【関数電卓】演算子と関数、角度を入力 (例: +s30): ");
        }

        // 演算子またはコマンドの最初の文字を入力
        scanf(" %c", &operator);

        // --- 全モード共通のコマンド処理 ---
        if (operator == 'q') {
            break; // 'q'が入力されたらループを抜けてプログラム終了
        }
        if (operator == 'c') {
            sum = 0.0;
            printf("クリアしました\n\n");
            continue; // ループの先頭へ
        }
        if (operator == 'f') {
            mode = 1; // 関数電卓モードに切り替え
            printf("関数電卓モードに切り替えました\n\n");
            continue;
        }
        if (operator == 'a') {
            mode = 0; // 四則演算モードに切り替え
            printf("四則演算モードに戻りました\n\n");
            continue;
        }

        // --- モード別の処理 ---
        if (mode == 0) { // 四則演算モードの処理
            if (operator != '+' && operator != '-' && operator != '*' && operator != '/') {
                printf("エラー: 無効な演算子です\n\n");
                while (getchar() != '\n'); // 不正な入力がバッファに残らないようにする
                continue;
            }

            printf("数字を入力してください: ");
            if (scanf("%lf", &num) != 1) { // 数字以外の入力への対策
                printf("エラー: 無効な数字です\n\n");
                while (getchar() != '\n');
                continue;
            }

            switch (operator) {
                case '+': sum = sum + num; break;
                case '-': sum = sum - num; break;
                case '*': sum = sum * num; break;
                case '/':
                    if (num != 0) {
                        sum = sum / num;
                    } else {
                        printf("エラー: 0で割ることはできません\n");
                    }
                    break;
            }
        } else { // 関数電卓モードの処理
            char func_type; // 's' (sin), 'c' (cos), 't' (tan) を格納
            double angle;   // 角度を格納
            double result;  // 三角関数の計算結果を格納

            if (operator != '+' && operator != '-' && operator != '*' && operator != '/') {
                printf("エラー: 演算子は +, -, *, / のいずれかを入力してください\n\n");
                while (getchar() != '\n');
                continue;
            }

            // 演算子に続いて関数(s,c,t)と角度(例: s30)を読み込む
            if (scanf("%c%lf", &func_type, &angle) != 2) {
                printf("エラー: 関数の形式が不正です(例: s30)\n\n");
                while (getchar() != '\n');
                continue;
            }

            // C言語の三角関数はラジアンを引数に取るため、度数から変換
            double radian = angle * M_PI / 180.0;
            int calculation_ok = 1; // 計算が成功したかを管理するフラグ

            switch (func_type) {
                case 's':
                    result = sin(radian);
                    printf("sin(%.2lf) = %.5lf\n", radian, sin(radian));
                    break;
                case 'c':
                    result = cos(radian);
                    printf("cos(%.2lf) = %.5lf\n", radian, cos(radian));
                    break;
                case 't':
                    // tan(90度)などは計算できないためチェック
                    if (fabs(cos(radian)) < 1e-9) {
                        printf("エラー: tan(%g) は計算できません\n", angle);
                        calculation_ok = 0;
                    } else {
                        result = tan(radian);
                        printf("tan(%.2lf) = %.5lf\n", radian, tan(radian));
                    }
                    break;
                default:
                    printf("エラー: 無効な関数です ('s', 'c', 't' を使用してください)\n");
                    calculation_ok = 0;
                    break;
            }

            // 三角関数の計算が成功した場合のみ、sumに結果を反映
            if (calculation_ok) {
                switch (operator) {
                    case '+': sum = sum + result; break;
                    case '-': sum = sum - result; break;
                    case '*': sum = sum * result; break;
                    case '/':
                        if (fabs(result) > 1e-9) { // ほぼ0でないかチェック
                            sum = sum / result;
                        } else {
                            printf("エラー: 0で割ることはできません\n");
                        }
                        break;
                }
            }
        }
        printf("\n"); // 見やすくするために改行
    }

    printf("----------------------------------------\n");
    printf("プログラムを終了します. 最終結果: %.4lf\n", sum);

    return 0;
}