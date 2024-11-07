#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <limits>


// Средние значение элементов кластера
double meanCluster(const std::vector<double>& cluster) {
    double sum = 0; // 10 байт
    int sizeCluster = cluster.size(); // 8 байт
    for (int i = 0; i < sizeCluster; ++i) {
        sum += cluster[i];
    }
    return sum / sizeCluster;
}


// Метрика для кластера
double metricCluster(const std::vector<double>& cluster) {
    int sizeCluster = cluster.size(); // 8 байт
    double sum = 0.0; // 10 байт
    double mean = meanCluster(cluster); // 10 байт
    for (int i = 0; i < sizeCluster; ++i){
        sum += fabs(cluster[i] - mean);
    }
    return sum;
}


// Получение оптимального набора кластеров из массива по заданной метрике
// index - элемент, который мы рассматриваем
// array - заданный массив
// curClusters - кластер, который получился на данной итерации
// optimalClusters - ответ, самый оптимальный кластер на данной итерации
// minMetric - метрика для оптимального кластера на данный момент
void getOptimalClusters(int index, const std::vector<double>& array, std::vector<std::vector<double>> curClusters,
std::vector<std::vector<double>>& optimalClusters, double& minMetric) {
    int sizeArray = array.size(); // Кол-во эл-ов массива 4 байта
    int numClusters = curClusters.size(); // Кол-во кластеров 4 байта

    if (index == sizeArray) { // Если рассмотрели все эл-ы
        // Проверка, чтобы все кластеры были не пустые
        bool anyNoneEmpty = std::any_of(curClusters.begin(), curClusters.end(),
        [](std::vector<double> value) -> bool { // Лямбда-функция, чтоб весело было)
            return value.size() == 0;
        });

        if (anyNoneEmpty) { // Если хотя бы один пустой - не подходит
            return;
        }

        // Общая метрика для кластеров
        double metricSum = 0.0; // 10 байт
        for (int i = 0; i < numClusters; ++i) {
            metricSum += metricCluster(curClusters[i]);
        }

        // Замена на лучший кластер
        if (metricSum < minMetric){
            minMetric = metricSum;
            optimalClusters = curClusters;
        }
        return;
    }

    // Проверка на пустоту предыд кластера
    // Если он пустой - не имеет смысла
    // заполнять след, так как порядок
    // не имеет смысла
    bool previousClusterWasEmpty = false; // 1 байт
    for (int i = 0; i < numClusters; ++i){
        if (previousClusterWasEmpty) {
            break;
        }

        // Рекурсивно смотрим последующий варианты
        curClusters[i].push_back(array[index]);
        getOptimalClusters(index + 1, array, curClusters, optimalClusters, minMetric);
        curClusters[i].pop_back();

        // Для след сохраняем состояние кластера
        // Если он пустой - true, иначе - false
        previousClusterWasEmpty = curClusters[i].size() == 0;
    }
}


void test(int numClusters, int sizeArray)
{
    std::random_device rd;
    std::mt19937 seed(rd());

    std::vector<double> array(sizeArray); // 8 * N байт

    for (int i = 0; i < sizeArray; ++i) array[i] = rd() % 999999;

    // Вывод массива
    for (int i : array) std::cout << i << " ";

    // Разбиения на кластеры
    std::vector<std::vector<double>> clusters(numClusters, std::vector<double>()); // 8 * 5 * N = 40 * N байт
    // Метрика для кластеров
    double metricClusters = std::numeric_limits<double>::max(); // 10 байт

    getOptimalClusters(0, array, clusters, clusters, metricClusters); // O(4.5^N) т. к. в среднем примерно 4.5 вызова функции рекурсивно

    // Вывод разбиения на кластеры
    std::cout << "\n| ";
    for (std::vector<double> i : clusters){
        for (double j : i){
            std::cout << j << " ";
        }
        std::cout << "| ";  
    }

    // Вывод метрики
    std::cout << "\nМетрика: " << metricClusters;
}


int main() {
    auto start = std::chrono::high_resolution_clock::now();

    test(5, 11); // 5 - кол-во кластеров
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "\nВремя выполнения программы: " << duration.count() << " секунд" << "\n";
    return 0;
}