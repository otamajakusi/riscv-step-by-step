#pragma once

/*
 * https://riscv.org/wp-content/uploads/2015/01/riscv-calling.pdf
 * x0     zero Hard-wired zero
 * x1     ra Return address Caller
 * x2     sp Stack pointer Callee
 * x3     gp Global pointer
 * x4     tp Thread pointer
 * x5-7   t0-2 Temporaries Caller
 * x8     s0/fp Saved register/frame pointer Callee
 * x9     s1 Saved register Callee
 * x10-11 a0-1 Function arguments/return values Caller
 * x12-17 a2-7 Function arguments Caller
 * x18-27 s2-11 Saved registers Callee
 * x28-31 t3-6 Temporaries Caller
 */
                                                     
#define REG_CTX_ZERO    0
#define REG_CTX_RA      1
#define REG_CTX_SP      2
#define REG_CTX_GP      3
#define REG_CTX_TP      4
#define REG_CTX_T0      5
#define REG_CTX_T1      6
#define REG_CTX_T2      7
#define REG_CTX_S0      8
#define REG_CTX_S1      9
#define REG_CTX_A0      10
#define REG_CTX_A1      11
#define REG_CTX_A2      12
#define REG_CTX_A3      13
#define REG_CTX_A4      14
#define REG_CTX_A5      15
#define REG_CTX_A6      16
#define REG_CTX_A7      17
#define REG_CTX_S2      18
#define REG_CTX_S3      19
#define REG_CTX_S4      20
#define REG_CTX_S5      21
#define REG_CTX_S6      22
#define REG_CTX_S7      23
#define REG_CTX_S8      24
#define REG_CTX_S9      25
#define REG_CTX_S10     26
#define REG_CTX_S11     27
#define REG_CTX_T3      28
#define REG_CTX_T4      29
#define REG_CTX_T5      30
#define REG_CTX_T6      31
