const API_URL = "";

// State
let gameState = null;

// Icons
const passIcon = '<svg viewBox="0 0 20 20" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M4 10.5l4 4 8-9"/></svg>';
const failIcon = '<svg viewBox="0 0 20 20" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M5 5l10 10M15 5L5 15"/></svg>';

// DOM Elements
const gridEl = document.getElementById("minesweeperGrid");
const minesCounterEl = document.getElementById("minesCounter");
const resetBtnEl = document.getElementById("resetBtn");
const statusMessageEl = document.getElementById("gameStatusMessage");
const presetSelectEl = document.getElementById("presetSelect");
const customControlsEl = document.getElementById("customControls");
const widthInputEl = document.getElementById("inputWidth");
const heightInputEl = document.getElementById("inputHeight");
const minesInputEl = document.getElementById("inputMines");

const errorAlertEl = document.getElementById("errorAlert");
const errorMessageEl = document.getElementById("errorMessage");
const errorCloseBtnEl = document.getElementById("errorCloseBtn");

const runBtnEl = document.getElementById("runBtn");
const passChipEl = document.getElementById("passChip");
const failChipEl = document.getElementById("failChip");
const listEl = document.getElementById("list");

// Initialize Game
async function init() {
  setupEventListeners();
  await fetchGameState();
  await runTests(true); // Run tests silently on startup to show initial progress
}

// Event Listeners Setup
function setupEventListeners() {
  presetSelectEl.addEventListener("change", handlePresetChange);
  resetBtnEl.addEventListener("click", () => resetGame());
  errorCloseBtnEl.addEventListener("click", hideError);
  runBtnEl.addEventListener("click", () => runTests());

  // Prevent right-click menu on the grid
  gridEl.addEventListener("contextmenu", e => e.preventDefault());
}

// Hide error banner
function hideError() {
  errorAlertEl.style.display = "none";
}

// Show error banner with message
function showError(msg) {
  errorMessageEl.textContent = msg;
  errorAlertEl.style.display = "flex";
  // Auto-scroll to error
  errorAlertEl.scrollIntoView({ behavior: "smooth", block: "nearest" });
}

// Preset selection changed
function handlePresetChange() {
  const preset = presetSelectEl.value;
  if (preset === "custom") {
    customControlsEl.style.display = "flex";
  } else {
    customControlsEl.style.display = "none";
    resetGame();
  }
}

// Fetch Board State
async function fetchGameState() {
  try {
    const res = await fetch(`${API_URL}/api/state`);
    if (!res.ok) {
      const errData = await res.json();
      throw new Error(errData.error || `HTTP 狀態碼 ${res.status}`);
    }
    gameState = await res.json();
    renderBoard();
  } catch (err) {
    console.error("無法載入遊戲狀態:", err);
    showError(err.message);
  }
}

// Dispatch Action
async function dispatchAction(action, params = {}) {
  hideError();
  try {
    const res = await fetch(`${API_URL}/api/action`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ action, params })
    });
    
    if (!res.ok) {
      const errData = await res.json();
      throw new Error(errData.error || `Action 失敗，HTTP ${res.status}`);
    }
    
    gameState = await res.json();
    renderBoard();
  } catch (err) {
    console.error(`執行 Action (${action}) 失敗:`, err);
    showError(err.message);
  }
}

// Reset Game
function resetGame() {
  const preset = presetSelectEl.value;
  let width = 9, height = 9, mines = 10;

  if (preset === "intermediate") {
    width = 16;
    height = 16;
    mines = 40;
  } else if (preset === "custom") {
    width = parseInt(widthInputEl.value) || 9;
    height = parseInt(heightInputEl.value) || 9;
    mines = parseInt(minesInputEl.value) || 10;
  }

  dispatchAction("reset", { width, height, mines });
}

