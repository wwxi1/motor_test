# Makefile for DJmotor_SetZero Unit Tests
#
# 使用方法:
#   make          - 编译测试
#   make run      - 编译并运行测试
#   make clean    - 清理构建文件

# 编译器设置
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -fprofile-arcs -ftest-coverage
LDFLAGS = -lunity -lm

# 目录结构
TEST_DIR = User/test
BUILD_DIR = build

# 源文件
TEST_SOURCES = $(TEST_DIR)/test_djmotor_setzero.c \
               $(TEST_DIR)/main_stub.c \
               $(TEST_DIR)/pid_stub.c

# Unity 框架 (假设从外部获取或使用系统安装的)
UNITY_DIR = /usr/local/lib/unity
UNITY_SOURCE = $(UNITY_DIR)/unity.c
UNITY_HEADER = $(UNITY_DIR)/unity.h

# 检查 Unity 是否存在
ifneq ($(wildcard $(UNITY_HEADER)),)
    UNITY_DEPS = $(UNITY_SOURCE)
    LDFLAGS += -L$(UNITY_DIR)
else
    # 尝试从系统路径查找
    UNITY_HEADER := $(shell pkg-config --cflags unity 2>/dev/null || echo "")
    ifneq ($(UNITY_HEADER),)
        LDFLAGS += $(shell pkg-config --libs unity)
    else
        # 尝试标准系统路径
        ifneq ($(wildcard /usr/include/unity.h),)
            UNITY_HEADER = /usr/include/unity.h
            UNITY_SOURCE =
        else ifneq ($(wildcard /usr/local/include/unity.h),)
            UNITY_HEADER = /usr/local/include/unity.h
            UNITY_SOURCE =
        endif
    endif
endif

# 目标可执行文件
TARGET = $(BUILD_DIR)/test_djmotor_setzero

# 包含路径
INCLUDES = -I$(TEST_DIR) -I/usr/include -I/usr/local/include

.PHONY: all run clean setup-unity

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 如果没有找到 Unity，从源码构建
ifneq ($(wildcard $(UNITY_HEADER)),)
$(TARGET): $(TEST_SOURCES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(TEST_SOURCES) $(LDFLAGS) -o $@
else
# 需要获取 Unity 框架
$(TARGET): $(TEST_SOURCES) | $(BUILD_DIR) setup-unity
	$(CC) $(CFLAGS) $(INCLUDES) $(TEST_SOURCES) $(UNITY_SOURCE) -o $@
endif

setup-unity:
	@echo "需要安装 Unity 测试框架..."
	@echo "请执行以下命令安装 Unity:"
	@echo "  git clone https://github.com/ThrowTheSwitch/Unity.git /tmp/unity"
	@echo "  sudo cp -r /tmp/unity/src/unity.h /usr/local/include/"
	@echo "  sudo cp -r /tmp/unity/src/unity.c /usr/local/lib/unity/"
	@echo ""
	@echo "或者使用包管理器安装:"
	@echo "  sudo apt-get install unity (如果可用)"
	@exit 1

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
	rm -f *.gcno *.gcda *.gcov

# 简化版本：直接内联 Unity 框架
SIMPLE_TARGET = $(BUILD_DIR)/test_simple

$(SIMPLE_TARGET): $(TEST_SOURCES) | $(BUILD_DIR)
	@echo "Unity 框架未找到，请先安装或使用: make setup-unity"

# 覆盖率报告
coverage: $(TARGET)
	gcov $(BUILD_DIR)/*.c 2>/dev/null || gcov $(BUILD_DIR)/*.o 2>/dev/null || echo "运行 'make run' 后可生成覆盖率报告"

help:
	@echo "DJmotor_SetZero 单元测试 Makefile"
	@echo ""
	@echo "使用方法:"
	@echo "  make          - 编译测试"
	@echo "  make run      - 编译并运行测试"
	@echo "  make clean    - 清理构建文件"
	@echo "  make help     - 显示帮助"
	@echo ""
	@echo "注意: 需要安装 Unity 测试框架"
	@echo "  快速安装: git clone https://github.com/ThrowTheSwitch/Unity.git /tmp/unity"
	@echo "            sudo cp /tmp/unity/src/unity.h /usr/local/include/"
	@echo "            sudo cp /tmp/unity/src/unity.c /usr/local/lib/unity/"
