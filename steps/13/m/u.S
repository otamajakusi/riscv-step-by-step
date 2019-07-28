
../u/u.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <_start>:
   0:	00001117          	auipc	sp,0x1
   4:	23810113          	addi	sp,sp,568 # 1238 <thread_stack>
   8:	00001517          	auipc	a0,0x1
   c:	03050513          	addi	a0,a0,48 # 1038 <_stacks>
  10:	858a                	mv	a1,sp
  12:	aaaab637          	lui	a2,0xaaaab
  16:	aaa60613          	addi	a2,a2,-1366 # aaaaaaaa <_bss_end+0xaaaa9372>
  1a:	00b50d63          	beq	a0,a1,34 <_start+0x34>
  1e:	c110                	sw	a2,0(a0)
  20:	0511                	addi	a0,a0,4
  22:	bfc5                	j	12 <_start+0x12>
  24:	00001517          	auipc	a0,0x1
  28:	00c50513          	addi	a0,a0,12 # 1030 <_bss_start>
  2c:	00001597          	auipc	a1,0x1
  30:	70c58593          	addi	a1,a1,1804 # 1738 <_bss_end>
  34:	00b50663          	beq	a0,a1,40 <_start+0x40>
  38:	00052023          	sw	zero,0(a0)
  3c:	0511                	addi	a0,a0,4
  3e:	bfdd                	j	34 <_start+0x34>
  40:	1a6000ef          	jal	ra,1e6 <arch_setup>
  44:	052000ef          	jal	ra,96 <main>
  48:	5d00006f          	j	618 <exit>

0000004c <__syscall>:
  4c:	00000073          	ecall
  50:	8082                	ret

00000052 <_atomic_compare_exchange>:
  52:	100526af          	lr.w	a3,(a0)
  56:	00b69763          	bne	a3,a1,64 <_atomic_compare_exchange+0x12>
  5a:	18c5252f          	sc.w	a0,a2,(a0)
  5e:	00153513          	seqz	a0,a0
  62:	a011                	j	66 <_atomic_compare_exchange+0x14>
  64:	4501                	li	a0,0
  66:	8082                	ret

00000068 <_atomic_exchange>:
  68:	100526af          	lr.w	a3,(a0)
  6c:	18b5262f          	sc.w	a2,a1,(a0)
  70:	fe061ce3          	bnez	a2,68 <_atomic_exchange>
  74:	8536                	mv	a0,a3
  76:	8082                	ret

00000078 <_atomic_fetch_add_relaxed>:
  78:	00b5252f          	amoadd.w	a0,a1,(a0)
  7c:	8082                	ret

0000007e <_atomic_load_relaxed>:
  7e:	4108                	lw	a0,0(a0)
  80:	8082                	ret

00000082 <thread_entry>:
  82:	1141                	addi	sp,sp,-16
  84:	c422                	sw	s0,8(sp)
  86:	c606                	sw	ra,12(sp)
  88:	842a                	mv	s0,a0
  8a:	245d                	jal	330 <thread_example>
  8c:	4408                	lw	a0,8(s0)
  8e:	40b2                	lw	ra,12(sp)
  90:	4422                	lw	s0,8(sp)
  92:	0141                	addi	sp,sp,16
  94:	8082                	ret

