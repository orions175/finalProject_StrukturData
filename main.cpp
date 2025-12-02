/**
 * @file main.cpp
 * @author Rio Meidi Ataurrahman (riomeidi29@gmail.com)
 * @brief Program untuk memenuhi tugas Final Projek Mata kuliah Struktur Data
 * @version 1.0
 * @date 2025-11-26
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <filesystem>
#include "include/json.hpp"

using json = nlohmann::json;
using namespace std;
namespace fs = std::filesystem;

// Constanta Variabel
const int MAX_NODES = 20; // Ini Estimasi Awal
const int INF = numeric_limits<int>::max();

// Struktur Info dari Kampus
struct UnivNode
{
    int id;
    string name;
};

/**
 * @brief Struktur Edge (Adjacency List) pada node kampus nanti
 *
 */
struct EdgeNode
{
    int to;         // [INDEX] Node Tujuan
    int weight;     // Biaya ke kampus
    EdgeNode *next; // Tetangga Kampus Lainnya
};

UnivNode univ[MAX_NODES];     // Array daftar Universitas yang nanti dipakai
EdgeNode *adjHead[MAX_NODES]; // Array Adjacency List
int numNodes = 0;             // Jumlah Node/Univ yang telah diinputkan

// Variabel untuk Algoritma Dijkstra
int distStart[MAX_NODES];     // Jarak dari start
int distEnd[MAX_NODES];       // Jarak dari end
int parentStart[MAX_NODES];   // Path tracking maju
int parentEnd[MAX_NODES];     // Path tracking mundur
bool visitedStart[MAX_NODES]; // Array Status yang sudah dikunjungi (depan)
bool visitedEnd[MAX_NODES];   // Array Status yang sudah dikunjungi (Mundur)

// Fungsi Utama

/**
 * @brief Fungsi Untuk Nambahin Edge ke Linked List (Adjacency List)
 *
 * @param s (Source) Index[id] Node asal
 * @param t (Target) Index[id] Node tujuan
 * @param w (Weight) Beban / Biaya ke node
 */
void addEdge(int s, int t, int w)
{
    EdgeNode *newNode = new EdgeNode;

    // Masukin Data ke Node
    newNode->to = t;
    newNode->weight = w;
    newNode->next = adjHead[s]; // Sambungin ke head lama

    // Ubah head menjadi yang paling terbaru
    adjHead[s] = newNode;
}

/**
 * @brief Mencari Jarak paling dekat dari node
 *
 * @param dist [Array] Jarak tiap edge
 * @param visited [Array] node sudah dikunjungi atau
 *               belum kalau sudah `True` kalau belum `False`
 * @param n Jumlah Edges yang mau di cari
 * @return `int` Index node `adjHead[]` yang paling dekat dari starting point
 */
int getMinDistNode(const int dist[], const bool visited[], const int n)
{
    // Init Awal
    int minVal = INF;
    int minIndex = -1;

    // Loop Sejumlah Node
    for (int i = 0; i < n; i++)
    {
        // Kalau belum dikunjungi(False) DAN jaraknya kurang dari jarak minVal
        if (!visited[i] && dist[i] < minVal)
        {
            minVal = dist[i];
            minIndex = i;
        }
    }

    // Outputkan Index node paling minimum itu
    return minIndex;
}

/**
 * @brief Load Data dari JSON ke Adjacency LIst sekaligus Ngerefresh data
 *
 * @param filename Nama File json BUKAN path
 */
