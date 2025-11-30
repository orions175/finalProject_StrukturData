# Diagram JSON Kampus

```mermaid

%% Index Sesuai Nama Kampus
graph TD
    subgraph Kampus di DIY
        0("(0) UGM")
        1("(1) UNY")
        2("(2) UPN")
        3("(3) UII")
        4("(4) UAD")
        5("(5) USD")
        6("(6) UMY")
        7("(7) UAJY")
        8("(8) ISI")
        9("(9) UIN")
        10("(10) UTY")
    end

    0 -- 2 --- 1
    1 -- 4 --- 2
    0 -- 3 --- 9
    9 -- 6 --- 4
    1 -- 2 --- 5
    5 -- 3 --- 7
    7 -- 3 --- 2
    6 -- 10 --- 0
    4 -- 5 --- 8
    6 -- 8 --- 8
    2 -- 5 --- 3
    10 -- 7 --- 0

```

## JSON File

isi File JSON pada Diagram diatas

```json
}
"nodes": [
    { "id": 0, "name": "UGM" },
    { "id": 1, "name": "UNY" },
    { "id": 2, "name": "UPN" },
    { "id": 3, "name": "UII" },
    { "id": 4, "name": "UAD" },
    { "id": 5, "name": "USD" },
    { "id": 6, "name": "UMY" },
    { "id": 7, "name": "UAJY" },
    { "id": 8, "name": "ISI" },
    { "id": 9, "name": "UIN" },
    { "id": 10, "name": "UTY" }
],
"edges": [
    { "source": 0, "target": 1, "weight": 2 },
    { "source": 1, "target": 2, "weight": 4 },
    { "source": 0, "target": 9, "weight": 3 },
    { "source": 9, "target": 4, "weight": 6 },
    { "source": 1, "target": 5, "weight": 2 },
    { "source": 5, "target": 7, "weight": 3 },
    { "source": 7, "target": 2, "weight": 3 },
    { "source": 6, "target": 0, "weight": 10 },
    { "source": 4, "target": 8, "weight": 5 },
    { "source": 6, "target": 8, "weight": 8 },
    { "source": 2, "target": 3, "weight": 5 },
    { "source": 10, "target": 0, "weight": 7 }
]
}
```
