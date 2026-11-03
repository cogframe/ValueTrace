; Evidence-specific failures: these records do not establish why a compiler
; transformed or removed a location. Never label every missing mapping "inline".
define i32 @computed(i32 %x) !dbg !10 {
entry:
  %next = add i32 %x, 1, !dbg !13
    #dbg_value(i32 %next, !12, !DIExpression(), !13)
  ret i32 %next, !dbg !13
}

define i32 @expression(i32 %x) !dbg !20 {
entry:
    #dbg_value(i32 %x, !22, !DIExpression(DW_OP_plus_uconst, 1, DW_OP_stack_value), !23)
  ret i32 %x, !dbg !23
}

define i32 @fragmented(i32 %low, i32 %high) !dbg !30 {
entry:
    #dbg_value(i32 %low, !32, !DIExpression(DW_OP_LLVM_fragment, 0, 32), !33)
    #dbg_value(i32 %high, !32, !DIExpression(DW_OP_LLVM_fragment, 32, 32), !33)
  ret i32 %low, !dbg !33
}

define i32 @ambiguous(i32 %a, i32 %b) !dbg !40 {
entry:
    #dbg_value(i32 %a, !42, !DIExpression(), !43)
    #dbg_value(i32 %b, !42, !DIExpression(), !43)
  ret i32 %a, !dbg !43
}

define i32 @missing_map(i32 %x) !dbg !50 {
entry:
  ret i32 %x, !dbg !53
}

define i32 @shadowed(i32 %x, i32 %other) !dbg !60 {
entry:
    #dbg_value(i32 %x, !62, !DIExpression(), !63)
    #dbg_value(i32 %other, !64, !DIExpression(), !63)
  ret i32 %x, !dbg !63
}

define i32 @addressed(i32 %x) !dbg !70 {
entry:
  %slot = alloca i32
  store i32 %x, ptr %slot
    #dbg_declare(ptr %slot, !72, !DIExpression(), !73)
  ret i32 %x, !dbg !73
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!4}
!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !1, producer: "ValueTrace test", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "parameter_mappings.cpp", directory: "/fixtures")
!2 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!3 = !DISubroutineType(types: !{!2, !2})
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !DIBasicType(name: "long long", size: 64, encoding: DW_ATE_signed)
!10 = distinct !DISubprogram(name: "computed", scope: !1, file: !1, line: 1, type: !3, scopeLine: 1, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !11)
!11 = !{!12}
!12 = !DILocalVariable(name: "count", arg: 1, scope: !10, file: !1, line: 1, type: !2)
!13 = !DILocation(line: 2, scope: !10)
!20 = distinct !DISubprogram(name: "expression", scope: !1, file: !1, line: 10, type: !3, scopeLine: 10, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !21)
!21 = !{!22}
!22 = !DILocalVariable(name: "count", arg: 1, scope: !20, file: !1, line: 10, type: !2)
!23 = !DILocation(line: 11, scope: !20)
!30 = distinct !DISubprogram(name: "fragmented", scope: !1, file: !1, line: 20, type: !3, scopeLine: 20, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !31)
!31 = !{!32}
!32 = !DILocalVariable(name: "count", arg: 1, scope: !30, file: !1, line: 20, type: !5)
!33 = !DILocation(line: 21, scope: !30)
!40 = distinct !DISubprogram(name: "ambiguous", scope: !1, file: !1, line: 30, type: !3, scopeLine: 30, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !41)
!41 = !{!42}
!42 = !DILocalVariable(name: "count", arg: 1, scope: !40, file: !1, line: 30, type: !2)
!43 = !DILocation(line: 31, scope: !40)
!50 = distinct !DISubprogram(name: "missing_map", scope: !1, file: !1, line: 40, type: !3, scopeLine: 40, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !51)
!51 = !{!52}
!52 = !DILocalVariable(name: "count", arg: 1, scope: !50, file: !1, line: 40, type: !2)
!53 = !DILocation(line: 41, scope: !50)
!60 = distinct !DISubprogram(name: "shadowed", scope: !1, file: !1, line: 50, type: !3, scopeLine: 50, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !61)
!61 = !{!62, !64}
!62 = !DILocalVariable(name: "count", arg: 1, scope: !60, file: !1, line: 50, type: !2)
!63 = !DILocation(line: 51, scope: !60)
!64 = !DILocalVariable(name: "count", scope: !65, file: !1, line: 51, type: !2)
!65 = distinct !DILexicalBlock(scope: !60, file: !1, line: 51, column: 1)
!70 = distinct !DISubprogram(name: "addressed", scope: !1, file: !1, line: 60, type: !3, scopeLine: 60, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !71)
!71 = !{!72}
!72 = !DILocalVariable(name: "count", arg: 1, scope: !70, file: !1, line: 60, type: !2)
!73 = !DILocation(line: 61, scope: !70)

define i64 @aggregate(i64 %packed) !dbg !80 {
entry:
    #dbg_value(i64 %packed, !82, !DIExpression(), !83)
  ret i64 %packed, !dbg !83
}
!80 = distinct !DISubprogram(name: "aggregate", scope: !1, file: !1, line: 70, type: !3, scopeLine: 70, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !81)
!81 = !{!82}
!82 = !DILocalVariable(name: "count", arg: 1, scope: !80, file: !1, line: 70, type: !84)
!83 = !DILocation(line: 71, scope: !80)
!84 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "Packed", file: !1, line: 70, size: 64, elements: !{})
