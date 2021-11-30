#include <stdio.h>
#include <math.h>
#define EPS (1.0)//Δx

double f(double x){//計算する関数
	return(1/x);
}

double daikei(double x){//台形法
	double a = 1, b = 2, sum_daikei = 0;//区間
	for(double x = a; x < b; x += EPS){
        sum_daikei += (f(x) + f(x+EPS));
    }
    sum_daikei *= (EPS / 2);
	printf("%f\n", sum_daikei);
}

double simpson(double x){//シンプソン法
	double a = 1, b = 2, sum_simpson = 0;//区間
	for(double x = a; x < b; x += EPS * 2){
		sum_simpson += (f(x) + 4 * f(x + EPS) + f(x + EPS * 2));
    }
	sum_simpson *= (EPS / 3);
	printf("%f\n", sum_simpson);
}

int main(){
	double x;
	daikei(x);
	simpson(x);
	return 0;
}