00000096 <main>:
  96:	7151                	addi	sp,sp,-240
  98:	00001517          	auipc	a0,0x1
  9c:	81850513          	addi	a0,a0,-2024 # 8b0 <vsnprintf+0x1c8>
  a0:	d786                	sw	ra,236(sp)
  a2:	d5a2                	sw	s0,232(sp)
  a4:	d3a6                	sw	s1,228(sp)
  a6:	d1ca                	sw	s2,224(sp)
  a8:	cfce                	sw	s3,220(sp)
  aa:	cdd2                	sw	s4,216(sp)
  ac:	cbd6                	sw	s5,212(sp)
  ae:	c9da                	sw	s6,208(sp)
  b0:	c7de                	sw	s7,204(sp)
  b2:	c5e2                	sw	s8,200(sp)
  b4:	c3e6                	sw	s9,196(sp)
  b6:	c1ea                	sw	s10,192(sp)
  b8:	2b55                	jal	66c <puts>
  ba:	2621                	jal	3c2 <atomic_test>
  bc:	00410c93          	addi	s9,sp,4
  c0:	8566                	mv	a0,s9
  c2:	2a79                	jal	260 <thread_mutex_init>
  c4:	00810c13          	addi	s8,sp,8
  c8:	8562                	mv	a0,s8
  ca:	2c05                	jal	2fa <thread_cond_init>
  cc:	1084                	addi	s1,sp,96
  ce:	06000613          	li	a2,96
  d2:	4581                	li	a1,0
  d4:	8526                	mv	a0,s1
  d6:	2b99                	jal	62c <memset>
  d8:	50000613          	li	a2,1280
  dc:	0aa00593          	li	a1,170
  e0:	00001517          	auipc	a0,0x1
  e4:	15850513          	addi	a0,a0,344 # 1238 <thread_stack>
  e8:	01010913          	addi	s2,sp,16
  ec:	2381                	jal	62c <memset>
  ee:	8a26                	mv	s4,s1
  f0:	00001a97          	auipc	s5,0x1
  f4:	148a8a93          	addi	s5,s5,328 # 1238 <thread_stack>
  f8:	8b4a                	mv	s6,s2
  fa:	8426                	mv	s0,s1
  fc:	4981                	li	s3,0
  fe:	4b91                	li	s7,4
 100:	5d7d                	li	s10,-1
 102:	01342423          	sw	s3,8(s0)
 106:	01942023          	sw	s9,0(s0)
 10a:	01842223          	sw	s8,4(s0)
 10e:	c444                	sw	s1,12(s0)
 110:	01742823          	sw	s7,16(s0)
 114:	01a42a23          	sw	s10,20(s0)
 118:	85d6                	mv	a1,s5
 11a:	855a                	mv	a0,s6
 11c:	14000613          	li	a2,320
 120:	0985                	addi	s3,s3,1
 122:	20c5                	jal	202 <thread_attr_setstack>
 124:	0461                	addi	s0,s0,24
 126:	0b21                	addi	s6,s6,8
 128:	140a8a93          	addi	s5,s5,320
 12c:	fd799be3          	bne	s3,s7,102 <main+0x6c>
 130:	03010993          	addi	s3,sp,48
 134:	844e                	mv	s0,s3
 136:	00000b17          	auipc	s6,0x0
 13a:	f4cb0b13          	addi	s6,s6,-180 # 82 <thread_entry>
 13e:	00000a97          	auipc	s5,0x0
 142:	792a8a93          	addi	s5,s5,1938 # 8d0 <vsnprintf+0x1e8>
 146:	86d2                	mv	a3,s4
 148:	85ca                	mv	a1,s2
 14a:	865a                	mv	a2,s6
 14c:	8522                	mv	a0,s0
 14e:	2875                	jal	20a <thread_create>
 150:	4010                	lw	a2,0(s0)
 152:	85aa                	mv	a1,a0
 154:	0431                	addi	s0,s0,12
 156:	8556                	mv	a0,s5
 158:	21dd                	jal	63e <printf>
 15a:	0921                	addi	s2,s2,8
 15c:	0a61                	addi	s4,s4,24
 15e:	fe8494e3          	bne	s1,s0,146 <main+0xb0>
 162:	08c0                	addi	s0,sp,84
 164:	00000497          	auipc	s1,0x0
 168:	78448493          	addi	s1,s1,1924 # 8e8 <vsnprintf+0x200>
 16c:	006c                	addi	a1,sp,12
 16e:	8522                	mv	a0,s0
 170:	20e1                	jal	238 <thread_join>
 172:	4632                	lw	a2,12(sp)
 174:	4014                	lw	a3,0(s0)
 176:	8526                	mv	a0,s1
 178:	85b2                	mv	a1,a2
 17a:	21d1                	jal	63e <printf>
 17c:	ff440793          	addi	a5,s0,-12
 180:	03341163          	bne	s0,s3,1a2 <main+0x10c>
 184:	50be                	lw	ra,236(sp)
 186:	542e                	lw	s0,232(sp)
 188:	549e                	lw	s1,228(sp)
 18a:	590e                	lw	s2,224(sp)
 18c:	49fe                	lw	s3,220(sp)
 18e:	4a6e                	lw	s4,216(sp)
 190:	4ade                	lw	s5,212(sp)
 192:	4b4e                	lw	s6,208(sp)
 194:	4bbe                	lw	s7,204(sp)
 196:	4c2e                	lw	s8,200(sp)
 198:	4c9e                	lw	s9,196(sp)
 19a:	4d0e                	lw	s10,192(sp)
 19c:	4501                	li	a0,0
 19e:	616d                	addi	sp,sp,240
 1a0:	8082                	ret
 1a2:	843e                	mv	s0,a5
 1a4:	b7e1                	j	16c <main+0xd6>

000001a6 <poweroff>:
 1a6:	1141                	addi	sp,sp,-16
 1a8:	85aa                	mv	a1,a0
 1aa:	02000513          	li	a0,32
 1ae:	c606                	sw	ra,12(sp)
 1b0:	3d71                	jal	4c <__syscall>
 1b2:	a001                	j	1b2 <poweroff+0xc>

000001b4 <console_putchar>:
 1b4:	1101                	addi	sp,sp,-32
 1b6:	c62a                	sw	a0,12(sp)
 1b8:	0070                	addi	a2,sp,12
 1ba:	4685                	li	a3,1
 1bc:	4581                	li	a1,0
 1be:	4505                	li	a0,1
 1c0:	ce06                	sw	ra,28(sp)
 1c2:	3569                	jal	4c <__syscall>
 1c4:	40f2                	lw	ra,28(sp)
 1c6:	4532                	lw	a0,12(sp)
 1c8:	6105                	addi	sp,sp,32
 1ca:	8082                	ret

000001cc <console_getchar>:
 1cc:	1101                	addi	sp,sp,-32
 1ce:	00f10613          	addi	a2,sp,15
 1d2:	4685                	li	a3,1
 1d4:	4581                	li	a1,0
 1d6:	4501                	li	a0,0
 1d8:	ce06                	sw	ra,28(sp)
 1da:	3d8d                	jal	4c <__syscall>
 1dc:	40f2                	lw	ra,28(sp)
 1de:	00f14503          	lbu	a0,15(sp)
 1e2:	6105                	addi	sp,sp,32
 1e4:	8082                	ret

000001e6 <arch_setup>:
 1e6:	1141                	addi	sp,sp,-16
 1e8:	00001517          	auipc	a0,0x1
 1ec:	e3050513          	addi	a0,a0,-464 # 1018 <console_user>
 1f0:	c606                	sw	ra,12(sp)
 1f2:	2efd                	jal	5f0 <register_console>
 1f4:	40b2                	lw	ra,12(sp)
 1f6:	00001517          	auipc	a0,0x1
 1fa:	e0a50513          	addi	a0,a0,-502 # 1000 <poweroff_user>
 1fe:	0141                	addi	sp,sp,16
 200:	a111                	j	604 <register_poweroff>

00000202 <thread_attr_setstack>:
 202:	c10c                	sw	a1,0(a0)
 204:	c150                	sw	a2,4(a0)
 206:	4501                	li	a0,0
 208:	8082                	ret

