#!/bin/bash

BINARY="$1"
FUNC_NAME="$2"

# 颜色定义
RED='\033[1;31m'
GREEN='\033[1;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# 严格检测非PIE可执行文件
check_non_pie() {
    elf_type=$(readelf -h "$BINARY" | awk -F: '/Type/ {print $2}' | xargs)
    case "$elf_type" in
        "EXEC (Executable file)") 
            echo -e "${GREEN}验证通过：标准非PIE可执行文件${NC}"
            return 0 ;;
        "DYN (Shared object file)")  
            echo -e "${RED}错误：检测到PIE/DSO类型文件${NC}"
            echo -e "请重新编译：g++ -no-pie -o output source.cpp"
            return 1 
	;;
        *)      
            echo -e "${RED}不支持的ELF类型：$elf_type${NC}"
            return 1 ;;
    esac
}

# 获取符号信息
get_symbol_info() {
    echo -e "\n[符号验证] 查找目标函数：$FUNC_NAME"
    symbol_info=$(nm -CS "$BINARY" | grep -Fw "$FUNC_NAME" | grep -v '_GLOBAL')
    
    if [ -z "$symbol_info" ]; then
        echo -e "${RED}错误：未找到函数符号${NC}"
        echo "可能原因："
        echo "1. 函数被内联优化（添加 __attribute__((noinline)) ）"
        echo "2. 编译时使用过高优化级别（尝试 -O0 ）"
        exit 2
    fi

    # 解析关键字段
    func_addr=$(echo "$symbol_info" | awk '{print $1}')
    hex_size=$(echo "$symbol_info" | awk '{print $2}')
    dec_size=$((0x$hex_size))
    mangled_name=$(echo "$symbol_info" | awk '{print $3}')

    echo -e "内存地址：${GREEN}0x$func_addr"${NC}
    echo -e "代码大小：${dec_size}字节"
    echo -e "修饰名称：$mangled_name"
}

# 主流程
[ $# -lt 2 ] && {
    echo -e "用法：${YELLOW}$0 <二进制文件> <函数名>${NC}"
    echo -e "示例：$0 a.out \"SceneUser::test()\""
    exit 1
}

check_non_pie || exit 1
get_symbol_info

# 热补丁可行性检查
if [ $dec_size -ge 12 ]; then
    echo -e "\n${GREEN}验证通过：可安全进行热补丁${NC}"
else
    echo -e "\n${RED}错误：代码段过小（${dec_size}字节 < 14字节）${NC}"
    exit 3
fi