// Render Board Grid
function renderBoard() {
  if (!gameState) return;

  const { width, height, minesCount, isGameOver, isGameWon, grid } = gameState;

  // Set CSS grid size variables
  gridEl.style.setProperty("--columns", width);
  gridEl.style.setProperty("--rows", height);
  gridEl.innerHTML = "";

  // Calculate flags count
  let flaggedCount = 0;
  
  // Render Cells
  for (let y = 0; y < height; ++y) {
    for (let x = 0; x < width; ++x) {
      const cellData = grid[y][x];
      const cellBtn = document.createElement("div");
      cellBtn.dataset.x = x;
      cellBtn.dataset.y = y;

      if (cellData.isRevealed) {
        cellBtn.className = "cell revealed";
        if (cellData.isMine) {
          // If game is won, mines shouldn't explode. If game lost, they show explosion or mine icon
          cellBtn.textContent = isGameWon ? "💣" : "💥";
          cellBtn.style.backgroundColor = isGameWon ? "rgba(74, 222, 128, 0.2)" : "rgba(239, 68, 68, 0.2)";
        } else if (cellData.neighborMines > 0) {
          cellBtn.textContent = cellData.neighborMines;
          cellBtn.classList.add(`num-${cellData.neighborMines}`);
        }
      } else {
        cellBtn.className = "cell unrevealed";
        if (cellData.isFlagged) {
          cellBtn.classList.add("flagged");
          cellBtn.textContent = "🚩";
          flaggedCount++;
        }
      }

      // Left click (Reveal)
      cellBtn.addEventListener("click", () => {
        if (!isGameOver && !cellData.isRevealed && !cellData.isFlagged) {
          dispatchAction("reveal", { x, y });
        }
      });

      // Right click (Flag)
      cellBtn.addEventListener("contextmenu", (e) => {
        e.preventDefault();
        if (!isGameOver && !cellData.isRevealed) {
          dispatchAction("flag", { x, y });
        }
      });

      gridEl.appendChild(cellBtn);
    }
  }

  // Update Stats Bar
  const remainingMines = Math.max(0, minesCount - flaggedCount);
  minesCounterEl.textContent = `💣 ${String(remainingMines).padStart(3, "0")}`;

  // Reset button state
  if (isGameOver) {
    resetBtnEl.textContent = isGameWon ? "😎" : "😵";
  } else {
    resetBtnEl.textContent = "😀";
  }

  // Status message
  if (isGameOver) {
    statusMessageEl.textContent = isGameWon ? "恭喜獲勝！🎉" : "踩到雷了，遊戲結束！💥";
    statusMessageEl.style.color = isGameWon ? "var(--pass)" : "var(--fail)";
  } else {
    statusMessageEl.textContent = "遊戲進行中...";
    statusMessageEl.style.color = "var(--text)";
  }
}

// Run C++ Unit Tests via API
async function runTests(silent = false) {
  if (!silent) {
    runBtnEl.classList.add("loading");
    runBtnEl.disabled = true;
  }

  try {
    const res = await fetch(`${API_URL}/api/tests/run`, { method: "POST" });
    if (!res.ok) {
      throw new Error(`執行測試失敗 (HTTP ${res.status})`);
    }

    const testResults = await res.json();
    if (testResults.error) {
      throw new Error(testResults.error);
    }

    renderTestPanel(testResults);
  } catch (err) {
    console.error("執行測試時發生錯誤:", err);
    if (!silent) {
      showError(`無法執行測試: ${err.message}`);
    }
  } finally {
    if (!silent) {
      runBtnEl.classList.remove("loading");
      runBtnEl.disabled = false;
    }
  }
}

// Render the Shared Test Panel with data
function renderTestPanel(data) {
  const passed = data.passed || 0;
  const failed = data.failed || 0;
  const tests = data.tests || [];

  passChipEl.textContent = `${passed} 通過`;
  failChipEl.textContent = `${failed} 失敗`;

  if (tests.length === 0) {
    listEl.innerHTML = '<div class="empty-hint">沒有找到任何測試案例</div>';
    return;
  }

  listEl.innerHTML = tests.map((t, i) => {
    const isFail = t.status === "failed";
    const detail = isFail ? `
      <div class="row-detail">
        <p class="message">${t.message || "測試失敗，但未提供詳細錯誤訊息。"}</p>
        ${t.expected !== undefined && t.actual !== undefined ? `
        <div class="diff">
          <div><span class="label">期望值</span><span class="val expected">${t.expected}</span></div>
          <div><span class="label">實際值</span><span class="val actual">${t.actual}</span></div>
        </div>` : ""}
      </div>` : "";

    return `
      <div class="row ${isFail ? "fail" : "pass"}" data-i="${i}">
        <div class="row-head" ${isFail ? 'role="button" tabindex="0"' : ""}>
          <div class="stripe"></div>
          <div class="status-icon">${isFail ? failIcon : passIcon}</div>
          <div class="test-name">${t.name}</div>
          <div class="disclosure">▶</div>
        </div>
        ${detail}
      </div>`;
  }).join("");

  // Bind expandable drawer events to failing test rows
  listEl.querySelectorAll(".row.fail .row-head").forEach(head => {
    const toggle = () => head.parentElement.classList.toggle("open");
    head.addEventListener("click", toggle);
    head.addEventListener("keydown", e => {
      if (e.key === "Enter" || e.key === " ") { 
        e.preventDefault(); 
        toggle(); 
      }
    });
  });
}

// Start everything
init();
