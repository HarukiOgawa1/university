#include <stdio.h>
#include <math.h>
double get_x1(double x2, double x3)
{
        return((10.0 - x2 - x3) / 5.0);
}

double get_x2(double x1, double x3)
{
        return((12.0 - x1 -x3) / 4.0);
}

double get_x3(double x1, double x2)
{
        return((13.0 - 2.0 * x1 - x2) / 3.0);
}

int main()
{
        double x1, x2, x3, y1, y2, y3, z1, z2, z3;
        int i;
	double e1,e2,e3;//各xの前と後の誤差
        x1 = x2 = x3 = 10000.0;//initial values
        for(i=0; i<20; i++)
        {
		x1 = get_x1(x2, x3);
		x2 = get_x2(x1, x3);
		x3 = get_x3(x1, x2);

		e1 = fabs(x1-z1);//xの前と後の各誤差
		e2 = fabs(x2-z2);
		e3 = fabs(x3-z3);
		if((e1 < 0.000001) && (e2 < 0.000001) && (e3 < 0.000001)){//各許容誤差が0.000001未満だと収束判定
			printf("convergence\n");
		}

		printf( "[i=%d] x1 = %.10lf, x2 = %.10lf, x3 = %.10lf\n ", i+1, x1, x2, x3);
		y1 = x1;//x1,x2,x3の値を前と後で比べる用に値を代入
		y2 = x2;
		y3 = x3;
		z1 = y1;
		z2 = y2;
		z3 = y3;
        }
        return 0;
}

