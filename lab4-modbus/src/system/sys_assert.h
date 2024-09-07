
#ifndef SYS_ASSERT_H_
#define SYS_ASSERT_H_

#if 0
#define ASSERT(expr)                (expr) ? (void) 0 : sys_assert(__BASE_FILE__ , __LINE__)
#else
#define ASSERT(expr)                if(expr) {} else { while(1) {} }
#endif

#if 0
#define assert(expr)                (expr) ? (void) 0 : sys_assert(__BASE_FILE__ , __LINE__)
#else
#define assert(expr)                if(expr) {} else { while(1) {} }
#endif


void  sys_assert( char *file_name, uint16_t line_num );



#endif /* SYS_ASSERT_H_ */
