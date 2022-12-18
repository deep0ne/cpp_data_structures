/*
--- ПРИНЦИП РАБОТЫ ---
В качестве контейнера для реализации хеш-таблицы я выбрал вектор, в котором лежат односвязные списки с парами ключ-значение.
Коллизии разрешаются методом цепочек, поэтому я и выбрал вектор с односвязными списками.

* ВСТАВКА
Вычисляется номер корзины, в которую мы хотим положить пару ключ-значение. В корзине лежит односвязный список, по которому
Нужно пройтись и заменить значение на новое, если ключ уже есть в нашей таблице. В противном случае, мы кладем пару в голову
Односвязного списка.
* ПОЛУЧЕНИЕ
Так же вычисляется номер корзины, в корзине идём по односвязному списку, если встречается ключ, возвращаем значение.
* УДАЛЕНИЕ
Вычисляем номер корзины и запоминаем итератор, который понадобится нам при удалении. Если встречаем ключ, выводим
Значение. По итератору лежит нода, у которой next-> это наш элемент. Поэтому с помощью erase_after удаляем с нужной позиции.

--- ДОКАЗАТЕЛЬСТВО КОРРЕКТНОСТИ ---
1. Для одного и того же ключа будет возвращаться одинаковый номер корзины.
2. Номер корзины вычисляется быстро и эффективно
3. Ключ всегда находится в диапазоне от 0 до М (кол-во корзин)
4. Односвязные списки решают проблему коллизий и мы не теряем элементы

--- ВРЕМЕННАЯ СЛОЖНОСТЬ ---

* Вычисление номера корзины - О(1)
* Вставка - Вставка в голову списка - О(1), но чтобы проверить, что этот элемент не лежит в списке, нужно пройтись по нему, и при наличии ключа
обновить значение. Поэтому в худшем случае - О(n)
* Получение - Если элемент лежит в голове списка - О(1), в худшем случае О(n)
* Удаление - Если элемент лежит в голове списка - О(1), в худшем случае O(n)

--- ПРОСТРАНСТВЕННАЯ СЛОЖНОСТЬ --- 
Храним элементы, поступающие на вход в односвязных списках - О(n)

*/



#include <vector>
#include <algorithm>
#include <forward_list>
#include <iostream>
#include <cmath>

int P = 16;
unsigned int MAGIC = 2654435769;
unsigned int APLHA = std::pow(2, 32);

class HashMap {
public:

    HashMap() {
        buckets_ = std::pow(2, P);
        hashmap_.resize(buckets_);
    }

    void put(int key, int value) {
        int num_of_bucket = get_bucket(key);
        auto& bucket = hashmap_[num_of_bucket];
        for (auto& kv_pair: bucket) {
            if (kv_pair.first == key) {
                kv_pair.second = value;
                return;
            }
        }
        hashmap_[num_of_bucket].push_front({key, value});
    }

    int get(int key) {
        int num_of_bucket = get_bucket(key);
        auto bucket = hashmap_[num_of_bucket];
        for (auto& kv_pair: bucket) {
            if(kv_pair.first == key) {
                return kv_pair.second;
            }
        }
        return -1;
    }

    void remove(int key) {
        int num_of_bucket = get_bucket(key);
        auto& bucket = hashmap_[num_of_bucket];
        auto prev = bucket.before_begin();
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if((*it).first == key) {
                std::cout << (*it).second << std::endl;
                bucket.erase_after(prev);
                return;
            }
            prev = it;
        }
    	std::cout << "None" << std::endl;
        
    }

    int get_bucket(int key) {
        int bucket = (get_hash(key) * MAGIC % APLHA) >> (32 - P);
        return bucket;
    }

    // Не будем выкидывать хеширование, а введём специальную тождественную хеш-функцию, 
    // Которая будет возвращать свой аргумент неизменным: hash(k)=k

    int get_hash(int key) {
        return key;
    }


private:
    std::vector<std::forward_list<std::pair<int, int>>> hashmap_;
    int buckets_;
};

int main() {
    HashMap hashmap;
    int requests;

    std::cin >> requests;
    for (int i = 0; i < requests; ++i) {
        std::string operation;
        std::cin >> operation;
        if (operation == "get") {
            int value;
            std::cin >> value;
            int result = hashmap.get(value);
            result == -1 ? std::cout << "None" : std::cout << result;
            std::cout << std::endl;
        }
        if (operation == "put") {
            int key, value;
            std::cin >> key >> value;
            hashmap.put(key, value);
        }
        if (operation == "delete") {
            int value;
            std::cin >> value;
            hashmap.remove(value);
        }
    }
}