0000020a <thread_create>:
 20a:	1141                	addi	sp,sp,-16
 20c:	c422                	sw	s0,8(sp)
 20e:	c606                	sw	ra,12(sp)
 210:	c150                	sw	a2,4(a0)
 212:	41dc                	lw	a5,4(a1)
 214:	4190                	lw	a2,0(a1)
 216:	842a                	mv	s0,a0
 218:	c514                	sw	a3,8(a0)
 21a:	963e                	add	a2,a2,a5
 21c:	86aa                	mv	a3,a0
 21e:	00000597          	auipc	a1,0x0
 222:	03258593          	addi	a1,a1,50 # 250 <thread_entry>
 226:	02100513          	li	a0,33
 22a:	350d                	jal	4c <__syscall>
 22c:	c008                	sw	a0,0(s0)
 22e:	40b2                	lw	ra,12(sp)
 230:	4422                	lw	s0,8(sp)
 232:	857d                	srai	a0,a0,0x1f
 234:	0141                	addi	sp,sp,16
 236:	8082                	ret

00000238 <thread_join>:
 238:	862e                	mv	a2,a1
 23a:	410c                	lw	a1,0(a0)
 23c:	02200513          	li	a0,34
 240:	b531                	j	4c <__syscall>

00000242 <thread_exit>:
 242:	410c                	lw	a1,0(a0)
 244:	1141                	addi	sp,sp,-16
 246:	02000513          	li	a0,32
 24a:	c606                	sw	ra,12(sp)
 24c:	3501                	jal	4c <__syscall>
 24e:	a001                	j	24e <thread_exit+0xc>

00000250 <thread_entry>:
 250:	415c                	lw	a5,4(a0)
 252:	4508                	lw	a0,8(a0)
 254:	1101                	addi	sp,sp,-32
 256:	ce06                	sw	ra,28(sp)
 258:	9782                	jalr	a5
 25a:	c62a                	sw	a0,12(sp)
 25c:	0068                	addi	a0,sp,12
 25e:	37d5                	jal	242 <thread_exit>

00000260 <thread_mutex_init>:
 260:	00050023          	sb	zero,0(a0)
 264:	000500a3          	sb	zero,1(a0)
 268:	00050123          	sb	zero,2(a0)
 26c:	000501a3          	sb	zero,3(a0)
 270:	4501                	li	a0,0
 272:	8082                	ret

00000274 <thread_mutex_trylock>:
 274:	4705                	li	a4,1
 276:	0f50000f          	fence	iorw,ow
 27a:	140527af          	lr.w.aq	a5,(a0)
 27e:	00079563          	bnez	a5,288 <thread_mutex_trylock+0x14>
 282:	1ce526af          	sc.w.aq	a3,a4,(a0)
 286:	faf5                	bnez	a3,27a <thread_mutex_trylock+0x6>
 288:	e399                	bnez	a5,28e <thread_mutex_trylock+0x1a>
 28a:	4501                	li	a0,0
 28c:	8082                	ret
 28e:	5555                	li	a0,-11
 290:	8082                	ret

00000292 <thread_mutex_lock>:
 292:	1141                	addi	sp,sp,-16
 294:	c422                	sw	s0,8(sp)
 296:	c606                	sw	ra,12(sp)
 298:	c226                	sw	s1,4(sp)
 29a:	842a                	mv	s0,a0
 29c:	3fe1                	jal	274 <thread_mutex_trylock>
 29e:	cd11                	beqz	a0,2ba <thread_mutex_lock+0x28>
 2a0:	4489                	li	s1,2
 2a2:	87a6                	mv	a5,s1
 2a4:	0cf427af          	amoswap.w.aq	a5,a5,(s0)
 2a8:	cb89                	beqz	a5,2ba <thread_mutex_lock+0x28>
 2aa:	4701                	li	a4,0
 2ac:	4689                	li	a3,2
 2ae:	4601                	li	a2,0
 2b0:	85a2                	mv	a1,s0
 2b2:	02300513          	li	a0,35
 2b6:	3b59                	jal	4c <__syscall>
 2b8:	b7ed                	j	2a2 <thread_mutex_lock+0x10>
 2ba:	40b2                	lw	ra,12(sp)
 2bc:	4422                	lw	s0,8(sp)
 2be:	4492                	lw	s1,4(sp)
 2c0:	4501                	li	a0,0
 2c2:	0141                	addi	sp,sp,16
 2c4:	8082                	ret

000002c6 <thread_mutex_unlock>:
 2c6:	1141                	addi	sp,sp,-16
 2c8:	c606                	sw	ra,12(sp)
 2ca:	4781                	li	a5,0
 2cc:	0cf527af          	amoswap.w.aq	a5,a5,(a0)
 2d0:	eb91                	bnez	a5,2e4 <thread_mutex_unlock+0x1e>
 2d2:	00000517          	auipc	a0,0x0
 2d6:	63650513          	addi	a0,a0,1590 # 908 <vsnprintf+0x220>
 2da:	2e49                	jal	66c <puts>
 2dc:	40b2                	lw	ra,12(sp)
 2de:	4501                	li	a0,0
 2e0:	0141                	addi	sp,sp,16
 2e2:	8082                	ret
 2e4:	4709                	li	a4,2
 2e6:	fee79be3          	bne	a5,a4,2dc <thread_mutex_unlock+0x16>
 2ea:	85aa                	mv	a1,a0
 2ec:	4701                	li	a4,0
 2ee:	4685                	li	a3,1
 2f0:	4605                	li	a2,1
 2f2:	02300513          	li	a0,35
 2f6:	3b99                	jal	4c <__syscall>
 2f8:	b7d5                	j	2dc <thread_mutex_unlock+0x16>

000002fa <thread_cond_init>:
 2fa:	00050023          	sb	zero,0(a0)
 2fe:	000500a3          	sb	zero,1(a0)
 302:	00050123          	sb	zero,2(a0)
 306:	000501a3          	sb	zero,3(a0)
 30a:	4501                	li	a0,0
 30c:	8082                	ret

