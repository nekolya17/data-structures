#pragma once
#include <array>
#include <iostream>
#include <fstream>
#include <string>
#include <queue>

namespace algos{
// Huffman coding class
// proccesses files with Extended ASCII symbols
// returns table of symbols and codes as array(symbol - index, code - string)



class Huffman{
private:
    
    struct Node{    
        unsigned char symbol;  
        int leftInd = -1;        // Left child index in nodes_
        int rightInd = -1;       // Right child index in nodes_
        size_t frequency;
    };

    std::array<size_t, 256> frequency_;
    std::array<Node, 511> nodes_;
    int counter_ = 0;
    int root_ = -1;

    bool count_frequency(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if(!file.is_open()){
            std::cerr << "File opening error\n";
            return false;
        }
        
        frequency_.fill(0);
        char c;
        while(file.get(c)){
            frequency_[static_cast<unsigned char>(c)]++;
        }

        return true;
    }

    void build_tree(){
        auto cmp = [this](int a, int b){
            return this->nodes_[a].frequency > this->nodes_[b].frequency;
        };
        std::priority_queue<int, std::vector<int>, decltype(cmp)> heap(cmp);
        counter_ = 0;
        // Write leaves
        for(int i = 0; i < frequency_.size();i++){
            if(frequency_[i] == 0)
                continue;
            nodes_[counter_] = Node{static_cast<unsigned char>(i), -1, -1, frequency_[i]};

            heap.push(counter_);
            counter_++;
        }

        while(heap.size() > 1){
            int min1 = heap.top();
            heap.pop();
            int min2 = heap.top();
            heap.pop();
            nodes_[counter_] = Node{'0', min1, min2, nodes_[min1].frequency + nodes_[min2].frequency};
            heap.push(counter_);
            counter_++;
        }

        if(!heap.empty()){
            root_ = heap.top();
            heap.pop();
        }
        else{
            root_ = -1;
        }
    }

    void get_codes(std::string& cur, int node, std::array<std::string, 256>& table) {

        if (nodes_[node].leftInd == -1 && nodes_[node].rightInd == -1) {
            table[nodes_[node].symbol] = cur.empty() ? "0" : cur;
            return; 
        }

        if (nodes_[node].leftInd != -1) {
            cur.push_back('0');
            get_codes(cur, nodes_[node].leftInd, table);
            cur.pop_back();
        }

        if (nodes_[node].rightInd != -1) {
            cur.push_back('1');
            get_codes(cur, nodes_[node].rightInd, table); 
            cur.pop_back();
        }
    }

    
public:
    std::array <std::string, 256> get_huffman_codes(const std::string& path){
        if(!count_frequency(path))
            return {};
        std::array <std::string, 256> code_table{};
        build_tree();
        std::string start = "";
        if (root_ == -1) 
            return {};
        get_codes(start, root_, code_table);
        return code_table;
    }

};

}