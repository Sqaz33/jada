#!/bin/bash
# Скрипт для тестирования производительности с PGO и без (O3)

set -e

# Пути
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$SCRIPT_DIR"
BUILD_DIR="$PROJECT_DIR/build"
BUILD_PGO_DIR="$PROJECT_DIR/build_pgo"
DATA_DIR="$PROJECT_DIR/test_data/final"

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${YELLOW}============================================${NC}"
echo -e "${YELLOW}   Тестирование производительности JADA${NC}"
echo -e "${YELLOW}   PGO O3 vs O3${NC}"
echo -e "${YELLOW}============================================${NC}"
echo ""

# Проверяем необходимые условия
echo -e "${BLUE}=== Проверка окружения ===${NC}"
echo ""

# Проверка g++ (WSL)
echo -n "g++ (WSL): "
if command -v g++ &> /dev/null; then
    g++ --version | head -n1 | sed 's/.*) //'
else
    echo -e "${RED}не найден${NC}"
    exit 1
fi

# Проверка cmake
echo -n "cmake: "
if command -v cmake &> /dev/null; then
    cmake --version | head -n1 | sed 's/.*version //'
else
    echo -e "${RED}не найден${NC}"
    exit 1
fi

# Проверка javac (WSL)
echo -n "javac (WSL): "
if command -v javac &> /dev/null; then
    javac -version 2>&1 | head -n1 | sed 's/.*version //'
else
    echo -e "${RED}не найден${NC}"
    exit 1
fi

echo ""
echo "Тестовые файлы: $(ls -1 $DATA_DIR/*.adb 2>/dev/null | wc -l)"
echo ""