void loadData_json(string filename)
{
    // Path Searching
    fs::path mainPath = __FILE__;                 // Ngambil Path file cpp ini
    fs::path cwd = mainPath.parent_path();        // ngambil parent path dari Working Directory ini
    fs::path full_path = cwd / "json" / filename; // Ngambil File Json itu yang digabgung dengan full Path

    // Mbukak File JSON
    ifstream file(full_path);
    // Error Handling kalau misal nanti file ternyata ga kebuka atau apalah itu
    if (!file.is_open())
    {
        cerr << "Error: Gagal buka file JSON, Pastikan [" << filename << "] ada." << endl;
        exit(1);
    }

    // Ngeparse file kedalam Object data json
    json data = json::parse(file);

    // Reset Head Pointers
    for (int i = 0; i < MAX_NODES; i++)
    {
        // Hapus memori lama
        EdgeNode *curr = adjHead[i];
        while (curr != nullptr)
        {
            EdgeNode *oldEgdes = curr;
            curr = curr->next;
            delete oldEgdes;
        }
        // Ngeset seluruh array adjHead menjadi Null Pointer sebagai awalan
        adjHead[i] = nullptr;
    }

    // Load Nodes
    // NOTE - INDEX ID HARUS SUDAH URUT DI JSON!!, WAJIB TOLONG PASTIKAN LAGI
    // WAJIB PERIKSA LAGI ID JSON
    numNodes = data["nodes"].size(); // Dengan Asumsi index ID itu terurut
    for (json &node : data["nodes"])
    {
        int id = node["id"];

        // Additional check jika id lebih dari node yang diperbolehkan
        if (id < MAX_NODES)
        {
            univ[id].id = id;
            univ[id].name = node["name"];
        }
    }

    // Load Edges dari objek Data Masukkan ke Adjacency List
    for (json &edge : data["edges"])
    {
        int s = edge["source"];
        int t = edge["target"];
        int w = edge["weight"];

        // Graph Undirect jadi buat 2 jalur dengan Weight yang sama
        addEdge(s, t, w);
        addEdge(t, s, w);
    }

    cout << "Data di-inject ke Linked List, Total Kampus: " << numNodes << endl;
}

/**
 * @brief Buat Menyimpan data ke persistence JSON
 *
 * @param filename nama file BUKAN path
 * @param data objek data yang akan dimasukkan atau disimpan
 */
void saveData_json(string filename, const json &data)
{
    fs::path mainPath = __FILE__;
    fs::path cwd = mainPath.parent_path();
    fs::path full_path = cwd / "json" / filename;

    ofstream file(full_path);
    if (!file.is_open())
    {
        cerr << "Error: Gagal menyimpan file JSON!" << endl;
        return;
    }

    file << data.dump(3);
    cout << "Data berhasil disimpan ke " << filename << endl;
}

/**
 * @brief Menambah node univ dan merefresh node runtime
 *
 * @param newUnivName nama univ baru [singkatan]
 * @param filename nama file JSON BUKAN Path
 */
void addNodeAndRefresh(const string &newUnivName, string filename)
{
    cout << "\n=== TAMBAH NODE BARU DAN REFRESH ===\n"
         << endl;

    fs::path mainPath = __FILE__;
    fs::path cwd = mainPath.parent_path();
    fs::path full_path = cwd / "json" / filename;

    ifstream inFile(full_path);
    if (!inFile.is_open())
    {
        cerr << "Error: Gagal membaca file JSON untuk penambahan." << endl;
        return;
    }

    json data = json::parse(inFile); // Parsing file JSON mentah ke objek data
    inFile.close();                  // Close Stream File

    int newId = data["nodes"].size();
    if (newId >= MAX_NODES)
    {
        cerr << "Error: MAX_NODES (" << MAX_NODES << ") sudah tercapai. Gagal menambah node." << endl;
        return;
    }

    // Menambahkan Node kedalam data
    json newNode = {
        {"id", newId},
        {"name", newUnivName}};
    data["nodes"].push_back(newNode);
    cout << "Node " << newUnivName << " berhasil ditambahkan dengan ID: " << newId << "." << endl;

    // Menyimpan Langsung ke file JSON
    saveData_json(filename, data);

    cout << "\nMerefresh struktur graf di memori..." << endl;
    loadData_json(filename);

    cout << "Refresh Selesai. Total Kampus baru: " << numNodes << endl;
}

