source_filename = "type_conflict.cpp"

define i32 @conflicting(i32 %x, i32 %y) !dbg !7 {
entry:
  %r = sdiv i32 %x, %y, !dbg !9
    #dbg_value(i32 %r, !8, !DIExpression(), !9)
  ret i32 %r, !dbg !9
}

define i8 @attribute_only(i8 signext %x) {
entry:
  %r = sub i8 %x, 1
  ret i8 %r
}

define i8 @attribute_zero_only(i8 zeroext %x) {
entry:
  %r = sub i8 %x, 1
  ret i8 %r
}

define i32 @promoted_u16_nsw(i16 %a, i16 %b) {
entry:
  %left = zext i16 %a to i32
  %right = zext i16 %b to i32
  %r = add nsw i32 %left, %right
  ret i32 %r
}

define i32 @qualified(i32 %x) !dbg !10 {
entry:
  %r = sub i32 %x, 1, !dbg !12
    #dbg_value(i32 %r, !11, !DIExpression(), !12)
  ret i32 %r, !dbg !12
}

define i32 @bitfield_direct(i32 %x) !dbg !21 {
entry:
  %r = sub i32 %x, 1, !dbg !22
    #dbg_value(i32 %r, !18, !DIExpression(), !22)
  ret i32 %r, !dbg !22
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3}
!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !1, producer: "ValueTrace test", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "type_conflict.cpp", directory: ".")
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !DISubroutineType(types: !5)
!5 = !{!6, !6, !6}
!6 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!7 = distinct !DISubprogram(name: "conflicting", scope: !1, file: !1, line: 1, type: !4, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0)
!8 = !DILocalVariable(name: "result", scope: !7, file: !1, line: 2, type: !6)
!9 = !DILocation(line: 2, scope: !7)
!10 = distinct !DISubprogram(name: "qualified", scope: !1, file: !1, line: 5, type: !4, scopeLine: 5, spFlags: DISPFlagDefinition, unit: !0)
!11 = !DILocalVariable(name: "result", scope: !10, file: !1, line: 6, type: !13)
!12 = !DILocation(line: 6, scope: !10)
!13 = !DIDerivedType(tag: DW_TAG_volatile_type, baseType: !14)
!14 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !15)
!15 = !DIDerivedType(tag: DW_TAG_typedef, name: "NamedCount", baseType: !6)
!18 = !DILocalVariable(name: "field", scope: !21, file: !1, line: 6, type: !19)
!19 = !DIDerivedType(tag: DW_TAG_member, name: "field", scope: !20, baseType: !6, size: 5, flags: DIFlagBitField, extraData: i64 0)
!20 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "Bits", file: !1, line: 6, size: 32)
!21 = distinct !DISubprogram(name: "bitfield_direct", scope: !1, file: !1, line: 8, type: !4, scopeLine: 8, spFlags: DISPFlagDefinition, unit: !0)
!22 = !DILocation(line: 9, scope: !21)
