#ifndef  __CPU_DELAY__
#define  __CPU_DELAY__

#include <stdint.h>
#include "stm32f4xx.h"

#define delay_us(x) cpu_delay(x);
#define delay_ms(x) cpu_delay((x) * 1000);

void cpu_delay(uint32_t us);


#endif //__CPU_DELAY__//

//【C语言知识扫盲：头文件保护锁】
//你可能会好奇为什么声明一个函数 void cpu_delay(uint32_t us);
//上下文还要包着 #ifndef、#define 和 #endif 这三行奇怪的代码？
//#ifndef 意思是 "If Not Defined"（如果没有定义）。
//这三行代码组合在一起，叫做头文件保护（Include Guard）
//打个比方： 假设你的 main.c 包含了 st7789.h，
//而 st7789.h 里又包含了 cpu_delay.h。如果你的 main.c 顺手又包含了一次 cpu_delay.h，
//编译器就会把 cpu_delay.h 里的内容抄写两遍，导致报错（重复声明）。
//有了这把锁，编译器第一次看到它时，会定义 __CPU_DELAY_H__ 这个标记并抄写代码；
//第二次再看到时，发现标记已经存在，就会直接跳过到 #endif，
//完美避免了重复抄写！这是C语言中写头文件的绝对标准格式
