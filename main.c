#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#pragma warning(disable : 4996)

typedef struct
{
    long double poczatek_przedzialu, koniec_przedzialu, kroki;
    double dokladnosc;
    long maksylana_liczba_iteracji;
} Parametry;

typedef struct
{
    long double x, suma, wyraz;
    long ostatnia_iteracja;
    short skonczono_iteracje, uzyskano_dokladnosc;
} SzeregInfo;

void otworz_sprawdz_pliki(FILE **plik_wizualny, FILE **plik_csv);
Parametry wczytaj_parametry();
int sprawdz_parametry(Parametry *parametry);
long double func(long double x);
SzeregInfo szereg(long double x, double dokladnosc, long liczba_iteracji);
SzeregInfo utworz_szereg_info(long double x);
void iteruj_szereg(long *iteracja, SzeregInfo *szereg_info);
int czy_uzyskano_dokladnosc(SzeregInfo *szereg_info, double *dokladnosc);
void zapisz_wyniki_i_wypisz(SzeregInfo *szereg_info, long double *y, FILE **plik_wizualny, FILE **plik_csv);
void zamknij_pliki(FILE **plik_wizualny, FILE **plik_csv);

int main()
{
    FILE *plik_wizualny, *plik_csv;
    otworz_sprawdz_pliki(&plik_wizualny, &plik_csv);
    Parametry parametry = wczytaj_parametry();
    long double x, *poczatek, *koniec, *kroki;
    poczatek = &parametry.poczatek_przedzialu;
    koniec = &parametry.koniec_przedzialu;
    kroki = &parametry.kroki;
    for (x = *poczatek; x <= *koniec + 1e-15; x += *kroki)
    {
        SzeregInfo szereg_info = szereg(x,
                                        parametry.dokladnosc,
                                        parametry.maksylana_liczba_iteracji);
        long double y = func(x);
        zapisz_wyniki_i_wypisz(&szereg_info, &y, &plik_wizualny, &plik_csv);
    }
    zamknij_pliki(&plik_wizualny, &plik_csv);
    return 0;
}

void otworz_sprawdz_pliki(FILE **plik_wizualny, FILE **plik_csv)
{
    *plik_wizualny = fopen("wyniki_tabelka.txt", "w");
    *plik_csv = fopen("wyniki.csv", "w");
    if (*plik_wizualny == NULL || *plik_csv == NULL)
    {
        printf("Nie udało się otworzyć plików\n");
        exit(1);
    }
    fprintf(*plik_csv, "x;f_szereg(x);f_scisle(x);liczba wyrazow szeregu;warunek stopu\n");
    fprintf(*plik_wizualny, "|         x|                   f_szereg(x)|                   f_scisle(x)|liczba wyrazow szeregu|warunek stopu|\n");
}

Parametry wczytaj_parametry()
{
    Parametry parametry;
    int ret = -1;
    do
    {
        printf("Podaj:\nPoczątek przedziału\nKoniec przedziału\nKroki\nDokładność\nMaksymalna liczba iteracji\n");
        ret = scanf("%Lf %Lf %Lf %lf %ld",
                    &parametry.poczatek_przedzialu,
                    &parametry.koniec_przedzialu,
                    &parametry.kroki,
                    &parametry.dokladnosc,
                    &parametry.maksylana_liczba_iteracji);
        if (ret != 5)
            printf("Błędne dane\n");
        else if (!sprawdz_parametry(&parametry))
            ret = -1;
        while (getchar() != '\n')
            ;
    } while (ret != 5);

    return parametry;
}

int sprawdz_parametry(Parametry *Parametry)
{
    if ((*Parametry).poczatek_przedzialu <= -1 || (*Parametry).poczatek_przedzialu >= 1)
    {
        printf("Początek przedziału musi być w przedziale (-1, 1)\n");
        return 0;
    }
    if ((*Parametry).koniec_przedzialu <= -1 || (*Parametry).koniec_przedzialu >= 1)
    {
        printf("Koniec przedziału musi być w przedziale (-1, 1)\n");
        return 0;
    }
    if ((*Parametry).poczatek_przedzialu >= (*Parametry).koniec_przedzialu)
    {
        printf("Początek przedziału musi być mniejszy od końca przedziału\n");
        return 0;
    }
    if ((*Parametry).kroki <= 0)
    {
        printf("Kroki muszą być dodatnie\n");
        return 0;
    }
    if ((*Parametry).dokladnosc <= 0)
    {
        printf("Dokładność musi być dodatnia\n");
        return 0;
    }
    if ((*Parametry).maksylana_liczba_iteracji <= 0)
    {
        printf("Maksymalna liczba iteracji musi być dodatnia\n");
        return 0;
    }
    return 1;
}

