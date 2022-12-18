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