/**
 * @brief Menampilkan hasil perhitungan dijkstra bidirect
 *
 * @param intersectNodeIdx [index] index node intersect
 * @param startNodeIdx [index] index node starting point
 * @param endNodeIdx [index] index node end point / walau ga kepake
 */
void printPath(const int intersectNodeIdx, const int startNodeIdx, const int endNodeIdx)
{
    cout << "\n=== HASIL SHORTEST PATH ===" << endl;

    // Jalur Maju (Start -> Intersect)
    int curr = intersectNodeIdx;
    string pathStr = "";

    // Backtracing dari Start
    while (curr != -1)
    {
        pathStr = univ[curr].name + (curr == intersectNodeIdx ? "" : " -> ") + pathStr;
        curr = parentStart[curr];
    }
    cout << "\033[34m";
    cout << pathStr;

    // dari Intersect ke node tujuan
    curr = parentEnd[intersectNodeIdx];
    while (curr != -1)
    {
        cout << " -> " << univ[curr].name;
        curr = parentEnd[curr];
    }
    cout << "\033[0m" << endl;
}

/**
 * @brief Algoritma Utama untuk menentukan Shortest path menuju kampus tujuan dengan pendekatan Bi Directional
 *
 * @param startNode [Index] `Id` kampus/ index kampus asal
 * @param endNode [Index] `Id` Kampus / Index kampus yang dituju
 */