long double func(long double x)
{
    return pow(1 - x, -1. / 3);
}

SzeregInfo szereg(long double x, double dokladnosc, long liczba_iteracji)
{
    long iteracja;
    SzeregInfo szereg_info = utworz_szereg_info(x);

    liczba_iteracji -= 2; // pierwsze 2 wyrazy są automatycznie sumowane

    for (iteracja = 1; iteracja <= liczba_iteracji; iteracja++)
    {
        iteruj_szereg(&iteracja, &szereg_info);
        if (czy_uzyskano_dokladnosc(&szereg_info, &dokladnosc))
            break;
    }

    szereg_info.ostatnia_iteracja = iteracja + 2;
    szereg_info.skonczono_iteracje = iteracja == liczba_iteracji;

    return szereg_info;
}

SzeregInfo utworz_szereg_info(long double x)
{
    SzeregInfo szereg_info;
    szereg_info.x = x;
    szereg_info.wyraz = ((double)1 / 3) * x;
    szereg_info.suma = 1 + szereg_info.wyraz;

    szereg_info.ostatnia_iteracja = 0;
    szereg_info.skonczono_iteracje = 0;
    szereg_info.uzyskano_dokladnosc = 0;
    return szereg_info;
}

void iteruj_szereg(long *iteracja, SzeregInfo *szereg_info)
{
    unsigned long licznik, mianownik;

    licznik = 1 + (*iteracja * 3);
    mianownik = (*iteracja + 1) * 3;

    (*szereg_info).wyraz *= ((long double)licznik / mianownik) * (*szereg_info).x;
    (*szereg_info).suma += (*szereg_info).wyraz;
}

int czy_uzyskano_dokladnosc(SzeregInfo *szereg_info, double *dokladnosc)
{
    if ((*szereg_info).wyraz < *dokladnosc)
        (*szereg_info).uzyskano_dokladnosc = 1;
    return (*szereg_info).uzyskano_dokladnosc;
}

void zapisz_wyniki_i_wypisz(SzeregInfo *szereg_info, long double *y, FILE **plik_wizualny, FILE **plik_csv)
{
    char *warunek_stopu;
    if ((*szereg_info).skonczono_iteracje && (*szereg_info).uzyskano_dokladnosc)
        warunek_stopu = "oba";
    else if ((*szereg_info).skonczono_iteracje)
        warunek_stopu = "iteracje";
    else if ((*szereg_info).uzyskano_dokladnosc)
        warunek_stopu = "dokladnosc";
    else
    {
        prinf("Gdzieś wystąpił błąd, nie ustalono warunku stopu\n");
        exit(1);
    }

    fprintf(*plik_wizualny, "|%10.5Lf|%30.25Lf|%30.25Lf|%22ld|%13s|\n",
            (*szereg_info).x,
            (*szereg_info).suma,
            *y,
            (*szereg_info).ostatnia_iteracja,
            warunek_stopu);
    fprintf(*plik_csv, "%20.18Lf;%50.40Lf;%50.40Lf;%ld;%s\n",
            (*szereg_info).x,
            (*szereg_info).suma,
            *y,
            (*szereg_info).ostatnia_iteracja,
            warunek_stopu);

    printf("x = %Lf\n", (*szereg_info).x);
    printf("f_szereg(x) = %Lf\n", (*szereg_info).suma);
    printf("f_scisle(x) = %Lf\n", *y);
    printf("liczba wyrazow szeregu = %ld\n", (*szereg_info).ostatnia_iteracja);
    printf("warunek stopu = %s\n", warunek_stopu);
    printf("\n\n\n");
}

void zamknij_pliki(FILE **plik_wizualny, FILE **plik_csv)
{
    fclose(*plik_wizualny);
    fclose(*plik_csv);
}