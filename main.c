#define CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#pragma warning(disable : 4996)

typedef struct
{
    long double interval_start, interval_end, n;
    double precision;
    long max_iterations;
} Parameters;

typedef struct
{
    long double x, sum, term;
    long last_iteration;
    short iterations_finished, precision_achieved;
} SeriesInfo;

void open_check_files(FILE **visual_file, FILE **csv_file);
Parameters read_parameters();
long double read_long_double(char *message);
double read_double(char *message);
int check_parameters_and_report_error(Parameters *Parameters);
long double function(long double x);
SeriesInfo series(long double x, double precision, long number_of_iterations);
SeriesInfo initialize_series_info(long double x);
void iterate_series(long *iteration, SeriesInfo *series_info);
int is_precision_achieved(SeriesInfo *series_info, double *precision);
void save_and_print_results(SeriesInfo *series_info, long double *y, FILE **visual_file, FILE **csv_file);
void close_files(FILE **visual_file, FILE **csv_file);

int main()
{
    FILE *visual_file, *csv_file;
    open_check_files(&visual_file, &csv_file);
    Parameters parameters = read_parameters();
    long double x, *start, *end, steps;
    start = &parameters.interval_start;
    end = &parameters.interval_end;
    steps = (end-start) / parameters.n;
    x = *start;
    while (x <= *end + 1e-15)
    {
        SeriesInfo series_info = series(x,
                                        parameters.precision,
                                        parameters.max_iterations);
        long double y = function(x);
        save_and_print_results(&series_info, &y, &visual_file, &csv_file);
        x += steps;
    }
    close_files(&visual_file, &csv_file);
    return 0;
}

void open_check_files(FILE **visual_file, FILE **csv_file)
{
    *visual_file = fopen("results_table.txt", "w");
    *csv_file = fopen("results.csv", "w");
    if (*visual_file == NULL || *csv_file == NULL)
    {
        printf("Can't open files\n");
        exit(1);
    }
    fprintf(*csv_file, "x;f_series(x);exact_function(x);Ilość liczb w szeregu;Warunek zatrzymania\n");
    fprintf(*visual_file, "|                  x|       f_series(x)| exact_function(x)|Ilość liczb w szeregu|Warunek zatrzymania|\n");
}

Parameters read_parameters()
{
    Parameters parameters;
    do
    {
        printf("UWAGA: liczby mogą mieć maksymalnie 16 cyfr po przecinku, w przeciwnym wypadku pojawią się błędy\n");
        parameters.interval_start = read_long_double("Podaj początek przedziału: ");
        parameters.interval_end = read_long_double("Podaj koniec przedziału: ");
        parameters.n = read_long_double("Podaj liczbę podziału: ");
        parameters.precision = read_double("Podaj dokładność: ");
        parameters.max_iterations = read_long_double("Podaj maksymalną liczbę iteracji: ");
    } while (!check_parameters_and_report_error(&parameters));
    return parameters;
}

long double read_long_double(char *message)
{
    long double number;
    int ret;
    do
    {
        printf("%s", message);
        ret = scanf("%Lf", &number);
        if (ret != 1)
            printf("Wrong data\n");
        while (getchar() != '\n')
            ;
    } while (ret != 1);
    return number;
}

double read_double(char *message){
    double number;
    int ret;
    do
    {
        printf("%s", message);
        ret = scanf("%lf", &number);
        if (ret != 1)
            printf("Wrong data\n");
        while (getchar() != '\n')
            ;
    } while (ret != 1);
    return number;
}

int check_parameters_and_report_error(Parameters *Parameters)
{
    if ((*Parameters).interval_start <= -1 || (*Parameters).interval_start >= 1)
    {
        printf("Początek przedziału musi być w przedziale (-1, 1)\n");
        return 0;
    }
    if ((*Parameters).interval_end <= -1 || (*Parameters).interval_end >= 1)
    {
        printf("Koniec przedziału musi być w przedziale (-1, 1)\n");
        return 0;
    }
    if ((*Parameters).interval_start >= (*Parameters).interval_end)
    {
        printf("Początek przedziału musi być mniejszy od końca przedziału\n");
        return 0;
    }
    if ((*Parameters).n <= 0)
    {
        printf("Kroki muszą być dodatnie\n");
        return 0;
    }
    if ((*Parameters).precision <= 0)
    {
        printf("Dokładność musi być dodatnia\n");
        return 0;
    }
    if ((*Parameters).max_iterations <= 1)
    {
        printf("Maksymalna liczba iteracji musi być większa od 1\n");
        return 0;
    }
    return 1;
}

long double function(long double x)
{
    return pow((double)(1 - x), -1. / 3);

}

SeriesInfo series(long double x, double precision, long number_of_iterations)
{
    long iteration;
    SeriesInfo series_info = initialize_series_info(x);

    number_of_iterations -= 2; // pierwsze 2 wyrazy są automatycznie sumowane

    for (iteration = 1; iteration <= number_of_iterations; iteration++)
    {
        iterate_series(&iteration, &series_info);
        if (is_precision_achieved(&series_info, &precision))
            break;
    }
    iteration--;
    series_info.last_iteration = iteration + 2;
    series_info.iterations_finished = iteration == number_of_iterations;

    return series_info;
}

SeriesInfo initialize_series_info(long double x)
{
    SeriesInfo series_info;
    series_info.x = x;
    series_info.term = ((double)1 / 3) * x;
    series_info.sum = 1 + series_info.term;

    series_info.last_iteration = 0;
    series_info.iterations_finished = 0;
    series_info.precision_achieved = 0;
    return series_info;
}

void iterate_series(long *iteration, SeriesInfo *series_info)
{
    unsigned long numerator, denominator;

    numerator = 1 + (*iteration * 3);
    denominator = (*iteration + 1) * 3;

    (*series_info).term *= ((long double)numerator / denominator) * (*series_info).x;
    (*series_info).sum += (*series_info).term;
}

int is_precision_achieved(SeriesInfo *series_info, double *precision)
{
    if (fabs((*series_info).term) < *precision)
        (*series_info).precision_achieved = 1;
    return (*series_info).precision_achieved;
}

void save_and_print_results(SeriesInfo *series_info, long double *y, FILE **visual_file, FILE **csv_file)
{
    char *stop_condition;
    if ((*series_info).iterations_finished && (*series_info).precision_achieved)
        stop_condition = "oba";
    else if ((*series_info).iterations_finished)
        stop_condition = "iteracje";
    else if ((*series_info).precision_achieved)
        stop_condition = "precision";
    else
    {
        printf("Gdzieś wystąpił błąd, nie ustalono warunku stopu\n");
        exit(1);
    }

    fprintf(*csv_file, "%+18.16Lf;%18.16Lf;%18.16Lf;%ld;%s\n",
            (*series_info).x,
            (*series_info).sum,
            *y,
            (*series_info).last_iteration,
            stop_condition);
    fprintf(*visual_file, "|%+18.16Lf|%18.16Lf|%18.16Lf|%21ld|%19s|\n",
            (*series_info).x,
            (*series_info).sum,
            *y,
            (*series_info).last_iteration,
            stop_condition);

    printf("x = %Lf\n", (*series_info).x);
    printf("suma szeregu = %Lf\n", (*series_info).sum);
    printf("f(x) = %Lf\n", *y);
    printf("liczba termow seriesu = %ld\n", (*series_info).last_iteration);
    printf("warunek zatrzymania = %s\n", stop_condition);
    printf("\n\n\n");
}

void close_files(FILE **visual_file, FILE **csv_file)
{
    fclose(*visual_file);
    fclose(*csv_file);
}