0000030e <increment_count.isra.1>:
 30e:	1141                	addi	sp,sp,-16
 310:	c422                	sw	s0,8(sp)
 312:	c606                	sw	ra,12(sp)
 314:	842a                	mv	s0,a0
 316:	3fb5                	jal	292 <thread_mutex_lock>
 318:	00001717          	auipc	a4,0x1
 31c:	d1870713          	addi	a4,a4,-744 # 1030 <_bss_start>
 320:	431c                	lw	a5,0(a4)
 322:	8522                	mv	a0,s0
 324:	4422                	lw	s0,8(sp)
 326:	40b2                	lw	ra,12(sp)
 328:	0785                	addi	a5,a5,1
 32a:	c31c                	sw	a5,0(a4)
 32c:	0141                	addi	sp,sp,16
 32e:	bf61                	j	2c6 <thread_mutex_unlock>

00000330 <thread_example>:
 330:	1141                	addi	sp,sp,-16
 332:	c422                	sw	s0,8(sp)
 334:	c226                	sw	s1,4(sp)
 336:	6461                	lui	s0,0x18
 338:	84aa                	mv	s1,a0
 33a:	00001517          	auipc	a0,0x1
 33e:	cfa50513          	addi	a0,a0,-774 # 1034 <printf_mutex>
 342:	c606                	sw	ra,12(sp)
 344:	6a040413          	addi	s0,s0,1696 # 186a0 <_bss_end+0x16f68>
 348:	3f21                	jal	260 <thread_mutex_init>
 34a:	4088                	lw	a0,0(s1)
 34c:	147d                	addi	s0,s0,-1
 34e:	37c1                	jal	30e <increment_count.isra.1>
 350:	fc6d                	bnez	s0,34a <thread_example+0x1a>
 352:	4422                	lw	s0,8(sp)
 354:	00001797          	auipc	a5,0x1
 358:	cdc78793          	addi	a5,a5,-804 # 1030 <_bss_start>
 35c:	448c                	lw	a1,8(s1)
 35e:	40b2                	lw	ra,12(sp)
 360:	4492                	lw	s1,4(sp)
 362:	4390                	lw	a2,0(a5)
 364:	00000517          	auipc	a0,0x0
 368:	5b850513          	addi	a0,a0,1464 # 91c <vsnprintf+0x234>
 36c:	0141                	addi	sp,sp,16
 36e:	acc1                	j	63e <printf>

00000370 <_lr_sc_test_1>:
 370:	100526af          	lr.w	a3,(a0)
 374:	c108                	sw	a0,0(a0)
 376:	18c5252f          	sc.w	a0,a2,(a0)
 37a:	00153513          	seqz	a0,a0
 37e:	8082                	ret

00000380 <_lr_sc_test_2>:
 380:	100526af          	lr.w	a3,(a0)
 384:	c110                	sw	a2,0(a0)
 386:	18c5252f          	sc.w	a0,a2,(a0)
 38a:	00153513          	seqz	a0,a0
 38e:	8082                	ret

00000390 <_lr_sc_test_3>:
 390:	100526af          	lr.w	a3,(a0)
 394:	18c5252f          	sc.w	a0,a2,(a0)
 398:	00153513          	seqz	a0,a0
 39c:	8082                	ret

0000039e <_lr_sc_test_4>:
 39e:	100526af          	lr.w	a3,(a0)
 3a2:	18c5252f          	sc.w	a0,a2,(a0)
 3a6:	18c5252f          	sc.w	a0,a2,(a0)
 3aa:	00153513          	seqz	a0,a0
 3ae:	8082                	ret

000003b0 <_lr_sc_test_5>:
 3b0:	100526af          	lr.w	a3,(a0)
 3b4:	100526af          	lr.w	a3,(a0)
 3b8:	18c5252f          	sc.w	a0,a2,(a0)
 3bc:	00153513          	seqz	a0,a0
 3c0:	8082                	ret

