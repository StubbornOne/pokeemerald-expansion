	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.syntax unified

	.text

//	thumb_func_start sp136_strengh_sound
//sp136_strengh_sound: @ 81BE89C
//	push {r4-r6,lr}
//	mov r6, r8
//	push {r6}
//	mov r8, r0
//	adds r4, r1, 0
//	adds r5, r2, 0
//	adds r6, r3, 0
//	lsls r0, 24
//	lsrs r0, 24
//	mov r8, r0
//	lsls r4, 24
//	lsrs r4, 24
//	lsls r5, 24
//	lsrs r5, 24
//	lsls r6, 24
//	lsrs r6, 24
//	ldr r0, =sub_81BE900
//	movs r1, 0x9
//	bl CreateTask
//	lsls r0, 24
//	lsrs r0, 24
//	ldr r2, =gTasks
//	lsls r1, r0, 2
//	adds r1, r0
//	lsls r1, 3
//	adds r1, r2
//	movs r0, 0
//	strh r4, [r1, 0x8]
//	strh r0, [r1, 0xA]
//	strh r5, [r1, 0xC]
//	strh r6, [r1, 0xE]
//	mov r0, r8
//	strh r0, [r1, 0x10]
//	movs r0, 0
//	bl SetCameraPanningCallback
//	movs r0, 0xD6
//	bl PlaySE
//	pop {r3}
//	mov r8, r3
//	pop {r4-r6}
//	pop {r0}
//	bx r0
//	.pool
//	thumb_func_end sp136_strengh_sound

//	thumb_func_start sub_81BE900
//sub_81BE900: @ 81BE900
//	push {r4,r5,lr}
//	lsls r0, 24
//	lsrs r5, r0, 24
//	lsls r0, r5, 2
//	adds r0, r5
//	lsls r0, 3
//	ldr r1, =gTasks + 0x8
//	adds r4, r0, r1
//	ldrh r0, [r4, 0x2]
//	adds r0, 0x1
//	strh r0, [r4, 0x2]
//	movs r1, 0x2
//	ldrsh r0, [r4, r1]
//	movs r2, 0x6
//	ldrsh r1, [r4, r2]
//	bl __modsi3
//	cmp r0, 0
//	bne _081BE95C
//	strh r0, [r4, 0x2]
//	ldrh r0, [r4, 0x4]
//	subs r0, 0x1
//	strh r0, [r4, 0x4]
//	ldrh r0, [r4]
//	negs r0, r0
//	strh r0, [r4]
//	ldrh r0, [r4, 0x8]
//	negs r0, r0
//	strh r0, [r4, 0x8]
//	movs r1, 0
//	ldrsh r0, [r4, r1]
//	movs r2, 0x8
//	ldrsh r1, [r4, r2]
//	bl SetCameraPanning
//	movs r1, 0x4
//	ldrsh r0, [r4, r1]
//	cmp r0, 0
//	bne _081BE95C
//	bl sub_81BE968
//	adds r0, r5, 0
//	bl DestroyTask
//	bl InstallCameraPanAheadCallback
//_081BE95C:
//	pop {r4,r5}
//	pop {r0}
//	bx r0
//	.pool
//	thumb_func_end sub_81BE900

//	thumb_func_start sub_81BE968
//sub_81BE968: @ 81BE968
//	push {lr}
//	ldr r0, =sub_81BE9C0
//	bl FindTaskIdByFunc
//	lsls r0, 24
//	lsrs r2, r0, 24
//	cmp r2, 0xFF
//	beq _081BE988
//	ldr r0, =gTasks
//	lsls r1, r2, 2
//	adds r1, r2
//	lsls r1, 3
//	adds r1, r0
//	ldrh r0, [r1, 0x8]
//	adds r0, 0x1
//	strh r0, [r1, 0x8]
//_081BE988:
//	pop {r0}
//	bx r0
//	.pool
//	thumb_func_end sub_81BE968

//	thumb_func_start sub_81BE994
//sub_81BE994: @ 81BE994
//	push {lr}
//	ldr r0, =gUnknown_08617D94
//	bl LoadSpriteSheets
//	bl sub_81BEA20
//	ldr r0, =sub_81BE9C0
//	movs r1, 0x8
//	bl CreateTask
//	movs r0, 0x2
//	movs r1, 0x1
//	movs r2, 0x10
//	movs r3, 0x3
//	bl sp136_strengh_sound
//	pop {r0}
//	bx r0
//	.pool
//	thumb_func_end sub_81BE994

//	thumb_func_start sub_81BE9C0
//sub_81BE9C0: @ 81BE9C0
//	push {r4,lr}
//	lsls r0, 24
//	lsrs r0, 24
//	lsls r1, r0, 2
//	adds r1, r0
//	lsls r3, r1, 3
//	ldr r4, =gTasks + 0x8
//	adds r2, r3, r4
//	ldrh r0, [r2, 0x2]
//	adds r0, 0x1
//	strh r0, [r2, 0x2]
//	lsls r0, 16
//	movs r1, 0xFA
//	lsls r1, 18
//	cmp r0, r1
//	beq _081BE9E6
//	ldrh r0, [r2]
//	cmp r0, 0x11
//	bne _081BE9F0
//_081BE9E6:
//	adds r0, r4, 0
//	subs r0, 0x8
//	adds r0, r3, r0
//	ldr r1, =sub_81BEA00
//	str r1, [r0]
//_081BE9F0:
//	pop {r4}
//	pop {r0}
//	bx r0
//	.pool
//	thumb_func_end sub_81BE9C0