void bidirectionalDijkstra(int startNode, int endNode)
{
    // Sesuai Konsep ngeset seluruh node sebagai belum dikunjungi dan jarak tertingi
    for (int i = 0; i < numNodes; i++)
    {
        distStart[i] = INF;
        distEnd[i] = INF;
        visitedStart[i] = false;
        visitedEnd[i] = false;
        parentStart[i] = -1;
        parentEnd[i] = -1;
    }

    // Inisialisasi awal pada starting point
    distStart[startNode] = 0; // Node Awal
    distEnd[endNode] = 0;     // Node Tujuan

    int intersectNode = -1; // Buat belum ada titik Temu
    int bestDist = INF;
    int loopCounter = 0;
    cout << "Traversal jalur " << univ[startNode].name << " <---> " << univ[endNode].name << "..." << endl;
    cout << "========================" << endl;

    // Selama Belum ada `break`
    while (true)
    {
        loopCounter++;
        // Forward Traverse (dari START)
        // Index Node dari Starting Point
        int startNodeIdx = getMinDistNode(distStart, visitedStart, numNodes);

        // Error Handling kalau Index ga valid
        if (startNodeIdx != -1)
        {
            // REVIEW - Buat Debugging
            cout << "\033[32mStart Pos : \033[0m" << univ[startNodeIdx].name << endl;
            visitedStart[startNodeIdx] = true;

            // Traversal Linked List, dari Adjacency paling baru
            EdgeNode *curr = adjHead[startNodeIdx];
            while (curr != nullptr)
            {
                int v = curr->to;          // Index kampus arah edges
                int weight = curr->weight; // beban Edges kearah kampus

                if (!visitedStart[v] && distStart[startNodeIdx] + weight < distStart[v])
                {
                    distStart[v] = distStart[startNodeIdx] + weight;
                    parentStart[v] = startNodeIdx;
                }
                // REVIEW - Buat Debugging

                string univStart = visitedStart[curr->to] ? "\033[31m" + univ[curr->to].name + "\033[0m" : univ[curr->to].name;
                cout << "Traversal (Start) " << univStart << ", Distance:  " << distStart[v] << endl;

                curr = curr->next; // Geser ke tetangga berikutnya
            }
        }

        // Backward Traversal (Dari END)
        int endNodeIdx = getMinDistNode(distEnd, visitedEnd, numNodes);
        if (endNodeIdx != -1)
        {
            // REVIEW Buat Debugging
            cout << "\033[31mEnd Pos : \033[0m" << univ[endNodeIdx].name << endl;

            visitedEnd[endNodeIdx] = true;

            // Traversal Linked List, dari Adjacency paling baru
            EdgeNode *curr = adjHead[endNodeIdx];
            while (curr != nullptr)
            {
                int e = curr->to;          // Index kampus arah edges
                int weight = curr->weight; // beban Edges kearah kampus

                if (!visitedEnd[e] && distEnd[endNodeIdx] + weight < distEnd[e])
                {
                    distEnd[e] = distEnd[endNodeIdx] + weight;
                    parentEnd[e] = endNodeIdx;
                }
                // REVIEW Buat Debugging
                string univEnd = visitedEnd[curr->to] ? "\033[31m" + univ[curr->to].name + "\033[0m" : univ[curr->to].name;
                cout << "Traversal (End) " << univEnd << ", Distance:  " << distEnd[e] << endl;

                curr = curr->next; // Geser ke tetangga berikutnya
            }
        }

        // Ngecek apabila semua node sudah di explorasi semua
        if (startNodeIdx == -1 && endNodeIdx == -1)
            break;

        // Cek titik temu terbaik
        // NOTE - Inisialisasi awal jika tidak berubah berarti ada yang salah
        int currentBest = INF;
        int currentIntersect = -1;

        cout << ">> Posible Intersection : ";
        // Traverse semua node buat cari titik temu (Intersection Check)
        for (int i = 0; i < numNodes; i++)
        {
            if (distStart[i] != INF && distEnd[i] != INF)
            {
                int total = distStart[i] + distEnd[i];
                if (total < currentBest)
                {
                    currentBest = total;
                    currentIntersect = i;
                }
                cout << "\n> Distance Check [" << univ[i].name << "] : " << distStart[i] << " ; " << distEnd[i];
            }
        }
        cout << endl
             << endl;

        // Kondisi Final stop, Node u sudah dikunjungi dari kedua arah Ter Intersect
        if (visitedStart[startNodeIdx] && visitedEnd[startNodeIdx])
        {
            if (currentIntersect != -1)
            {
                intersectNode = currentIntersect;
                bestDist = currentBest;
                // REVIEW - Buat Debug
                cout << "\n>> Iterasi Terakhir " << "Current Best: " << currentBest << " Intersect Loc: " << currentIntersect << endl
                     << endl;
                break;
            }
        }
        // REVIEW - Buat Debug
        cout << "\033[33;5m>> Iterasi [" << loopCounter << "]\033[0m,  Current Best: " << currentBest << " Intersect Loc: " << currentIntersect << endl
             << endl;
    }

    // Jika bertemu dengan node intersect
    if (intersectNode != -1)
    {
        cout << ">> Intersaction di : " << univ[intersectNode].name << endl;
        // Menampilkan seluruh Node yang sudah di traversal
        printPath(intersectNode, startNode, endNode);
        cout << "Total Jarak: " << bestDist << " km" << endl;
    }
    // Kalau ga ketemu ya berarti ga connected graph
    else
    {
        cout << "Jalur buntu bro!" << endl;
    }
}

