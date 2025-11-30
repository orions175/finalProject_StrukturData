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
#include "include/json.hpp" // Pastikan path ini benar!

using json = nlohmann::json;
using namespace std;
namespace fs = std::filesystem;

// Constanta Variabel
const int MAX_NODES = 20; // Ini Estimasi Awal bisa dirubah, sekaligus mengestimasi
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
    int to;         // Node Tujuan
    int weight;     // Biaya ke kampus
    EdgeNode *next; // Tetangga Kampus Lainnya
};

UnivNode univ[MAX_NODES];     // Array daftar Universitas yang nanti dipakai
EdgeNode *adjHead[MAX_NODES]; // Array of Pointers (Head dari Linked List)
int numNodes = 0;

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
 * @brief Load Data dari JSON ke Adjacency LIst
 *
 * @param filename Nama File json berupa PATH
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
        adjHead[i] = nullptr; // Ngeset seluruh array adjHead menjadi Null Pointer sebagai awalan
    }

    // Load Nodes
    // NOTE - INDEX ID HARUS SUDAH URUT DI JSON!!
    numNodes = data["nodes"].size(); // Dengan Asumsi index ID itu terurut
    for (json &node : data["nodes"])
    {
        int id = node["id"];

        // Error Handling jika id lebih dari node yang diperbolehkan
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
    cout << pathStr;

    // dari Intersect ke node tujuan
    curr = parentEnd[intersectNodeIdx];
    while (curr != -1)
    {
        cout << " -> " << univ[curr].name;
        curr = parentEnd[curr];
    }
    cout << endl;
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

    cout << "Traversal jalur " << univ[startNode].name << " <---> " << univ[endNode].name << "..." << endl;

    // Selama Belum ada `break`
    while (true)
    {
        // Forward Step (dari START)
        // Index Node dari Starting Point
        int startNodeIdx = getMinDistNode(distStart, visitedStart, numNodes);

        // Error Handling kalau Index ga valid
        if (startNodeIdx != -1)
        {
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

                // FIXME Buat Debugging
                cout << "Traversal (Start) " << univ[curr->to].name << ", Distance:  " << distStart[v] << endl;

                curr = curr->next; // Geser ke tetangga berikutnya
            }
        }

        // Backward Traversal (Dari END)
        int endNodeIdx = getMinDistNode(distEnd, visitedEnd, numNodes);
        if (endNodeIdx != -1)
        {
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

                // FIXME Buat Debugging
                cout << "Traversal (End) " << univ[curr->to].name << ", Distance:  " << distEnd[e] << endl;

                curr = curr->next; // Geser ke tetangga berikutnya
            }
        }

        // Ngecek apabila semua node sudah di explorasi semua
        if (startNodeIdx == -1 && endNodeIdx == -1)
            break;

        // Cek titik temu terbaik
        int currentBest = INF;
        int currentIntersect = -1;

        // Scan semua node buat cari titik temu (Intersection Check)
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
            }
        }

        // Kondisi berhenti: Node u sudah dikunjungi dari kedua arah Ter Intersect
        if (visitedStart[startNodeIdx] && visitedEnd[startNodeIdx])
        {
            if (currentIntersect != -1)
            {
                intersectNode = currentIntersect;
                bestDist = currentBest;
                break;
            }
        }

        // Buat Debug
        cout << ">> Iterasi " << endl;
    }

    // Jika bertemu dengan node intersect
    if (intersectNode != -1)
    {
        cout << "Terintercept di : " << univ[intersectNode].name << endl;
        // Menampilkan seluruh
        printPath(intersectNode, startNode, endNode);
        cout << "Total Jarak: " << bestDist << " km" << endl;
    }
    // Kalau ga ketemu ya berarti ga connected graph
    else
    {
        cout << "Jalur buntu bro!" << endl;
    }
}

// TODO - Buat ulang menunya
int main()
{
    cout << "=== PENCARI JALAN KAMPUS JOGJA (Adjacency List Edition) ===\n"
         << endl;

    // Load Data Univ dari JSON
    loadData_json("universitas.json");

    // FIXME Buat Debugging
    for (int i = 0; i < numNodes; i++)
    {
        cout << ">>> Adjacency List : " << univ[i].name << endl;

        EdgeNode *curr = adjHead[i];

        cout << "Adjacency Listnyo: " << endl;
        while (curr != nullptr)
        {

            cout << univ[curr->to].name << "|" << curr->to << endl;
            curr = curr->next;
        }
    }

    int startIdx, endIdx;

    cout << "List Kampus:" << endl;
    for (int i = 0; i < numNodes; i++)
    {
        cout << i << ": " << univ[i].name << endl;
    }

    cout << "\nDari ID Kampus: ";
    cin >> startIdx;
    cout << "Ke ID Kampus: ";
    cin >> endIdx;

    if (startIdx >= 0 && startIdx < numNodes && endIdx >= 0 && endIdx < numNodes)
    {
        bidirectionalDijkstra(startIdx, endIdx);
    }
    else
    {
        cout << "ID tidak valid!" << endl;
    }

    system("pause");
    return 0;
}