000003c2 <atomic_test>:
 3c2:	1101                	addi	sp,sp,-32
 3c4:	cc22                	sw	s0,24(sp)
 3c6:	0060                	addi	s0,sp,12
 3c8:	4581                	li	a1,0
 3ca:	4605                	li	a2,1
 3cc:	8522                	mv	a0,s0
 3ce:	ce06                	sw	ra,28(sp)
 3d0:	ca26                	sw	s1,20(sp)
 3d2:	c84a                	sw	s2,16(sp)
 3d4:	c602                	sw	zero,12(sp)
 3d6:	3f69                	jal	370 <_lr_sc_test_1>
 3d8:	05800593          	li	a1,88
 3dc:	0e051f63          	bnez	a0,4da <atomic_test+0x118>
 3e0:	02b00513          	li	a0,43
 3e4:	2ca5                	jal	65c <putchar>
 3e6:	47b2                	lw	a5,12(sp)
 3e8:	00f40b63          	beq	s0,a5,3fe <atomic_test+0x3c>
 3ec:	05900593          	li	a1,89
 3f0:	00000517          	auipc	a0,0x0
 3f4:	55c50513          	addi	a0,a0,1372 # 94c <__func__.1634+0x20>
 3f8:	2499                	jal	63e <printf>
 3fa:	4505                	li	a0,1
 3fc:	2c31                	jal	618 <exit>
 3fe:	02b00513          	li	a0,43
 402:	2ca9                	jal	65c <putchar>
 404:	deadc4b7          	lui	s1,0xdeadc
 408:	4581                	li	a1,0
 40a:	eef48613          	addi	a2,s1,-273 # deadbeef <_bss_end+0xdeada7b7>
 40e:	8522                	mv	a0,s0
 410:	c602                	sw	zero,12(sp)
 412:	37bd                	jal	380 <_lr_sc_test_2>
 414:	05d00593          	li	a1,93
 418:	0c051163          	bnez	a0,4da <atomic_test+0x118>
 41c:	02b00513          	li	a0,43
 420:	2c35                	jal	65c <putchar>
 422:	47b2                	lw	a5,12(sp)
 424:	eef48493          	addi	s1,s1,-273
 428:	00978963          	beq	a5,s1,43a <atomic_test+0x78>
 42c:	05e00593          	li	a1,94
 430:	00000517          	auipc	a0,0x0
 434:	53450513          	addi	a0,a0,1332 # 964 <__func__.1634+0x38>
 438:	b7c1                	j	3f8 <atomic_test+0x36>
 43a:	02b00513          	li	a0,43
 43e:	2c39                	jal	65c <putchar>
 440:	4581                	li	a1,0
 442:	4605                	li	a2,1
 444:	8522                	mv	a0,s0
 446:	c602                	sw	zero,12(sp)
 448:	37a1                	jal	390 <_lr_sc_test_3>
 44a:	4785                	li	a5,1
 44c:	892a                	mv	s2,a0
 44e:	06200593          	li	a1,98
 452:	0cf51663          	bne	a0,a5,51e <atomic_test+0x15c>
 456:	02b00513          	li	a0,43
 45a:	2409                	jal	65c <putchar>
 45c:	44b2                	lw	s1,12(sp)
 45e:	06300593          	li	a1,99
 462:	05249a63          	bne	s1,s2,4b6 <atomic_test+0xf4>
 466:	02b00513          	li	a0,43
 46a:	2acd                	jal	65c <putchar>
 46c:	4581                	li	a1,0
 46e:	4605                	li	a2,1
 470:	8522                	mv	a0,s0
 472:	c602                	sw	zero,12(sp)
 474:	372d                	jal	39e <_lr_sc_test_4>
 476:	06700593          	li	a1,103
 47a:	e125                	bnez	a0,4da <atomic_test+0x118>
 47c:	02b00513          	li	a0,43
 480:	2af1                	jal	65c <putchar>
 482:	4932                	lw	s2,12(sp)
 484:	06800593          	li	a1,104
 488:	02991763          	bne	s2,s1,4b6 <atomic_test+0xf4>
 48c:	02b00513          	li	a0,43
 490:	22f1                	jal	65c <putchar>
 492:	4581                	li	a1,0
 494:	4605                	li	a2,1
 496:	8522                	mv	a0,s0
 498:	c602                	sw	zero,12(sp)
 49a:	3f19                	jal	3b0 <_lr_sc_test_5>
 49c:	84aa                	mv	s1,a0
 49e:	06c00593          	li	a1,108
 4a2:	07251e63          	bne	a0,s2,51e <atomic_test+0x15c>
 4a6:	02b00513          	li	a0,43
 4aa:	2a4d                	jal	65c <putchar>
 4ac:	47b2                	lw	a5,12(sp)
 4ae:	00978963          	beq	a5,s1,4c0 <atomic_test+0xfe>
 4b2:	06d00593          	li	a1,109
 4b6:	00000517          	auipc	a0,0x0
 4ba:	4de50513          	addi	a0,a0,1246 # 994 <__func__.1634+0x68>
 4be:	bf2d                	j	3f8 <atomic_test+0x36>
 4c0:	02b00513          	li	a0,43
 4c4:	2a61                	jal	65c <putchar>
 4c6:	0c800613          	li	a2,200
 4ca:	06400593          	li	a1,100
 4ce:	8522                	mv	a0,s0
 4d0:	c602                	sw	zero,12(sp)
 4d2:	3641                	jal	52 <_atomic_compare_exchange>
 4d4:	c901                	beqz	a0,4e4 <atomic_test+0x122>
 4d6:	07100593          	li	a1,113
 4da:	00000517          	auipc	a0,0x0
 4de:	45e50513          	addi	a0,a0,1118 # 938 <__func__.1634+0xc>
 4e2:	bf19                	j	3f8 <atomic_test+0x36>
 4e4:	02b00513          	li	a0,43
 4e8:	2a95                	jal	65c <putchar>
 4ea:	47b2                	lw	a5,12(sp)
 4ec:	cb81                	beqz	a5,4fc <atomic_test+0x13a>
 4ee:	07200593          	li	a1,114
 4f2:	00000517          	auipc	a0,0x0
 4f6:	4b650513          	addi	a0,a0,1206 # 9a8 <__func__.1634+0x7c>
 4fa:	bdfd                	j	3f8 <atomic_test+0x36>
 4fc:	02b00513          	li	a0,43
 500:	2ab1                	jal	65c <putchar>
 502:	06400493          	li	s1,100
 506:	0c800613          	li	a2,200
 50a:	06400593          	li	a1,100
 50e:	8522                	mv	a0,s0
 510:	c626                	sw	s1,12(sp)
 512:	3681                	jal	52 <_atomic_compare_exchange>
 514:	4785                	li	a5,1
 516:	00f50963          	beq	a0,a5,528 <atomic_test+0x166>
 51a:	07600593          	li	a1,118
 51e:	00000517          	auipc	a0,0x0
 522:	46250513          	addi	a0,a0,1122 # 980 <__func__.1634+0x54>
 526:	bdc9                	j	3f8 <atomic_test+0x36>
 528:	02b00513          	li	a0,43
 52c:	2a05                	jal	65c <putchar>
 52e:	4932                	lw	s2,12(sp)
 530:	0c800793          	li	a5,200
 534:	07700593          	li	a1,119
 538:	02f91b63          	bne	s2,a5,56e <atomic_test+0x1ac>
 53c:	02b00513          	li	a0,43
 540:	2a31                	jal	65c <putchar>
 542:	0c800593          	li	a1,200
 546:	8522                	mv	a0,s0
 548:	c626                	sw	s1,12(sp)
 54a:	3e39                	jal	68 <_atomic_exchange>
 54c:	00950963          	beq	a0,s1,55e <atomic_test+0x19c>
 550:	07b00593          	li	a1,123
 554:	00000517          	auipc	a0,0x0
 558:	47c50513          	addi	a0,a0,1148 # 9d0 <__func__.1634+0xa4>
 55c:	bd71                	j	3f8 <atomic_test+0x36>
 55e:	02b00513          	li	a0,43
 562:	28ed                	jal	65c <putchar>
 564:	47b2                	lw	a5,12(sp)
 566:	01278963          	beq	a5,s2,578 <atomic_test+0x1b6>
 56a:	07c00593          	li	a1,124
 56e:	00000517          	auipc	a0,0x0
 572:	44e50513          	addi	a0,a0,1102 # 9bc <__func__.1634+0x90>
 576:	b549                	j	3f8 <atomic_test+0x36>
 578:	02b00513          	li	a0,43
 57c:	20c5                	jal	65c <putchar>
 57e:	12c00493          	li	s1,300
 582:	19000593          	li	a1,400
 586:	8522                	mv	a0,s0
 588:	c626                	sw	s1,12(sp)
 58a:	3cf9                	jal	68 <_atomic_exchange>
 58c:	00950963          	beq	a0,s1,59e <atomic_test+0x1dc>
 590:	08000593          	li	a1,128
 594:	00000517          	auipc	a0,0x0
 598:	45050513          	addi	a0,a0,1104 # 9e4 <__func__.1634+0xb8>
 59c:	bdb1                	j	3f8 <atomic_test+0x36>
 59e:	02b00513          	li	a0,43
 5a2:	286d                	jal	65c <putchar>
 5a4:	4732                	lw	a4,12(sp)
 5a6:	19000793          	li	a5,400
 5aa:	00f70963          	beq	a4,a5,5bc <atomic_test+0x1fa>
 5ae:	08100593          	li	a1,129
 5b2:	00000517          	auipc	a0,0x0
 5b6:	44650513          	addi	a0,a0,1094 # 9f8 <__func__.1634+0xcc>
 5ba:	bd3d                	j	3f8 <atomic_test+0x36>
 5bc:	02b00513          	li	a0,43
 5c0:	2871                	jal	65c <putchar>
 5c2:	00000597          	auipc	a1,0x0
 5c6:	36a58593          	addi	a1,a1,874 # 92c <__func__.1634>
 5ca:	00000517          	auipc	a0,0x0
 5ce:	44250513          	addi	a0,a0,1090 # a0c <__func__.1634+0xe0>
 5d2:	20b5                	jal	63e <printf>
 5d4:	40f2                	lw	ra,28(sp)
 5d6:	4462                	lw	s0,24(sp)
 5d8:	44d2                	lw	s1,20(sp)
 5da:	4942                	lw	s2,16(sp)
 5dc:	6105                	addi	sp,sp,32
 5de:	8082                	ret