int main()
{
    system("cls");
    cout << "\033[7m=== PENCARI JALAN KAMPUS JOGJA (Dijkstra Bidirectional) ===\033[0m" << endl;

    // Nama file json BUKAN pathnya
    string jsonFile = "universitas.json";
    // Load Data Univ dari JSON
    loadData_json(jsonFile);

    int startIdx, endIdx;
    short opt;

    do
    {
        cout << "\n\033[4m<--- MENU UTAMA --->\033[0m" << endl;
        cout << "1: Cari Jalur Terpendek" << endl;
        cout << "2: Tampilkan Adjacency List (Debug)" << endl;
        cout << "3: Tambah Node Kampus Baru" << endl;
        cout << "4: Refresh" << endl;
        cout << "\033[31m5: Keluar\033[0m" << endl;
        cout << "Pilihan: ";

        // Error Handling
        if (!(cin >> opt))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            opt = 0;
            cout << "Input tidak valid. Coba lagi." << endl;
            continue;
        }

        switch (opt)
        {
        case 1:
        {
            system("cls");
            cout << "List Kampus:" << endl;
            for (int i = 0; i < numNodes; i++)
            {
                cout << i << ": " << univ[i].name << endl;
            }
            cout << "\nDari ID Kampus: ";
            cin >> startIdx;
            cout << "Ke ID Kampus: ";
            cin >> endIdx;

            // Error Handling User Input
            if (startIdx >= 0 && startIdx < numNodes && endIdx >= 0 && endIdx < numNodes)
            {
                bidirectionalDijkstra(startIdx, endIdx);
            }
            else
            {
                cout << "ID tidak valid!" << endl;
            }
        }
        break;

        case 2:
        {
            system("cls");
            // Menampilkan Seluruh Adjacency List
            for (int i = 0; i < numNodes; i++)
            {
                cout << "\n>>> Adjacency List : " << univ[i].name << endl;
                EdgeNode *curr = adjHead[i];
                cout << "Tetangga: ";
                if (curr == nullptr)
                    cout << "Tidak ada";
                while (curr != nullptr)
                {
                    cout << "[" << univ[curr->to].name << "|W:" << curr->weight << "] ";
                    curr = curr->next;
                }
                cout << endl;
            }
        }
        break;

        case 3:
        {
            system("cls");
            string newUnivName;
            cout << "Masukkan Nama Kampus Baru (Singkatan ex: UTIYI ): ";
            cin >> newUnivName;

            // Tambah Node Univ
            addNodeAndRefresh(newUnivName, jsonFile);

            // Menambahkan Edges satu persatu BUKAN yang akan di pass ke fungsi
            int newId = numNodes - 1;

            cout << "\n--- Tambahkan Edges / Jalan untuk " << newUnivName << " (ID " << newId << ") ---" << endl;
            cout << "List Univ Lama:" << endl;
            for (int i = 0; i < newId; i++)
            {
                cout << i << ": " << univ[i].name << endl;
            }

            while (true)
            {
                int targetId, weight;
                cout << "  Target ID (Lama) / -1 untuk selesai: ";
                if (!(cin >> targetId) || targetId == -1)
                    break;

                // Cek ID, harus kurang dari ID baru
                if (targetId >= newId || targetId < 0)
                {
                    cout << "Id target tidak valid. Ulangi!" << endl;
                    continue;
                }

                cout << "  Jarak (Weight) ke " << univ[targetId].name << ": ";
                if (!(cin >> weight))
                    break;

                fs::path mainPath = __FILE__;
                fs::path cwd = mainPath.parent_path();
                fs::path full_path = cwd / "json" / jsonFile;

                ifstream inFile(full_path);
                json data = json::parse(inFile);
                inFile.close();

                // Penambahan Edges kedalam data
                json newEdge = {
                    {"source", newId},
                    {"target", targetId},
                    {"weight", weight}};
                data["edges"].push_back(newEdge);

                saveData_json(jsonFile, data); // Simpan
                loadData_json(jsonFile);       // Refresh di Runtime
                cout << "-> Koneksi " << newUnivName << " <---> " << univ[targetId].name << " berhasil ditambahkan dan di-refresh." << endl;
            }
        }
        break;

        case 4:
        {
            system("cls");
            cout << "\n>> Refresh File from JSON" << jsonFile << endl;
            loadData_json(jsonFile);
        }
        break;

        case 5:
        {
            cout << "Terima kasih sudah menggunakan program ini!!, Sampai Jumpa lagi!!" << endl;
        }
        break;

        default:
        {
            system("cls");
            cout << "Apa yang anda Mau sebenarnya?" << endl;
        }
        break;
        }
    } while (opt != 5);

    // REVIEW - Buat Debugging
    // system("pause");
    return 0;
}