# Функция для запуска бенчмарка
run_benchmark() {
    local name=$1
    local binary=$2
    local iterations=${3:-5}
    
    echo -e "${GREEN}>>> $name <<<${NC}"
    echo "Итераций: $iterations"
    
    # Проверяем существование бинарника
    if [ ! -f "$binary" ]; then
        echo -e "${RED}Ошибка: бинарник $binary не найден${NC}"
        return 1
    fi
    
    times=()
    
    for i in $(seq 1 $iterations); do
        echo -n "  Итерация $i... "
        
        # Замеряем время выполнения всех файлов
        start=$(date +%s.%N)
        
        for adb_file in $DATA_DIR/*.adb; do
            if [ -f "$adb_file" ]; then
                $binary "$adb_file" > /dev/null 2>&1
            fi
        done
        
        end=$(date +%s.%N)
        elapsed=$(echo "$end - $start" | bc -l)
        times+=("$elapsed")
        echo "${elapsed} сек"
    done
    
    # Вычисляем среднее
    total=0
    for t in "${times[@]}"; do
        total=$(echo "$total + $t" | bc -l)
    done
    avg=$(echo "scale=6; $total / $iterations" | bc -l)
    
    # Вычисляем min и max
    min=$(printf '%s\n' "${times[@]}" | sort -n | head -1)
    max=$(printf '%s\n' "${times[@]}" | sort -n | tail -1)
    
    echo ""
    echo -e "${YELLOW}  Результаты $name:${NC}"
    echo "    Среднее: ${avg} сек"
    echo "    Min: ${min} сек"
    echo "    Max: ${max} сек"
    echo ""
    
    # Сохраняем результат
    echo "$avg" > "/tmp/benchmark_${name// /_}.txt"
    echo "$min" > "/tmp/benchmark_${name// /_}_min.txt"
    echo "$max" > "/tmp/benchmark_${name// /_}_max.txt"
}

# Компилируем Java классы в WSL
echo -e "${YELLOW}=== Компиляция Java классов ===${NC}"
cd "$PROJECT_DIR"
javac java/AdaUtility.java
echo -e "${GREEN}Java классы готовы${NC}"
echo ""

# ==========================================
# ТЕСТ 1: O3 без PGO
# ==========================================
echo -e "${BLUE}=== Этап 1: Сборка O3 (без PGO) ===${NC}"
echo ""

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Настройка CMake..."
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-O3" \
      .. 2>&1 | tail -n3

echo ""
echo "Компиляция..."
make -j$(nproc) 2>&1 | tail -n3

if [ ! -f "$BUILD_DIR/jada" ]; then
    echo -e "${RED}Ошибка компиляции!${NC}"
    exit 1
fi

echo -e "${GREEN}Сборка O3 завершена${NC}"
echo ""

# Запускаем бенчмарк O3
echo -e "${BLUE}=== Замеры O3 ===${NC}"
run_benchmark "O3" "$BUILD_DIR/jada" 5

# ==========================================
# ТЕСТ 2: PGO O3
# ==========================================
echo -e "${BLUE}=== Этап 2: PGO O3 (сборка с профилем) ===${NC}"
echo ""

# Очищаем и создаем директорию для PGO сборки
rm -rf "$BUILD_PGO_DIR"
mkdir -p "$BUILD_PGO_DIR"
cd "$BUILD_PGO_DIR"

echo "Настройка CMake (Этап 1: Profile Generation)..."
cmake -DCMAKE_BUILD_TYPE=Release \
      -DUSE_PGO=ON \
      .. 2>&1 | tail -n3

echo ""
echo "Компиляция (Этап 1: с -fprofile-generate)..."
make -j$(nproc) 2>&1 | tail -n3

if [ ! -f "$BUILD_PGO_DIR/jada" ]; then
    echo -e "${RED}Ошибка компиляции!${NC}"
    exit 1
fi

echo ""
echo "Запуск на тестовых данных для сбора профиля..."
for i in $(seq 1 3); do
    echo "  Прогон $i..."
    for adb_file in $DATA_DIR/*.adb; do
        if [ -f "$adb_file" ]; then
            "$BUILD_PGO_DIR/jada" "$adb_file" > /dev/null 2>&1
        fi
    done
done

echo -e "${GREEN}Профиль собран${NC}"
echo ""

# Удаляем старые объектные файлы и перенастраиваем для второго этапа
echo "Подготовка к этапу 2 (перенастройка с -fprofile-use)..."
rm -rf CMakeCache.txt CMakeFiles
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-O3 -fprofile-use=$(pwd)" \
      .. 2>&1 | tail -n3

echo ""
echo "Компиляция (Этап 2: с -fprofile-use)..."
make -j$(nproc) 2>&1 | tail -n3

if [ ! -f "$BUILD_PGO_DIR/jada" ]; then
    echo -e "${RED}Ошибка финальной компиляции!${NC}"
    exit 1
fi

echo -e "${GREEN}Сборка PGO O3 завершена${NC}"
echo ""

# Запускаем бенчмарк PGO O3
echo -e "${BLUE}=== Замеры PGO O3 ===${NC}"
run_benchmark "PGO_O3" "$BUILD_PGO_DIR/jada" 5

# ==========================================
# ИТОГОВОЕ СРАВНЕНИЕ
# ==========================================
echo -e "${YELLOW}============================================${NC}"
echo -e "${YELLOW}           ИТОГОВОЕ СРАВНЕНИЕ${NC}"
echo -e "${YELLOW}============================================${NC}"
echo ""

o3_avg=$(cat "/tmp/benchmark_O3.txt" 2>/dev/null || echo "0")
pgo_avg=$(cat "/tmp/benchmark_PGO_O3.txt" 2>/dev/null || echo "0")

o3_min=$(cat "/tmp/benchmark_O3_min.txt" 2>/dev/null || echo "0")
pgo_min=$(cat "/tmp/benchmark_PGO_O3_min.txt" 2>/dev/null || echo "0")

o3_max=$(cat "/tmp/benchmark_O3_max.txt" 2>/dev/null || echo "0")
pgo_max=$(cat "/tmp/benchmark_PGO_O3_max.txt" 2>/dev/null || echo "0")

echo "┌─────────────┬─────────────┬─────────────┐"
echo "│             │     O3      │   PGO O3    │"
echo "├─────────────┼─────────────┼─────────────┤"
printf "│ Среднее     │ %10.4f с │ %10.4f с │\n" "$o3_avg" "$pgo_avg"
printf "│ Min         │ %10.4f с │ %10.4f с │\n" "$o3_min" "$pgo_min"
printf "│ Max         │ %10.4f с │ %10.4f с │\n" "$o3_max" "$pgo_max"
echo "└─────────────┴─────────────┴─────────────┘"
echo ""

# Вычисляем ускорение
if [ "$(echo "$o3_avg > 0" | bc)" -eq 1 ]; then
    speedup=$(echo "scale=2; ($o3_avg - $pgo_avg) / $o3_avg * 100" | bc -l)
    if [ "$(echo "$speedup > 0" | bc)" -eq 1 ]; then
        echo -e "${GREEN}Ускорение PGO: +${speedup}%${NC}"
    elif [ "$(echo "$speedup < 0" | bc)" -eq 1 ]; then
        abs_speedup=$(echo "scale=2; $speedup * -1" | bc -l)
        echo -e "${RED}Замедление PGO: -${abs_speedup}%${NC}"
    else
        echo "Без изменений"
    fi
fi

echo ""
echo -e "${YELLOW}============================================${NC}"
echo -e "${YELLOW}           Тестирование завершено${NC}"
echo -e "${YELLOW}============================================${NC}"