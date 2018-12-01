//
//  main.cpp
//  CubeSplain
//
//  Created by Максим Лихачев on 26/11/2018.
//  Copyright © 2018 Максим Лихачев. All rights reserved.
//

#include <locale>
#include <fstream>
#include <iostream>
using namespace std;

struct Function // Структура функции
{
    double x;
    double f;
};

struct BoundaryValues // Структура для хранения граничных значений
{
    double A;
    double B;
    double XX;
};

struct Coefficients //  Структура коэффициентов трехдиагональной матрицы
{
    double A;
    double B;
    double C;
    double F;
};

double * ThreePointSweep(int n, Coefficients * rate) // Метод трехточечной прогонки
{
    double * x = new double [n+1];
    double * mu = new double [n+1];
    double * nu = new double [n+1];

    mu[1] = - rate[0].B / rate[0].C;
    nu[1] =   rate[0].F / rate[0].C;

    for( int i=1; i<n; i++ )
    {
        double D = rate[i].C + rate[i].A * mu[i];
        mu[i+1] = - rate[i].B/ D;
        nu[i+1] = ( rate[i].F - rate[i].A * nu[i] )/ D;
    }

    x[n] = (rate[n].F - rate[n].A * nu[n]) / (rate[n].C + rate[n].A * mu[n]);

    for(int i=n-1; i>=0; i--) x[i] = mu[i+1] * x[i+1] + nu[i+1];

    return x;
}

double FindYY(double XX, double * h, double * c, Function * vector) // Нахождение требуемого значения функции через кубический сплайн
{
    int i=1;
    while (vector[i].x<XX) i++;

    cout << "\nКоэффициенты S[" << i << "]:\n";

    double a=vector[i].f;
    double d=(c[i]-c[i-1])/h[i];
    double b=h[i]/2.0*c[i] - h[i]*h[i]/6.0*d + (vector[i].f-vector[i-1].f)/h[i];

    cout << a << " " << b << " " << c[i] << " " << d << endl;

    double x = XX - vector[i].x;
    return a + x*b + x*x*c[i]/2.0 + x*x*x*d/6.0;;
}


double * GetH(int n, Function * vector) // Определение шага
{
    double * h = new double[n+1];
    for (int i=1; i<=n; i++) h[i] = vector[i].x - vector[i-1].x;
    return h;
}

Coefficients * InitCoefficients(int n, BoundaryValues constants, double * h, Function * vector) // Определение коэффициентов СУ
{
    Coefficients *rate = new Coefficients[n+1];

    rate[0].C = 1.0;
    rate[0].B = 0.0;
    rate[0].F = constants.A;

    rate[n].C = 2.0;
    rate[n].A = 0.0;
    rate[n].F = (6.0/h[n]) * (constants.B - (vector[n].f-vector[n-1].f)/h[n]);

    for (int i=1; i<n; i++)
    {
        rate[i].A = h[i];
        rate[i].B = h[i+1];
        rate[i].C = 2.0 * (h[i] + h[i+1]);
        rate[i].F = 6.0 * vector[i].f;
    }

    return rate;
}

int Read(BoundaryValues &constant, Function *&vector) // Чтение из файла
{
    ifstream input("input.txt");

    int n;
    input >> n;
    input >> constant.A;
    input >> constant.B;
    input >> constant.XX;

    vector = new Function[n+1];
    for (int i=0; i<=n; i++) input >> vector[i].x;
    for (int i=0; i<=n; i++) input >> vector[i].f;

    input.close();
    return n;
}

void PutResult(int IER, double YY, double XX)  // Запись результата в файл
{
    ofstream outputfile("output.txt");
    switch(IER)
    {
        case 1:outputfile << "N слишком мало! (N < 2)\n\n";break;
        case 2:outputfile << "Нарушен порядок возрастания аргументов функции\n\n";break;
        case 3:outputfile << "XX не является элементом заданного множества аргументов функции\n\n";break;
        default: outputfile << "Программа завершена успешно\nЗначение функции в точке " << XX << " = " << YY << "\n";
    }
    outputfile.close();
}

int IsError(int N, Function *vector, double XX)    //    Проверка ошибок
{
    bool isError = false;

    for (int i = 1; i < N && !isError; i++)
        if (vector[i-1].x > vector[i].x)
            isError = true;

    int IER = 0;
    if (N < 2)
    {
        IER = 1;
        cout << "IER = 1\nN слишком мало! (N < 2)\nПрограмма будет завершена\n\n";
    }
    else if (isError)
    {
        IER = 2;
        cout << "IER = 2\nНарушен порядок возрастания аргументов функции\nПрограмма будет завершена\n\n";
    }
    else if (XX < vector[0].x || XX > vector[N].x)
    {
        IER = 3;
        cout << "IER = 3\nXX не является элементом заданного множества аргументов функции\nПрограмма будет завершена\n\n";
    }
    else cout << "IER = 0\n\n";

    return IER;
}

int main(int argc, const char * argv[])
{
    setlocale(LC_ALL, "Russian");

    BoundaryValues constant;
    Function *vector;
    int n = Read(constant,vector);

    int isError = IsError(n, vector, constant.XX);

    if(isError==0)
    {
        double *h = GetH(n, vector);

        cout << "Выполнил студент 3 курса 61 группы\nЛихачев Максим\n\n";
        cout << "Функция имеет " << n+1 << " значений и граничные условия: S''(" << vector[0].x << ") = " << constant.A << " и S'(" <<vector[n].x << ") = " << constant.B << endl;
        for(int i=0; i<=n; i++) cout <<"f("<<vector[i].x << ") = " << vector[i].f<<endl;
        cout<<"Найти значение функции в точке: "<<constant.XX<<endl;

        double YY = FindYY(constant.XX, h, ThreePointSweep(n, InitCoefficients(n,constant,h,vector)),vector);

        cout<<"\nИскомое значение функции в точке "<<constant.XX<<": "<< YY<<endl;
        PutResult(isError, YY, constant.XX);
    }

    return 0;
}