000005e0 <default_getchar>:
 5e0:	9002                	ebreak
 5e2:	4501                	li	a0,0
 5e4:	8082                	ret

000005e6 <default_putchar>:
 5e6:	9002                	ebreak
 5e8:	4501                	li	a0,0
 5ea:	8082                	ret

000005ec <default_poweroff>:
 5ec:	9002                	ebreak
 5ee:	a001                	j	5ee <default_poweroff+0x2>

000005f0 <register_console>:
 5f0:	00052303          	lw	t1,0(a0)
 5f4:	00001797          	auipc	a5,0x1
 5f8:	a0a7aa23          	sw	a0,-1516(a5) # 1008 <console_dev>
 5fc:	00030363          	beqz	t1,602 <register_console+0x12>
 600:	8302                	jr	t1
 602:	8082                	ret

00000604 <register_poweroff>:
 604:	00052303          	lw	t1,0(a0)
 608:	00001797          	auipc	a5,0x1
 60c:	a0a7a223          	sw	a0,-1532(a5) # 100c <poweroff_dev>
 610:	00030363          	beqz	t1,616 <register_poweroff+0x12>
 614:	8302                	jr	t1
 616:	8082                	ret

00000618 <exit>:
 618:	00001797          	auipc	a5,0x1
 61c:	9f478793          	addi	a5,a5,-1548 # 100c <poweroff_dev>
 620:	439c                	lw	a5,0(a5)
 622:	1141                	addi	sp,sp,-16
 624:	c606                	sw	ra,12(sp)
 626:	43dc                	lw	a5,4(a5)
 628:	9782                	jalr	a5
 62a:	a001                	j	62a <exit+0x12>

0000062c <memset>:
 62c:	962a                	add	a2,a2,a0
 62e:	87aa                	mv	a5,a0
 630:	00c79363          	bne	a5,a2,636 <memset+0xa>
 634:	8082                	ret
 636:	0785                	addi	a5,a5,1
 638:	feb78fa3          	sb	a1,-1(a5)
 63c:	bfd5                	j	630 <memset+0x4>

0000063e <printf>:
 63e:	7139                	addi	sp,sp,-64
 640:	d22e                	sw	a1,36(sp)
 642:	104c                	addi	a1,sp,36
 644:	ce06                	sw	ra,28(sp)
 646:	d432                	sw	a2,40(sp)
 648:	d636                	sw	a3,44(sp)
 64a:	d83a                	sw	a4,48(sp)
 64c:	da3e                	sw	a5,52(sp)
 64e:	dc42                	sw	a6,56(sp)
 650:	de46                	sw	a7,60(sp)
 652:	c62e                	sw	a1,12(sp)
 654:	281d                	jal	68a <vprintf>
 656:	40f2                	lw	ra,28(sp)
 658:	6121                	addi	sp,sp,64
 65a:	8082                	ret

0000065c <putchar>:
 65c:	00001797          	auipc	a5,0x1
 660:	9ac78793          	addi	a5,a5,-1620 # 1008 <console_dev>
 664:	439c                	lw	a5,0(a5)
 666:	0087a303          	lw	t1,8(a5)
 66a:	8302                	jr	t1

0000066c <puts>:
 66c:	1141                	addi	sp,sp,-16
 66e:	c422                	sw	s0,8(sp)
 670:	c606                	sw	ra,12(sp)
 672:	842a                	mv	s0,a0
 674:	00044503          	lbu	a0,0(s0)
 678:	e511                	bnez	a0,684 <puts+0x18>
 67a:	4422                	lw	s0,8(sp)
 67c:	40b2                	lw	ra,12(sp)
 67e:	4529                	li	a0,10
 680:	0141                	addi	sp,sp,16
 682:	bfe9                	j	65c <putchar>
 684:	0405                	addi	s0,s0,1
 686:	3fd9                	jal	65c <putchar>
 688:	b7f5                	j	674 <puts+0x8>

