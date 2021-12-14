#include <bdd.h>
#include <fstream>
#include <memory>
#include <cmath>
#include <vector>


std::ofstream out;
const int N = 9; // Система из N объектов, принимающих различные значения
const int M = 4; // Обладающих M свойствами
const int ROW_LENGTH = 3;
const int LOG_N = std::ceil(std::log2(N)); //Максимальное изменение индекса, отвечающего за бит свойства
char *var;
int arr[N][M];

enum class Neighbour {
    UPPERLEFT,
    LEFT
};

void fun(char *varset, int size);  //Для вывода решения

void init(bdd p[M][N][N]);

bdd limit_1(const bdd p[M][N][N], int m, int n, int value);

bdd limit_2(const bdd p[M][N][N], int m1, int value1, int m2, int value2);

bdd limit_3(const bdd p[M][N][N], const Neighbour neighbour, int m1, int value1, int m2, int value2);

bdd limit_4(const bdd p[M][N][N], int m1, int value1, int m2, int value2);

void limit_5(bdd &tree, const bdd p[M][N][N]);

bdd limit_6(const bdd p[M][N][N]);

int main() {
    var = std::make_unique<char[]>(N * M * LOG_N).get();

    //Инициализация
    bdd_init(10000000, 1000000);
    bdd_setvarnum(N * M * LOG_N);

    //вводим функцию p(k, i, j) следующим образом (кодируем p[k][i][j] в виде трехмерного вектора для масштабирования)
    bdd p[M][N][N];
    init(p);

    //Вводим ограничения
    bdd task = bddtrue;

    // ограничение 1:  Свойство m у объекта n = value
    task &= limit_1(p, 0, 8, 4);
    task &= limit_1(p, 0, 3, 1);
    task &= limit_1(p, 0, 1, 5);
    task &= limit_1(p, 2, 7, 3);
    task &= limit_1(p, 0, 5, 2);
    task &= limit_1(p, 0, 2, 6);

    // ограничение 2
    // Если у объекта св-во m1 = value1, то св-во m2 = value2 и наоборот
    task &= limit_2(p, 0, 2, 3, 2);
    task &= limit_2(p, 2, 5, 0, 0);
    task &= limit_2(p, 1, 0, 2, 4);
    // Доп
    task &= limit_2(p, 1, 6, 2, 6);
    task &= limit_2(p, 2, 5, 1, 5);
    task &= limit_2(p, 3, 8, 2, 7);
    task &= limit_2(p, 0, 5, 1, 4);
    task &= limit_2(p, 1, 3, 3, 7);
    task &= limit_2(p, 0, 3, 2, 0);
    task &= limit_2(p, 1, 2, 2, 3);

    // ограничение 3
    // Позиция 2-го относительно 1-го
    task &= limit_3(p, Neighbour::LEFT, 0, 6, 2, 1);
    task &= limit_3(p, Neighbour::LEFT, 1, 4, 2, 2);
    task &= limit_3(p, Neighbour::UPPERLEFT, 3, 6, 3, 8);
    task &= limit_3(p, Neighbour::UPPERLEFT, 0, 2, 3, 3);
    task &= limit_3(p, Neighbour::LEFT, 3, 6, 3, 5);
    // Доп
    task &= limit_3(p, Neighbour::UPPERLEFT, 2, 4, 3, 7);
//    task &= limit_3(p, Neighbour::UPPERLEFT, 0, 4, 3, 7);
//    task &= limit_3(p, Neighbour::UPPERLEFT, 3, 7, 3, 1);

    // ограничение 4
    // Если у объекта свойство m1 имеет значение value, то он располагается слева-сверху или слева от объекта, у которого св-во m2 = value2
    task &= limit_4(p, 1, 8, 1, 4);
    task &= limit_4(p, 2, 3, 3, 5);
    task &= limit_4(p, 3, 4, 0, 7); // по склейке
    task &= limit_4(p, 2, 0, 3, 1);

    // ограничение 5
    // У двух различных параметров значения свойств не совпадают
    limit_5(task, p);

    // ограничение 6
    // Параметры принимают значения только из заданных множеств (значение свойств меньше N)
    task &= limit_6(p);

    // Вывод
    out.open("out.txt");

    auto satcount = (int) bdd_satcount(task);

    out << satcount << " solutions:\n" << std::endl;
    std::cout << satcount << " solutions\n" << std::endl;

    if (satcount) {
        bdd_allsat(task, fun);
    }
    std::cout << satcount << " solutions\n" << std::endl;

    out.close();
    bdd_done(); //Завершение работы с библиотекой

    return 0;
}

