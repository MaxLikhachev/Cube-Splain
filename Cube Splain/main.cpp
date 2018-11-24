//
//  main.cpp
//  Cube Splain
//
//  Created by Максим Лихачев on 28/10/2018.
//  Copyright © 2018 Максим Лихачев. All rights reserved.
//

#include <locale>
#include <fstream>
#include <iostream>
using namespace std;

void ShowIntro();                                                                           //  Заставка
bool IsConfirm();                                                                           //  Подтверждение выбора пользователя
void ShowVector(int N, double **vector);                                                    //  Вывод аргументов и значений функций из файла на экран
bool isVectorInvalid(int N, double **vector);                                               //  Проверка на ошибки
double **GetVector();                                                                       //  Получение вектора значений из файла
void PutResult(int IER, double YY, double XX);                                              //  Запись результата в файл
int IsError(int N, double **vector, double XX);                                             //  Проверка малости количества значений
double *EnterConstants();                                                                   //  Ввод количества значений и проверка валидности
double *ThreePointSweep (int N, int A, int B, double **vector);                             //  Решение методом трехточечной прогонки
double **FindSplainCoefficients(int N, double **vector, double *c);                         //  Определение коэффициентов сплайна
double FindYY(int N, double XX, double **splain, double **vector);                          //  Определение значения функции в данной точке

int main(int argc, const char * argv[])
{
    setlocale(LC_ALL, "Russian");

    ShowIntro();

    double *coefficients = EnterConstants();
    int N = coefficients[0];
    int A = coefficients[1];
    int B = coefficients[2];
    double XX = coefficients[3];
    delete coefficients;

    //cout<<N<<" "<<A<<" "<<B<< " " << XX << endl<<endl;

    double **vector = GetVector();

    
    if(IsError(N, vector, XX)==0)
    {
        cout<<"OK: Значения функции прочитаны\n";
        ShowVector(N, vector);

        int YY = FindYY(N, XX, FindSplainCoefficients(N, vector, ThreePointSweep(N, A, B, vector)), vector);
        cout << "Значение функции в точке " << XX << " = " << YY << endl;

        PutResult(IsError(N, vector, XX), YY, XX);

    }
    cout << "\nПрограмма завершена\n";

    for(int k = 0; k < 2; k++) delete vector[k];
    delete vector;
    return 0;
}

void ShowIntro()    //    Заставка
{
    cout << "Выполнил студент 3 курса 61 группы\nЛихачев Максим\n\n";
}

bool IsConfirm()    //    Подтверждение выбора пользователя
{
    cout << "\tY/N\n";
    bool isConfirm = false;
    char command;
    cin >> command;
    if (command == 'y' || command == 'Y') isConfirm = true;

    return isConfirm;
}

void ShowVector(int N, double **vector)    //    Вывод значений функций из файла на экран
{
    cout << "Данная функция y имеет " << N << " значений:\n";
    for (int i = 0; i < N; i++)
        cout << "y" << i << "("<<vector[0][i]<<") = " << vector[1][i] << endl;
    cout << endl;
}

bool isVectorInvalid(int N, double **vector)    //  Проверка элементов вектора на возрастание
{
    bool isError = false;

    for (int i = 1; i < N && !isError; i++)
        if (vector[0][i-1] > vector[0][i])
            isError = true;

    return isError;
}

double **GetVector() // Получение вектора значений из файла
{
    ifstream inputfile("input.txt");

    int N, A, B, XX;
    inputfile >> N;
    inputfile >> A;
    inputfile >> B;
    inputfile >> XX;

    double **vector = new double*[2];
    for(int k = 0; k < 2; k++)
        vector[k] = new double[N];

    for(int k = 0; k < 2; k++)
        for (int i = 0; i < N; i++)
            inputfile >> vector[k][i];

    inputfile.close();

    return vector;
}

void PutResult(int IER, double YY, double XX)  // Запись результата в файл
{
    ofstream outputfile("output.txt");
    switch(IER)
    {
        case 1:outputfile << "N слишком мало! (N < 2)\n\n";break;
        case 2:outputfile << "Нарушен порядок возрастания аргументов функции\n\n";break;
        case 3:outputfile << "XX не является элементом заданного множества аргументов функции\n\n";break;
        default: outputfile << "Значение функции в точке " << XX << " = " << YY << "\n";
    }
    outputfile.close();
}