0000068a <vprintf>:
 68a:	1101                	addi	sp,sp,-32
 68c:	cc22                	sw	s0,24(sp)
 68e:	ca26                	sw	s1,20(sp)
 690:	c84a                	sw	s2,16(sp)
 692:	c64e                	sw	s3,12(sp)
 694:	c452                	sw	s4,8(sp)
 696:	ce06                	sw	ra,28(sp)
 698:	1000                	addi	s0,sp,32
 69a:	86ae                	mv	a3,a1
 69c:	862a                	mv	a2,a0
 69e:	89aa                	mv	s3,a0
 6a0:	8a2e                	mv	s4,a1
 6a2:	4501                	li	a0,0
 6a4:	55fd                	li	a1,-1
 6a6:	2089                	jal	6e8 <vsnprintf>
 6a8:	01850793          	addi	a5,a0,24
 6ac:	9bc1                	andi	a5,a5,-16
 6ae:	40f10133          	sub	sp,sp,a5
 6b2:	00f10493          	addi	s1,sp,15
 6b6:	98c1                	andi	s1,s1,-16
 6b8:	892a                	mv	s2,a0
 6ba:	00150593          	addi	a1,a0,1
 6be:	86d2                	mv	a3,s4
 6c0:	864e                	mv	a2,s3
 6c2:	8526                	mv	a0,s1
 6c4:	2015                	jal	6e8 <vsnprintf>
 6c6:	0004c503          	lbu	a0,0(s1)
 6ca:	ed01                	bnez	a0,6e2 <vprintf+0x58>
 6cc:	fe040113          	addi	sp,s0,-32
 6d0:	40f2                	lw	ra,28(sp)
 6d2:	854a                	mv	a0,s2
 6d4:	4462                	lw	s0,24(sp)
 6d6:	44d2                	lw	s1,20(sp)
 6d8:	4942                	lw	s2,16(sp)
 6da:	49b2                	lw	s3,12(sp)
 6dc:	4a22                	lw	s4,8(sp)
 6de:	6105                	addi	sp,sp,32
 6e0:	8082                	ret
 6e2:	0485                	addi	s1,s1,1
 6e4:	3fa5                	jal	65c <putchar>
 6e6:	b7c5                	j	6c6 <vprintf+0x3c>

