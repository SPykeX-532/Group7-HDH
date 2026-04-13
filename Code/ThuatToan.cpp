#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <chrono>

using namespace std;
using namespace std::chrono;

void ReadCSV(string file_path, vector<int>& BlockSize, vector<int>& ProcessSize) {
    ifstream file(file_path);
    if (!file) {
        cout << "Không thể mở file!" << endl;
        return;
    }

    string block, process;
    getline(file, block);

    while (getline(file, block, ';') && getline(file, process)) {
        BlockSize.push_back(stoi(block));
        ProcessSize.push_back(stoi(process));
    }

    file.close();
}

void PrintResult(vector<int>& allocation, vector<int> ProcessSize, vector<int> BlockSize) {
    for (int i = 0; i < allocation.size(); i++) {
        cout << "Process " << i + 1 << " (Size=" << ProcessSize[i] << ")   --->   ";
        if (allocation[i] != -1) {
            cout << "Block " << allocation[i] + 1 << " (Size=" << BlockSize[allocation[i]] << ")";
        }
        else {
            cout << "Not Allocated";
        }
        cout << endl;
    }
}

void ExportCSV(string filename, string algoName, long long times, vector<int>& allocation, vector<int>& ProcessSize, vector<int>& BlockSize) {

    ofstream out(filename, ios::app);

    if (out.tellp() == 0) {
        out << "Algorithm;Process;ProcessSize;Block;BlockSize\n";
    }

    for (int i = 0; i < allocation.size(); i++) {
        out << algoName << ";" << i + 1 << ";" << ProcessSize[i] << ";";
        if (allocation[i] != -1) {
            out << allocation[i] + 1 << ";" << BlockSize[allocation[i]];
        }
        else {
            out << "Not Allocated;0";
        }
        out << "\n";
    }

    out << "Time: " << times << " us" << endl;
    out.close();
}

vector<int> FirstFit(vector<int> blockSize, vector<int> processSize) {
    vector<int> allocation(processSize.size(), -1);

    for (int i = 0; i < processSize.size(); i++) {
        for (int j = 0; j < blockSize.size(); j++) {
            if (blockSize[j] >= processSize[i]) {
                allocation[i] = j;
                blockSize[j] -= processSize[i];
                break;
            }
        }
    }

    return allocation;
}

vector<int> BestFit(vector<int> BlockSize, vector<int> ProcessSize) {
    vector<int> allocation(ProcessSize.size(), -1);

    multiset<pair<int, int>> ms;

    for (int i = 0; i < BlockSize.size(); i++) {
        ms.insert({ BlockSize[i], i });
    }

    for (int i = 0; i < ProcessSize.size(); i++) {
        int p = ProcessSize[i];

        auto it = ms.lower_bound({ p, -1 });

        if (it != ms.end()) {
            int blockSize = it->first;
            int index = it->second;

            allocation[i] = index;

            ms.erase(it);

            int remain = blockSize - p;
            if (remain > 0) {
                ms.insert({ remain, index });
            }
        }
    }

    return allocation;
}

vector<int> WorstFit(vector<int> BlockSize, vector<int> ProcessSize) {
    vector<int> allocation(ProcessSize.size(), -1);

    priority_queue<pair<int, int>> heap;

    for (int i = 0; i < BlockSize.size(); i++) {
        heap.push({ BlockSize[i], i });
    }

    for (int i = 0; i < ProcessSize.size(); i++) {
        if (heap.empty()) break;

        auto top = heap.top();
        heap.pop();

        int largest = top.first;
        int index = top.second;

        if (largest >= ProcessSize[i]) {
            allocation[i] = index;

            int remain = largest - ProcessSize[i];
            if (remain > 0) {
                heap.push({ remain, index });
            }
        }
        else {
            heap.push(top);
            allocation[i] = -1;
        }
    }

    return allocation;
}
int main(int argc, char* argv[]) {
    //Input từ GUI Python 
    string file_path = argv[1];
    bool ff = stoi(argv[2]);
    bool bf = stoi(argv[3]);
    bool wf = stoi(argv[4]);

    //Biến dùng xử lí
    vector<int> BlockSize, ProcessSize, Allocation;

    //Đọc file CSV
    ReadCSV(file_path, BlockSize, ProcessSize);

    //In dữ liệu từ CSV
    cout << ">>====================-----INPUT DATA-----====================<<" << endl;
    cout << "#                    Process Size                    Block Size" << endl;
    for (int i = 0; i < BlockSize.size(); i++) {
        cout << i + 1 << "                        " << ProcessSize[i] << "                             " << BlockSize[i] << endl;
    }
    cout << "Marker" << endl;

    //FirstFit
    if (ff) {
        //Chạy thuật toán
        auto Start = high_resolution_clock::now();
        Allocation = FirstFit(BlockSize, ProcessSize);
        auto End = high_resolution_clock::now();

        //In ra
        cout << ">>====================-----FIRST FIT-----====================<<" << endl;
        PrintResult(Allocation, ProcessSize, BlockSize);
        cout << "Time: " << duration_cast<microseconds>(End - Start).count() << " us" << endl;

        //Ghi ra file
        ExportCSV("Output.csv", "FirstFit", duration_cast<microseconds>(End - Start).count(), Allocation, ProcessSize, BlockSize);
    }

    //BestFit
    if (bf) {
        //Chạy thuật toán
        auto Start = high_resolution_clock::now();
        Allocation = BestFit(BlockSize, ProcessSize);
        auto End = high_resolution_clock::now();

        //In ra
        cout << ">>====================-----BEST FIT-----=====================<<" << endl;
        PrintResult(Allocation, ProcessSize, BlockSize);
        cout << "Time: " << duration_cast<microseconds>(End - Start).count() << " us" << endl;

        //Ghi ra file
        ExportCSV("Output.csv", "BestFit", duration_cast<microseconds>(End - Start).count(), Allocation, ProcessSize, BlockSize);
    }

    //WorstFit
    if (wf) {
        //Chạy thuật toán
        auto Start = high_resolution_clock::now();
        Allocation = WorstFit(BlockSize, ProcessSize);
        auto End = high_resolution_clock::now();

        //In ra
        cout << ">>====================-----WORST FIT-----====================<<" << endl;
        PrintResult(Allocation, ProcessSize, BlockSize);
        cout << "Time: " << duration_cast<microseconds>(End - Start).count() << " us" << endl;

        //Ghi ra file
        ExportCSV("Output.csv", "WorstFit", duration_cast<microseconds>(End - Start).count(), Allocation, ProcessSize, BlockSize);
    }

    return 0;
}