int IsError(int N, double **vector, double XX)    //    Проверка ошибок
{
    int IER = 0;
    if (N < 2)
    {
        IER = 1;
        cout << "IER = 1\nN слишком мало! (N < 2)\nПрограмма будет завершена\n\n";
    }
    else if (isVectorInvalid(N, vector))
    {
        IER = 2;
        cout << "IER = 2\nНарушен порядок возрастания аргументов функции\nПрограмма будет завершена\n\n";
    }
    else if (XX < vector[0][0] || XX > vector[0][N-1])
    {
        IER = 3;
        cout << "IER = 3\nXX не является элементом заданного множества аргументов функции\nПрограмма будет завершена\n\n";
    }
    else cout << "IER = 0\n\n";

    return IER;
}

double *EnterConstants()    //    Ввод количества значений, и граничных услловий
{
    ifstream inputfile("input.txt");

    double *constants = new double[4];

    for(int i=0; i<4; i++)
        inputfile >> constants[i];

    inputfile.close();
    return constants;
}

int FindSplainIndex(int N, double XX, double **vector)                             //  Определение местоположения данной точки
{
    //for(int i=0; i<N; i++) cout << "x["<<i<<"] = " << vector[0][i]<<endl;

    int i=1;
    while(i<N && XX > vector[0][i-1])
    {
        cout << "x["<<i<<"] = " << vector[0][i-1]<<endl;
        i++;
    }
    return i;
}

double FindYY(int N, double XX, double **splain, double **vector)                     //  Определение значения функции в данной точке
{
    int i = FindSplainIndex(N, XX, vector);
    cout<<"i="<<i<<endl;
    double d = XX - vector[0][i];
    double YY = splain[0][i] + splain[1][i] * d + splain[2][i]/2.0 * d * d + splain[3][i]/6.0 * d * d * d;

    return YY;
}

double **FindSplainCoefficients(int N, double **vector, double *c)                  //  Определение коэффициентов сплайна
{
    double ** splain = new double*[4];
    for (int i=0; i<4; i++)
        splain[i] = new double[N];

    for (int i=1; i<N; i++)
    {
        splain[0][i] = vector[1][i];
        splain[1][i] = -1.0/3.0 * ( vector[0][i] - vector[0][i-1] ) * c[i] -1.0/6.0 * ( vector[0][i] - vector[0][i-1] ) * c[i-1] + ( vector[1][i] - vector[1][i-1] )/( vector[0][i] - vector[0][i-1] );
        splain[2][i] = c[i];
        splain[3][i] = ( c[i] - c[i-1] )/( vector[0][i] - vector[0][i-1] );
    }

    return splain;
}

double *ThreePointSweep(int N, int A, int B, double **vector)                                                 //    Решение методом трехточечной прогонки
{
    double *c = new double[N+1];

    double *mu = new double[N];
    double *nu = new double[N];

    mu[0]=0;
    nu[0]=A;

    for(int i=1; i<N; i++)
    {
        double d = 2.0*(vector[0][i+1]-vector[0][i-1]) + (vector[0][i]-vector[0][i-1])*mu[i-1];
        mu[i] = (vector[0][i+1]-vector[0][i])/d;
        nu[i] = (6.0*vector[1][i] - (vector[0][i]-vector[0][i-1])*mu[i-1]);
    }

    for(int i=0; i<N+1; i++) cout << "mu["<< i <<"]=" << mu[i]<< " nu["<< i <<"]=" <<nu[i]<<endl;

    c[N]=((6.0/(vector[0][N]-vector[0][N-1]))*(B - (vector[1][N]-vector[1][N-1])/(vector[0][N]-vector[0][N-1]) ) - nu[N-1])/(2+mu[N-1]);

    for (int i=N-1; i>=0; i--)
        c[i] = mu[i]*c[i+1]+nu[i];

    for(int i=0; i<N+1; i++) cout << "c["<<i<<"] = " << c[i]<<endl;
    return c;
}