000006e8 <vsnprintf>:
 6e8:	7179                	addi	sp,sp,-48
 6ea:	d622                	sw	s0,44(sp)
 6ec:	d426                	sw	s1,40(sp)
 6ee:	d24a                	sw	s2,36(sp)
 6f0:	d04e                	sw	s3,32(sp)
 6f2:	ce52                	sw	s4,28(sp)
 6f4:	cc56                	sw	s5,24(sp)
 6f6:	ca5a                	sw	s6,20(sp)
 6f8:	c85e                	sw	s7,16(sp)
 6fa:	c662                	sw	s8,12(sp)
 6fc:	c466                	sw	s9,8(sp)
 6fe:	c26a                	sw	s10,4(sp)
 700:	4781                	li	a5,0
 702:	4701                	li	a4,0
 704:	02500e13          	li	t3,37
 708:	06c00313          	li	t1,108
 70c:	07300e93          	li	t4,115
 710:	07800f13          	li	t5,120
 714:	5ff1                	li	t6,-4
 716:	42a5                	li	t0,9
 718:	43a1                	li	t2,8
 71a:	07000413          	li	s0,112
 71e:	07800493          	li	s1,120
 722:	03000913          	li	s2,48
 726:	06300993          	li	s3,99
 72a:	06400a13          	li	s4,100
 72e:	48a9                	li	a7,10
 730:	5afd                	li	s5,-1
 732:	02d00b13          	li	s6,45
 736:	00064803          	lbu	a6,0(a2)
 73a:	02081663          	bnez	a6,766 <vsnprintf+0x7e>
 73e:	c511                	beqz	a0,74a <vsnprintf+0x62>
 740:	16b7f063          	bgeu	a5,a1,8a0 <vsnprintf+0x1b8>
 744:	953e                	add	a0,a0,a5
 746:	00050023          	sb	zero,0(a0)
 74a:	5432                	lw	s0,44(sp)
 74c:	54a2                	lw	s1,40(sp)
 74e:	5912                	lw	s2,36(sp)
 750:	5982                	lw	s3,32(sp)
 752:	4a72                	lw	s4,28(sp)
 754:	4ae2                	lw	s5,24(sp)
 756:	4b52                	lw	s6,20(sp)
 758:	4bc2                	lw	s7,16(sp)
 75a:	4c32                	lw	s8,12(sp)
 75c:	4ca2                	lw	s9,8(sp)
 75e:	4d12                	lw	s10,4(sp)
 760:	853e                	mv	a0,a5
 762:	6145                	addi	sp,sp,48
 764:	8082                	ret
 766:	12070063          	beqz	a4,886 <vsnprintf+0x19e>
 76a:	00680863          	beq	a6,t1,77a <vsnprintf+0x92>
 76e:	01036863          	bltu	t1,a6,77e <vsnprintf+0x96>
 772:	0f380e63          	beq	a6,s3,86e <vsnprintf+0x186>
 776:	07480963          	beq	a6,s4,7e8 <vsnprintf+0x100>
 77a:	0605                	addi	a2,a2,1
 77c:	bf6d                	j	736 <vsnprintf+0x4e>
 77e:	0dd80163          	beq	a6,t4,840 <vsnprintf+0x158>
 782:	03e80363          	beq	a6,t5,7a8 <vsnprintf+0xc0>
 786:	fe881ae3          	bne	a6,s0,77a <vsnprintf+0x92>
 78a:	cd11                	beqz	a0,7a6 <vsnprintf+0xbe>
 78c:	00b7f663          	bgeu	a5,a1,798 <vsnprintf+0xb0>
 790:	00f50733          	add	a4,a0,a5
 794:	01270023          	sb	s2,0(a4)
 798:	00178713          	addi	a4,a5,1
 79c:	00b77563          	bgeu	a4,a1,7a6 <vsnprintf+0xbe>
 7a0:	972a                	add	a4,a4,a0
 7a2:	00970023          	sb	s1,0(a4)
 7a6:	0789                	addi	a5,a5,2
 7a8:	0006ac03          	lw	s8,0(a3)
 7ac:	4801                	li	a6,0
 7ae:	0691                	addi	a3,a3,4
 7b0:	c515                	beqz	a0,7dc <vsnprintf+0xf4>
 7b2:	01078bb3          	add	s7,a5,a6
 7b6:	02bbf363          	bgeu	s7,a1,7dc <vsnprintf+0xf4>
 7ba:	03f80733          	mul	a4,a6,t6
 7be:	0771                	addi	a4,a4,28
 7c0:	40ec5733          	sra	a4,s8,a4
 7c4:	8b3d                	andi	a4,a4,15
 7c6:	0ff77d13          	andi	s10,a4,255
 7ca:	057d0c93          	addi	s9,s10,87
 7ce:	00e2c463          	blt	t0,a4,7d6 <vsnprintf+0xee>
 7d2:	030d0c93          	addi	s9,s10,48
 7d6:	9baa                	add	s7,s7,a0
 7d8:	019b8023          	sb	s9,0(s7)
 7dc:	0805                	addi	a6,a6,1
 7de:	fc7819e3          	bne	a6,t2,7b0 <vsnprintf+0xc8>
 7e2:	07a1                	addi	a5,a5,8
 7e4:	4701                	li	a4,0
 7e6:	bf51                	j	77a <vsnprintf+0x92>
 7e8:	00468c93          	addi	s9,a3,4
 7ec:	4294                	lw	a3,0(a3)
 7ee:	0006dc63          	bgez	a3,806 <vsnprintf+0x11e>
 7f2:	40d006b3          	neg	a3,a3
 7f6:	c519                	beqz	a0,804 <vsnprintf+0x11c>
 7f8:	00b7f663          	bgeu	a5,a1,804 <vsnprintf+0x11c>
 7fc:	00f50833          	add	a6,a0,a5
 800:	01680023          	sb	s6,0(a6)
 804:	0785                	addi	a5,a5,1
 806:	883a                	mv	a6,a4
 808:	8736                	mv	a4,a3
 80a:	03174733          	div	a4,a4,a7
 80e:	e71d                	bnez	a4,83c <vsnprintf+0x154>
 810:	fff80b93          	addi	s7,a6,-1
 814:	cd01                	beqz	a0,82c <vsnprintf+0x144>
 816:	01778c33          	add	s8,a5,s7
 81a:	00bc7963          	bgeu	s8,a1,82c <vsnprintf+0x144>
 81e:	0316ed33          	rem	s10,a3,a7
 822:	9c2a                	add	s8,s8,a0
 824:	030d0d13          	addi	s10,s10,48
 828:	01ac0023          	sb	s10,0(s8)
 82c:	1bfd                	addi	s7,s7,-1
 82e:	0316c6b3          	div	a3,a3,a7
 832:	ff5b91e3          	bne	s7,s5,814 <vsnprintf+0x12c>
 836:	97c2                	add	a5,a5,a6
 838:	86e6                	mv	a3,s9
 83a:	b781                	j	77a <vsnprintf+0x92>
 83c:	0805                	addi	a6,a6,1
 83e:	b7f1                	j	80a <vsnprintf+0x122>
 840:	0006ab83          	lw	s7,0(a3)
 844:	00468813          	addi	a6,a3,4
 848:	873e                	mv	a4,a5
 84a:	40f706b3          	sub	a3,a4,a5
 84e:	96de                	add	a3,a3,s7
 850:	0006c683          	lbu	a3,0(a3)
 854:	e681                	bnez	a3,85c <vsnprintf+0x174>
 856:	86c2                	mv	a3,a6
 858:	87ba                	mv	a5,a4
 85a:	b769                	j	7e4 <vsnprintf+0xfc>
 85c:	c519                	beqz	a0,86a <vsnprintf+0x182>
 85e:	00b77663          	bgeu	a4,a1,86a <vsnprintf+0x182>
 862:	00e50c33          	add	s8,a0,a4
 866:	00dc0023          	sb	a3,0(s8)
 86a:	0705                	addi	a4,a4,1
 86c:	bff9                	j	84a <vsnprintf+0x162>
 86e:	c911                	beqz	a0,882 <vsnprintf+0x19a>
 870:	00b7f963          	bgeu	a5,a1,882 <vsnprintf+0x19a>
 874:	0006a803          	lw	a6,0(a3)
 878:	00f50733          	add	a4,a0,a5
 87c:	0691                	addi	a3,a3,4
 87e:	01070023          	sb	a6,0(a4)
 882:	0785                	addi	a5,a5,1
 884:	b785                	j	7e4 <vsnprintf+0xfc>
 886:	01c80b63          	beq	a6,t3,89c <vsnprintf+0x1b4>
 88a:	c519                	beqz	a0,898 <vsnprintf+0x1b0>
 88c:	00b7f663          	bgeu	a5,a1,898 <vsnprintf+0x1b0>
 890:	00f50c33          	add	s8,a0,a5
 894:	010c0023          	sb	a6,0(s8)
 898:	0785                	addi	a5,a5,1
 89a:	b5c5                	j	77a <vsnprintf+0x92>
 89c:	4705                	li	a4,1
 89e:	bdf1                	j	77a <vsnprintf+0x92>
 8a0:	ea0585e3          	beqz	a1,74a <vsnprintf+0x62>
 8a4:	952e                	add	a0,a0,a1
 8a6:	fe050fa3          	sb	zero,-1(a0)
 8aa:	b545                	j	74a <vsnprintf+0x62>