//	thumb_func_start sub_81BEA00
//sub_81BEA00: @ 81BEA00
//	push {r4,lr}
//	adds r4, r0, 0
//	lsls r4, 24
//	lsrs r4, 24
//	movs r0, 0xFA
//	lsls r0, 4
//	bl FreeSpriteTilesByTag
//	adds r0, r4, 0
//	bl DestroyTask
//	bl EnableBothScriptContexts
//	pop {r4}
//	pop {r0}
//	bx r0
//	thumb_func_end sub_81BEA00

//	thumb_func_start sub_81BEA20
//sub_81BEA20: @ 81BEA20
//	push {r4-r7,lr}
//	movs r4, 0
//	ldr r5, =gUnknown_08617D64
//	adds r7, r5, 0x2
//	ldr r6, =gSprites
//_081BEA2A:
//	lsls r0, r4, 1
//	adds r0, r4
//	lsls r0, 1
//	adds r1, r0, r5
//	ldrh r1, [r1]
//	adds r1, 0x78
//	lsls r1, 16
//	asrs r1, 16
//	adds r0, r7
//	movs r3, 0
//	ldrsh r2, [r0, r3]
//	ldr r0, =gUnknown_08617E60
//	movs r3, 0x8
//	bl CreateSprite
//	lsls r0, 24
//	lsrs r1, r0, 24
//	lsls r0, r1, 4
//	adds r0, r1
//	lsls r0, 2
//	adds r0, r6
//	ldrb r2, [r0, 0x5]
//	movs r3, 0xD
//	negs r3, r3
//	adds r1, r3, 0
//	ands r2, r1
//	movs r1, 0xF
//	ands r2, r1
//	strb r2, [r0, 0x5]
//	strh r4, [r0, 0x2E]
//	adds r0, r4, 0x1
//	lsls r0, 24
//	lsrs r4, r0, 24
//	cmp r4, 0x7
//	bls _081BEA2A
//	movs r4, 0
//	ldr r5, =gUnknown_08617D64
//	adds r7, r5, 0x2
//	ldr r6, =gSprites
//_081BEA78:
//	lsls r0, r4, 1
//	adds r0, r4
//	lsls r0, 1
//	adds r1, r0, r5
//	ldrh r1, [r1]
//	adds r1, 0x73
//	lsls r1, 16
//	asrs r1, 16
//	adds r0, r7
//	ldrh r2, [r0]
//	subs r2, 0x3
//	lsls r2, 16
//	asrs r2, 16
//	ldr r0, =gUnknown_08617E34
//	movs r3, 0x8
//	bl CreateSprite
//	lsls r0, 24
//	lsrs r1, r0, 24
//	lsls r0, r1, 4
//	adds r0, r1
//	lsls r0, 2
//	adds r0, r6
//	ldrb r2, [r0, 0x5]
//	movs r3, 0xD
//	negs r3, r3
//	adds r1, r3, 0
//	ands r2, r1
//	movs r1, 0xF
//	ands r2, r1
//	strb r2, [r0, 0x5]
//	strh r4, [r0, 0x2E]
//	adds r0, r4, 0x1
//	lsls r0, 24
//	lsrs r4, r0, 24
//	cmp r4, 0x7
//	bls _081BEA78
//	pop {r4-r7}
//	pop {r0}
//	bx r0
//	.pool
//	thumb_func_end sub_81BEA20

	thumb_func_start sub_81BEAD8
sub_81BEAD8: @ 81BEAD8
	push {r4,lr}
	adds r4, r0, 0
	ldrh r0, [r4, 0x30]
	adds r0, 0x2
	strh r0, [r4, 0x30]
	movs r1, 0x30
	ldrsh r0, [r4, r1]
	lsrs r1, r0, 31
	adds r0, r1
	asrs r0, 1
	strh r0, [r4, 0x26]
	movs r0, 0x22
	ldrsh r3, [r4, r0]
	movs r1, 0x26
	ldrsh r0, [r4, r1]
	adds r3, r0
	ldr r2, =gUnknown_08617D64
	movs r0, 0x2E
	ldrsh r1, [r4, r0]
	lsls r0, r1, 1
	adds r0, r1
	lsls r0, 1
	adds r2, 0x4
	adds r0, r2
	movs r1, 0
	ldrsh r0, [r0, r1]
	cmp r3, r0
	ble _081BEB1A
	adds r0, r4, 0
	bl DestroySprite
	bl sub_81BE968
_081BEB1A:
	pop {r4}
	pop {r0}
	bx r0
	.pool
	thumb_func_end sub_81BEAD8

	.align 2, 0 @ Don't pad with nop.