void print() {
    for (unsigned i = 0; i < N; i++) {
        for (unsigned j = 0; j < M; j++) {
            int J = i * M * LOG_N + j * LOG_N;
            int num = 0;

            for (unsigned k = 0; k < LOG_N; k++) {
                num += (unsigned) (var[J + k] << k);
            }

            arr[i][j] = num;
        }
    }

    for (unsigned i = 0; i < N; i++) {
        std::cout << i << ": ";
        out << i << ": ";

        for (unsigned j = 0; j < M; j++) {
            std::cout << arr[i][j] << " ";
            out << arr[i][j] << " ";
        }

        std::cout << "\n";
        out << "\n";
    }
    std::cout << "\n";
    out << "\n";
}

void build(char *varset, unsigned n, unsigned I) {
    if (I == n - 1) {
        if (varset[I] >= 0) {
            var[I] = varset[I];
            print();
            return;
        }
        var[I] = 0;
        print();
        var[I] = 1;
        print();
        return;
    }
    if (varset[I] >= 0) {
        var[I] = varset[I];
        build(varset, n, I + 1);
        return;
    }
    var[I] = 0;
    build(varset, n, I + 1);
    var[I] = 1;
    build(varset, n, I + 1);
}

void fun(char *varset, int size)  //Для bdd_allsat
{
    build(varset, size, 0);
}

void init(bdd p[M][N][N]) {
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            for (int k = 0; k < N; ++k) {
                p[i][j][k] = bddtrue;
                for (int l = 0; l < LOG_N; ++l) {
                    p[i][j][k] &= ((k >> l) & 1) ? bdd_ithvar((j * LOG_N * M) + l + LOG_N * i)
                                                 : bdd_nithvar((j * LOG_N * M) + l + LOG_N * i);
                }
            }
        }
    }
}

bdd limit_1(const bdd p[M][N][N], int m, int n, int value) {
    return p[m][n][value];
}

bdd limit_2(const bdd p[M][N][N], int m1, int value1, int m2, int value2) {
    bdd tree = bddtrue;
    for (int i = 0; i < N; ++i) {
        tree &= !(p[m1][i][value1] ^ p[m2][i][value2]);
    }

    return tree;
}

bdd
limit_3(const bdd p[M][N][N], const Neighbour neighbour, int m1, int value1, int m2,
        int value2) {
    bdd tree = bddtrue;
    switch (neighbour) {
        case Neighbour::UPPERLEFT: {
            for (int i = ROW_LENGTH + 1; i < N; ++i) {
                if (i % ROW_LENGTH != 0) {
                    tree &= (!p[m1][i][value1] ^ p[m2][i - ROW_LENGTH - 1][value2]);
                }
            }
            break;
        }
        case Neighbour::LEFT: {
            for (int i = 1; i < N; ++i) {
                if (i % ROW_LENGTH != 0) {
                    tree &= (!p[m1][i][value1] ^ p[m2][i - 1][value2]);
                }
            }
            break;
        }
    }

    return tree;
}

bdd limit_4(const bdd p[M][N][N], int m1, int value1, int m2, int value2) {
    bdd tree = bddfalse;
    static const Neighbour arr[] = {Neighbour::UPPERLEFT, Neighbour::LEFT};
    std::vector<Neighbour> neighbours(arr, arr + sizeof(arr) / sizeof(arr[0]));

    for (Neighbour neighbour: neighbours) {
        tree |= limit_3(p, neighbour, m1, value1, m2, value2);
    }

    return tree;
}

void limit_5(bdd &tree, const bdd p[M][N][N]) {
    for (unsigned j = 0; j < N; j++) {
        for (unsigned i = 0; i < N - 1; i++) {
            for (unsigned k = i + 1; k < N; k++) {
                for (unsigned m = 0; m < M; m++) {
                    tree &= p[m][i][j] >> !p[m][k][j];
                }
            }
        }
    }
}

bdd limit_6(const bdd p[M][N][N]) {
    bdd tree = bddtrue;
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < M; ++k) {
            bdd temp0 = bddfalse;
            for (int j = 0; j < N; j++) {
                temp0 |= p[k][i][j];
            }
            tree &= temp0;
        }
    }

    return tree;
}
//*/