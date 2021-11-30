#include <stdio.h>
#include <math.h>

double f(double x){//関数
	return x*x*x-x-1;
}
double fd(double x){//微分関数
	double h=1.0e-10;
	return (f(x+h)-f(x))/h;
}

int main(){
	double eps=1.0e-10;
	int n=1;//実行回数
	int max = 100;//最大実行回数
	double delta;
	double x = 10;//初期値
	
	delta = - f(x)/fd(x);
	x += delta;

	while(fabs(delta) > eps && n <= max){//収束条件
		delta = - f(x)/fd(x);
		x += delta;
		printf("%d: delta = %0.6f: x= %0.6f\n", n, delta, x);
		n++;
	}

	return 0;
}
