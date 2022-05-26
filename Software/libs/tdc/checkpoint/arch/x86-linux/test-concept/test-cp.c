#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>

#include <setjmp.h>

#include <ucontext.h>

#define MAX_STACK_SIZE 2048*2
#define CHECKPOINT_EXCLUDE_DATA __attribute__((section(".norestore_data")))
#define CHECKPOINT_EXCLUDE_BSS __attribute__((section(".norestore_bss")))

__attribute__ ((aligned (16)))
char _stack[MAX_STACK_SIZE];

CHECKPOINT_EXCLUDE_BSS
__attribute__ ((aligned (16)))
char _restore_stack[MAX_STACK_SIZE];

CHECKPOINT_EXCLUDE_BSS
char bss_cp[2][MAX_STACK_SIZE*2];

CHECKPOINT_EXCLUDE_BSS
char data_cp[2][MAX_STACK_SIZE];


//__attribute__ ((aligned (16)))
//char _restore_stack[2048];

/* .data section */
extern uint32_t __sdata;
extern uint32_t __sdata_norestore;

/* .bss section */
extern uint32_t __sbss;
extern uint32_t __sbss_norestore;

typedef uint8_t lclock_t;

typedef sigjmp_buf register_cp_t;

CHECKPOINT_EXCLUDE_BSS
volatile lclock_t lclock = 0;

// Register checkpoint
CHECKPOINT_EXCLUDE_BSS
register_cp_t register_cp[2];

//// Checkpoint stuff
#define MPATCH_ACTIVE_IDX()     (lclock%2)
#define MPATCH_INACTIVE_IDX()   ((lclock+1)%2)

volatile int cp_restored = 0;
#define checkpoint_restored() (cp_restored)
#define checkpoint_set_ceckpoint() (cp_restored=0)
#define checkpoint_set_restore() (cp_restored=1)

#if 0
__attribute__((always_inline))
static inline void registers_checkpoint(register_cp_t *cp)
{
}

__attribute__((always_inline))
static inline void registers_restore(register_cp_t *cp)
{
    longjmp(*cp, 1);
}

static inline void stack_checkpoint(void)
{
    memcpy(stack_cp[MPATCH_ACTIVE_IDX()], _stack, sizeof(_stack));
}

static inline void stack_restore(void)
{
    memcpy(_stack, stack_cp[MPATCH_ACTIVE_IDX()], sizeof(_stack));
}
#endif

size_t data_size(void) {
  size_t data_size = (size_t)&__sdata_norestore - (size_t)&__sdata;
  return data_size;
}

size_t bss_size(void) {
  size_t bss_size = (size_t)&__sbss_norestore - (size_t)&__sbss;
  return bss_size;
}


static inline void mem_checkpoint(void)
{
    size_t dsize = data_size();
    size_t bsize= bss_size();

    printf("Checkpoint .data (%d)\n", (int)dsize);
    printf("Checkpoint .bss (%d)\n", (int)bsize);

    memcpy(data_cp[MPATCH_ACTIVE_IDX()], (char *)&__sdata, dsize);
    memcpy(bss_cp[MPATCH_ACTIVE_IDX()], (char *)&__sbss, bsize);
}

__attribute__((always_inline))
static inline void mem_restore(void)
{
    size_t dsize = data_size();
    size_t bsize= bss_size();

    printf("Restore .data (%d)\n", (int)dsize);
#if 0
    char *c_sdata = (char *)&__sdata;
    char *c_datacp = data_cp[MPATCH_INACTIVE_IDX()];
    for (size_t i=0; i<dsize; i++) {
        c_sdata[i] = c_datacp[i];
    }
#else
    memcpy((char *)&__sdata, data_cp[MPATCH_INACTIVE_IDX()], dsize);
#endif

    printf("Restore .bss (%d)\n", (int)bsize);
#if 0
    char *c_sbss = (char *)&__sbss;
    char *c_bsscp = bss_cp[MPATCH_INACTIVE_IDX()];
    for (size_t i=0; i<dsize; i++) {
        c_sbss[i] = c_bsscp[i];
    }
#else
    memcpy((char *)&__sbss, bss_cp[MPATCH_INACTIVE_IDX()], bsize);
#endif

}

__attribute__((noinline))
void cp_restore(void)
{
#if 1
    size_t dsize = data_size();
    size_t bsize= bss_size();

    printf("Scramble current memory\n");
    printf("Scramble .data (%d)\n", (int)dsize);
    memset((char *)&__sdata, 0, dsize);

    printf("Scramble .bss (%d)\n", (int)bsize);
    memset((char *)&__sbss, 0, bsize);
#endif

    checkpoint_set_restore();

    mem_restore();
    //printf("Restore registers\n");
    //printf("Continue:\n\n");

    siglongjmp(register_cp[MPATCH_INACTIVE_IDX()], 1);
}

__attribute__((noinline))
int cp_checkpoint(void)
{
    register_cp_t *active_register_cp;

    checkpoint_set_ceckpoint();

    mem_checkpoint();
    sigsetjmp(register_cp[MPATCH_ACTIVE_IDX()], SIGQUIT);
    if (checkpoint_restored() == 0) {
        /* Normal operation */

        // Commit
        printf("Commit cp\n");
        lclock += 1;
    }

    return 0;
}

int cp_post_checkpoint(void)
{
    cp_restore();
    return 0;
}


CHECKPOINT_EXCLUDE_BSS
ucontext_t restore_ctx;

void sig_handler(int signo)
{
    //signal(signo, SIG_IGN);
    printf("received SIGKILL\n");

    printf("\nFake power failure!\n");

    //cp_restore();

    getcontext(&restore_ctx);
    restore_ctx.uc_stack.ss_sp = _restore_stack;
    restore_ctx.uc_stack.ss_size = sizeof(_restore_stack);
    restore_ctx.uc_link = NULL;
    makecontext(&restore_ctx, cp_restore, 0);
    setcontext(&restore_ctx);
}

int data_cnt = 0;
int bss_cnt;

void _main(void)
{
    if (signal(SIGQUIT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGQUIT\n");
    // A long long wait so that we can easily issue a signal to this process
    int cnt = 0;
    while(1) {
        //for (int i=0; i<1000; i++) {}
        sleep(1);
        printf("local Cnt: %d\n", cnt++);
        printf("global (.data) Cnt: %d\n", data_cnt++);
        printf("global (.bss) Cnt: %d\n", bss_cnt++);
        cp_checkpoint();
    };
}

CHECKPOINT_EXCLUDE_BSS
ucontext_t ctx;

int main(void)
{
    //setbuf(stdout, NULL);

    printf("---Initializing Memory Space---\n");
    printf("* new stack range: [%p-%p]\n",
            _stack, _stack+sizeof(_stack));
    printf("* .data start: [%p]\n",
            &__sdata);
    printf("* .bss start: [%p]\n",
            &__sbss);
    printf("------------------------\n\n");

    getcontext(&ctx);
    ctx.uc_stack.ss_sp = _stack;
    ctx.uc_stack.ss_size = sizeof(_stack);
    ctx.uc_link = NULL;
    makecontext(&ctx, _main, 0);
    setcontext(&ctx);

    return 0;
}

