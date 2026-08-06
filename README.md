# C++ Minesweeper (踩地雷) OOP 練習套件

歡迎來到 **C++ 踩地雷物件導向設計 (OOP) 實戰練習套件**！

本專案是一個特別為學習 C++ 物件導向程式設計而設計的互動式練習環境。你將透過實作「格子 (Cell)」與「棋盤 (Board)」的底層物件模型，來完成一個完整的、可在瀏覽器中遊玩的踩地雷遊戲！

專案內建了**單元測試自動回饋面板**與**互動式遊戲畫面**，無須在本地配置複雜的 C++ 圖形介面庫，透過 Codespaces 的 Port Forwarding 即可在瀏覽器中直接執行、測試與遊玩！

---

## 📂 目錄結構

```text
oop-minesweeper-cpp/
├── .devcontainer/          # Codespaces 開發容器環境設定
├── practice/               # 【學生練習區】你主要的實作範圍！
│   ├── include/            # 標頭檔 (Cell.h, Board.h, TestFramework.h)
│   ├── src/
│   │   ├── Cell.cpp        # 步驟引導與空實作（需填空）
│   │   ├── Board.cpp       # 步驟引導與空實作（需填空）
│   │   ├── adapter.cpp     # Web API 後端轉接器（已完成，請勿修改）
│   │   └── tests.cpp       # 單元測試套件（已完成，請勿修改）
│   ├── CMakeLists.txt      # 練習區 CMake 建置設定
│   ├── index.html          # 遊戲前端畫面 HTML（已完成）
│   ├── app.js              # 遊戲前端行為 JS（已完成）
│   └── style.css           # 遊戲前端樣式 CSS（已完成）
├── answer/                 # 【標準答案參考區】包含完整的實作，卡關時可參考
│   ├── ...                 # 結構與練習區完全相同，但 Cell.cpp / Board.cpp 具有完整實作
└── README.md               # 本導引說明文件
```

---

## 🛠️ 開發與測試開發流程 (The Dev Loop)

本套件僅支援在 **GitHub Codespaces** 或具備 CMake & C++ 編譯器的環境下執行。Codespaces 已為你配置好一切，開箱即用！

### 步驟 1: 啟動後端伺服器

打開 Codespaces 終端機，執行以下指令編譯並啟動後端：

```bash
# 切換至練習區
cd practice

# 建立並進入建置資料夾
mkdir -p build && cd build

# 執行 CMake 與編譯
cmake .. && make

# 啟動後端轉接器伺服器
./backend_adapter
```

*當伺服器啟動後，Codespaces 會自動彈出提示詢問是否在瀏覽器中開啟 `8080` 連接埠，點選 **Open in Browser** 即可看到踩地雷的遊戲與測試畫面！*

---

### 步驟 2: 開始實作 (你的開發循環)

1. 開啟 `practice/src/Cell.cpp`，依據裡面的 **步驟 1 ~ 步驟 3** 註釋指示，實作 `Cell` 類別的屬性與 Getter/Setter。
2. 開啟 `practice/src/Board.cpp`，依據 **步驟 4 ~ 步驟 9** 註釋指示，實作 `Board` 類別的核心遊戲邏輯。
3. **編譯與套用變更**：
   在 C++ 中，你的程式碼修改後必須重新編譯才能生效。請按照以下循環：
   - 在執行 `./backend_adapter` 的終端機中，按下 `Ctrl + C` 終止伺服器。
   - 執行編譯指令並重新啟動：
     ```bash
     make && ./backend_adapter
     ```
   - 切換回瀏覽器分頁，按下 **F5 (重新整理)** 載入新的 C++ 邏輯。
4. **驗證你的實作**：
   - 在網頁底部的「單元測試結果」面板中，點擊 **「執行測試」** 按鈕。
   - 觀看測試回饋。如果某個測試失敗，可以**點擊該行展開**，查看詳細的失敗訊息、期望值與實際值的對比。
   - 也可以直接在網頁上點擊格子，嘗試遊玩踩地雷，測試隨機擺放、自動連鎖翻開空地等邏輯！

---

## 🧪 單元測試清單 (Test Suite)

本套件包含 7 個精心設計的單元測試，涵蓋了踩地雷的核心要素：

1. **Cell - 預設建構子與 Getter/Setter 測試**：驗證 `Cell` 的基本初始狀態。
2. **Cell - 帶座標的建構子測試**：驗證格子座標設定。
3. **Board - 建構子參數驗證測試**：驗證對於無效的棋盤大小、負數地雷或過多地雷，是否正確拋出 `std::invalid_argument` 異常。
4. **Board - 邊界越界驗證測試**：驗證進行 reveal 或 flag 時，若座標越界，是否正確拋出 `std::out_of_range` 異常。
5. **Board - 第一次點擊安全初始化測試**：驗證踩地雷核心的「第一步絕不踩雷」設計！
6. **Board - 遞迴/連鎖翻開空地測試**：驗證當點擊到周圍無雷的空地 (0) 時，是否正確將周圍格子連鎖翻開。
7. **Board - 踩雷輸掉與翻開非雷獲勝測試**：驗證遊戲的勝負判定邏輯。

---

## 💡 物件導向程式設計 (OOP) 學習重點

在完成此踩地雷遊戲的過程中，你將學到並實踐以下物件導向與高階 C++ 設計觀念：

*   **封裝 (Encapsulation)**：
    *   將格子的狀態（是否為地雷、是否被插旗、被翻開、周圍雷數等）與座標，高度封裝在 `Cell` 類別中，僅透過 Getter/Setter 對外暴露，避免外部程式（如 `Board` 或轉接器）直接干涉內部邏輯。
*   **陣列與容器的操作 (Vector Matrix Manipulation)**：
    *   使用 C++ 的二維 `std::vector<std::vector<Cell>>` 管理棋盤格點矩陣，練習在 C++ 的記憶體管理機制中，安全地傳遞格子參照 (`Cell&`)。
*   **邊界防禦與例外處理 (Exception Handling)**：
    *   在公開方法（如 `getCell`, `reveal`, `toggleFlag`）中加入防禦性設計，對非法參數與越界存取主動拋出標準 C++ 例外（`std::invalid_argument` 與 `std::out_of_range`）。
*   **演算法在 OOP 中的運用 (Algorithm & State)**：
    *   在 `Board` 的方法中，實作隨機擺放地雷演算法（Knuth-Shuffle 概念）與深度/廣度優先搜尋（DFS/BFS）的區域擴展演算法，將經典演算法與類別的狀態變更緊密結合。
*   **強健的後端轉接與錯誤隔離 (Robust Adapter Design)**：
    *   瀏覽器與後端透過 REST API 通訊。即使你的 C++ 程式碼中途拋出異常，轉接器也能安全捕捉，並回傳友善的錯誤訊息至前端 Alert 提示，確保你的後端程式不會輕易崩潰（Crash）。

---

## 🆘 需要協助嗎？

*   **卡關時**：可以切換到 `answer/src/` 查看標準實作答案。
*   **如何直接在終端機跑測試？**：
    如果你不想開瀏覽器，也可以在練習區的 `build` 資料夾中直接執行：
    ```bash
    ./tests
    ```
    這會直接以 JSON 格式在終端機輸出單元測試的執行結果。

祝你程式撰寫順利，享受實作踩地雷的樂趣！🚀
